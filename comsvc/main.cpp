#include <QCoreApplication>
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

static QTextStream stdOut(stdout);

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("comsvc");
	QCoreApplication::setApplicationVersion("1.0");

	QCommandLineParser cliParser;
	cliParser.setApplicationDescription(QCoreApplication::translate("main",
			"Service between serial port."));
	const QCommandLineOption helpOption = cliParser.addHelpOption();
	const QCommandLineOption versionOption = cliParser.addVersionOption();

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

	if (cliParser.isSet(listSerialPortOption)) {
		const QList<QSerialPortInfo> serialPorts =
				QSerialPortInfo::availablePorts();
		for (int i = 0; i < serialPorts.size(); i++) {
			QSerialPortInfo serialPort = serialPorts.at(i);

			QString msg = serialPort.systemLocation();
			if (serialPort.isBusy()) {
				msg += QCoreApplication::translate("main", "(Busy)");
			}
            stdOut << msg << endl;
		}

		QStringList canbusIfceNames = QCanBus::instance()->plugins();
		QString indent1 = "";
		for (int i = 0; i < canbusIfceNames.size(); i++) {
			QString ifceName = canbusIfceNames.at(i);
            stdOut << indent1 << ifceName << endl;



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
