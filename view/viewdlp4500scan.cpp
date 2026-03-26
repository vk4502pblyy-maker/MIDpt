#include "viewdlp4500scan.h"
#include <QLayout>
#include <QFileDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDateTime>
#include <QMessageBox>

#pragma execution_character_set(push, "utf-8")
ViewDLP4500Scan::ViewDLP4500Scan(QWidget *parent)
    : QWidget{parent}
{
    m_pStyle = "gbl";
    m_pRowPass = false,m_pColumnPass = false,
    m_pExpPass = false,m_pPrdPass = false,
    m_pStartPass = false,m_pStopPass = false,
    m_pStepPass = false;
    m_pIsStop = false;
    m_pIsScript = false;
    m_pcamInit = false;
    m_pIsScan = false;
    m_pIsTestInit = false;
    m_pTestTp = 0;
    m_pCamImgBack = 0;
    m_pPZTReady = false;

    dmd = FuncDLP3500Ctl::getInstance();
    pzt = FuncPZTCtl::getInstance();

    if(m_pStyle == "N"){
        btnCam = new EleIconBtn(":/icon/resource/icon-N/camera.png",64,64,"打开实时相机");
        btnCam->setStatusIcon({":/icon/resource/icon-N/camera.png",
                              ":/icon/resource/icon-N/camLive.png"},
                              {"打开观测","关闭观测"},true,true);
//        btnInit = new EleIconBtn(":/icon/resource/icon-N/initDev.png",64,64,"初始化，请确认曝光及周期是否正确");
        btnTriggerE = new EleIconBtn(":/icon/resource/icon-N/triggerEx.png",64,64,"外触发");
        btnTriggerE->setToggleBtn(true);
        btnTriggerI = new EleIconBtn(":/icon/resource/icon-N/triggerIn.png",64,64,"内触发");
        btnTriggerI->setToggleBtn(true);
        labelImgInFlash = new QLabel("0");
        btnSetMin = new EleIconBtn(":/icon/resource/icon-N/currentPos.png",64,64,"设为当前位置为起点");
        btnSetMax = new EleIconBtn(":/icon/resource/icon-N/currentPos.png",64,64,"设为当前位置为终点");
        btnRecipeGen = new EleIconBtn(":/icon/resource/icon-N/saveDoc.png",64,64,"保存当前配置");
        btnRecipeLoad = new EleIconBtn(":/icon/resource/icon-N/load-doc-2.png",64,64,"加载配置");
        btnImgBackward = new EleIconBtn(":/icon/resource/icon-N/left-arr.png",64,64,"上一张照片");
        btnImgForward = new EleIconBtn(":/icon/resource/icon-N/right-arr.png",64,64,"下一张照片");
        btnDir = new EleIconBtn(":/icon/resource/icon-N/openDir.png",64,64,"浏览文件夹");
        btnResultCalculate = new EleIconBtn(":/icon/resource/icon-N/analysis.png",64,64,"分析结果");
        btnStart = new EleIconBtn(":/icon/resource/icon-N/radar.png",64,64,"一键启动");
        btnStop = new EleIconBtn(":/icon/resource/icon-N/stop.png",64,64,"停止检测");
    }
    else if(m_pStyle.contains("gbl")){
        btnCam = new EleIconBtn(":/icon/resource/icon-gbl/camera.png",64,64,"拍照并保存");
//        btnCam->setStatusIcon({":/icon/resource/icon-N/camera.png",
//                              ":/icon/resource/icon-N/camLive.png"},
//                              {"打开观测","关闭观测"},true,true);
//        btnInit = new EleIconBtn(":/icon/resource/icon-gbl/initDev.png",64,64,"初始化，请确认曝光及周期是否正确");
        btnTriggerE = new EleIconBtn(":/icon/resource/icon-gbl/triggerEx.png",64,64,"外触发");
        btnTriggerE->setToggleBtn(true);
        btnTriggerI = new EleIconBtn(":/icon/resource/icon-gbl/triggerIn.png",64,64,"内触发");
        btnTriggerI->setToggleBtn(true);
        labelImgInFlash = new QLabel("0");
        btnSetMin = new EleIconBtn(":/icon/resource/icon-gbl/currentPos.png",64,64,"设为当前位置为起点");
        btnSetMax = new EleIconBtn(":/icon/resource/icon-gbl/currentPos.png",64,64,"设为当前位置为终点");
        btnRecipeGen = new EleIconBtn(":/icon/resource/icon-gbl/saveDoc.png",64,64,"保存当前配置");
        btnRecipeLoad = new EleIconBtn(":/icon/resource/icon-gbl/loadDoc.png",64,64,"加载配置");
        btnImgBackward = new EleIconBtn(":/icon/resource/icon-gbl/left-arr.png",64,64,"上一张照片");
        btnImgForward = new EleIconBtn(":/icon/resource/icon-gbl/right-arr.png",64,64,"下一张照片");
        btnDir = new EleIconBtn(":/icon/resource/icon-gbl/openDir.png",64,64,"浏览文件夹");
        btnResultCalculate = new EleIconBtn(":/icon/resource/icon-gbl/analysis.png",64,64,"分析结果");
        btnStart = new EleIconBtn(":/icon/resource/icon-gbl/radar-off.png",64,64,"一键启动");
        btnStart->setStatusIcon({":/icon/resource/icon-gbl/radar-off.png",
                              ":/icon/resource/icon-gbl/radar.png"},
                              {"打开观测","关闭观测"},true,true);
        btnStop = new EleIconBtn(":/icon/resource/icon-gbl/stop-off.png",64,64,"停止检测");
        btnStop->setHoverIcon(":/icon/resource/icon-gbl/stop.png");
    }




    QHBoxLayout *layoutMain = new QHBoxLayout();
    this->setLayout(layoutMain);

    QVBoxLayout *layout1 = new QVBoxLayout;
    layoutMain->addLayout(layout1);
    QHBoxLayout *layout11 = new QHBoxLayout;
    layout1->addLayout(layout11);

    QVBoxLayout *layout2 = new QVBoxLayout;
    layoutMain->addLayout(layout2,0);

    labelPosCur = new QLabel("X:--; Y:--; Z:--;");
    labelPosPZT = new QLabel("PZT:--;");
    QHBoxLayout *layoutPos = new QHBoxLayout;
    layoutPos->addWidget(labelPosCur);
    layoutPos->addWidget(labelPosPZT);
    layout2->addLayout(layoutPos);

    QHBoxLayout *layout21 = new QHBoxLayout;
    layout2->addLayout(layout21);
//    layout21->addWidget(btnInit);
    layout21->addWidget(btnTriggerE);
    layout21->addWidget(btnTriggerI);
    layout21->addWidget(btnCam);
    layout21->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
    layout21->addWidget(new QLabel("内存图片数量: "));
    layout21->addWidget(labelImgInFlash);

//    layout2->addWidget(new QLabel("扫描矩阵"));
    QHBoxLayout *layout22 = new QHBoxLayout;
    layout2->addLayout(layout22);
    boxRow = new QSpinBox;
    boxRow->setRange(1,256);
    boxColumn = new QSpinBox;
    boxColumn->setRange(1,256);
    layout22->addWidget(new QLabel("行数"));
    layout22->addWidget(boxRow,1);
    layout22->addWidget(new QLabel("列数"));
    layout22->addWidget(boxColumn,1);

//    layout2->addWidget(new QLabel("曝光与周期"));
    QHBoxLayout *layout23 = new QHBoxLayout;
    layout2->addLayout(layout23);
    boxExp = new QSpinBox;
    boxExp->setRange(10000,100000000);
    boxExp->setValue(1000000);
    boxPrd = new QSpinBox;
    boxPrd->setRange(10000,100000000);
    boxPrd->setValue(1000000);
    layout23->addWidget(new QLabel("曝光"));
    layout23->addWidget(boxExp,1);
    layout23->addWidget(new QLabel("周期"));
    layout23->addWidget(boxPrd,1);

//    layout2->addWidget(new QLabel("扫描行程"));
    QHBoxLayout *layout24 = new QHBoxLayout;
    layout2->addLayout(layout24);
    boxStart = new QDoubleSpinBox;
    boxStart->setRange(1,185);
    boxStop = new QDoubleSpinBox;
    boxStop->setRange(1,185);
    layout24->addWidget(new QLabel("起点"));
    layout24->addWidget(boxStart,1);
    layout24->addWidget(btnSetMin);
    layout24->addWidget(new QLabel("终点"));
    layout24->addWidget(boxStop,1);
    layout24->addWidget(btnSetMax);
    QHBoxLayout *layout25 = new QHBoxLayout;
    layout2->addLayout(layout25);
    boxStep = new QDoubleSpinBox;
    boxStep->setRange(1,50);
    checkSpain = new QCheckBox("平面模式");
    layout25->addWidget(new QLabel("步进"));
    layout25->addWidget(boxStep,1);
    layout25->addWidget(checkSpain);
    layout25->addWidget(btnRecipeGen);
    layout25->addWidget(btnRecipeLoad);

    QHBoxLayout *layout25A1 = new QHBoxLayout;
    layout2->addLayout(layout25A1);
    boxFrameStart = new QSpinBox;
    boxFrameStop = new QSpinBox;
    checkFrameAll = new QCheckBox("全部图片");
    layout25A1->addWidget(new QLabel("起始序号"));
    layout25A1->addWidget(boxFrameStart);
    layout25A1->addWidget(new QLabel("终止序号"));
    layout25A1->addWidget(boxFrameStop);
    layout25A1->addWidget(checkFrameAll);


//    layout2->addWidget(new QLabel("结果保存"));
    QHBoxLayout *layout26 = new QHBoxLayout();
    layout2->addLayout(layout26);
    lineSaveDir = new QLineEdit;

    layout26->addWidget(new QLabel("保存位置"));
    layout26->addWidget(lineSaveDir,1);
    layout26->addWidget(btnDir);

    QHBoxLayout *layout27 = new QHBoxLayout;
    layout2->addLayout(layout27);
    layout27->addWidget(btnStart);
    layout27->addWidget(btnStop);

    layout27->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
//    progressTest = new QProgressBar;
    labelProgessCur = new QLabel("未开始");
    layout27->addWidget(new QLabel("当前进度："));
//    layout27->addWidget(progressTest,1);
    layout27->addWidget(labelProgessCur);

    QHBoxLayout *layout28 = new QHBoxLayout;
    layout2->addLayout(layout28);
    btnTest = new QPushButton("来测！");
    layout28->addWidget(btnTest);

    spacerWid = new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding);
    layout2->addItem(spacerWid);
