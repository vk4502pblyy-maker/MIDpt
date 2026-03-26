#ifndef ZSENSORKEYECOLOR_H
#define ZSENSORKEYECOLOR_H

#include "Prototype/prototypesensor.h"
#include <QTcpSocket>
#include <QTimer>
#include <QEventLoop>

class SENSORS_EXPORT ZSensorKeyeColor :public PrototypeSensor
{
    Q_OBJECT
public:
    ZSensorKeyeColor();
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

protected:
    virtual int test() override;

private slots:
    void onTcpConnected();
    void onTcpDisconnected();
    void onTimeOut();
    void onSampleOver();
    void onLoopTimeOut();

private:
    int sample();

private:
    QString     m_pIp;
    int         m_pPort;
    QTcpSocket  socket;
    QTimer      timer;
    int         m_pInterval;
    QJsonObject m_pPara;
    double      m_pLastData;
    QEventLoop  sampleLoop;
    QTimer      loopTimer;

};

#endif // ZSENSORKEYECOLOR_H
