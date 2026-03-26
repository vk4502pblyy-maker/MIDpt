#ifndef ZSENSORMIEP_H
#define ZSENSORMIEP_H

#include "Prototype/prototypesensor.h"
#include "ZSensor/MEDAQLib.h"
#include <QTimer>

class SENSORS_EXPORT ZSensorMiEp : public PrototypeSensor
{
    Q_OBJECT
public:
    ZSensorMiEp();
    virtual int init(QJsonObject param) override;
    virtual int open() override;
    virtual int close() override;
    virtual int startAcq() override;
    virtual int stopAcq() override;
    virtual int block() override;
    virtual int unblock() override;
    virtual int reset() override;
    virtual int getLastData(void* data) override;
    virtual int shutDown() override;
    virtual int restart() override;
    virtual int startRadio() override;
    virtual int stopRadio() override;


private slots:
    void onTimeOut();

protected:
    virtual int test() override;

private:
    int checkRes(QString funcName);
    int sample();

private:
    DWORD       sensor;
    QString     m_pIp;
    ME_SENSOR   m_pSensorType;
    int         m_pRes;
    int         m_pInterval;
    QTimer      timer;
    double      m_pZLast;
    QJsonObject m_pInitParam;
};

#endif // ZSENSORMIEP_H