//    processWid = new GLImageWidget();
//    processWid->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    layout2->addWidget(processWid);
//    processWid->updateImage(QImage("D:/videosoft/tree-img/frame_0001.jpg"));
//    processWid->setVisible(false);
//    processWid->setBackGroundColor("#F6F0E4");
    QHBoxLayout *layout210 = new QHBoxLayout;
    layout2->addLayout(layout210);
    layout210->addWidget(btnImgBackward);
    layout210->addWidget(btnImgForward);
    layout210->addWidget(btnResultCalculate);

    connect(btnCam,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnCam);
    connect(btnDir,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnDir);
//    connect(btnInit,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnInit);
    connect(btnSetMax,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnSetMax);
    connect(btnSetMin,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnSetMin);
    connect(btnTriggerE,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnTriggerE);
    connect(btnTriggerI,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnTriggerI);
    connect(btnStart,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnStart);
    connect(btnStop,&EleIconBtn::clicked,this,&ViewDLP4500Scan::onBtnStop);
    connect(btnTest,&QPushButton::clicked,this,&ViewDLP4500Scan::onBtnTest);
    connect(boxColumn,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxExp,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxPrd,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxRow,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxFrameStart,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxFrameStop,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChange);
    connect(boxStart,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChangeDouble);
    connect(boxStep,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChangeDouble);
    connect(boxStop,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLP4500Scan::onSpinValueChangeDouble);

    connect(&timerV30,&QTimer::timeout,this,&ViewDLP4500Scan::onTimerScanV30Timeout);

    btnTest->setVisible(false);

}

