#include "viewfloatmovement.h"
#include <QLayout>
#include <QCoreApplication>
#include <QMessageBox>

#pragma execution_character_set(push, "utf-8")
ViewFloatMovement::ViewFloatMovement(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(
        Qt::WindowStaysOnTopHint    // 保持在顶层（浮动特性）
        | Qt::Window                // 标准窗口（带标题栏和边框）
    );
    setWindowTitle("运动控制台"); // 标题栏文本
    this->resize(250,500);

    pzt = FuncPZTCtl::getInstance();
    moveWM = FuncMoveWM::getInstance();
    pztEnable = false;
    moveEnable = false;
    m_pStyle = "line2D";

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QHBoxLayout *layoutPos = new QHBoxLayout;
    layoutPos->addWidget(new QLabel("当前位置  "));
    labelPos = new QLabel("X:--;Y:--;Z:--;");
    labelPZT = new QLabel("PZT:--;");
    layoutPos->addWidget(labelPos);
    layoutPos->addWidget(labelPZT);
    mainLayout->addLayout(layoutPos,0);

    QGridLayout *layoutXYZ = new QGridLayout;
    QString iconLocalPath = ":/icon/resource/";
    if(m_pStyle == ""){
        iconLocalPath += "icon-N/";
    }
    else if(m_pStyle.contains("gbl")){
        iconLocalPath += "icon-gbl/";
    }
    else if(m_pStyle.contains("line2D")){
        iconLocalPath += "icon-line2D/";
    }
    btnPZTD = new EleIconBtn(iconLocalPath + "down-arr.png",64,64,"微动台下降");
    btnPZTU = new EleIconBtn(iconLocalPath + "up-arr.png",64,64,"微动台上升");
    btnXB   = new EleIconBtn(iconLocalPath + "left-arr.png",64,64,"X负向运动");
    btnXF   = new EleIconBtn(iconLocalPath + "right-arr.png",64,64,"X正向运动");
    btnYB   = new EleIconBtn(iconLocalPath + "down-arr.png",64,64,"Y负向运动");
    btnYF   = new EleIconBtn(iconLocalPath + "up-arr.png",64,64,"Y正向运动");
    btnZD   = new EleIconBtn(iconLocalPath + "downdown-3.png",64,64,"Z下降");
    btnZU   = new EleIconBtn(iconLocalPath + "upup-3.png",64,64,"Z上升");
    btnMoveX    = new EleIconBtn(iconLocalPath + "play.png",64,64,"X点动");
    btnMoveY    = new EleIconBtn(iconLocalPath + "play.png",64,64,"Y点动");
    btnMoveZ    = new EleIconBtn(iconLocalPath + "play.png",64,64,"Z点动");
    btnMovePZT  = new EleIconBtn(iconLocalPath + "play.png",64,64,"PZT点动");

    boxX = new QDoubleSpinBox;
    boxX->setRange(-10000000,10000000);
    boxX->setValue(0);
    boxX->setDecimals(3);

    boxStepX = new QDoubleSpinBox;
    boxStepX->setRange(0,10000000);
    boxStepX->setValue(0);
    boxStepX->setDecimals(3);

    boxY = new QDoubleSpinBox;
    boxY->setRange(-10000000,10000000);
    boxY->setValue(0);
    boxY->setDecimals(3);

    boxStepY = new QDoubleSpinBox;
    boxStepY->setRange(0,10000000);
    boxStepY->setValue(0);
    boxStepY->setDecimals(3);

    boxZ = new QDoubleSpinBox;
    boxZ->setRange(-10000000,10000000);
    boxZ->setValue(0);
    boxZ->setDecimals(3);

    boxStepZ = new QDoubleSpinBox;
    boxStepZ->setRange(0,10000000);
    boxStepZ->setValue(0);
    boxStepZ->setDecimals(3);

    boxPZT = new QDoubleSpinBox;
    boxPZT->setRange(0,185);
    boxPZT->setValue(92.5);
    boxPZT->setDecimals(3);

    boxStepPZT = new QDoubleSpinBox;
    boxStepPZT->setRange(-25,25);
    boxStepPZT->setValue(0);
    boxStepPZT->setDecimals(3);

    layoutXYZ->addWidget(btnYF,0,1,1,1);
    layoutXYZ->addWidget(btnXB,1,0,1,1);
    layoutXYZ->addWidget(new QLabel("XY"),1,1,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnXF,1,2,1,1);
    layoutXYZ->addWidget(btnYB,2,1,1,1);
    layoutXYZ->addWidget(btnZU,0,3,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(new QLabel("Z"),1,3,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnZD,2,3,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnPZTU,0,4,1,1);
    layoutXYZ->addWidget(new QLabel("PZT"),1,4,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnPZTD,2,4,1,1);
    layoutXYZ->addWidget(new QLabel("轴"),3,0,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(new QLabel("运动"),3,1,1,1);
    layoutXYZ->addWidget(new QLabel("点动"),3,2,1,1);
    layoutXYZ->addWidget(new QLabel("步进"),3,3,1,1);
    layoutXYZ->addWidget(new QLabel("X"),4,0,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnMoveX,4,1,1,1);
    layoutXYZ->addWidget(boxX,4,2,1,1);
    layoutXYZ->addWidget(boxStepX,4,3,1,1);

    layoutXYZ->addWidget(new QLabel("Y"),5,0,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnMoveY,5,1,1,1);
    layoutXYZ->addWidget(boxY,5,2,1,1);
    layoutXYZ->addWidget(boxStepY,5,3,1,1);

    layoutXYZ->addWidget(new QLabel("Z"),6,0,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnMoveZ,6,1,1,1);
    layoutXYZ->addWidget(boxZ,6,2,1,1);
    layoutXYZ->addWidget(boxStepZ,6,3,1,1);

    layoutXYZ->addWidget(new QLabel("PZT"),7,0,1,1,Qt::AlignCenter);
    layoutXYZ->addWidget(btnMovePZT,7,1,1,1);
    layoutXYZ->addWidget(boxPZT,7,2,1,1);
    layoutXYZ->addWidget(boxStepPZT,7,3,1,1);

    mainLayout->addLayout(layoutXYZ,10);

    connect(pzt,&FuncPZTCtl::sigInfo,
            this,&ViewFloatMovement::onDevInfo);
    connect(btnMoveX,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnMove);
    connect(btnMoveY,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnMove);
    connect(btnMoveZ,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnMove);
    connect(btnXB,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnDec);
    connect(btnYB,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnDec);
    connect(btnZD,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnDec);
    connect(btnXF,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnInc);
    connect(btnYF,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnInc);
    connect(btnZU,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnInc);
    connect(btnPZTD,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnPZTD);
    connect(btnPZTU,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnPZTU);
    connect(btnMovePZT,&EleIconBtn::clicked,
            this,&ViewFloatMovement::onBtnPZTMove);
    connect(pzt,&FuncPZTCtl::sigPos,
            this,&ViewFloatMovement::setPZTPos);
}

void ViewFloatMovement::setPosMsg(QString type, QString posMsg)
{
    if(type == "WM"){
        if(posMsg.split("_").count()!=2)
            return ;
        QStringList posList = posMsg.split("_").last().split(",");
        switch (posList.count()) {
        case 2:
            labelPos->setText(QString("X:--;Y:--;Z:%1;").arg(posList.at(1)));
            break;
        case 4:
            labelPos->setText(QString("X:%1;Y:%2;Z:%1;")
                                 .arg(posList.at(1)).arg(posList.at(3)));
            break;
        case 6:
            labelPos->setText(QString("X:--;Y:--Z:%1;")
                                 .arg(posList.at(1)).arg(posList.at(3)).arg(posList.at(5)));
            break;
        default:
            break;
        }
    }
    return;
}

void ViewFloatMovement::setPZTPos(double pos)
{
    labelPZT->setText("PZT:"+QString::number(pos,'f',3)+";");
    boxPZT->setValue(pos);
}

void ViewFloatMovement::setMoveEnable(QString nodes)
{
    moveEnable = true;
}

void ViewFloatMovement::setPZTEnable()
{
    pztEnable = true;
}

void ViewFloatMovement::closeEvent(QCloseEvent *event)
{
    // 阻止默认关闭行为（默认会销毁窗口）
    event->ignore();
    // 改为隐藏窗口
    hide();
    // 可选：发送信号通知主窗口
}

void ViewFloatMovement::onTimerAcqTimeout()
{
    if(pztEnable)
        pzt->getPos(0);
}

void ViewFloatMovement::onPztSliderChanged()
{
}

void ViewFloatMovement::onPztBoxChanged()
{
}

void ViewFloatMovement::onDevInfo(QString msg)
{
    if(msg.contains("PZT INIT SUCCESS")){
        pztEnable = true;
        boxPZT->setEnabled(true);
        btnPZTD->setEnabled(true);
        btnPZTU->setEnabled(true);
    }
}

void ViewFloatMovement::onBtnPZTU()
{
    if(!pztEnable){
        return;
    }
    double pos = pzt->getPos(0);
    boxPZT->setValue(pos+boxStepPZT->value());
    pzt->setPos(0,pos+boxStepPZT->value());
    setStepBtnsEnabled(false);
    etimerStepBtns.restart();
    while(etimerStepBtns.elapsed()<50){
        QCoreApplication::processEvents();
    }
    setStepBtnsEnabled(true);
}

void ViewFloatMovement::onBtnPZTD()
{
    if(!pztEnable){
        return;
    }
    double pos = pzt->getPos(0);
    boxPZT->setValue(pos-boxStepPZT->value());
    pzt->setPos(0,pos-boxStepPZT->value());
    setStepBtnsEnabled(false);
    etimerStepBtns.restart();
    while(etimerStepBtns.elapsed()<50){
        QCoreApplication::processEvents();
    }
    setStepBtnsEnabled(true);
}

void ViewFloatMovement::onBtnPZTMove()
{
    if(!pztEnable){
        return;
    }
    pzt->setPos(0,boxPZT->value());
}

void ViewFloatMovement::onBtnInc()
{
    if(!moveEnable){
        QMessageBox::warning(this,"尚未初始化运动控制","尚未初始化运动控制");
        return;
    }
    FuncMoveWM::Axis axis = FuncMoveWM::UNDEFINE;
    double pos = 0;
    EleIconBtn *btn = qobject_cast<EleIconBtn*>(sender());
    if (btn) {
        if(btn == btnXF){
            axis = FuncMoveWM::X;
            pos = boxStepX->value();
        }
        else if(btn == btnYF){
            axis = FuncMoveWM::Y;
            pos = boxStepY->value();
        }
        else if(btn == btnZU){
            axis = FuncMoveWM::Z;
            pos = boxStepZ->value();
        }
        moveWM->moveRelative(axis,pos);
    }
    else {
        return;
    }
}

void ViewFloatMovement::onBtnDec()
{
    if(!moveEnable){
        QMessageBox::warning(this,"尚未初始化运动控制","尚未初始化运动控制");
        return;
    }
    FuncMoveWM::Axis axis = FuncMoveWM::UNDEFINE;
    double pos = 0;
    EleIconBtn *btn = qobject_cast<EleIconBtn*>(sender());
    if (btn) {
        if(btn == btnXB){
            axis = FuncMoveWM::X;
            pos = -1*boxStepX->value();
        }
        else if(btn == btnYB){
            axis = FuncMoveWM::Y;
            pos = -1*boxStepY->value();
        }
        else if(btn == btnZD){
            axis = FuncMoveWM::Z;
            pos = -1*boxStepZ->value();
        }
        moveWM->moveRelative(axis,pos);
    }
    else {
        return;
    }
}

void ViewFloatMovement::onBtnMove()
{
    if(!moveEnable){
        QMessageBox::warning(this,"尚未初始化运动控制","尚未初始化运动控制");
        return;
    }
    FuncMoveWM::Axis axis = FuncMoveWM::UNDEFINE;
    double pos = 0;
    EleIconBtn *btn = qobject_cast<EleIconBtn*>(sender());
    if (btn) {
        if(btn == btnMoveX){
            axis = FuncMoveWM::X;
            pos = boxX->value();
        }
        else if(btn == btnMoveY){
            axis = FuncMoveWM::Y;
            pos = boxY->value();
        }
        else if(btn == btnMoveZ){
            axis = FuncMoveWM::Z;
            pos = boxZ->value();
        }
        moveWM->moveAbsolute(axis,pos);
    }
    else {
        return;
    }
}

void ViewFloatMovement::setStepBtnsEnabled(bool enabled)
{
    btnPZTD->setEnabled(enabled);
    btnPZTU->setEnabled(enabled);
}
#pragma execution_character_set(pop)
