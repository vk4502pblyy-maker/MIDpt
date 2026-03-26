#ifndef VIEWDLP4500SCAN_H
#define VIEWDLP4500SCAN_H

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
#include "element/eleiconbtn.h"
#include "function/funcdlp3500ctl.h"
#include "function/funcpztctl.h"
#include "function/funcmovewm.h"


class ViewDLP4500Scan : public QWidget
{
    Q_OBJECT
public:
    explicit ViewDLP4500Scan(QWidget *parent = nullptr);
    ~ViewDLP4500Scan();
    int setMainWdCmd(QString cmd);
    int camImgBackMW(int camNum,QImage img,QString serviceCur);
    int setPosMsg(QString type,QString posMsg);
    int setPZTPos(double pos);
    int setPZTEnable();

    //新版扫描V30
    void move2Next();
    void checkDevResult(QString result);
    void setImageCur(const QImage &img);//弃用
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
    void onBtnTest();
    void onSpinValueChange(int value);
    void onSpinValueChangeDouble(double value);
    void onCamAcqTimeout();
    void onBtnTriggerE();
    void onBtnTriggerI();
    void onTimerScanTimeout();
    void onCamInit();
    void onTimerTestTimeout();

    //新版扫描V30
    void onTimerScanV30Timeout();


private:
    int dmdInit();
    int dmdSetMode();
    int dmdSetPattern();
    int dmdValidate();

    int dmdLoadFlashImg();
    int dmdAddPatternAll();
    int dmdAddPattern(int start,int stop);
    int dmdSendPattern();

    int dmdImgProcess();

    int processScanData();
    int startScan();
    int stopScan();
    int scriptScan();
    int scriptScanTest();
    int singleHeightTest();
    int scanImgSave();

    void testScriptTp1(int interval,int pztint);
    void stopTest();
    void processTest();
    void lockUI(bool lock);

    //新版扫描
    void startScanV20();
    void stopScanV20();
    int checkDevStatus();
    int askAvailable();
    int checkParameterScan();
    int processScanImg();

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
    EleIconBtn  *btnStart;
    EleIconBtn  *btnCam;
    EleIconBtn  *btnStop;
    EleIconBtn  *btnImgForward;
    EleIconBtn  *btnImgBackward;
    EleIconBtn  *btnResultCalculate;
    EleIconBtn  *btnClear;
    EleIconBtn  *btnDir;
    EleIconBtn  *btnSetMin;
    EleIconBtn  *btnSetMax;
    QPushButton *btnTest;
//    QLabel      *labelImgCam;
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
    QCheckBox   *checkSpain;
    QLineEdit   *lineSaveDir;
//    GLImageWidget   *processWid;
    QSpacerItem *spacerWid;

//    QListWidget *listLog;

    QProgressBar    *progressTest;
    QDoubleSpinBox  *boxStart;
    QDoubleSpinBox  *boxStop;
    QDoubleSpinBox  *boxStep;

    QTimer  timerCamAcq;
    QTimer  timerScan;
    QTimer  timerTest;
    QImage  m_pImg;

    QString m_pScanDirPathRaw;
    QString m_pScanDirPathCur;

    bool    m_pRowPass,m_pColumnPass,
            m_pExpPass,m_pPrdPass,
            m_pStartPass,m_pStopPass,
            m_pStepPass;

    FuncDLP3500Ctl  *dmd;
    FuncPZTCtl      *pzt;

    int m_pImgInFlash;
    int m_pScanRowCur = 1;
    int m_pScanColumnCur = 1;
    bool m_pIsStop;
    bool m_pIsScript;
    bool m_pcamInit;
    bool m_pIsScan;
    bool m_pIsTestInit;
    int m_pTestTp;
    int m_pCamImgBack;

    //test1
    int m_pTest1pztTime;
    int m_pTest1PztCur;

    //startV20
    int     m_pCamReady;
    int     m_pPZTReady;
    int     m_pDMDReady;
    bool    m_pMainWdCmdBack;
    QString m_pMainWdCmd;
    int     m_pProgressCount = 0;
    int     m_pProgressCur = 0;
    QString m_pStyle;

    //startV30
    QEventLoop loopV30;
    QTimer timerV30;
    int m_pProcessVidCount;

//    QString savePath4500Root;
//    int     column4500;
//    int     row4500;
//    int     columnCur;
//    int     rowCur;


};

#endif // VIEWDLP4500SCAN_H
