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

    QString iconPathRoot = ":/icon/resource/icon-";

    esBtnCam = new PBtnToggle(iconPathRoot+m_pStyle+"/connectDev.png",iconPathRoot+m_pStyle+"/disconnectDev.png",
                              64,64,"点击以连接");
    esBtnDMD = new PBtnToggle(iconPathRoot+m_pStyle+"/connectDev.png",iconPathRoot+m_pStyle+"/disconnectDev.png",
                              64,64,"点击以连接");
    esBtnMov = new PBtnToggle(iconPathRoot+m_pStyle+"/connectDev.png",iconPathRoot+m_pStyle+"/disconnectDev.png",
                              64,64,"点击以连接");
    esBtnPZT = new PBtnToggle(iconPathRoot+m_pStyle+"/connectDev.png",iconPathRoot+m_pStyle+"/disconnectDev.png",
                              64,64,"点击以连接");


    mainLayout->addWidget(new QLabel("相 机"),0,0,1,1);
    mainLayout->addWidget(esBtnCam,0,1,1,1);
    mainLayout->addWidget(new QLabel("运动台"),1,0,1,1);
    mainLayout->addWidget(esBtnMov,1,1,1,1);
    mainLayout->addWidget(new QLabel("PZT"),2,0,1,1);
    mainLayout->addWidget(esBtnPZT,2,1,1,1);
    mainLayout->addWidget(new QLabel("DMD"),3,0,1,1);
    mainLayout->addWidget(esBtnDMD,3,1,1,1);
    connect(esBtnCam,&PBtnToggle::clicked,this,&viewFloatConfig::onBtnCam);
    connect(esBtnDMD,&PBtnToggle::clicked,this,&viewFloatConfig::onBtnDMD);
    connect(esBtnPZT,&PBtnToggle::clicked,this,&viewFloatConfig::onBtnPZT);
    connect(esBtnMov,&PBtnToggle::clicked,this,&viewFloatConfig::onBtnMov);



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
        esBtnDMD->setChecked(true);
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
    if(esBtnCam->isChecked()){
        emit sigOpenCamera(0);
    }
    else{
        emit sigCloseCamera(0);

    }
}

void viewFloatConfig::onBtnMov()
{
    if(!esBtnMov->isChecked()){
        QMessageBox::information(this,"硬件设定","请关闭主程序断开硬件连接");
    }
    else{
        openMotor();
    }

}

void viewFloatConfig::onBtnPZT()
{
    if(!esBtnPZT->isChecked()){
        QMessageBox::information(this,"硬件设定","请关闭主程序断开硬件连接");
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
