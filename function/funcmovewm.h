#ifndef FUNCMOVEWM_H
#define FUNCMOVEWM_H

#include <QObject>
#include <QMutex>
#include <QScopedPointer>
#include <QEventLoop>
#include <QTimer>
#include <QElapsedTimer>
#include "function/functcpclient.h"

class FuncMoveWM : public QObject
{
    Q_OBJECT
public:
    enum Axis{
        X = 1,
        Y = 4,
        Z = 3,
        UNDEFINE = 0,
    };
public:
    static FuncMoveWM*  getInstance();
    int openDev(QString comPort,quint16 port=0);
    int moveAbsolute(Axis node,double pos);
    int moveRelative(Axis node,double pos);

signals:
    void sigError(QString msg);
    void sigInfo(QString msg);
    void sigNimServer(QString msg);

private slots:
    void onTimerQuitOut();
    void onServerMsg(QString msg);

private:
    explicit FuncMoveWM(QObject *parent = nullptr);

private:
    int wait4Server(QString keyWord);

private:
    static QMutex                       mutex;
    static QScopedPointer<FuncMoveWM>   instance;
    QEventLoop  loopTcp;
    QTimer      timerQuit;
    QString     m_pServerMsg;

    QElapsedTimer   eTimerServer;


    FuncTcpClient   *tcpMove;



    Q_DISABLE_COPY(FuncMoveWM)
};

#endif // FUNCMOVEWM_H
