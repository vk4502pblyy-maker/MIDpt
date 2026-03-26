#ifndef MOVEMENTACS_H
#define MOVEMENTACS_H

#include "movement.h"
#include "ACSC.h"
#include <QJsonArray>

class MOVEMENT_EXPORT MovementACS :public Movement
{
    Q_OBJECT
public:
    MovementACS();
    // QString ip,int port,QJsonArray axises
    virtual int setPara(const QJsonObject &param) override;
    virtual int connectDev() override;
    virtual int disconnectDev() override;
    virtual int setVelocities(QJsonObject velocities) override;
    virtual int moveAxisAbsolute(int axis,double point) override;
    virtual int moveAxisRelative(int axis,double step) override;
    virtual int getPosAxis(int axis,double &pos) override;
    virtual int getPosAll(QJsonObject &pos) override;
    virtual int resetDev() override;

public:
    int axisEnable(int axis);
    int axisDisable(int axis);

private:
    int checkRes();

private:
    HANDLE      hComm;
    QString     ip;
    int         port;
    int         m_pRes;
    QJsonObject m_pVelocities;
    QJsonArray  m_pAxisAll;
};

#endif // MOVEMENTACS_H