ViewDLP4500Scan::~ViewDLP4500Scan()
{
    if(timerScan.isActive()){
        timerScan.stop();
    }
    if(dmd->getInitFlag()){
        dmd->PatSeqCtrlStop();
    }
    if(loopV30.isRunning()){
        loopV30.quit();
    }
}

int ViewDLP4500Scan::setMainWdCmd(QString cmd)
{
    m_pMainWdCmd = cmd;
    m_pMainWdCmdBack = true;
    return 0;
}

int ViewDLP4500Scan::camImgBackMW(int camNum, QImage img, QString serviceCur)
{
    m_pImg = img;
    if(serviceCur == "DLP4500Scan"){
        processScanImg();
    }
    return 0;
}

int ViewDLP4500Scan::setPosMsg(QString type, QString posMsg)
{
    if(type == "WM"){
        if(posMsg.split("_").count()!=2)
            return -1;
        QStringList posList = posMsg.split("_").last().split(",");
        switch (posList.count()) {
        case 2:
            labelPosCur->setText(QString("X:--;Y:--;Z:%1;").arg(posList.at(1)));
            break;
        case 4:
            labelPosCur->setText(QString("X:%1;Y:%2;Z:--;")
                                 .arg(posList.at(1)).arg(posList.at(3)));
            break;
        case 6:
            labelPosCur->setText(QString("X:--;Y:--Z:%1;")
                                 .arg(posList.at(1)).arg(posList.at(3)).arg(posList.at(5)));
            break;
        default:
            break;
        }
    }
    return 0;
}

int ViewDLP4500Scan::setPZTPos(double pos)
{
    labelPosPZT->setText("PZT:"+QString::number(pos,'f',3)+";");
    return 0;
}

int ViewDLP4500Scan::setPZTEnable()
{
    m_pPZTReady = true;
    return 0;
}

void ViewDLP4500Scan::move2Next()
{
    double pos = pzt->getPos(0);
    m_pProgressCur++;
    setProgessVid(m_pProgressCur);

    //单列扫描判断if：单列扫描完，else：未扫描完
    if(m_pScanColumnCur == boxColumn->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur++;
        emit sigInfo("DMD启动第"+QString::number(m_pScanRowCur)+"行开始："+QDateTime::currentDateTime().toString());
        setProcessLabel("当前在第："+QString::number(m_pScanRowCur)+"行");
    }
    else{
        m_pScanColumnCur++;
    }

    //单次DMD循环结束
    if(m_pScanRowCur>boxRow->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur = 1;
        if(pos >= boxStop->value()-0.05){
            stopScanV30();
            return;
        }
        pzt->setPos(0,pos+boxStep->value());
    }
    return;
}

