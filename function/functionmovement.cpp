#include "functionmovement.h"
#include <QDebug>

QMutex FunctionMovement::mutex;
QScopedPointer<FunctionMovement> FunctionMovement::instance;

FunctionMovement *FunctionMovement::getInstance()
{
    if (instance.isNull()) {
        QMutexLocker locker(&mutex);
        if (instance.isNull()) {
            instance.reset(new FunctionMovement);
        }
    }
    return instance.data();
}

int FunctionMovement::absoluteMoveSingle(int axis, double point, QString unit)
{
    int ret = -1;
    if(unit == "um"){
        point = double(point/1000.0);
    }
    else if(unit != "mm")
    {
        emit sigError("Wrong move unit: "+unit);
        return -1;
    }
    moveLimit limit = limits.value(axis);
    if((point>limit.max) || (point<limit.min)){
        emit sigError(QString("out of limit_Axis:%1_Point:%2").arg(axis).arg(point));
        return -1;
    }
    ret = acs.moveAxisAbsolute(axis,point);
    return ret;
}

int FunctionMovement::relativeMoveSingle(int axis, double point, QString unit)
{
    int ret = -1;
    if(unit == "um"){
        point = double(point/1000.0);
    }
    else if(unit != "mm")
    {
        emit sigError("Wrong move unit: "+unit);
        return -1;
    }
    moveLimit limit = limits.value(axis);
    double value = getPosAxis(limit.axis)+point;
    if((value>limit.max) || (value<limit.min)){
        emit sigError(QString("out of limit_Axis:%1_Point:%2").arg(axis).arg(point));
        return -1;
    }
    ret = acs.moveAxisRelative(axis,point);
    return ret;
}

QVector<double> FunctionMovement::getPosAll()
{
    acs.getPosAxis(0,pos[1]);
    acs.getPosAxis(1,pos[0]);
    acs.getPosAxis(4,pos[2]);
    return pos;
}

double FunctionMovement::getPosAxis(int axis)
{
    double pos;
    acs.getPosAxis(axis,pos);
    return pos;
}

void FunctionMovement::startPosAcq()
{
    posTimer.start(500);
    connect(&posTimer,&QTimer::timeout,this,&FunctionMovement::onPosTimeOut);
}

void FunctionMovement::stopPosAcq()
{
    disconnect(&posTimer,&QTimer::timeout,this,&FunctionMovement::onPosTimeOut);
    posTimer.stop();
}

void FunctionMovement::onPosTimeOut()
{
    getPosAll();
    emit sigPosAll(pos);
}

FunctionMovement::FunctionMovement(QObject *parent)
    : QObject{parent}
{
    initAcs();
    qDebug()<<"FunctionMovement created";
}

void FunctionMovement::destroyInstance()
{
    QMutexLocker locker(&mutex);
    instance.reset();
}

void FunctionMovement::initAcs()
{
    QString ip = "10.0.0.100";
    int port = 701;
    QJsonArray axis = {0,1,2,4};
    QJsonObject para;
    para["ip"] = ip;
    para["port"] = port;
    para["axis"] = axis;
    acs.setPara(para);
    acs.connectDev();
    pos = {0,0,0};

    moveLimit limitX,limitY,limitZ;

    limitX.axis = X;
    limitX.min = 450;
    limitX.max = 650;

    limitY.axis = Y;
    limitY.min = 5;
    limitY.max = 250;

    limitZ.axis = Z;
    limitZ.min = 60;
    limitZ.max = 79;

    limits.insert(X,limitX);
    limits.insert(Y,limitY);
    limits.insert(Z,limitZ);
}
