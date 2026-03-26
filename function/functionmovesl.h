#ifndef FUNCTIONMOVESL_H
#define FUNCTIONMOVESL_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QScopedPointer>
#include <QJsonObject>
#include "plugin/movementSL/LTSMC.h"

class FunctionMoveSL : public QObject
{
    Q_OBJECT
public:
    enum AXIS{
        X = 0,
        Y = 1
    };
    enum STATUS{
        OFFLINE = 0,
        IDLE = 1,
        RUNNING = 2,
        DEVERR = -1
    };
public:
    static FunctionMoveSL* getInstance();
    int connectDev(QString ip);//"192.168.5.11"
    int disconnectDev();
    int reset(bool isCold);
    QJsonObject getStatus();
    int moveAbsolute(AXIS axis,double posmm);
    int moveRelative(AXIS axis,double posmm);
    int getPos(AXIS axis,double &posmm);

signals:
    void sigInfo(QString msg);
    void sigError(QString msg);

private:
    explicit FunctionMoveSL(QObject *parent = nullptr);
    int initDev(char *ip);

private:
    static QMutex mutex;
    static QScopedPointer<FunctionMoveSL> instance;

    STATUS status;


Q_DISABLE_COPY(FunctionMoveSL)
};

#endif // FUNCTIONMOVESL_H
