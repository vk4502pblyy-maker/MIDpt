#include "rawimgwidget.h"
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>

RawImgWidget::RawImgWidget(QWidget *parent) : QDialog(parent)
{
    m_style = "gbl";
    btnPlay = new PBtnSingleShot(":/icon/resource/icon-"+m_style+"/play.png",64,64,"播放图片");
    btnSave = new PBtnSingleShot(":/icon/resource/icon-"+m_style+"/saveDoc.png",64,64,"保存图片");
    btnAlbum = new PBtnSingleShot(":/icon/resource/icon-"+m_style+"/album.png",64,64,"切换相册");
    btnForward = new PBtnSingleShot(":/icon/resource/icon-"+m_style+"/right-arr.png",64,64,"保存图片");
    btnBackward = new PBtnSingleShot(":/icon/resource/icon-"+m_style+"/left-arr.png",64,64,"保存图片");
    boxIndex = new QSpinBox;
    boxIndex->setRange(0,1);
    videoImage = new VideoWidget(this);
    videoImage->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(videoImage);
    QHBoxLayout *layout1 = new QHBoxLayout;
    mainLayout->addLayout(layout1);
    layout1->addWidget(btnAlbum);
    layout1->addWidget(btnBackward);
    layout1->addWidget(btnForward);
    layout1->addWidget(new QLabel("跳转至"));
    layout1->addWidget(boxIndex);
    layout1->addWidget(btnPlay);
    layout1->addWidget(btnSave);

    connect(btnAlbum,&PBtnSingleShot::clicked,this,&RawImgWidget::onBtnAlbum);
    connect(btnSave,&PBtnSingleShot::clicked,this,&RawImgWidget::onBtnSave);
    connect(btnPlay,&PBtnSingleShot::clicked,this,&RawImgWidget::onBtnPlay);
    connect(btnForward,&PBtnSingleShot::clicked,this,&RawImgWidget::onBtnForward);
    connect(btnBackward,&PBtnSingleShot::clicked,this,&RawImgWidget::onBtnBackward);

    connect(&showTimer,&QTimer::timeout,this,&RawImgWidget::onShowTimerOut);
    this->resize(700,800);

}

void RawImgWidget::setFilePath(QString filePath)
{
    m_filePath = filePath;
}

void RawImgWidget::setFrameWH(int width, int height)
{
    m_frameWidth = width;
    m_frameHeight = height;
}

void RawImgWidget::showEvent(QShowEvent *)
{
    if(!m_frameWidth || !m_frameHeight){
        return;
    }
    double whRate = double(m_frameWidth/m_frameHeight);
    int maxWidth = 1800;
    int maxHeight = 900;
    int w = 800;
    int h = 800;
    if(whRate>2.25){
        w = maxWidth;
        h = int(w/whRate);
    }
    else{
        h = maxHeight;
        w = int(h*whRate);
    }
    this->resize(w,h);
    showTimer.setSingleShot(true);
    showTimer.start(20);
}

void RawImgWidget::closeEvent(QCloseEvent *event)
{
    // 阻止默认关闭行为（默认会销毁窗口）
    event->ignore();
    rawReader->close();
    // 改为隐藏窗口
    hide();
    // 可选：发送信号通知主窗口
}

void RawImgWidget::initRawReader(QString filePath)
{
    rawReader->open(filePath);
    m_ImgCount = rawReader->getTotalFrames();
    if(m_ImgCount == 0){
        return;
    }
    boxIndex->setRange(0,int(m_ImgCount-1));
    boxIndex->setValue(0);
    if(this->isVisible() && !this->isMinimized()){
        videoImage->updateImage(rawReader->getFrame(0));
    }
}

void RawImgWidget::refreshRawReader(QString filePath)
{
    rawReader->open(filePath);
    if(m_ImgCount == 0){
        return;
    }
    videoImage->updateImage(rawReader->getFrame(boxIndex->value()));
}

void RawImgWidget::onShowTimerOut()
{
    if(rawReader == nullptr){
        rawReader = new RawReader(this);
        rawReader->setResolution(m_frameWidth,m_frameHeight);
        initRawReader(m_filePath);
    }
    else{
        refreshRawReader(m_filePath);
    }
}

void RawImgWidget::onBtnAlbum()
{
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
    m_filePath = selectedFile;
    initRawReader(selectedFile);
}

void RawImgWidget::onBtnForward()
{
    if(boxIndex->value() == boxIndex->maximum()-1){
        QMessageBox::information(this,"图片索引","已到达最后一张");
        return;
    }
    boxIndex->setValue(boxIndex->value()+1);
    videoImage->updateImage(rawReader->getFrame(boxIndex->value()));
}

void RawImgWidget::onBtnBackward()
{
    if(boxIndex->value() == 0){
        QMessageBox::information(this,"图片索引","已到达第一张");
        return;
    }
    boxIndex->setValue(boxIndex->value()-1);
    videoImage->updateImage(rawReader->getFrame(boxIndex->value()));
}

void RawImgWidget::onBtnSave()
{
    // 0. 检查图像是否有效 (防止保存空图像导致崩溃)
        // 假设你的 QImage 变量名为 m_currentImage
    QImage m_currentImage = rawReader->getFrame(boxIndex->value());
        if (m_currentImage.isNull()) {
            QMessageBox::warning(this, "警告", "当前没有可保存的图像数据！");
            return;
        }

        // 1. 定义保存格式的过滤器
        // 用户在对话框的下拉菜单中可以选择这些格式
        QString filter = "PNG 图片 (*.png);;JPEG 图片 (*.jpg *.jpeg);;BMP 图片 (*.bmp)";

        // 2. 弹出“另存为”对话框
        // 参数：父窗口, 标题, 默认保存路径和默认文件名, 过滤器
        QString savePath = QFileDialog::getSaveFileName(
            this,
            "保存图片",
            "C:/捕获图片.png",  // 你可以留空 "/"，或者给一个默认生成的文件名
            filter
        );

        // 3. 判断用户是否点击了“取消”
        if (savePath.isEmpty()) {
            qDebug() << "用户取消了保存操作";
            return;
        }

        // 4. 保存图像
        // QImage 的 save() 函数非常聪明，它会自动读取 savePath 里的后缀名（如 .jpg 或 .png），
        // 并以对应的编码格式进行压缩和保存。
        bool isSaved = m_currentImage.save(savePath);

        // 5. 给予用户反馈
        if (isSaved) {
            qDebug() << "图片已成功保存到:" << savePath;
            QMessageBox::information(this, "保存成功", "图片已成功保存！");
        } else {
            qDebug() << "保存失败，路径:" << savePath;
            QMessageBox::critical(this, "保存失败", "图片保存失败，请检查目录权限或磁盘空间！");
        }
}

void RawImgWidget::onBtnPlay()
{
    videoImage->updateImage(rawReader->getFrame(boxIndex->value()));
}
