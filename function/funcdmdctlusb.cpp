#include "funcdmdctlusb.h"
#include <QDebug>

QMutex FuncDmdCtlUsb::mutex;
QScopedPointer<FuncDmdCtlUsb> FuncDmdCtlUsb::instance;

FuncDmdCtlUsb *FuncDmdCtlUsb::getInstance()
{
    if (instance.isNull()) {
        QMutexLocker locker(&mutex);
        if (instance.isNull()) {
            instance.reset(new FuncDmdCtlUsb);
        }
    }
    return instance.data();
}

int FuncDmdCtlUsb::initDMD()
{
    int res = 0;
    res = hid_init();
    if(res){

    }
    return 0;
}

FuncDmdCtlUsb::FuncDmdCtlUsb(QObject *parent)
    : QObject{parent}
{

}
