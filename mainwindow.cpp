#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QMenuBar>
#include <QAction>
#include <QDateTime>
#include <QMetaObject>

#pragma execution_character_set(push, "utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    initUI();
    m_cameraThread = new CameraThread(this);
    m_videoWidget = new VideoWidget(this);
    this->setWindowTitle("桌面式高精密样本观测平台V1.0");
    this->setWindowIcon(QIcon(":/icon-gbl/resource/icon-gbl/window2.png"));

    funcMoveWM = FuncMoveWM::getInstance();


    viewTitleMsgBox = new ViewTitleMsgBox;
    ui->layoutMsgBox->addWidget(viewTitleMsgBox,0);

//    viewDLP4500Scan = new ViewDLP4500Scan;
//    viewDLP4500Scan->setObjectName("controlPanel");
//    ui->layoutDLPC350Scan->addWidget(viewDLP4500Scan,1);

    viewDLPScan = new ViewDLPScan;
    viewDLPScan->setObjectName("controlPanel");
    ui->layoutDLPC350Scan->addWidget(viewDLPScan,1);

    ui->layoutImg->addWidget(m_videoWidget);


    viewFloatMovement = new ViewFloatMovement(this);
    viewConfigFloat = new viewFloatConfig(this);


    sigslotInit();
    devInit();
    viewTitleMsgBox->writeMsg("欢迎使用桌面式高精密样本观测平台，软件版本V1.0");
    viewTitleMsgBox->writeMsg("请先在左上角开始菜单中连接硬件设备");

    serviceCur = "";
//    m_glImageWidget->updateImage(QImage("./86.bmp"));

}

MainWindow::~MainWindow()
{
    viewFloatMovement->close();
    viewConfigFloat->cloWid();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
}

void MainWindow::onInfo(QString msg)
{
    if(msg.contains("DISPLAY")&&msg.contains("SET")){
        viewTitleMsgBox->setDisplay(msg);
    }
    else{
        viewTitleMsgBox->writeMsg(msg);
    }
}

void MainWindow::onError(QString msg)
{
    viewTitleMsgBox->writeMsg(msg);
    QMessageBox::warning(this,"error",msg);
}

void MainWindow::onFloatMovement()
{
    viewFloatMovement->show();
}

void MainWindow::onFloatConfig()
{
    viewConfigFloat->show();
}

void MainWindow::onViewCommand(QString msg)
{
    QStringList cmds = msg.split("_");
    if(cmds.first() == "viewTitleMsgBox"){
        viewTitleMsgBox->cmdFrMainWindow(msg);
    }
}

void MainWindow::onPZTPos(double pos)
{
//    if(viewDLP4500Scan->isVisible()){
//        viewDLP4500Scan->setPZTPos(pos);
//    }
    if(viewDLPScan->isVisible()){
        viewDLPScan->setPZTPos(pos);
    }
    if(viewFloatMovement->isVisible()){
        viewFloatMovement->setPZTPos(pos);
    }
}

void MainWindow::onCamImgBack(int camNum,QImage img)
{
//    camPixmap.convertFromImage(img.scaled(labelImg->width(),labelImg->height()));
//    labelImg->setPixmap(camPixmap);
//    if(serviceCur == "DLP4500Scan"){
//        viewDLP4500Scan->camImgBackMW(camNum,img,serviceCur);
//    }
}

void MainWindow::onRegistService(QString serviceName)
{
    QWidget *wid = static_cast<QWidget*>(sender());
//    if(wid == viewDLP4500Scan){
//        if(serviceCur.isEmpty()){
//            viewDLP4500Scan->setMainWdCmd("Service Regist Success");
//            serviceCur = serviceName;
//        }
//        else{
//            viewDLP4500Scan->setMainWdCmd(serviceCur);
//        }
//    }

    if(wid == viewDLPScan){
        if(serviceCur.isEmpty()){
            serviceCur = serviceName;
        }
        else{
        }
    }
}

void MainWindow::onMoveWMSerMsg(QString msg)
{
    if(msg.contains("POS")){
//        if(viewDLP4500Scan->isVisible()){
//            viewDLP4500Scan->setPosMsg("WM",msg);
//        }
        if(viewDLPScan->isVisible()){
            viewDLPScan->setPosMsg("WM",msg);
        }
        if(viewFloatMovement->isVisible()){
            viewFloatMovement->setPosMsg("WM",msg);
        }

    }
    else{
        viewTitleMsgBox->writeMsg(msg);
        if(msg.contains("Motor_")){
            viewFloatMovement->setMoveEnable(msg.split("_").last());
        }
    }

}

void MainWindow::oncheckDevStatus(QString viewName)
{
    QString result = "";

    result += "camera:";
    result += m_cameraThread->getCameraOpen()? "true":"false";
    result += ";";

    result += "pzt:";
    result += FuncPZTCtl::getInstance()->pztIsOpen()? "true":"false";
    result += ";";

    result += "dmd:";
    result += FuncDLP3500Ctl::getInstance()->getIsInit()? "true":"false";
    result += ";";

    if(viewName == "ViewDLPScan"){
        if(!serviceCur.isEmpty()){
            result += "service:unavailable;";
        }
//        viewDLP4500Scan->checkDevResult(result);
        viewDLPScan->checkDevResult(result);
        serviceCur = "DLPScan";
    }
}

void MainWindow::onServiceReady(QString dirPath)
{
    if(serviceCur == "DLP4500Scan"){
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QString filePath = dirPath + QString("/DLP4500Scan_%1.raw").arg(timestamp);
        m_cameraThread->openStorage(filePath);
//        viewDLP4500Scan->startService();
        viewDLPScan->startService();
    }
}

