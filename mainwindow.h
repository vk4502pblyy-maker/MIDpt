#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "view/viewtitlemsgbox.h"
#include "view/viewdlp4500scan.h"
#include "view/viewfloatmovement.h"
#include "view/viewFloatConfig.h"
#include "view/viewdlpscan.h"
#include "function/funcmovewm.h"
#include "function/camerathread.h"
#include "view/videowidget.h"
#include "function/rawreader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 【新增】重写closeEvent，拦截窗口关闭事件，触发清理流程
    void closeEvent(QCloseEvent *event) override;

signals:
    void sigOpenCam(int camNum);
    void sigInitCam(int camNum);

    // 向工作线程发送控制指令的信号
    void sig_openCamera();
    void sig_closeCamera();
    void sig_startGrab();
    void sig_stopGrab();
    void sig_setGevSCPSPacketSize(int packetSize);
    void sigImgSave();
    void requestStopCapture();

private slots:
    void onInfo(QString msg);
    void onError(QString msg);
    void onFloatMovement();
    void onFloatConfig();
    void onViewCommand(QString msg);
    void onPZTPos(double pos);

    void onCamImgBack(int camNum,QImage img);
    void onRegistService(QString serviceName);
    void onMoveWMSerMsg(QString msg);

    void oncheckDevStatus(QString viewName);
    void onServiceReady(QString dirPath);
    void onServiceOver();

    // 相机状态/数据槽函数
    void onCameraOpened();
    void onCameraClosed();
    void onGrabStarted();
    void onGrabStopped();
    void onImgUpdateStopped();
    void onImgUpdateStarted();
    void onCameraDisconnected();
    void onErrorOccurred(const QString& errorMsg);
    void onImageReceived(const QImage& image);

    // 清理完成槽函数
    void onShutdownFinished();

    //viewDlp4500Scan
    void onDlp4500ScanPara(QString paraList);
    void onDlp4500ScanGrab();
    void onDlp4500ScanStop();

    void onCamExp(double exp);


private:
    void devInit();
    void sigslotInit();
    void initUI();

private:
    Ui::MainWindow *ui;
    ViewTitleMsgBox *viewTitleMsgBox;
    ViewDLP4500Scan *viewDLP4500Scan;
    ViewDLPScan     *viewDLPScan;
    ViewFloatMovement *viewFloatMovement;
    viewFloatConfig *viewConfigFloat;
    VideoWidget *m_videoWidget;

    FuncMoveWM  *funcMoveWM;
    RawReader *rawReader;
    CameraThread *m_cameraThread;
    QPixmap camPixmap;

    QString serviceCur;
    bool m_isShuttingDown = false;

    //viewDLP4500Scan
    QImage  camImgCur;
};
#endif // MAINWINDOW_H
