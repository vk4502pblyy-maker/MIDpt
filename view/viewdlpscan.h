#ifndef VIEWDLPSCAN_H
#define VIEWDLPSCAN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QListWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTimer>
#include <QLineEdit>
#include <QEventLoop>
#include <QSpacerItem>
#include "element/pbtnsingleshot.h"
#include "element/eleiconbtn.h"
#include "element/pbtntoggle.h"
#include "function/funcdlp3500ctl.h"
#include "function/funcpztctl.h"
#include "function/funcmovewm.h"

class ViewDLPScan : public QWidget
{
    Q_OBJECT
public:
    explicit ViewDLPScan(QWidget *parent = nullptr);
    ~ViewDLPScan();

    int setPosMsg(QString type,QString posMsg);
    int setPZTPos(double pos);
    int setPZTEnable();

    //新版扫描V30
    void move2Next();
    void checkDevResult(QString result);
    void startService();

public slots:
    void onCamImgBack(int camNum,QImage img);

signals:
    void sigCamImg(int CamNum);
    void sigInfo(QString msg);
    void sigInitCamera(int camNum);
    void sigRegistService(QString serviceName);

    //新版扫描V30
    void sigGrab();
    void sigParaList(QStringList datas);
    void sigStopScan();
    void sigCheckDev(QString viewName);
    void sigServiceReady(QString dirPath);

private slots:
    void onBtnInit();
    void onBtnCam();
    void onBtnDir();
    void onBtnSetMin();
    void onBtnSetMax();
    void onBtnStart();
    void onBtnStop();
    void onSpinValueChange(int value);
    void onSpinValueChangeDouble(double value);
    void onCamAcqTimeout();
    void onBtnTriggerE();
    void onBtnTriggerI();
    void onCamInit();

    //新版扫描V30
    void onTimerScanV30Timeout();

private:
    int dmdInit();
    int dmdSetMode();
    int dmdSetPattern();
    int dmdValidate();

    int dmdLoadFlashImg();
    int dmdAddPatternAll();
    int dmdSendPattern();

    int checkParameterScan(); // 供 V30 检查参数使用

    //新版扫描V30
    void startScanV30();
    int checkDataV30();
    QStringList setParaStrList();
    void lockUIV30();
    void unlockUIV30();
    int move2Start();
    int startDMDV30();
    int stopScanV30();
    int countVidFile(const QString& dirPath);
    void setProcessLabel(QString text);
    void setProgessVid(int value);
    void refreshParas();

private:
    EleIconBtn  *btnInit;
    EleIconBtn  *btnTriggerI;
    EleIconBtn  *btnTriggerE;
    EleIconBtn  *btnRecipeGen;
    EleIconBtn  *btnRecipeLoad;
//    EleIconBtn  *btnStart;
    EleIconBtn  *btnStop;
    PBtnToggle  *btnStart;
//    PBtnToggle  *btnStop;
    EleIconBtn  *btnCam;
    EleIconBtn  *btnImgForward;
    EleIconBtn  *btnImgBackward;
    EleIconBtn  *btnResultCalculate;
    EleIconBtn  *btnClear;
    EleIconBtn  *btnDir;
    EleIconBtn  *btnSetMin;
    EleIconBtn  *btnSetMax;
    PBtnSingleShot  *btnAlbum;

    QLabel      *labelImgInFlash;
    QLabel      *labelProgessCur;
    QLabel      *labelPosCur;
    QLabel      *labelPosPZT;
    QSpinBox    *boxRow;
    QSpinBox    *boxColumn;
    QSpinBox    *boxExp;
    QSpinBox    *boxPrd;
    QSpinBox    *boxFrameStart;
    QSpinBox    *boxFrameStop;
    QCheckBox   *checkFrameAll;
    QLineEdit   *lineSaveDir;
    QSpacerItem *spacerWid;

    QDoubleSpinBox  *boxStart;
    QDoubleSpinBox  *boxStop;
    QDoubleSpinBox  *boxStep;

    QTimer  timerCamAcq;
    QImage  m_pImg;

    QString m_pScanDirPathRaw;
    QString m_pScanDirPathCur;

    FuncDLP3500Ctl  *dmd;
    FuncPZTCtl      *pzt;

    int m_pImgInFlash;
    int m_pScanRowCur = 1;
    int m_pScanColumnCur = 1;
    bool m_pcamInit;

    int     m_pProgressCount = 0;
    int     m_pProgressCur = 0;
    QString m_pStyle;

    //startV30
    QEventLoop loopV30;
    QTimer timerV30;
    int m_pProcessVidCount;
};

#endif // VIEWDLPSCAN_H
