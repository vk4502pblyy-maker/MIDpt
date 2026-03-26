#include "funcdlp3500ctl.h"
#include <QColor>
#include <QEventLoop>
#include <QTimer>

#define MAX_NUM_RETRIES 5
#define MAX_NUM_SOURCE_DETECT_RETRIES 15
#pragma execution_character_set(push, "utf-8")
QMutex FuncDLP3500Ctl::mutex;
QScopedPointer<FuncDLP3500Ctl>  FuncDLP3500Ctl::instance;

FuncDLP3500Ctl::FuncDLP3500Ctl(QObject *parent)
    : QObject{parent}
{
    m_pInitFlag = false;
    connect(&dlpc350,&FuncDLPC350Api::sigErr,this,&FuncDLP3500Ctl::onDLPInfo);
    connect(&dlpc350,&FuncDLPC350Api::sigInfo,this,&FuncDLP3500Ctl::onDLPInfo);
}

FuncDLP3500Ctl *FuncDLP3500Ctl::getInstance()
{
    if (instance.isNull()) {
        QMutexLocker locker(&mutex);
        if (instance.isNull()) {
            instance.reset(new FuncDLP3500Ctl);
        }
    }
    return instance.data();
}

FuncDLP3500Ctl::~FuncDLP3500Ctl()
{
    dlpc350.DLPC350_USB_Close();
    dlpc350.DLPC350_USB_Exit();
}

int FuncDLP3500Ctl::connectDMD()
{
    int res = 0;
    res = dlpc350.DLPC350_USB_Init();
    if(!res){
        emit sigInfo("DMD端口初始化");
    }
    emit sigDebug("DLPC350_USB_Init():"+QString::number(res));
    res = dlpc350.DLPC350_USB_Open();
    if(!res){
        emit sigInfo("DMD端口打开");
    }
    emit sigDebug("DLPC350_USB_Open():"+QString::number(res));
    res = dlpc350.DLPC350_USB_IsConnected();
    if(res == 1){
        emit sigInfo("DMD已连接");
//        emit sigConnectionChange(true);
    }
    emit sigDebug("DLPC350_USB_IsConnected():"+QString::number(res));
    m_pInitFlag = true;
    return 0;
}

int FuncDLP3500Ctl::getConnection()
{
    return dlpc350.DLPC350_USB_IsConnected();
}

int FuncDLP3500Ctl::getIsInit()
{
    return m_pInitFlag;
}

int FuncDLP3500Ctl::setMode(OperatingMode op, PatternSource ps)
{
    int res = 0;
    res = PatSeqCtrlStop();
    if(op == PATTERN_SEQUENCE_VE){
        if(ps == FLASH){
            dlpc350.DLPC350_SetPatternTriggerMode(0x03); //Trigger Mode - 3
            dlpc350.DLPC350_SetPatternDisplayMode(false);
        }
    }
    m_pOperatingMode = op;
    m_pPatternSource = ps;
    setInPatMode();
    int img = getFlasImages();
    emit sigInfo("getFlasImages():"+QString::number(img));
    return 0;
}

int FuncDLP3500Ctl::VarExpPatSeqAddPatToLut(int frameIndex, TriggerType type, QString &patternTxt, int exp, int prd)
{
    int firstItem=0, lastItem=0;
    unsigned int pat_num=0;
    bool bufSwap;
    int trigger_type = type;
    int bit = 1;
    int color = 1;


    bufSwap = true;

    patternTxt = VarExpGenerateItemText(frameIndex,bit, firstItem, lastItem,false,exp,prd);
    emit sigInfo("pattern info:"+patternTxt);
    VarExpUpdateSeqItemData(trigger_type, frameIndex, bit, color,frameIndex, false, true, bufSwap, 0, exp,prd);
    return 0;
}

int FuncDLP3500Ctl::loadImages(QString imgBinPath)
{
    dlpc350.FWUpload(imgBinPath,true);
    return 0;
}

int FuncDLP3500Ctl::getFlasImages()
{
    unsigned int num;
    int res = dlpc350.DLPC350_GetNumImagesInFlash(&num);
    emit sigInfo("DLPC350_GetNumImagesInFlashs():"+QString::number(res));
    return num;
}

