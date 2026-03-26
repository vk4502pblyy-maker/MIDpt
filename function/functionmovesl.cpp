#include "functionmovesl.h"
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDebug>

QMutex FunctionMoveSL::mutex;
QScopedPointer<FunctionMoveSL> FunctionMoveSL::instance;

FunctionMoveSL *FunctionMoveSL::getInstance()
{
    if (instance.isNull()) {
        QMutexLocker locker(&mutex);
        if (instance.isNull()) {
            instance.reset(new FunctionMoveSL);
        }
    }
    return instance.data();
}

int FunctionMoveSL::connectDev(QString ip)
{
    if((status == OFFLINE) || (status == DEVERR)){
        initDev(ip.toUtf8().data());
        QElapsedTimer timer;
        timer.start();
        while(timer.elapsed()<100)
            QCoreApplication::processEvents();
        int status = smc_get_connect_status(0);
        if(status == 1){
            qDebug()<<"FunctionMoveSL::connectDev success";
            emit sigInfo("Move LeiSai Init Success");
        }
        else{
            qDebug()<<"FunctionMoveSL::connectDev fail";
            emit sigError("Move LeiSai Init Fail");
        }
    }
    else{
        emit sigInfo("Move LeiSai Has Already Init");
    }

    return 0;
}

int FunctionMoveSL::disconnectDev()
{
    smc_board_close(0);
    return 0;
}

int FunctionMoveSL::reset(bool isCold)
{
    if(isCold){
        smc_board_reset(0);
    }
    else{
        smc_soft_reset(0);
    }
    return 0;
}

QJsonObject FunctionMoveSL::getStatus()
{
    QJsonObject status;
    DWORD para;
    int ret = 0;
    ret = smc_get_total_axes(0,&para);
    if(!ret){
        status["total_axis"] = int(para);
    }
    else{
        qDebug()<<"smc_get_total_axes error:"<<QString::number(ret);
    }
    ret = smc_get_card_version(0,&para);
    if(!ret){
        status["card_version"] = int(para);
    }
    else{
        qDebug()<<"smc_get_card_version error:"<<QString::number(ret);
    }
    return status;
}

int FunctionMoveSL::moveAbsolute(AXIS axis, double posmm)
{
    int res = smc_pmove_unit(0,axis,posmm,1);
    return res;
}

int FunctionMoveSL::moveRelative(AXIS axis, double posmm)
{
    int res = smc_pmove_unit(0,axis,posmm,0);
    return res;
}

int FunctionMoveSL::getPos(AXIS axis, double &posmm)
{
    int res = smc_get_position_unit(0,axis,&posmm);
    return res;
}

FunctionMoveSL::FunctionMoveSL(QObject *parent)
    : QObject{parent}
{
    status = OFFLINE;
}

int FunctionMoveSL::initDev(char *ip)
{
    int rec = smc_board_init(0,2,ip,0);
    return rec;
}