void ViewDLP4500Scan::checkDevResult(QString result)
{
    QStringList ress = result.split(';');
    QString res = ress.at(0) + ress.at(1) + ress.at(2);
    if(res.contains("false") || res.contains("unavailable")){
        loopV30.exit(-1);
    }
    else{
        loopV30.exit(0);
    }
}

void ViewDLP4500Scan::setImageCur(const QImage &img)
{
    double pos = pzt->getPos(0);
    img.save(QDir(m_pScanDirPathCur).filePath(QString::number(m_pScanRowCur)+"-"+
                QString::number(m_pScanColumnCur)+".png"),"PNG");

    //单列扫描判断if：单列扫描完，else：未扫描完
    if(m_pScanColumnCur == boxColumn->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur++;
        emit sigInfo("DMD启动第"+QString::number(m_pScanRowCur)+"行开始："+QDateTime::currentDateTime().toString());
    }
    else{
        m_pScanColumnCur++;
    }

    //单次DMD循环结束
    if(m_pScanRowCur>boxRow->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur = 1;
        if(pos >= boxStop->value()-0.05){
            emit sigInfo("DMD启动，检测结束:"+QDateTime::currentDateTime().toString());
            stopScanV30();
            return;
        }
        pzt->setPos(0,pos+boxStep->value());
        m_pScanDirPathCur = QDir(m_pScanDirPathRaw).filePath(QString::number(pos+boxStep->value(),'f',3));
        emit sigInfo("m_pScanDirPathCur:"+m_pScanDirPathCur);
        QDir().mkdir(m_pScanDirPathCur);
        lineSaveDir->setText(m_pScanDirPathCur);
    }
    return;
}

void ViewDLP4500Scan::startService()
{
    loopV30.exit(0);
}

void ViewDLP4500Scan::onBtnInit()
{
    int res = 0;
    if(!dmd->getInitFlag()){
        res = dmdInit();
        if(res){
            return;
        }
        dmd->setMode(FuncDLP3500Ctl::PATTERN_SEQUENCE_VE,FuncDLP3500Ctl::FLASH);
        emit sigInfo("DMD初始化");
    }
    m_pImgInFlash = dmdLoadFlashImg();
    labelImgInFlash->setText(QString::number(m_pImgInFlash));
    boxFrameStart->setRange(0,m_pImgInFlash-1);
    boxFrameStop->setRange(0,m_pImgInFlash-1);
    if(m_pImgInFlash == 0){
        return;
    }
    if(!m_pcamInit){
        emit sigInitCamera(0);
    }
    return;
}

void ViewDLP4500Scan::onBtnCam()
{
    int status = btnCam->getStatus();
    if(status == 1){
        connect(&timerCamAcq,&QTimer::timeout,this,&ViewDLP4500Scan::onCamAcqTimeout);
        timerCamAcq.setInterval(100);
        timerCamAcq.start();
    }
    else{
        timerCamAcq.stop();
        disconnect(&timerCamAcq,&QTimer::timeout,this,&ViewDLP4500Scan::onCamAcqTimeout);
    }
}

void ViewDLP4500Scan::onBtnDir()
{
    lineSaveDir->setText(QFileDialog::getExistingDirectory(this,"选择保存文件夹"));
}

void ViewDLP4500Scan::onBtnSetMin()
{
    boxStart->setValue(pzt->getPos(0));
}

void ViewDLP4500Scan::onBtnSetMax()
{
    boxStop->setValue(pzt->getPos(0));
}

void ViewDLP4500Scan::onBtnStart()
{
    startScanV30();
}

void ViewDLP4500Scan::onBtnStop()
{
    emit sigInfo("用户中止检测");
    stopScanV30();
}

void ViewDLP4500Scan::onBtnTest()
{
    spacerWid->changeSize(0,0,QSizePolicy::Minimum,QSizePolicy::Minimum);
//    processWid->show();
    QString dirPath = "./process_vid/rocket";
    int count = countVidFile(dirPath);
    if(count == 0){
        return;
    }
    QElapsedTimer timer;
    for(int i=0;i<count;i++){
        QString imgPath = dirPath + "/" +"frame_";
        if(i<10){
            imgPath += "000"+QString::number(i)+".jpg";
        }
        else if(i<100){
            imgPath += "00"+QString::number(i)+".jpg";
        }
        else if(i<1000){
            imgPath += "0"+QString::number(i)+".jpg";
        }
//        processWid->updateImage(QImage(imgPath));
        timer.restart();
        while(timer.elapsed()<80){
            QCoreApplication::processEvents();
        }
    }
}

void ViewDLP4500Scan::onSpinValueChange(int value)
{
    QSpinBox *box = qobject_cast<QSpinBox*>(sender());
//    box->setStyleSheet("QSpinBox{background-color: #00D26A;}");
    box->setValue(value);
}

