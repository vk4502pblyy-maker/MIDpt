#ifndef FUNCDLP3500CTL_H
#define FUNCDLP3500CTL_H

#include <QObject>
#include <QThread>
#include <QScopedPointer>
#include <QMutex>
#include "funcdlpc350api.h"

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};


class FuncDLP3500Ctl : public QObject
{
    Q_OBJECT
public:
enum OperatingMode{
    PATTERN_SEQUENCE,
    PATTERN_SEQUENCE_VE,
    VIDEO_MODE,
    POWER_STANDBY
    };
enum PatternSource{
    FLASH,
    VIDEO_PORT
};
struct PatSeq{
    int trigType;
    int pat_num;
    int bitDepth;
    int Led_Select;
    int frame_index;
    bool invertPat;
    bool insertBlack;
    bool bufSwap;
    bool TrigOutPrev;
    int PatExp;
    int PatPrd;
};

public:
    static FuncDLP3500Ctl *getInstance();
    ~FuncDLP3500Ctl();
    int connectDMD();
    int getConnection();
    int getIsInit();
    int setMode(OperatingMode op,PatternSource ps);
    int VarExpPatSeqAddPatToLut(int frameIndex,TriggerType type,
                                QString &patternTxt,int exp = 1000000,int prd = 1000000);
    int loadImages(QString imgBinPath);
    int getFlasImages();
    int VarExpPatSeqSendLUT();
    int ValidatePatSeq();
    int PatSeqCtrlStop();
    int PatSeqCtrlStart();
    int PatSeqCtrlPause();
    int PatSeqCtrlStep();
    int clearPatSeq();

    void    setInitFlag(bool flag);
    bool    getInitFlag();


signals:
    void sigInfo(QString msg);
    void sigDebug(QString msg);
    void sigErr(QString msg);
    void sigImgNum(int num);
    void sigConnectionChange(bool connection);

private:
    explicit FuncDLP3500Ctl(QObject *parent = nullptr);

    int GetDLPC350Status();
    int VarExpInsertTriggerItem(int trig_type);
    QString VarExpGenerateItemText(int frame_Index, int bitDepth, int firstItem, int lastItem, bool invert, int patExp, int patPeriod);
    QStringList VarExpUpdateSeqItemData(int trigType, int pat_num,int bitDepth,int Led_Select,int frame_index, bool invertPat, bool insertBlack,bool bufSwap, bool TrigOutPrev, int PatExp, int PatPrd);
    int setInPatMode();

private slots:
    void onDLPInfo(QString msg);

private:
    static QMutex                       mutex;
    static QScopedPointer<FuncDLP3500Ctl>   instance;

    FuncDLPC350Api  dlpc350;
    OperatingMode   m_pOperatingMode;
    PatternSource   m_pPatternSource;
    QList<PatSeq>   m_pPatSeqList;

    bool    m_pInitFlag;

    Q_DISABLE_COPY(FuncDLP3500Ctl)
};

#endif // FUNCDLP3500CTL_H