void MainWindow::onCameraOpened()
{
    onGrabStarted();
}

void MainWindow::onCameraClosed()
{
    m_cameraThread->disconnectCamera();
}

void MainWindow::onGrabStarted()
{
    connect(m_cameraThread, &CameraThread::imageReady,
            m_videoWidget, &VideoWidget::updateImage);
    m_cameraThread->startAcquisition();
    viewTitleMsgBox->writeMsg("相机采集已开始");
}

void MainWindow::onGrabStopped()
{
    disconnect(m_cameraThread, &CameraThread::imageReady,
               m_videoWidget, &VideoWidget::updateImage);
    m_cameraThread->stopAcquisition();
    viewTitleMsgBox->writeMsg("相机采集已结束");
}

void MainWindow::onImgUpdateStopped()
{
    disconnect(m_cameraThread, &CameraThread::imageReady,
               m_videoWidget, &VideoWidget::updateImage);
}

void MainWindow::onImgUpdateStarted()
{
    connect(m_cameraThread, &CameraThread::imageReady,
            m_videoWidget, &VideoWidget::updateImage);
}

void MainWindow::onCameraDisconnected()
{

}

void MainWindow::onErrorOccurred(const QString &errorMsg)
{

}

void MainWindow::onImageReceived(const QImage &image)
{
//    m_glImageWidget->updateImage(image.scaled(m_glImageWidget->width(),m_glImageWidget->height()));
}

void MainWindow::onShutdownFinished()
{
    // 1. 调用接口等待线程退出
    //CameraWorker::instance().waitForThreadShutdown();
//    // 清理完成，安全退出
//    this->close();

    // 2. 【核心】直接退出Qt事件循环，程序安全退出，不需要再调用close()
    QCoreApplication::quit();
}

void MainWindow::onDlp4500ScanPara(QString paraList)
{
    Q_UNUSED(paraList)
    serviceCur = "DLP4500Scan";
}

void MainWindow::onDlp4500ScanGrab()
{
    m_cameraThread->triggerSnapshot();
//    viewDLP4500Scan->move2Next();
    viewDLPScan->move2Next();
}

void MainWindow::onDlp4500ScanStop()
{
    m_cameraThread->closeStorage();
    viewTitleMsgBox->writeMsg("正在停止相机并等待剩余数据落盘...");

}

void MainWindow::devInit()
{

}

void MainWindow::sigslotInit()
{
    //相机信号
    connect(viewConfigFloat,&viewFloatConfig::sigOpenCamera,
            m_cameraThread,&CameraThread::connectCamera);
    connect(m_cameraThread,&CameraThread::sigCameraConnected,
            this,&MainWindow::onCameraOpened);
    connect(m_cameraThread, &CameraThread::cameraError,
            this, &MainWindow::onInfo);


    //其他信号
    connect(FuncPZTCtl::getInstance(),&FuncPZTCtl::sigInfo,
            this,&MainWindow::onInfo);
    connect(viewConfigFloat,&viewFloatConfig::sigInfo,
            this,&MainWindow::onInfo);
    connect(viewDLPScan,&ViewDLPScan::sigInfo,
            this,&MainWindow::onInfo);

    //DLP4500Scan
//    connect(viewDLP4500Scan,&ViewDLP4500Scan::sigCheckDev,
//            this,&MainWindow::oncheckDevStatus);
//    connect(viewDLP4500Scan,&ViewDLP4500Scan::sigGrab,
//            this,&MainWindow::onDlp4500ScanGrab);
//    connect(viewDLP4500Scan,&ViewDLP4500Scan::sigStopScan,
//            this,&MainWindow::onDlp4500ScanStop);
//    connect(viewDLP4500Scan,&ViewDLP4500Scan::sigServiceReady,
//            this,&MainWindow::onServiceReady);
    connect(viewDLPScan,&ViewDLPScan::sigCheckDev,
            this,&MainWindow::oncheckDevStatus);
    connect(viewDLPScan,&ViewDLPScan::sigGrab,
            this,&MainWindow::onDlp4500ScanGrab);
    connect(viewDLPScan,&ViewDLPScan::sigStopScan,
            this,&MainWindow::onDlp4500ScanStop);
    connect(viewDLPScan,&ViewDLPScan::sigServiceReady,
            this,&MainWindow::onServiceReady);
}

void MainWindow::initUI()
{
    // 在 QMainWindow 构造函数中
    QMenuBar *menuBar = this->menuBar();

    // 1. 创建“系统”菜单
    QMenu *fileMenu = menuBar->addMenu(tr("系统(&F)"));
    QMenu *moveMenu = menuBar->addMenu(tr("运动台(&C)"));

    // 2. 在系统菜单添加动作
    QAction *cntAct = new QAction(tr("设备连接(&N)"), this);
    cntAct->setShortcut(tr("Ctrl+N")); // 设置快捷键 Ctrl+N
    fileMenu->addAction(cntAct);
    connect(cntAct, &QAction::triggered, this, [this](){
        viewConfigFloat->show();
    });

    //3. 在运动台菜单添加动作
    QAction *motorAct = new QAction(tr("打开运动台控制窗口(&M)"), this);
    motorAct->setShortcut(tr("Ctrl+M")); // 设置快捷键 Ctrl+M
    moveMenu->addAction(motorAct);
    connect(motorAct, &QAction::triggered, this, [this](){
        viewFloatMovement->show();
    });

    // 3. 添加分隔线和“退出”
    fileMenu->addSeparator();
    QAction *exitAct = fileMenu->addAction(tr("退出(&X)"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
}