void ViewDLP4500Scan::onSpinValueChangeDouble(double value)
{
    QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox*>(sender());
//    box->setStyleSheet("QDoubleSpinBox{background-color: #00D26A;}");
    box->setValue(value);
}

void ViewDLP4500Scan::onCamImgBack(int camNum, QImage img)
{
    if(m_pCamImgBack<15){
//        listLog->addItem("onCamImgBack");
    }
//    timerCamAcq.stop();
    Q_UNUSED(camNum)
    m_pImg = img;
//    labelImgCam->setPixmap(QPixmap::fromImage(img.scaled(labelImgCam->width(),labelImgCam->height())));
    if(btnCam->getStatus() == 1)
        timerCamAcq.start(50);
    if(m_pIsScan){
       processScanData();
//        timerScan.start();
    }
    if(m_pTestTp){
        processTest();
    }

}

void ViewDLP4500Scan::onCamAcqTimeout()
{
    emit sigCamImg(0);
//    timerCamAcq.stop();
}

void ViewDLP4500Scan::onBtnTriggerE()
{
    if((btnTriggerE->getToggleStatus() == true) &&
            (btnTriggerI->getToggleStatus() == true)){
        btnTriggerI->changeToggleStatus();
    }
}

void ViewDLP4500Scan::onBtnTriggerI()
{
    if((btnTriggerI->getToggleStatus() == true) &&
            (btnTriggerE->getToggleStatus() == true)){
        btnTriggerE->changeToggleStatus();
    }
    m_pIsScript = true;
}

void ViewDLP4500Scan::onTimerScanTimeout()
{
    emit sigCamImg(0);
}

void ViewDLP4500Scan::onCamInit()
{
    m_pcamInit = true;
}

void ViewDLP4500Scan::onTimerTestTimeout()
{
    if(m_pCamImgBack<15)
//        listLog->addItem("onTimerTestTimeout");
        emit sigCamImg(0);
}

void ViewDLP4500Scan::onTimerScanV30Timeout()
{
    emit sigGrab();
}

int ViewDLP4500Scan::dmdInit()
{
    return dmd->connectDMD();
}

int ViewDLP4500Scan::dmdSetMode()
{
    return dmd->setMode(FuncDLP3500Ctl::PATTERN_SEQUENCE_VE,FuncDLP3500Ctl::FLASH);
}

int ViewDLP4500Scan::dmdSetPattern()
{
    int res = dmdAddPatternAll();
    if(res){
        return -1;
    }
    res = dmdSendPattern();
    if(res){
        return -1;
    }
    return 0;
}

int ViewDLP4500Scan::dmdValidate()
{
    setProcessLabel("DMD使能");
    return dmd->ValidatePatSeq();
}

int ViewDLP4500Scan::dmdLoadFlashImg()
{
    return dmd->getFlasImages();
}

int ViewDLP4500Scan::dmdAddPatternAll()
{
    setProcessLabel("加载全部pattern");
    QString txt;
    for(int i=0;i<m_pImgInFlash;i++){
        dmd->VarExpPatSeqAddPatToLut(i,INTERNAL,txt,boxExp->value(),boxPrd->value());
    }
    return 0;
}

int ViewDLP4500Scan::dmdAddPattern(int start, int stop)
{
    QString txt;
    for(int i=start;i<stop;i++){
        dmd->VarExpPatSeqAddPatToLut(i,INTERNAL,txt,boxExp->value(),boxPrd->value());
    }
    return 0;
}

int ViewDLP4500Scan::dmdSendPattern()
{
    setProcessLabel("传输pattern");
    return dmd->VarExpPatSeqSendLUT();
}

int ViewDLP4500Scan::dmdImgProcess()
{
    int res = 0;
    m_pImgInFlash = dmdLoadFlashImg();
    labelImgInFlash->setText(QString::number(m_pImgInFlash));
    res = dmdAddPatternAll();

//    if(checkFrameAll->isChecked())
//        res = dmdAddPatternAll();
//    else{
//        res = dmdAddPattern(boxFrameStart->value(),boxFrameStop->value());
//    }
    if(res){
        return -1;
    }
    res = dmdSendPattern();
    if(res){
        return -1;
    }
    res = dmdValidate();
    if(res){
        return -1;
    }
    return 0;
}

int ViewDLP4500Scan::processScanData()
{
//    scanImgSave();
    double pos = pzt->getPos(0);
    m_pImg.save(QDir(m_pScanDirPathCur).filePath(QString::number(m_pScanRowCur)+"-"+
                QString::number(m_pScanRowCur)+".png"),"PNG");
    //单列扫描结束判据
    if(m_pScanColumnCur == boxColumn->value()){
        m_pScanColumnCur = 0;
        m_pScanRowCur++;
    }
    else
        m_pScanColumnCur++;
    //单面扫描结束判据
    if(m_pScanRowCur>boxRow->value()){
        m_pScanColumnCur = 0;
        m_pScanRowCur = 0;
        if(pos>=boxStop->value()-0.05)
        {
            stopScan();
            return 0;
        }
        pzt->setPos(0,pos+boxStep->value());
        m_pScanDirPathCur = QDir(m_pScanDirPathRaw).filePath(QString::number(pos+boxStep->value(),'f',3));
        QDir().mkdir(m_pScanDirPathCur);
    }
    return 0;
}

