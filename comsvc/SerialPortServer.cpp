/*
 * SerialPortServer.h
 *
 *  Created on: Aug 13, 2019
 *      Author: joelai
 */
#include <QCoreApplication>
#include <QByteArray>
#include <QtDebug>
#include <QStringList>
#include <QSerialPort>
#include <QIODevice>

#include "SerialPortServer.h"

static QString portNameFromSystemLocation(const QString &source) {
    return source.startsWith(QLatin1String("/dev/")) ? source.mid(5) :
            (source.startsWith(QLatin1String("\\\\.\\")) ||
            source.startsWith(QLatin1String("//./"))) ? source.mid(4) :
            source;
}

SerialPortServer::SerialPortServer(QObject *parent) : QObject(parent),
        serialPort(new QSerialPort(this)) {
    connect(serialPort, &QSerialPort::errorOccurred,
            this, &SerialPortServer::serialPortError);
    connect(serialPort, &QSerialPort::readyRead, this,
            &SerialPortServer::serialPortReadyRead);
}

SerialPortServer::~SerialPortServer() {

}

bool SerialPortServer::openSerialPort(const QString &portName, int baudRate) {
	if (serialPort->isOpen()) serialPort->close();

    serialPort->setPortName(portNameFromSystemLocation(portName));
    serialPort->setBaudRate(baudRate);
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);
	if (!serialPort->open(QIODevice::ReadWrite)) {
		qWarning() << serialPort->errorString() << endl;
		return false;
	}
	return true;
}

void SerialPortServer::serialPortError(QSerialPort::SerialPortError err) {
	if (err == QSerialPort::ResourceError) {
		qWarning() << QCoreApplication::translate("main", "Serial port unavailable") << endl;
	}
}

void SerialPortServer::serialPortReadyRead() {
	const QByteArray data = serialPort->readAll();
	qDebug() << data;
}
