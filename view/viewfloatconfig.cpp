#include "viewfloatconfig.h"
#include <QLayout>
#include <QSerialPort>
#include <QMessageBox>
#include <QDir>
#include <QSerialPortInfo>
#include "function/funcdlp3500ctl.h"
#include "function/funcpztctl.h"
#include "function/funcmovewm.h"
#include "IO/txtio.h"

viewFloatConfig::viewFloatConfig(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(
        Qt::WindowStaysOnTopHint    // 保持在顶层（浮动特性）
        | Qt::Window                // 标准窗口（带标题栏和边框）
    );
    setWindowTitle("设备控制台"); // 标题栏文本
    this->resize(150,300);
    m_pCamInit = false;
    m_pStyle = "line2D";
    QGridLayout *mainLayout = new QGridLayout;
    this->setLayout(mainLayout);

    if(m_pStyle == "N"){
        esBtnCam = new EleIconBtn(":/icon/resource/icon-N/disconnectDev.png",64,64,"点击以连接");
        esBtnCam->setStatusIcon({":/icon/resource/icon-N/disconnectDev.png",
                                 ":/icon/resource/icon-N/connectDev.png"},
                                {"连接","断开连接"});
        esBtnDMD = new EleIconBtn(":/icon/resource/icon-N/disconnectDev.png",64,64,"点击以连接DMD");
        esBtnDMD->setStatusIcon({":/icon/resource/icon-N/disconnectDev.png",
                                 ":/icon/resource/icon-N/connectDev.png"},
                                {"连接DMD","断开连接DMD"});
        esBtnMov = new EleIconBtn(":/icon/resource/icon-N/disconnectDev.png",64,64,"点击以连接");
        esBtnMov->setStatusIcon({":/icon/resource/icon-N/disconnectDev.png",
                                 ":/icon/resource/icon-N/connectDev.png"},
                                {"连接","断开连接"});
        esBtnPZT = new EleIconBtn(":/icon/resource/icon-N/disconnectDev.png",64,64,"点击以连接");
        esBtnPZT->setStatusIcon({":/icon/resource/icon-N/disconnectDev.png",
                                 ":/icon/resource/icon-N/connectDev.png"},
                                {"连接","断开连接"});
    }
    else if(m_pStyle.contains("gbl")){
        esBtnCam = new EleIconBtn(":/icon/resource/icon-gbl/disconnectDev.png",64,64,"点击以连接");
        esBtnCam->setStatusIcon({":/icon/resource/icon-gbl/disconnectDev.png",
                                 ":/icon/resource/icon-gbl/connectDev.png"},
                                {"连接","断开连接"});
        esBtnDMD = new EleIconBtn(":/icon/resource/icon-gbl/disconnectDev.png",64,64,"点击以连接DMD");
        esBtnDMD->setStatusIcon({":/icon/resource/icon-gbl/disconnectDev.png",
                                 ":/icon/resource/icon-gbl/connectDev.png"},
                                {"连接DMD","断开连接DMD"});
        esBtnMov = new EleIconBtn(":/icon/resource/icon-gbl/disconnectDev.png",64,64,"点击以连接");
        esBtnMov->setStatusIcon({":/icon/resource/icon-gbl/disconnectDev.png",
                                 ":/icon/resource/icon-gbl/connectDev.png"},
                                {"连接","断开连接"});
        esBtnPZT = new EleIconBtn(":/icon/resource/icon-gbl/disconnectDev.png",64,64,"点击以连接");
        esBtnPZT->setStatusIcon({":/icon/resource/icon-gbl/disconnectDev.png",
                                 ":/icon/resource/icon-gbl/connectDev.png"},
                                {"连接","断开连接"});
    }
    else if(m_pStyle.contains("line2D")){
        esBtnCam = new EleIconBtn(":/icon/resource/icon-line2D/disconnectDev.png",64,64,"点击以连接");
        esBtnCam->setStatusIcon({":/icon/resource/icon-line2D/disconnectDev.png",
                                 ":/icon/resource/icon-line2D/connectDev.png"},
                                {"连接","断开连接"});
        esBtnDMD = new EleIconBtn(":/icon/resource/icon-line2D/disconnectDev.png",64,64,"点击以连接DMD");
        esBtnDMD->setStatusIcon({":/icon/resource/icon-line2D/disconnectDev.png",
                                 ":/icon/resource/icon-line2D/connectDev.png"},
                                {"连接DMD","断开连接DMD"});
        esBtnMov = new EleIconBtn(":/icon/resource/icon-line2D/disconnectDev.png",64,64,"点击以连接");
        esBtnMov->setStatusIcon({":/icon/resource/icon-line2D/disconnectDev.png",
                                 ":/icon/resource/icon-line2D/connectDev.png"},
                                {"连接","断开连接"});
        esBtnPZT = new EleIconBtn(":/icon/resource/icon-line2D/disconnectDev.png",64,64,"点击以连接");
        esBtnPZT->setStatusIcon({":/icon/resource/icon-line2D/disconnectDev.png",
                                 ":/icon/resource/icon-line2D/connectDev.png"},
                                {"连接","断开连接"});
    }


    mainLayout->addWidget(new QLabel("相 机"),0,0,1,1);
    mainLayout->addWidget(esBtnCam,0,1,1,1);
    mainLayout->addWidget(new QLabel("运动台"),1,0,1,1);
    mainLayout->addWidget(esBtnMov,1,1,1,1);
    mainLayout->addWidget(new QLabel("PZT"),2,0,1,1);
    mainLayout->addWidget(esBtnPZT,2,1,1,1);
    mainLayout->addWidget(new QLabel("DMD"),3,0,1,1);
    mainLayout->addWidget(esBtnDMD,3,1,1,1);
    connect(esBtnCam,&EleIconBtn::clicked,this,&viewFloatConfig::onBtnCam);
    connect(esBtnDMD,&EleIconBtn::clicked,this,&viewFloatConfig::onBtnDMD);
    connect(esBtnPZT,&EleIconBtn::clicked,this,&viewFloatConfig::onBtnPZT);
    connect(esBtnMov,&EleIconBtn::clicked,this,&viewFloatConfig::onBtnMov);



}