int ViewDLP4500Scan::startScan()
{
    dmd->PatSeqCtrlStart();
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<int(boxPrd->value()/30000))
        QCoreApplication::processEvents();
    connect(&timerScan,&QTimer::timeout,this,&ViewDLP4500Scan::onTimerScanTimeout);
    timerScan.setInterval(boxPrd->value()/1000);
    timerScan.start();
    return 0;
}

int ViewDLP4500Scan::stopScan()
{
    timerScan.stop();
    dmd->PatSeqCtrlStop();
    m_pIsScan = false;
    return 0;
}

int ViewDLP4500Scan::scriptScan()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString imgDirPath = appDir+"/DMDScanIMG";
    QDir imgDir(imgDirPath);
    QFileInfoList folderList = imgDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot,QDir::Name);
    foreach (const QFileInfo &fileInfo, folderList){
        if(!checkSpain->isChecked()){
            if(fileInfo.fileName().contains(".")){
                continue;
            }
        }
        else
        {
            if(fileInfo.fileName().contains("-")){
                continue;
            }
            emit sigInfo("SET POS DISPLAY PZT:"+fileInfo.fileName());
        }
        QString dirPath = fileInfo.filePath();
        QDir imgDirSec(dirPath);
        QStringList fileNames = imgDirSec.entryList(QDir::Files | QDir::NoDotAndDotDot);
        QString imgPath;
        QElapsedTimer timer;
        labelProgessCur->setText("当前数字掩膜序号:"+fileInfo.fileName());
//        progressTest->setRange(0,fileNames.count());
//        progressTest->setValue(0);
        for(int i=0;i<fileNames.count();i++){
            timer.restart();
            while(timer.elapsed()<boxPrd->value()/1000){
                QCoreApplication::processEvents();
            }
            imgPath =dirPath +"/"+fileNames.at(i);
            m_pImg.load(imgPath);
            if(lineSaveDir->text() != "")
                m_pImg.save(lineSaveDir->text()+"/"+fileNames.at(i));
//            labelImgCam->setPixmap(QPixmap::fromImage(m_pImg.scaled(labelImgCam->width(),labelImgCam->height())));
            QStringList pzts = fileNames.at(i).split(".");
//            progressTest->setValue(i+1);
            pzts.removeLast();
            if(!checkSpain->isChecked()){
                emit sigInfo("SET POS DISPLAY PZT:"+pzts.join('.'));
            }
            if(m_pIsStop){
                m_pIsStop = false;
                return 0;
            }

        }
    }

    return 0;
}

int ViewDLP4500Scan::scriptScanTest()
{
    dmd->PatSeqCtrlStart();
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<30000){
        QCoreApplication::processEvents();
    }
    dmd->PatSeqCtrlStop();
    return 0;
}

int ViewDLP4500Scan::singleHeightTest()
{
    for(int i=0;i<boxRow->value();i++){
        for(int j=0;j<boxColumn->value();j++){

        }
    }
    return 0;
}

int ViewDLP4500Scan::scanImgSave()
{
    m_pImg.save(lineSaveDir->text()+"/"+QString::number(m_pScanRowCur)
                +"_"+QString::number(m_pScanColumnCur)+".png");
    return 0;
}

void ViewDLP4500Scan::testScriptTp1(int interval, int pztint)
{
    //烤机测试，前提是DMD和相机和PZT连上了
//    listLog->addItem("测试开始");
    if(timerCamAcq.isActive()){
        onBtnCam();
    }
    m_pImgInFlash = dmdLoadFlashImg();
    labelImgInFlash->setText(QString::number(m_pImgInFlash));
    m_pIsStop = false;
    m_pScanRowCur = 1;
    m_pScanColumnCur = 1;
    lockUI(true);
    QString txt;
    for(int i=0;i<m_pImgInFlash;i++){
        dmd->VarExpPatSeqAddPatToLut(i,INTERNAL,txt,interval*1000,interval*1000);
    }
//    listLog->addItem("图片已加载");
    int res = 0;
    res = dmdSendPattern();
    res = dmdValidate();
    dmd->PatSeqCtrlStart();
//    listLog->addItem("投影启动");
    m_pTestTp = 1;
    m_pTest1pztTime = m_pImgInFlash;
    m_pTest1PztCur = 0;
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<int(interval/20))
        QCoreApplication::processEvents();
    connect(&timerTest,&QTimer::timeout,this,&ViewDLP4500Scan::onTimerTestTimeout);
    timerTest.setInterval(interval);
    timerTest.start();
}

