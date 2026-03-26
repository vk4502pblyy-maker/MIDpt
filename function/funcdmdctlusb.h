#ifndef FUNCDMDCTLUSB_H
#define FUNCDMDCTLUSB_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QScopedPointer>
#include <QEventLoop>
#include "hidapi.h"

class FuncDmdCtlUsb : public QObject
{
    Q_OBJECT
public:
    static FuncDmdCtlUsb*  getInstance();

    int initDMD();

signals:

private:
    explicit FuncDmdCtlUsb(QObject *parent = nullptr);
    static QMutex                           mutex;
    static QScopedPointer<FuncDmdCtlUsb>    instance;

private:


    Q_DISABLE_COPY(FuncDmdCtlUsb)
};

#endif // FUNCDMDCTLUSB_H
