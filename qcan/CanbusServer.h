#ifndef _H_QCAN_CANBUSSERVER
#define _H_QCAN_CANBUSSERVER

#include <QCanBus>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class CanbusServer: public QObject
{
	Q_OBJECT

public:
    explicit CanbusServer(QObject *parent = nullptr);
    virtual ~CanbusServer();

    bool openCanbusPort(const QString &portName, int baudRate);
};

#endif /* _H_QCAN_CANBUSSERVER */