void ViewDLP4500Scan::stopTest()
{
    timerTest.stop();
    disconnect(&timerTest,&QTimer::timeout,this,&ViewDLP4500Scan::onTimerTestTimeout);
    if(m_pTestTp == 1){
        dmd->PatSeqCtrlStop();
        lockUI(false);
    }
    m_pTestTp = 0;

}

void ViewDLP4500Scan::processTest()
{
    if(m_pTestTp == 1){
        if(m_pTest1PztCur == m_pTest1pztTime-1){
            m_pTest1PztCur = 0;
            double pos = pzt->getPos(0);
            if(pos>170){
                pzt->setPos(0,0);
            }
            else {
                pzt->setPos(0,pos+1);
            }
        }
        else
            m_pTest1PztCur++;
    }
}

void ViewDLP4500Scan::lockUI(bool lock)
{
    btnCam->setDisabled(lock);
//    btnInit->setDisabled(lock);
    btnStart->setDisabled(lock);
    btnStop->setDisabled(lock);

}

void ViewDLP4500Scan::startScanV20()
{
    if(checkDevStatus())
        return;
    emit sigInfo("硬件设备检测完毕");
    emit sigInfo("业务状态检测完成");
    emit sigInfo("参数检测完成");
    m_pScanRowCur = 1;
    m_pScanColumnCur = 1;
    pzt->setPos(0,boxStart->value());
    dmdImgProcess();
    emit sigInfo("DMD启动，检测开始:"+QDateTime::currentDateTime().toString());
    m_pProgressCur = 0;
    startScan();
}

void ViewDLP4500Scan::stopScanV20()
{
    dmd->PatSeqCtrlStop();
    timerScan.stop();
    timerV30.stop();
}

int ViewDLP4500Scan::checkDevStatus()
{
//    if(!m_pCamReady){
//        QMessageBox::warning(this,"设备未就绪","相机未就绪");
//        return -1;
//    }
//    if(!m_pDMDReady){
//        QMessageBox::warning(this,"设备未就绪","DMD未就绪");
//        return -1;
//    }
//    if(!m_pPZTReady){
//        QMessageBox::warning(this,"设备未就绪","PZT未就绪");
//        return -1;
//    }
    return 0;
}

int ViewDLP4500Scan::askAvailable()
{
    m_pMainWdCmdBack = false;
    emit sigRegistService("DLP4500Scan");
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<10000){
        if(m_pMainWdCmdBack){
            if(m_pMainWdCmd == "Service Regist Success"){
                m_pMainWdCmdBack = false;
                return 0;
            }
            else{
                QMessageBox::warning(this,"业务占用","当前业务占用："+m_pMainWdCmd);
            }
        }
        QCoreApplication::processEvents();
    }
    QMessageBox::warning(this,"应答超时","内部应答超时");
    return -1;
}

int ViewDLP4500Scan::checkParameterScan()
{
    setProcessLabel("检查文件夹地址");
    //检查文件夹地址
    if(lineSaveDir->text() == ""){
        QMessageBox::warning(this,"缺少参数","缺少保存文件夹");
        return -1;
    }
    else{
//        m_pScanDirPathRaw = QDir(lineSaveDir->text()).filePath(QDateTime::currentDateTime().toString());
//        QDir().mkdir(m_pScanDirPathRaw);
//        m_pScanDirPathCur = QDir(m_pScanDirPathRaw).filePath(QString::number(boxStart->value()));
//        QDir().mkdir(m_pScanDirPathCur);
    }

    //检查行列输入参数
    setProcessLabel("检查行列输入参数");
    int frameNum = 0;
    if(checkFrameAll->isChecked())
        frameNum = m_pImgInFlash;
    else
        frameNum = boxFrameStop->value()-boxFrameStart->value();
    if(frameNum <= 0){
        QMessageBox::warning(this,"参数错误","无内存照片或照片起止索引错误");
        return -1;
    }
    if(frameNum<boxRow->value()*boxColumn->value()){
        QMessageBox::warning(this,"参数错误","行列相乘总照片数大于内存照片数");
        return -1;
    }
    if(boxStop->value()-boxStart->value()<=0){
        QMessageBox::warning(this,"参数错误","步进次数为负");
        return -1;
    }
    m_pProgressCount = frameNum*int((boxStop->value()-boxStart->value())/boxStep->value());
    return 0;
}

