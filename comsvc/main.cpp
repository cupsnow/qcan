#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QSysInfo>
#include <QByteArray>
#include <QTextStream>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCanBus>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtDebug>
#include <cstdio>

#include "SerialPortServer.h"
#include "CanbusCfg.h"

static QTextStream stdOut(stdout);
static QString indent = "  ";
static CanbusCfg &cfg = CanbusCfg::instance();

static QString indents(QString indent, int level = 1) {
    if (level > 1) return indent + indents(indent, level - 1);
    return indent;
}

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("comsvc");
	QCoreApplication::setApplicationVersion("1.0");

	QCommandLineParser cliParser;
	cliParser.setApplicationDescription(QCoreApplication::translate("main",
			"Service between serial port."));
	const QCommandLineOption helpOption = cliParser.addHelpOption();
	const QCommandLineOption versionOption = cliParser.addVersionOption();

    const QCommandLineOption jsonCfgOption(QStringList() << "c" << "cfg",
            QCoreApplication::translate("main", "JSON configuration file."),
            QCoreApplication::translate("main", "CFG-FILE"));
    cliParser.addOption(jsonCfgOption);

    const QCommandLineOption listSerialPortOption(QStringList() << "l" << "list",
			QCoreApplication::translate("main", "List available serial port."));
	cliParser.addOption(listSerialPortOption);

	const QCommandLineOption serialPortOption(QStringList() << "s" << "serialport",
			QCoreApplication::translate("main", "Open serial port."),
			QCoreApplication::translate("main", "SERIAL-PORT"));
	cliParser.addOption(serialPortOption);

	const QCommandLineOption baudRateOption(QStringList() << "b" << "baudrate",
			QCoreApplication::translate("main", "Baud rate to serial port."),
			QCoreApplication::translate("main", "BAUD-RATE"));
	cliParser.addOption(baudRateOption);

	const QCommandLineOption bindTCPOption(QStringList() << "t" << "tcp",
			QCoreApplication::translate("main", "Bind tcp."),
			QCoreApplication::translate("main", "IP:PORT"));
	cliParser.addOption(bindTCPOption);

	const QCommandLineOption bindWSOption(QStringList() << "w" << "ws",
			QCoreApplication::translate("main", "Bind websocket."),
			QCoreApplication::translate("main", "IP:PORT"));
	cliParser.addOption(bindWSOption);

	cliParser.process(app);

    do {
        QString jsonCfg = cliParser.value(jsonCfgOption);
        if (jsonCfg.isEmpty()) break;

        QFile cfgFile(jsonCfg);

        if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << QCoreApplication::translate("main", "Failed to load default config.") << endl;
            break;
        }

        QJsonParseError parseErr;
        QJsonDocument cfgDoc = QJsonDocument::fromJson(cfgFile.readAll(), &parseErr);
        if (cfgDoc.isNull()) {
            qWarning() << QCoreApplication::translate("main", "Empty default config.") <<
                          QString(" (%1)").arg(parseErr.errorString()) << endl;
            break;
        }

        cfg.json = cfgDoc.object();

    } while(0);

    for (QJsonObject::iterator iter = cfg.json.begin(); iter != cfg.json.end(); ++iter) {
        stdOut << "key: " << iter.key() << endl;
    }

	if (cliParser.isSet(listSerialPortOption)) {
        int indentLevel = 1;

		const QList<QSerialPortInfo> serialPorts =
				QSerialPortInfo::availablePorts();
        stdOut << QCoreApplication::translate("main", "Serial port:") << endl;
		for (int i = 0; i < serialPorts.size(); i++) {
			QSerialPortInfo serialPort = serialPorts.at(i);
			QString msg = serialPort.systemLocation();
			if (serialPort.isBusy()) {
				msg += QCoreApplication::translate("main", "(Busy)");
			}
            stdOut << indents(indent, indentLevel) << msg << endl;
		}

        stdOut << endl << QCoreApplication::translate("main", "CAN bus list:") << endl;
        QStringList canbusPlugins = QCanBus::instance()->plugins();
        for (int i = 0; i < canbusPlugins.size(); i++) {
            QString plugin = canbusPlugins.at(i);
            stdOut << indents(indent, indentLevel) << plugin << endl;
            const QList<QCanBusDeviceInfo> canbusDevs =
                    QCanBus::instance()->availableDevices(plugin);
            if (canbusDevs.empty()) continue;
            indentLevel++;
            for (int j = 0; j < canbusDevs.size(); j++) {
                QCanBusDeviceInfo canbus = canbusDevs.at(j);
                QString msg = canbus.name();
                if (!canbus.description().isEmpty()) msg += ", " + canbus.description();
                if (canbus.hasFlexibleDataRate()) msg += ", CanFD";
                if (canbus.isVirtual()) msg += ", Virtual";
                if (!canbus.serialNumber().isEmpty()) msg += ", SN: " + canbus.serialNumber();
                stdOut << indents(indent, indentLevel) << msg << endl;
            }
            indentLevel--;
        }

		return 1;
	}

	QString serialPort = cliParser.value(serialPortOption);
	if (serialPort.isEmpty()) {
        qWarning() << QCoreApplication::translate("main", "Miss serial port.") << endl;
		cliParser.showHelp(1);
//		return 1;
	}

	int baudRate = cliParser.value(baudRateOption).toInt();

    qDebug() << "Serial port: " << serialPort << ", baud rate: " << baudRate << endl;

	QStringList bindTCP = cliParser.value(bindTCPOption).split(":");
	if (bindTCP.size() > 1) {
		bool ok = true;
		bindTCP[1].toInt(&ok);
		if (!ok) {
            qWarning() << QCoreApplication::translate("main", "Invalid tcp port.") << endl;
			cliParser.showHelp(1);
//			return 1;
		}
	}

	if (bindTCP.size() > 0) {
        qDebug() << "Bind tcp: " << bindTCP[0];

		if (bindTCP.size() > 1) {
            qDebug() << ", port: " << bindTCP[1];
		}
        qDebug() << endl;
	}

	QStringList bindWS = cliParser.value(bindWSOption).split(":");
	if (bindWS.size() > 1) {
		bool ok = true;
		bindWS[1].toInt(&ok);
		if (!ok) {
            qWarning() << QCoreApplication::translate("main", "Invalid websocket port.") << endl;
			cliParser.showHelp(1);
//			return 1;
		}
	}

	if (bindWS.size() > 0) {
        qDebug() << "Bind websocket: " << bindWS[0];

		if (bindWS.size() > 1) {
            qDebug() << ", port: " << bindWS[1];
		}
        qDebug() << endl;
	}

    SerialPortServer serialPortServer(nullptr);
	serialPortServer.openSerialPort(serialPort, baudRate);

	return app.exec();
}
