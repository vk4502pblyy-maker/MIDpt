#include "viewdlpscan.h"
#include <QLayout>
#include <QFileDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QFrame>

#pragma execution_character_set(push, "utf-8")
ViewDLPScan::ViewDLPScan(QWidget *parent)
    : QWidget{parent}
{
    m_pStyle = "gbl";
    m_pcamInit = false;

    dmd = FuncDLP3500Ctl::getInstance();
    pzt = FuncPZTCtl::getInstance();

    if(m_pStyle == "N"){
        btnCam = new EleIconBtn(":/icon/resource/icon-N/camera.png",64,64,"打开实时相机");
        btnCam->setStatusIcon({":/icon/resource/icon-N/camera.png",
                              ":/icon/resource/icon-N/camLive.png"},
                              {"打开观测","关闭观测"},true,true);
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
//        btnStart = new EleIconBtn(":/icon/resource/icon-N/radar.png",64,64,"一键启动");
        btnStop = new EleIconBtn(":/icon/resource/icon-N/stop.png",64,64,"停止检测");
        btnAlbum = new PBtnSingleShot(":/icon/resource/icon-N/album-off.png",128,128,
                                      "查找图片",":/icon/resource/icon-N/album.png");
    }
    else if(m_pStyle.contains("gbl")){
        btnCam = new EleIconBtn(":/icon/resource/icon-gbl/camera.png",64,64,"拍照并保存");
        btnTriggerE = new EleIconBtn(":/icon/resource/icon-gbl/triggerEx.png",64,64,"外触发");
        btnTriggerE->setToggleBtn(true);
        btnTriggerI = new EleIconBtn(":/icon/resource/icon-gbl/triggerIn.png",64,64,"内触发");
        btnTriggerI->setToggleBtn(true);
        labelImgInFlash = new QLabel("0");
        btnSetMin = new EleIconBtn(":/icon/resource/icon-gbl/currentPos.png",64,64,"设为当前位置为起点");
        btnSetMax = new EleIconBtn(":/icon/resource/icon-gbl/currentPos.png",64,64,"设为当前位置为终点");
        btnRecipeGen = new EleIconBtn(":/icon/resource/icon-gbl/saveDoc.png",64,64,"保存当前配置");
        btnRecipeLoad = new EleIconBtn(":/icon/resource/icon-gbl/loadDoc.png",64,64,"加载配置");
        btnImgBackward = new EleIconBtn(":/icon/resource/icon-gbl/left-arr.png",128,128,"上一张照片");
        btnImgForward = new EleIconBtn(":/icon/resource/icon-gbl/right-arr.png",128,128,"下一张照片");
        btnDir = new EleIconBtn(":/icon/resource/icon-gbl/openDir.png",64,64,"浏览文件夹");
        btnResultCalculate = new EleIconBtn(":/icon/resource/icon-gbl/analysis.png",128,128,"分析结果");
//        btnStart = new EleIconBtn(":/icon/resource/icon-gbl/radar-off.png",64,64,"一键启动");
//        btnStart->setStatusIcon({":/icon/resource/icon-gbl/radar-off.png",
//                              ":/icon/resource/icon-gbl/radar.png"},
//                              {"打开观测","关闭观测"},true,true);
//        btnStop = new EleIconBtn(":/icon/resource/icon-gbl/stop-off.png",64,64,"停止检测");
//        btnStop->setHoverIcon(":/icon/resource/icon-gbl/stop.png");
        btnStart = new PBtnToggle(":/icon/resource/icon-gbl/radar.png",":/icon/resource/icon-gbl/radar-off.png"
                                  ,128,128,"一键启动","一键停止");
        btnStop = new EleIconBtn(":/icon/resource/icon-gbl/stop-off.png",64,64,"停止检测");
        btnStop->setHoverIcon(":/icon/resource/icon-gbl/stop.png");
        btnStop->setVisible(false);
        btnAlbum = new PBtnSingleShot(":/icon/resource/icon-gbl/album-off.png",128,128,
                                      "查找图片",":/icon/resource/icon-gbl/album.png");
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
    layout21->addWidget(btnTriggerE);
    layout21->addWidget(btnTriggerI);
    layout21->addWidget(btnCam);
    layout21->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
    layout21->addWidget(new QLabel("内存图片数量: "));
    layout21->addWidget(labelImgInFlash);

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
    layout25->addWidget(new QLabel("步进"));
    layout25->addWidget(boxStep,1);
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

    QHBoxLayout *layout26 = new QHBoxLayout();
    layout2->addLayout(layout26);
    lineSaveDir = new QLineEdit;
    layout26->addWidget(new QLabel("保存位置"));
    layout26->addWidget(lineSaveDir,1);
    layout26->addWidget(btnDir);

    QFrame *line1 = new QFrame(this);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    layout2->addWidget(line1);

    QHBoxLayout *layout27 = new QHBoxLayout;
    layout2->addLayout(layout27);
    layout27->addWidget(btnStart);
    layout27->addWidget(btnStop);
    layout27->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
    labelProgessCur = new QLabel("未开始");
    layout27->addWidget(new QLabel("当前进度："));
    layout27->addWidget(labelProgessCur);

    spacerWid = new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding);
    layout2->addItem(spacerWid);

    QHBoxLayout *layout210 = new QHBoxLayout;
    layout2->addLayout(layout210);
    layout210->addWidget(btnAlbum);
    layout210->addWidget(btnImgBackward);
    layout210->addWidget(btnImgForward);
    layout210->addWidget(btnResultCalculate);

    connect(btnCam,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnCam);
    connect(btnDir,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnDir);
    connect(btnSetMax,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnSetMax);
    connect(btnSetMin,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnSetMin);
    connect(btnTriggerE,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnTriggerE);
    connect(btnTriggerI,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnTriggerI);
    connect(btnStart,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnStart);
    connect(btnStop,&EleIconBtn::clicked,this,&ViewDLPScan::onBtnStop);
    connect(btnAlbum,&PBtnSingleShot::clicked,this,&ViewDLPScan::onBtnAlbum);

    connect(boxColumn,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxExp,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxPrd,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxRow,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxFrameStart,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxFrameStop,QOverload<int>::of(&QSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChange);
    connect(boxStart,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChangeDouble);
    connect(boxStep,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChangeDouble);
    connect(boxStop,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&ViewDLPScan::onSpinValueChangeDouble);

    connect(&timerV30,&QTimer::timeout,this,&ViewDLPScan::onTimerScanV30Timeout);
}

ViewDLPScan::~ViewDLPScan()
{
    if(dmd->getInitFlag()){
        dmd->PatSeqCtrlStop();
    }
    if(loopV30.isRunning()){
        loopV30.quit();
    }
}

int ViewDLPScan::setPosMsg(QString type, QString posMsg)
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

int ViewDLPScan::setPZTPos(double pos)
{
    labelPosPZT->setText("PZT:"+QString::number(pos,'f',3)+";");
    return 0;
}

int ViewDLPScan::setPZTEnable()
{
    return 0;
}

void ViewDLPScan::move2Next()
{
    double pos = pzt->getPos(0);
    m_pProgressCur++;
    setProgessVid(m_pProgressCur);

    if(m_pScanColumnCur == boxColumn->value()){
        m_pScanColumnCur = 1;
        m_pScanRowCur++;
        emit sigInfo("DMD启动第"+QString::number(m_pScanRowCur)+"行开始："+QDateTime::currentDateTime().toString());
        setProcessLabel("当前在第："+QString::number(m_pScanRowCur)+"行");
    }
    else{
        m_pScanColumnCur++;
    }

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

void ViewDLPScan::checkDevResult(QString result)
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

void ViewDLPScan::startService()
{
    loopV30.exit(0);
}

void ViewDLPScan::onBtnInit()
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

void ViewDLPScan::onBtnCam()
{
    int status = btnCam->getStatus();
    if(status == 1){
        connect(&timerCamAcq,&QTimer::timeout,this,&ViewDLPScan::onCamAcqTimeout);
        timerCamAcq.setInterval(100);
        timerCamAcq.start();
    }
    else{
        timerCamAcq.stop();
        disconnect(&timerCamAcq,&QTimer::timeout,this,&ViewDLPScan::onCamAcqTimeout);
    }
}

void ViewDLPScan::onBtnDir()
{
    lineSaveDir->setText(QFileDialog::getExistingDirectory(this,"选择保存文件夹"));
}

void ViewDLPScan::onBtnSetMin()
{
    boxStart->setValue(pzt->getPos(0));
}

void ViewDLPScan::onBtnSetMax()
{
    boxStop->setValue(pzt->getPos(0));
}

void ViewDLPScan::onBtnStart()
{
    startScanV30();
}

void ViewDLPScan::onBtnStop()
{
    emit sigInfo("用户中止检测");
    stopScanV30();
}

void ViewDLPScan::onSpinValueChange(int value)
{
    QSpinBox *box = qobject_cast<QSpinBox*>(sender());
    box->setValue(value);
}

void ViewDLPScan::onSpinValueChangeDouble(double value)
{
    QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox*>(sender());
    box->setValue(value);
}

void ViewDLPScan::onCamImgBack(int camNum, QImage img)
{
    Q_UNUSED(camNum)
    m_pImg = img;
    if(btnCam->getStatus() == 1)
        timerCamAcq.start(50);
}

void ViewDLPScan::onCamAcqTimeout()
{
    emit sigCamImg(0);
}

void ViewDLPScan::onBtnTriggerE()
{
    if((btnTriggerE->getToggleStatus() == true) &&
            (btnTriggerI->getToggleStatus() == true)){
        btnTriggerI->changeToggleStatus();
    }
}

void ViewDLPScan::onBtnTriggerI()
{
    if((btnTriggerI->getToggleStatus() == true) &&
            (btnTriggerE->getToggleStatus() == true)){
        btnTriggerE->changeToggleStatus();
    }
}

void ViewDLPScan::onCamInit()
{
    m_pcamInit = true;
}

void ViewDLPScan::onBtnAlbum()
{
    if(imgWid == nullptr){
        imgWid = new RawImgWidget(this);
        // 1. 定义过滤器：格式为 "描述文字 (*.后缀1 *.后缀2)"
            // 如果有多个过滤器，用两个分号 ";;" 隔开
            QString filter = "RAW Files (*.raw *.RAW);;All Files (*.*)";

            // 2. 弹出文件选择对话框
            // 参数：父窗口, 标题, 默认打开路径, 过滤器
            QString selectedFile = QFileDialog::getOpenFileName(
                this,
                "选择一个RAW文件",
                "/",
                filter,
                nullptr, // selectedFilter (如果不关心具体匹配了哪个过滤器，填 nullptr)
                QFileDialog::DontUseNativeDialog // <--- 加上这个选项！
            );

            // 3. 检查用户是否取消了选择
            if (selectedFile.isEmpty()) {
                qDebug() << "用户取消了选择";
                return;
            }
        imgWid->setFrameWH(2448,2048);
        imgWid->setFilePath(selectedFile);
    }
    imgWid->show();

}

void ViewDLPScan::onTimerScanV30Timeout()
{
    emit sigGrab();
}

int ViewDLPScan::dmdInit()
{
    return dmd->connectDMD();
}

int ViewDLPScan::dmdSetMode()
{
    return dmd->setMode(FuncDLP3500Ctl::PATTERN_SEQUENCE_VE,FuncDLP3500Ctl::FLASH);
}

int ViewDLPScan::dmdSetPattern()
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

int ViewDLPScan::dmdValidate()
{
    setProcessLabel("DMD使能");
    return dmd->ValidatePatSeq();
}

int ViewDLPScan::dmdLoadFlashImg()
{
    return dmd->getFlasImages();
}

int ViewDLPScan::dmdAddPatternAll()
{
    setProcessLabel("加载全部pattern");
    QString txt;
    for(int i=0;i<m_pImgInFlash;i++){
        dmd->VarExpPatSeqAddPatToLut(i,INTERNAL,txt,boxExp->value(),boxPrd->value());
    }
    return 0;
}

int ViewDLPScan::dmdSendPattern()
{
    setProcessLabel("传输pattern");
    return dmd->VarExpPatSeqSendLUT();
}

int ViewDLPScan::checkParameterScan()
{
    setProcessLabel("检查文件夹地址");
    if(lineSaveDir->text() == ""){
        QMessageBox::warning(this,"缺少参数","缺少保存文件夹");
        return -1;
    }

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

void ViewDLPScan::startScanV30()
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
    emit sigCheckDev("ViewDLPScan");
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

int ViewDLPScan::checkDataV30()
{
    int ret = checkParameterScan();
    return ret;
}

QStringList ViewDLPScan::setParaStrList()
{
    QStringList para;
    return para;
}

void ViewDLPScan::lockUIV30()
{
    boxStart->setEnabled(false);
    boxStop->setEnabled(false);
    boxStep->setEnabled(false);
}

void ViewDLPScan::unlockUIV30()
{
    boxStart->setEnabled(true);
    boxStop->setEnabled(true);
    boxStep->setEnabled(true);
}

int ViewDLPScan::move2Start()
{
    setProcessLabel("运动到初始位置");
    pzt->setPos(0,boxStart->value());
    return 0;
}

int ViewDLPScan::startDMDV30()
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

int ViewDLPScan::stopScanV30()
{
    timerV30.stop();
    dmd->PatSeqCtrlStop();
    emit sigStopScan();
    setProcessLabel("检测结束");
    emit sigInfo("检测结束"+QDateTime::currentDateTime().toString());
    return 0;
}

int ViewDLPScan::countVidFile(const QString &dirPath)
{
    QDir imgDir(dirPath);
    if (!imgDir.exists()) {
        qDebug() << "错误：图片文件夹不存在！路径：" << dirPath;
        return 0;
    }
    int imgCount = imgDir.entryList(QDir::Files).count();
    return imgCount;
}

void ViewDLPScan::setProcessLabel(QString text)
{
    labelProgessCur->setText(text);
}

void ViewDLPScan::setProgessVid(int value)
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
}

void ViewDLPScan::refreshParas()
{
    m_pProgressCur = 0;
    m_pScanRowCur = 0;
    m_pScanColumnCur = 0;
}
#pragma execution_character_set(pop)
