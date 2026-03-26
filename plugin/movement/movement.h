#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Movement_global.h"
#include <QObject>
#include <QJsonObject>

class MOVEMENT_EXPORT Movement : public QObject
{
    Q_OBJECT
public:
    enum StatusDev
    {
        DEV_UNCONNECTED = 0,
        DEV_IDLE = 1,
        DEV_RUNNING = 2,
        DEV_ERROR = -1
    };
    enum StatueAxis
    {
        AXIS_DISABLE = 0,
        AXIS_IDLE = 1,
        AXIS_ERROR = -1
    };
public:
    Movement(QObject *parent = nullptr);
    virtual int setPara(const QJsonObject &param) = 0;
    virtual int connectDev() = 0;
    virtual int disconnectDev() = 0;
    virtual int setVelocities(QJsonObject velocities) = 0;
    virtual int moveAxisAbsolute(int axis,double point) = 0;
    virtual int moveAxisRelative(int axis,double step) = 0;
    virtual int getPosAxis(int axis,double &pos) = 0;
    virtual int getPosAll(QJsonObject &pos) = 0;
    virtual int resetDev() = 0;

protected:
    StatusDev m_pStatusDev;
};

#endif // MOVEMENT_H