int FuncDLP3500Ctl::VarExpPatSeqSendLUT()
{
    int i;
    char errorStr[256];
    int res = 0;
    unsigned char splashLut[256];
    int numSplashLutEntries = 0;
    int numPatLutEntries = 0;
    int numPatsPerTrigOut2 = 1;
    int trigMode=0;
    unsigned int min_pat_exposure[8] = {235, 700, 1570, 1700, 2000, 2500, 4500, 8333};

    dlpc350.DLPC350_ClearExpLut();
    for(int i=0;i<m_pPatSeqList.count();i++){
        PatSeq seq = m_pPatSeqList.at(i);
        res = dlpc350.DLPC350_AddToExpLut(seq.trigType,seq.pat_num,seq.bitDepth,
                                    seq.Led_Select,seq.invertPat,seq.insertBlack,
                                    seq.bufSwap,seq.TrigOutPrev,
                                    seq.PatExp,seq.PatPrd);
        emit sigInfo("DLPC350_AddToExpLut():FrameIndex:"+QString::number(seq.pat_num)+"_"+QString::number(res));
        splashLut[i] = seq.frame_index;
        numPatLutEntries++;
    }
    numSplashLutEntries = m_pPatSeqList.count();
    res = dlpc350.DLPC350_SetPatternDisplayMode(false);
    emit sigInfo("DLPC350_SetPatternDisplayMode(false):"+QString::number(res));
    res = dlpc350.DLPC350_SetVarExpPatternConfig(numPatLutEntries, numPatsPerTrigOut2, numSplashLutEntries, true);
    emit sigInfo("DLPC350_SetVarExpPatternConfig():"+QString::number(res));
    res = dlpc350.DLPC350_SetPatternTriggerMode(3);
    emit sigInfo("DLPC350_SetPatternTriggerMode(3):"+QString::number(res));
    res = dlpc350.DLPC350_SendVarExpPatLut();
    emit sigInfo("DLPC350_SendVarExpPatLut():"+QString::number(res));
    res = dlpc350.DLPC350_SendVarExpImageLut(&splashLut[0], numSplashLutEntries);
    emit sigInfo("DLPC350_SendVarExpImageLut():"+QString::number(res));
    return 0;
}

int FuncDLP3500Ctl::ValidatePatSeq()
{
    int i = 0;
    unsigned int status;
    bool ready;
    QEventLoop loop;

    do
    {
        if(dlpc350.DLPC350_CheckPatLutValidate(&ready,&status) < 0)
        {
            emit sigErr("DLPC350_CheckPatLutValidate():"+QString::number(-1));
            return -1;
        }

        if(ready)
        {
            break;
        }
        else
        {
            QTimer::singleShot(1000, &loop, SLOT(quit()));
            loop.exec();
        }

        if(i++ > MAX_NUM_RETRIES)
            break;
    } while(1);

    return 0;
}

int FuncDLP3500Ctl::PatSeqCtrlStop()
{
    int i = 0;
    unsigned int patMode;
    int res = 0;

    res = dlpc350.DLPC350_PatternDisplay(0);
    emit sigInfo("DLPC350_PatternDisplay(0):"+QString::number(res));
    SleeperThread::msleep(100);
    while(1)
    {
        dlpc350.DLPC350_GetPatternDisplay(&patMode);
        if(patMode == 0)
            break;
        else
            dlpc350.DLPC350_PatternDisplay(0);
        SleeperThread::msleep(100);
        if(i++ > MAX_NUM_RETRIES)
            break;
    }
    return 0;
}

int FuncDLP3500Ctl::PatSeqCtrlStart()
{
//    int i = 0;
//    unsigned int patMode;
    int res = dlpc350.DLPC350_PatternDisplay(2);
    emit sigInfo("DLPC350_PatternDisplay(2):"+QString::number(res));
    return 0;
}

int FuncDLP3500Ctl::clearPatSeq()
{
    m_pPatSeqList.clear();
    return 0;
}