int ViewDLP4500Scan::processScanImg()
{

    if(!lineSaveDir->text().isEmpty()){
        m_pImg.save(lineSaveDir->text()+"/"+QString::number(m_pScanRowCur)
                    +"_"+QString::number(m_pScanColumnCur)+".png");
    }
    double pos = pzt->getPos(0);
    m_pProgressCur++;

    //单列扫描判断if：单列扫描完，else：未扫描完
    if(m_pScanColumnCur == boxColumn->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur++;
        emit sigInfo("DMD启动第"+QString::number(m_pScanRowCur)+"行开始："+QDateTime::currentDateTime().toString());
    }
    else{
        m_pScanColumnCur++;
    }

    //单次DMD循环结束
    if(m_pScanRowCur>boxRow->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur = 1;
        if(pos >= boxStop->value()-0.05){
            emit sigInfo("DMD启动，检测结束:"+QDateTime::currentDateTime().toString());
            stopScanV20();
            return 0;
        }
        pzt->setPos(0,pos+boxStep->value());
        m_pScanDirPathCur = QDir(m_pScanDirPathRaw).filePath(QString::number(pos+boxStep->value(),'f',3));
        emit sigInfo("m_pScanDirPathCur:"+m_pScanDirPathCur);
        QDir().mkdir(m_pScanDirPathCur);
        lineSaveDir->setText(m_pScanDirPathCur);
    }
    return 0;
}

void ViewDLP4500Scan::startScanV30()
{
    setProcessLabel("初始化");
    refreshParas();
    setProcessLabel("加载DMD内存");
    m_pImgInFlash = dmdLoadFlashImg();
    labelImgInFlash->setText(QString::number(m_pImgInFlash));
    int ret = checkDataV30();
    if(ret){
        return;
    }
    setProcessLabel("检查硬件连接");
    emit sigCheckDev("ViewDLP4500Scan");
    ret = loopV30.exec();
    if(ret){
        return;
    }
    lockUIV30();
    ret = move2Start();
    if(ret){
        return;
    }
    int steps = int((boxStop->value()-boxStart->value())/boxStep->value());
    m_pProgressCount = steps*m_pImgInFlash;
    m_pProcessVidCount = countVidFile("./process_vid/tree");
    ret = startDMDV30();
    if(ret){
        return;
    }

    emit sigServiceReady(lineSaveDir->text());
    loopV30.exec();

    timerV30.setInterval(boxPrd->value()/1000);
    dmd->PatSeqCtrlStart();
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<int(boxPrd->value()/30000))
        QCoreApplication::processEvents();

    setProcessLabel("当前扫描第1行");
    onTimerScanV30Timeout();
    timerV30.start();
    emit sigInfo("DMD启动，检测开始:"+QDateTime::currentDateTime().toString());
}

int ViewDLP4500Scan::checkDataV30()
{
    int ret = checkParameterScan();
    return ret;
}

QStringList ViewDLP4500Scan::setParaStrList()
{
    QStringList para;
    return para;
}

void ViewDLP4500Scan::lockUIV30()
{
    boxStart->setEnabled(false);
    boxStop->setEnabled(false);
    boxStep->setEnabled(false);
}

void ViewDLP4500Scan::unlockUIV30()
{
    boxStart->setEnabled(true);
    boxStop->setEnabled(true);
    boxStep->setEnabled(true);
}

int ViewDLP4500Scan::move2Start()
{
    setProcessLabel("运动到初始位置");
    pzt->setPos(0,boxStart->value());
    return 0;
}

int ViewDLP4500Scan::startDMDV30()
{
    setProcessLabel("配置开始DMD");
    int res = 0;
    res = dmdAddPatternAll();
    if(res){
        return -1;
    }
    res = dmdSendPattern();
    if(res){
        return -1;
    }
    res = dmdValidate();
    if(res){
        return -1;
    }
    return 0;
}

int ViewDLP4500Scan::stopScanV30()
{
    timerV30.stop();
    dmd->PatSeqCtrlStop();
    emit sigStopScan();
    setProcessLabel("检测结束");
    emit sigInfo("检测结束"+QDateTime::currentDateTime().toString());
    return 0;
}

int ViewDLP4500Scan::countVidFile(const QString &dirPath)
{
    QDir imgDir(dirPath);
    if (!imgDir.exists()) {
        qDebug() << "错误：图片文件夹不存在！路径：" << dirPath;
        return 0;
    }
    // 2. 只统计文件（排除目录），直接获取文件数量
    int imgCount = imgDir.entryList(QDir::Files).count();
    return imgCount;
}

void ViewDLP4500Scan::setProcessLabel(QString text)
{
    labelProgessCur->setText(text);
}

void ViewDLP4500Scan::setProgessVid(int value)
{
    QString dirPath = "./process_vid/rocket";
    int index = int(m_pProcessVidCount*value/m_pProgressCount);
    QString imgPath = dirPath + "/" +"frame_";
    if(index<10){
        imgPath += "000"+QString::number(index)+".jpg";
    }
    else if(index<100){
        imgPath += "00"+QString::number(index)+".jpg";
    }
    else if(index<1000){
        imgPath += "0"+QString::number(index)+".jpg";
    }
//    processWid->updateImage(QImage(imgPath));
}

void ViewDLP4500Scan::refreshParas()
{
    m_pProgressCur = 0;
    m_pScanRowCur = 0;
    m_pScanColumnCur = 0;
}
#pragma execution_character_set(pop)