void viewFloatConfig::cloWid()
{
    this->close();
}

void viewFloatConfig::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    readDevInfo();
    registDevSigSlot();
}

void viewFloatConfig::closeEvent(QCloseEvent *event)
{
    // 阻止默认关闭行为（默认会销毁窗口）
    event->ignore();
    // 改为隐藏窗口
    hide();
    // 可选：发送信号通知主窗口
}

void viewFloatConfig::onBtnDMD()
{
    FuncDLP3500Ctl *dlp = FuncDLP3500Ctl::getInstance();
    if(dlp->getInitFlag()){
        return;
    }
    int res = dlp->connectDMD();
    if(!res){
        emit sigInfo("DMD已连接");
        esBtnDMD->changeStatus(1);
    }
    else{
        emit sigInfo("DMD连接失败");
        return;
    }

    dlp->setMode(FuncDLP3500Ctl::PATTERN_SEQUENCE_VE,FuncDLP3500Ctl::FLASH);
    int num = dlp->getFlasImages();
    emit sigInfo("DMD内存图案数量："+QString::number(num));
    dlp->setInitFlag(true);
}

void viewFloatConfig::onBtnCam()
{
    if(esBtnCam->getToggleStatus()){
        emit sigOpenCamera(0);
    }
    else{
        emit sigCloseCamera(0);

    }
}

void viewFloatConfig::onBtnMov()
{
    if(esBtnMov->getToggleStatus()){
        QMessageBox::information(this,"硬件设定","请关闭主程序断开硬件连接");
        esBtnPZT->changeToggleStatus();
    }
    else{
        openMotor();
    }

}

void viewFloatConfig::onBtnPZT()
{
    if(esBtnPZT->getToggleStatus()){
        QMessageBox::information(this,"硬件设定","请关闭主程序断开硬件连接");
        esBtnPZT->changeToggleStatus();
    }
    else{
        QStringList pztParams = m_pPZT.split(',');
        if(pztParams.count()<2){
            emit sigInfo("微动台初始化参数缺失，请检查设备文档");
            return;
        }
        if(pztParams.at(0) == "XMT"){
            int ret = FuncPZTCtl::getInstance()->initPZT(pztParams.at(1));
            if(!ret){
                }
        }
    }
}

void viewFloatConfig::readDevInfo()
{
    QString absolutePath = QDir::current().absoluteFilePath("DevInfo.txt");
    QStringList devInfo = TxtIO::ReadATxt(absolutePath);
    if(devInfo.isEmpty()){
        emit sigInfo("DevInfo.txt 缺失或不存在");
        return;
    }
    else{
        auto info = std::find_if(devInfo.begin(),devInfo.end(),[&](const QString& str){
            return str.contains("DMD");
        });
        if (info != devInfo.end()){
            QString dev = *info;
            m_pDMD = dev.split(":").last();
        }

        info = std::find_if(devInfo.begin(),devInfo.end(),[&](const QString& str){
            return str.contains("Camera");
        });
        if (info != devInfo.end()){
            QString dev = *info;
            m_pCam = dev.split(":").last();
        }

        info = std::find_if(devInfo.begin(),devInfo.end(),[&](const QString& str){
            return str.contains("PZT");
        });
        if (info != devInfo.end()){
            QString dev = *info;
            m_pPZT = dev.split(":").last();
        }

        info = std::find_if(devInfo.begin(),devInfo.end(),[&](const QString& str){
            return str.contains("Motor");
        });
        if (info != devInfo.end()){
            QString dev = *info;
            m_pMotor = dev.split(":").last();
        }
    }
}

void viewFloatConfig::registDevSigSlot()
{
    if(m_pCam == "Basler"){
    }

}

void viewFloatConfig::openMotor()
{
    QStringList motorParams = m_pMotor.split(',');
    if(motorParams.count()<4){
        emit sigInfo("电机初始化参数缺失，请检查设备文档");
        return;
    }
    if(motorParams.at(0) == "WM"){
        if(motorParams.at(1) == "localhost"){
            FuncMoveWM::getInstance()->openDev(motorParams.at(2));
        }
    }
}
#pragma execution_character_set(pop)
