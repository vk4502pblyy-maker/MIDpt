#include "viewtitlemsgbox.h"
#include "qevent.h"
#include <qlayout.h>

#pragma execution_character_set(push, "utf-8")
ViewTitleMsgBox::ViewTitleMsgBox(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setMargin(3);

    movement = FunctionMoveSL::getInstance();
    pzt = FuncPZTCtl::getInstance();
    timeAcq = new QTimer;
    zInitFlag = false;
    xyInitFlag = false;
    xCur = 0,yCur = 0,zCur = 0,PZTCur = 0;

    QHBoxLayout *layout1 = new QHBoxLayout;
    mainLayout->addLayout(layout1);
    btnClear = new EleIconBtn(":/icon/resource/icon-line2D/clear.png",64,64,"清空消息栏");
    layout1->addWidget(new QLabel("消息栏"));
    layout1->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
    layout1->addWidget(btnClear);


//    mainLayout->addWidget(new QLabel("消息栏"),0);
    listMsg = new QListWidget;
    listMsg->setWordWrap(true);
    listMsg->setResizeMode(QListWidget::Adjust);
//    listMsg->setFixedHeight(120);
    mainLayout->addWidget(listMsg,1);

//    QGridLayout *layoutBtns = new QGridLayout;
//    mainLayout->addLayout(layoutBtns,0);
//    labelPos = new QLabel("X:0      Y:0     Z:0     PZT:0");
//    btnCamera = new EleIconBtn(":/icon/resource/icon-N/red-cir.png",64,64,"当前断开");
//    btnCamera->setStatusIcon({":/icon/resource/icon-N/red-cir.png",
//                              ":/icon/resource/icon-N/green-cir.png"},
//                             {"当前断开","当前联通"},true,false);
//    btnDMD = new EleIconBtn(":/icon/resource/icon-N/red-cir.png",64,64,"当前断开");
//    btnDMD->setStatusIcon({":/icon/resource/icon-N/red-cir.png",
//                              ":/icon/resource/icon-N/green-cir.png"},
//                             {"当前断开","当前联通"},true,false);
//    btnMove = new EleIconBtn(":/icon/resource/icon-N/red-cir.png",64,64,"当前断开");
//    btnMove->setStatusIcon({":/icon/resource/icon-N/red-cir.png",
//                              ":/icon/resource/icon-N/green-cir.png"},
//                             {"当前断开","当前联通"},true,false);
//    btnPZT = new EleIconBtn(":/icon/resource/icon-N/red-cir.png",64,64,"当前断开");
//    btnPZT->setStatusIcon({":/icon/resource/icon-N/red-cir.png",
//                              ":/icon/resource/icon-N/green-cir.png"},
//                             {"当前断开","当前联通"},true,false);
//    btnConfig = new EleIconBtn(":/icon/resource/icon-N/devConfig.png",64,64,"配置硬件连接");
//    btnMovement = new EleIconBtn(":/icon/resource/icon-N/Basement.png",64,64,"呼出运动控制台");

//    layoutBtns->addWidget(labelPos,0,0,1,6);
//    layoutBtns->addWidget(new QLabel("相机"),1,0,1,1,Qt::AlignCenter);
//    layoutBtns->addWidget(btnCamera,2,0,1,1);
//    layoutBtns->addWidget(new QLabel("DMD"),1,1,1,1,Qt::AlignCenter);
//    layoutBtns->addWidget(btnDMD,2,1,1,1);
//    layoutBtns->addWidget(new QLabel("运动台"),1,2,1,1,Qt::AlignCenter);
//    layoutBtns->addWidget(btnMove,2,2,1,1);
//    layoutBtns->addWidget(new QLabel("PZT"),1,3,1,1,Qt::AlignCenter);
//    layoutBtns->addWidget(btnPZT,2,3,1,1);
//    layoutBtns->addWidget(btnConfig,2,4,1,1);
//    layoutBtns->addWidget(btnMovement,2,5,1,1);

//    connect(timeAcq,&QTimer::timeout,this,&ViewTitleMsgBox::onTimeAcqOut);
//    connect(btnConfig,&EleIconBtn::clicked,this,&ViewTitleMsgBox::onFloatConfig);
//    connect(btnMovement,&EleIconBtn::clicked,this,&ViewTitleMsgBox::onFloatMovement);
}

ViewTitleMsgBox::~ViewTitleMsgBox()
{
//    timeAcq->stop();
}

void ViewTitleMsgBox::writeMsg(QString msg)
{
    if(!msg.contains("PZT POS"))
        listMsg->addItem(msg);
//    listMsg->setCurrentRow(listMsg->count()-1);
//    if(msg == "Move LeiSai Init Success"){
//        xyInitFlag = true;
//        if(!timeAcq->isActive())
//            timeAcq->start(1000);
//    }
//    if(msg.contains("PZT INIT SUCCESS PORT")){
//        zInitFlag = true;
//        if(!timeAcq->isActive())
//            timeAcq->start(1000);
//    }
}

void ViewTitleMsgBox::setDisplay(QString msg)
{
//    if(msg.contains("SET POS DISPLAY PZT")){
//        QString pos = "X:"+QString::number(xCur)+"\t"+
//                "Y:"+QString::number(yCur)+"\t"+
//                "Z:"+QString::number(zCur)+"\t"+
//                "PZT:"+msg.split(":").last();
//        labelPos->setText(pos);
//    }
//    else if(msg.contains("SET DISPLAY CONNECTION")){
//        btnCamera->changeStatus(msg.split(":").last().toInt());
//        btnMove->changeStatus(msg.split(":").last().toInt());
//        btnDMD->changeStatus(msg.split(":").last().toInt());
//        btnPZT->changeStatus(msg.split(":").last().toInt());
//    }
}

void ViewTitleMsgBox::cmdFrMainWindow(QString cmd)
{
//    QStringList cmds = cmd.split("_");
//    cmds.removeFirst();
//    if(cmds.first() == "Set"){
//        QStringList cmdl = cmds.at(1).split(":");
//        if(cmdl.first() == "DMDConnection"){
//            btnDMD->changeStatus(cmdl.at(1).toInt());
//        }
//    }
}

void ViewTitleMsgBox::onTimeAcqOut()
{
//    double x = 0,y = 0;
//    double z = 0;
//    if(xyInitFlag){
//        movement->getPos(FunctionMoveSL::X,x);
//        movement->getPos(FunctionMoveSL::Y,y);
//    }
//    if(zInitFlag)
//        z = pzt->getPos(0);
//    labelPos->setText("X:"+QString::number(x,'f',3)+
//                      ";\nY:"+QString::number(y,'f',3)+
//                      ";\nZ:"+QString::number(z,'f',3));
}

void ViewTitleMsgBox::onBtnShowAll()
{
//    emit sigFloatMovement();
}

void ViewTitleMsgBox::onFloatMovement()
{
//    emit sigFloatMovement();
}

void ViewTitleMsgBox::onFloatConfig()
{
//    emit sigFloatConfig();
}
#pragma execution_character_set(pop)
