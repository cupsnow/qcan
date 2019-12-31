#include <QCoreApplication>
#include <QByteArray>
#include <QtDebug>
#include <QStringList>
//#include <QSerialPort>
#include <QIODevice>

#include "CanbusServer.h"

static QString portNameFromSystemLocation(const QString &source) {
    return source.startsWith(QLatin1String("/dev/")) ? source.mid(5) :
            (source.startsWith(QLatin1String("\\\\.\\")) ||
            source.startsWith(QLatin1String("//./"))) ? source.mid(4) :
            source;
}

CanbusServer::CanbusServer(QObject *parent) : QObject(parent) {

}

CanbusServer::~CanbusServer() {

}

bool CanbusServer::openCanbusPort(const QString &portName, int baudRate) {
    return false;
}
