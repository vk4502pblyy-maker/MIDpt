#include "viewdlpc350scan.h"
#include <QLayout>
#include "IO/txtio.h"
#include <QFileDialog>

#pragma execution_character_set(push, "utf-8")
ViewDLPC350Scan::ViewDLPC350Scan(QWidget *parent)
    : QWidget{parent}
{
    dlp = FuncDLP3500Ctl::getInstance();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    QVBoxLayout *layout1 = new QVBoxLayout;
    btnConnect = new QPushButton("连接");
    btnSetMode = new QPushButton("设置Pattern Sequence [Variable Exposure]及参数");
    btnUpload = new QPushButton("上传图片");
    spImg = new QSpinBox();
    spImg->setRange(0,0);
    btnAddPattern = new QPushButton("Add Pattern to Sequence");
    btnSendPattern = new QPushButton("send");
    btnValidateSeq = new QPushButton("Validate Sequence");
    btnPlay = new QPushButton("play");
    btnStop = new QPushButton("stop");
    btnTest = new QPushButton("Test");
    btnclear = new QPushButton("清除文字区");
    listLog = new QListWidget;
    layout1->addWidget(btnConnect);
    layout1->addWidget(btnSetMode);
    layout1->addWidget(btnUpload);
    layout1->addWidget(spImg);
    layout1->addWidget(btnAddPattern);
    layout1->addWidget(btnSendPattern);
    layout1->addWidget(btnValidateSeq);
    layout1->addWidget(btnPlay);
    layout1->addWidget(btnStop);
    layout1->addWidget(btnTest);
    layout1->addWidget(btnclear);
    layout1->addWidget(listLog,1);
    mainLayout->addLayout(layout1);
    connect(btnConnect,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnConnect);
    connect(btnAddPattern,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnAddPattern);
    connect(btnSendPattern,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnSendPattern);
    connect(btnPlay,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnPlay);
    connect(btnSetMode,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnSetMode);
    connect(btnStop,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnStop);
    connect(btnTest,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnTest);
    connect(btnUpload,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnUpload);
    connect(btnValidateSeq,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnValidateSeq);
    connect(btnclear,&QPushButton::clicked,this,&ViewDLPC350Scan::onBtnClear);
    connect(dlp,&FuncDLP3500Ctl::sigInfo,this,&ViewDLPC350Scan::onDLPInfo);
    connect(dlp,&FuncDLP3500Ctl::sigErr,this,&ViewDLPC350Scan::onDLPInfo);

}

void ViewDLPC350Scan::onBtnConnect()
{
    dlp->connectDMD();
}

void ViewDLPC350Scan::onBtnSetMode()
{
    dlp->setMode(FuncDLP3500Ctl::PATTERN_SEQUENCE_VE,FuncDLP3500Ctl::FLASH);
    int num = dlp->getFlasImages();
    spImg->setRange(0,num);
}

void ViewDLPC350Scan::onBtnUpload()
{
//    QString txtPath = "";
//    m_pListImg = TxtIO::ReadATxt(txtPath);
//    foreach (QString s, m_pListImg) {
//        listLog->addItem(s);
//    }
//    dlp->loadImages(m_pListImg.at(0));
    QUrl fileUrl = QFileDialog::getOpenFileUrl();
    dlp->loadImages(fileUrl.toString());
}

void ViewDLPC350Scan::onBtnAddPattern()
{
    int frameIndex = spImg->value();
    QString txt;
    dlp->VarExpPatSeqAddPatToLut(frameIndex,INTERNAL,txt);
    listLog->addItem(txt);
}

void ViewDLPC350Scan::onBtnSendPattern()
{
    dlp->VarExpPatSeqSendLUT();
}

void ViewDLPC350Scan::onBtnValidateSeq()
{
    dlp->ValidatePatSeq();
}

void ViewDLPC350Scan::onBtnPlay()
{
    dlp->PatSeqCtrlStart();
}

void ViewDLPC350Scan::onBtnStop()
{
    dlp->PatSeqCtrlStop();
}

void ViewDLPC350Scan::onBtnTest()
{
    int num = dlp->getFlasImages();
    spImg->setRange(0,num);
}

void ViewDLPC350Scan::onBtnClear()
{
    listLog->clear();
}

void ViewDLPC350Scan::onDLPInfo(QString msg)
{
    listLog->addItem(msg);
    if(msg.contains("DLPC350_GetNumImagesInFlashs")){
        spImg->setRange(0,msg.split(':').last().toInt()-1);
    }
}

#pragma execution_character_set(pop)
