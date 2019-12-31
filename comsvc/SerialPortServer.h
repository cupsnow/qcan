/*
 * SerialPortServer.h
 *
 *  Created on: Aug 13, 2019
 *      Author: joelai
 */

#ifndef _H_MIGRU_COMSVC_SERIALPORTSERVER
#define _H_MIGRU_COMSVC_SERIALPORTSERVER

#include <QSerialPort>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class SerialPortServer: public QObject
{
	Q_OBJECT

public:
    explicit SerialPortServer(QObject *parent = nullptr);
    virtual ~SerialPortServer();

    bool openSerialPort(const QString &portName, int baudRate);

protected slots:
	void serialPortError(QSerialPort::SerialPortError err);

	void serialPortReadyRead();

protected:
    QSerialPort *serialPort;

};

#endif /* _H_MIGRU_COMSVC_SERIALPORTSERVER */
