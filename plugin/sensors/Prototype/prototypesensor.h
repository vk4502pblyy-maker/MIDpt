#ifndef PROTOTYPESENSOR_H
#define PROTOTYPESENSOR_H

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QDebug>
#include "Sensors_global.h"

class SENSORS_EXPORT PrototypeSensor : public QObject
{
    Q_OBJECT
public:
    enum status
    {
        DEV_UNINIT = 0,
        DEV_IDLE = 1,
        DEV_RUNNING = 2,
        DEV_BLOCKED = 3,
        DEV_CLOSED = 4,
        DEV_ERROR = -1,
        DEV_OFFLINE = -2
    };

public:
    explicit PrototypeSensor(QObject *parent = nullptr);
    virtual int init(QJsonObject param) = 0;
    virtual int open() = 0;
    virtual int close() = 0;
    virtual int startAcq() = 0;
    virtual int stopAcq() = 0;
    virtual int block() = 0;
    virtual int unblock() = 0;
    virtual int reset() = 0;
    virtual int getLastData(void* data) = 0;
    virtual int shutDown() = 0;
    virtual int restart() = 0;
    virtual int startRadio() = 0;
    virtual int stopRadio() = 0;
    virtual int getKeys(QStringList &keys);
    virtual int getStatusCur();
    virtual int funcStart(QString func);
    virtual int funcOver(QString func);
    virtual int audienceConform(QString token,QString &newToken);


protected:
    virtual int     test() = 0;
    virtual QString generateToken();

signals:
    void sigData(QJsonObject ret);

protected:
    QString     m_pSensorNbr;
    int         m_pDataLength;
    QThread     threadDataAcq;
    QJsonObject objData;
    QStringList m_pKeys;
    QString     m_pIp;
    int         m_pPort;
    status      m_pStatusCur;
    QStringList m_pTokenList;
    QString     m_pBlockToken;

};

inline int PrototypeSensor::getKeys(QStringList &keys)
{
    keys = m_pKeys;
    return 0;
}

inline int PrototypeSensor::funcStart(QString func)
{
    qDebug()<<func<<" start";
    return 0;
}

inline int PrototypeSensor::funcOver(QString func)
{
    qDebug()<<func<<" over";
    return 0;
}

#endif // PROTOTYPESENSOR_H
