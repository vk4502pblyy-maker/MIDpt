#include "funcmovewm.h"
#include <QCoreApplication>

#pragma execution_character_set(push, "utf-8")
//QMutex FuncMoveWM::mutex;
//QScopedPointer<FuncMoveWM>  FuncMoveWM::instance;
FuncMoveWM *FuncMoveWM::getInstance()
{
//    if (instance.isNull()) {
//        QMutexLocker locker(&mutex);
//        if (instance.isNull()) {
//            instance.reset(new FuncMoveWM);
//        }
//    }
//    return instance.data();
    static FuncMoveWM instance;

    return &instance;
}

int FuncMoveWM::openDev(QString comPort, quint16 port)
{
    if(port == 0){
        port = 7890;
    }
    tcpMove->connectToServer("localhost",port);
    if(wait4Server("CONNECT Nim Servo")){
        return -1;
    }
//    tcpMove->sendMessage("InitDev_"+comPort);
//    m_ComPort = comPort;
    return 0;
}

int FuncMoveWM::moveAbsolute(Axis node, double pos)
{
    QString msg = "AbsoluteMove_"+QString::number(node)+","+QString::number(pos);
    tcpMove->sendMessage(msg);
    return 0;
}

int FuncMoveWM::moveRelative(Axis node, double pos)
{
    QString msg = "RelativeMove_"+QString::number(node)+","+QString::number(pos);
    tcpMove->sendMessage(msg);
    return 0;
}

int FuncMoveWM::reconnect()
{
    return 0;
}

void FuncMoveWM::onTimerQuitOut()
{
    loopTcp.exit(-1);
}

void FuncMoveWM::onServerMsg(QString msg)
{
    m_pServerMsg = msg;
    emit sigNimServer(m_pServerMsg);
}

FuncMoveWM::FuncMoveWM(QObject *parent)
    : QObject{parent}
{
    timerQuit.setSingleShot(true);
    timerQuit.setInterval(10000);
    tcpMove = new FuncTcpClient(this);
    connect(&timerQuit,&QTimer::timeout,this,&FuncMoveWM::onTimerQuitOut);
    connect(tcpMove,&FuncTcpClient::messageReceived,this,&FuncMoveWM::onServerMsg);
}

int FuncMoveWM::wait4Server(QString keyWord)
{
    eTimerServer.restart();
    while(eTimerServer.elapsed()<20000){
        if(m_pServerMsg.contains(keyWord)){
            return 0;
        }
        QCoreApplication::processEvents();
    }
    return -1;
}
#pragma execution_character_set(pop)
