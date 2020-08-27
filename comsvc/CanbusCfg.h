#ifndef CANBUSCFG_H
#define CANBUSCFG_H

#include <QObject>
#include <QJsonObject>

class CanbusCfg: public QObject
{
    Q_OBJECT
public:
    QJsonObject json;
    static CanbusCfg& instance();

protected:
    CanbusCfg();
};

#endif // CANBUSCFG_H