void FuncDLP3500Ctl::setInitFlag(bool flag)
{
    m_pInitFlag = flag;
}

bool FuncDLP3500Ctl::getInitFlag()
{
    return m_pInitFlag;
}

int FuncDLP3500Ctl::GetDLPC350Status()
{
    unsigned char HWStatus, SysStatus, MainStatus;
    dlpc350.DLPC350_GetStatus(&HWStatus, &SysStatus, &MainStatus);
    return 0;
}

int FuncDLP3500Ctl::VarExpInsertTriggerItem(int trig_type)
{
    return trig_type;
}

QString FuncDLP3500Ctl::VarExpGenerateItemText(int frame_Index, int bitDepth, int firstItem, int lastItem, bool invert, int patExp, int patPeriod)
{
    QString itemText = "S";

//    if(ui->radioButton_VarExpPatSeqSrcFrmFlash->isChecked())
//        itemText = "S";
//    else
//        itemText = "F";

    itemText.append(QString::number(frame_Index));
    itemText.append("::");
    itemText.append("G0");
    if(lastItem != firstItem)
    {
        itemText.append("-");
        itemText.append(QString::number(lastItem));
    }
    else
        itemText.prepend("   ");

    itemText.append("\n");
    itemText.append("  (");
    itemText.append(QString::number(bitDepth));
    itemText.append(")\n");
    itemText.append("Exp: ");
    itemText.append(QString::number(patExp));
    itemText.append("\n");
    itemText.append("Prd: ");
    itemText.append(QString::number(patPeriod));
    itemText.append("\n");

    if(invert)
        itemText.append("\n    ~");

    return itemText;
}

QStringList FuncDLP3500Ctl::VarExpUpdateSeqItemData(int trigType, int pat_num, int bitDepth, int Led_Select, int frame_index, bool invertPat, bool insertBlack, bool bufSwap, bool TrigOutPrev, int PatExp, int PatPrd)
{
    PatSeq pat;
    pat.trigType = trigType;
    pat.pat_num = pat_num;
    pat.bitDepth = bitDepth;
    pat.Led_Select = Led_Select;
    pat.insertBlack = insertBlack;
    pat.invertPat = invertPat;
    pat.frame_index = frame_index;
    pat.bufSwap = bufSwap;
    pat.TrigOutPrev = TrigOutPrev;
    pat.PatExp = PatExp;
    pat.PatPrd = PatPrd;
    m_pPatSeqList.append(pat);
    QStringList list;
    list.append(QString::number(trigType));
    list.append(QString::number(pat_num));
    list.append(QString::number(bitDepth));
    list.append(QString::number(Led_Select));
    list.append(QString::number(invertPat));
    list.append(QString::number(insertBlack));
    list.append(QString::number(frame_index));
    list.append(QString::number(bufSwap));
    list.append(QString::number(TrigOutPrev));
    list.append(QString::number(PatExp));
    list.append(QString::number(PatPrd));
    return list;
}

int FuncDLP3500Ctl::setInPatMode()
{
    int i = 0;
    bool mode;
    unsigned int patMode;

    //Check if it is in Pattern Mode
    int res = dlpc350.DLPC350_GetMode(&mode);
    emit sigInfo("DLPC350_GetMode(mode):"+QString::number(mode)+":"+QString::number(res));
    if(mode == false)
    {
        //Switch to Pattern Mode
        dlpc350.DLPC350_SetMode(true);
        SleeperThread::msleep(100);
        while(1)
        {
            dlpc350.DLPC350_GetMode(&mode);
            if(mode)
                break;
            SleeperThread::msleep(100);
            if(i++ > MAX_NUM_RETRIES)
                return -1;
        }
    }
    else
    {
        //First stop pattern sequence
        dlpc350.DLPC350_GetPatternDisplay(&patMode);
        //if it is in PAUSE or RUN mode
        if(patMode != 0)
        {
            PatSeqCtrlStop();
        }
    }

    return 0;
}

void FuncDLP3500Ctl::onDLPInfo(QString msg)
{
    emit sigInfo(msg);
}
#pragma execution_character_set(pop)
