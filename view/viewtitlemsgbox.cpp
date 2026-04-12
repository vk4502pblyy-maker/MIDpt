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

    listMsg = new QListWidget;
    listMsg->setWordWrap(true);
    listMsg->setResizeMode(QListWidget::Adjust);
    mainLayout->addWidget(listMsg,1);




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

void ViewTitleMsgBox::onDevInfo(QString msg)
{
    listMsg->addItem(msg);
}
#pragma execution_character_set(pop)
