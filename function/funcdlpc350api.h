#ifndef FUNCDLPC350API_H
#define FUNCDLPC350API_H

#define USB_MIN_PACKET_SIZE 64
#define USB_MAX_PACKET_SIZE 64

#define HID_MESSAGE_MAX_SIZE        512

#define MY_VID 0x0451
#define MY_PID 0x6401

#ifndef MIN
#define MIN(a, b)					((a) < (b) ? (a) : (b))
#endif

#include <QObject>

typedef struct _readCmdData
{
    unsigned char CMD2;
    unsigned char CMD3;
    unsigned short len;
}CmdFormat;

typedef struct _hidmessageStruct
{
    struct _hidhead
    {
        struct _packetcontrolStruct
        {
            unsigned char dest		:3; /* 0 - ProjCtrl; 1 - RFC; 7 - Debugmsg */
            unsigned char reserved	:2;
            unsigned char nack		:1; /* Command Handler Error */
            unsigned char reply	:1; /* Host wants a reply from device */
            unsigned char rw		:1; /* Write = 0; Read = 1 */
        }flags;
        unsigned char seq;
        unsigned short length;
    }head;
    union
    {
        unsigned short cmd;
        unsigned char data[HID_MESSAGE_MAX_SIZE];
    }text;
}hidMessageStruct;

typedef enum
    {
        VID_SIG_STAT,
        SOURCE_SEL,
        PIXEL_FORMAT,
        CLK_SEL,
        CHANNEL_SWAP,
        FPD_MODE,
        CURTAIN_COLOR,
        POWER_CONTROL,
        FLIP_LONG,
        FLIP_SHORT,
        TPG_SEL,
        PWM_INVERT,
        LED_ENABLE,
        GET_VERSION,
        GET_FIRMWAE_TAG_INFO,
        SW_RESET,
        DMD_PARK,
        BUFFER_FREEZE,
        STATUS_HW,
        STATUS_SYS,
        STATUS_MAIN,
        CSC_DATA,
        GAMMA_CTL,
        BC_CTL,
        PWM_ENABLE,
        PWM_SETUP,
        PWM_CAPTURE_CONFIG,
        GPIO_CONFIG,
        LED_CURRENT,
        DISP_CONFIG,
        TEMP_CONFIG,
        TEMP_READ,
        MEM_CONTROL,
        I2C_CONTROL,
        LUT_VALID,
        DISP_MODE,
        TRIG_OUT1_CTL,
        TRIG_OUT2_CTL,
        RED_STROBE_DLY,
        GRN_STROBE_DLY,
        BLU_STROBE_DLY,
        PAT_DISP_MODE,
        PAT_TRIG_MODE,
        PAT_START_STOP,
        BUFFER_SWAP,
        BUFFER_WR_DISABLE,
        CURRENT_RD_BUFFER,
        PAT_EXPO_PRD,
        INVERT_DATA,
        PAT_CONFIG,
        MBOX_ADDRESS,
        MBOX_CONTROL,
        MBOX_DATA,
        TRIG_IN1_DELAY,
        TRIG_IN2_CONTROL,
        IMAGE_LOAD,
        IMAGE_LOAD_TIMING,
        I2C0_CTRL,
        MBOX_EXP_DATA,
        MBOX_EXP_ADDRESS,
        EXP_PAT_CONFIG,
        NUM_IMAGE_IN_FLASH,
        I2C0_STAT,
        GPCLK_CONFIG,
        PULSE_GPIO_23,
        ENABLE_DLPC350_DEBUG,
        TPG_COLOR,
        PWM_CAPTURE_READ,
        PROG_MODE,
        BL_STATUS,
        BL_SPL_MODE,
        BL_GET_MANID,
        BL_GET_DEVID,
        BL_GET_CHKSUM,
        BL_SET_SECTADDR,
        BL_SECT_ERASE,
        BL_SET_DNLDSIZE,
        BL_DNLD_DATA,
        BL_FLASH_TYPE,
        BL_CALC_CHKSUM,
        BL_PROG_MODE
    }DLPC350_CMD;

enum TriggerType{
    INTERNAL = 0,
    EXTERNAL_POSITIVE = 1,
    EXTERNAL_NEGATIVE = 2,
    NO_TRIGGER = 3
};

class FuncDLPC350Api : public QObject
{
    Q_OBJECT

public:
    explicit FuncDLPC350Api(QObject *parent = nullptr);
    int DLPC350_GetStatus(unsigned char *pHWStatus, unsigned char *pSysStatus, unsigned char *pMainStatus);

    int DLPC350_USB_Init();
    int DLPC350_USB_IsConnected();
    int DLPC350_USB_Open();
    int DLPC350_SetPatternTriggerMode(int trigMode);
    int DLPC350_SetPatternDisplayMode(bool external);
    int DLPC350_ClearExpLut(void);
    int DLPC350_EnterProgrammingMode(void);
    int DLPC350_GetFlashManID(unsigned short *pManID);
    int DLPC350_GetFlashDevID(unsigned long long *pDevID);
    int DLPC350_SetFlashAddr(unsigned int Addr);
    int DLPC350_SetUploadSize(unsigned long int dataLen);
    int DLPC350_CalculateFlashChecksum(void);
    void DLPC350_WaitForFlashReady();
    int DLPC350_GetBLStatus(unsigned char *BL_Status);
    int DLPC350_GetFlashChecksum(unsigned int*checksum);
    int DLPC350_SetFlashType(unsigned char Type);
    int DLPC350_FlashSectorErase(void);
    int DLPC350_UploadData(unsigned char *pByteArray, unsigned int dataLen);
    int DLPC350_ExitProgrammingMode(void);
    int DLPC350_AddToExpLut(int TrigType, int PatNum,int BitDepth,
                            int LEDSelect,bool InvertPat, bool InsertBlack,
                            bool BufSwap, bool trigOutPrev,
                            unsigned int exp_time_us,
                            unsigned int ptn_frame_period_us);
    int DLPC350_SetVarExpPatternConfig(unsigned int numLutEntries,
                                       unsigned int numPatsForTrigOut2,
                                       unsigned int numImages, bool repeat);
    int DLPC350_SendVarExpPatLut(void);
    int DLPC350_OpenMailbox(int MboxNum);
    int DLPC350_SetVarExpMboxAddr(int Addr);
    int DLPC350_CloseMailbox(void);
    int DLPC350_SendVarExpImageLut(unsigned char *lutEntries, unsigned int numEntries);
    int DLPC350_PatternDisplay(unsigned int Action);
    int DLPC350_GetPatternDisplay(unsigned int *pAction);
    int DLPC350_StartPatLutValidate();
    int DLPC350_CheckPatLutValidate(bool *ready, unsigned int *pStatus);
    int DLPC350_SetMode(bool SLmode);
    int DLPC350_GetMode(bool *pMode);
    int DLPC350_GetNumImagesInFlash(unsigned int *pNumImgInFlash);
    int DLPC350_USB_Close();
    int DLPC350_USB_Exit();

    int FWUpload(QString imgPath,bool fastLoad);
    int setColor();
    int setTrigger(TriggerType type);



signals:
    void sigErr(QString err);
    void sigInfo(QString msg);

private:
    void ShowError(QString err);
    bool ProcessFlashParamsLine(QString line);
    int GetSectorNum(unsigned int Addr);


};

#endif // FUNCDLPC350API_H
