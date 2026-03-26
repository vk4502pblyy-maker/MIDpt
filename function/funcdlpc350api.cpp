#include "funcdlpc350api.h"
#include "hidapi.h"
#include "dlpc350_firmware.h"
#include "dlpc350_flashDevice.h"
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTime>
#include <QDebug>
#include <QCoreApplication>

FlashDevice             g_FlashDevice;
static unsigned char    g_SeqNum=0;
static int              USBConnected = 0;
static int              USBInit      = 0;
unsigned char           g_OutputBuffer[USB_MAX_PACKET_SIZE+1];
unsigned char           g_InputBuffer[USB_MAX_PACKET_SIZE+1];
static unsigned int     g_ExpLutIndex = 0;
static unsigned long int g_ExpLut[MAX_VAR_EXP_PAT_LUT_ENTRIES*3] = {0};

#define SOURCE_LOCK_MASK 0x03
#define SOURCE_LOCKED 0x02

static hid_device *DeviceHandle;


static int DLPC350_USB_Write();
static int DLPC350_USB_Read();
static int DLPC350_PrepReadCmd(DLPC350_CMD cmd);
static int DLPC350_Write(bool ackRequired);
static int DLPC350_Read();
static int DLPC350_SendMsg(hidMessageStruct *pMsg, bool ackRequired);
static int DLPC350_PrepWriteCmd(hidMessageStruct *pMsg, DLPC350_CMD cmd);

CmdFormat CmdList[255] =
{
    {   0x07,  0x1C,  0x1C   },      //VID_SIG_STAT,
    {   0x1A,  0x00,  0x01   },      //SOURCE_SEL,
    {   0x1A,  0x02,  0x01   },      //PIXEL_FORMAT,
    {   0x1A,  0x03,  0x01   },      //CLK_SEL,
    {   0x1A,  0x37,  0x01   },      //CHANNEL_SWAP,
    {   0x1A,  0x04,  0x01   },      //FPD_MODE,
    {   0,  0,  0   },      //CURTAIN_COLOR,
    {   0x02,  0x00,  0x01   },      //POWER_CONTROL,
    {   0x10,  0x08,  0x01   },      //FLIP_LONG,
    {   0x10,  0x09,  0x01   },      //FLIP_SHORT,
    {   0x12,  0x03,  0x01   },      //TPG_SEL,
    {   0x1A,  0x05,  0x01   },      //PWM_INVERT,
    {   0x1A,  0x07,  0x01   },      //LED_ENABLE,
    {   0x02,  0x05,  0x00   },      //GET_VERSION,
    {   0x1A,  0xFF,  0x00   },      //GET_FIRMWAE_TAG_INFO
    {   0x08,  0x02,  0x00   },      //SW_RESET,
    {   0,  0,  0   },      //DMD_PARK,
    {   0x10,  0x0A,  0x01   },      //BUFFER_FREEZE,
    {   0x1A,  0x0A,  0x00   },      //STATUS_HW,
    {   0x1A,  0x0B,  0x00   },      //STATUS_SYS,
    {   0x1A,  0x0C,  0x00   },      //STATUS_MAIN,
    {   0,  0,  0   },      //CSC_DATA,
    {   0,  0,  0   },      //GAMMA_CTL,
    {   0,  0,  0   },      //BC_CTL,
    {   0x1A,  0x10,  0x01   },      //PWM_ENABLE,
    {   0x1A,  0x11,  0x06   },      //PWM_SETUP,
    {   0x1A,  0x12,  0x05   },      //PWM_CAPTURE_CONFIG,
    {   0x1A,  0x38,  0x02   },      //GPIO_CONFIG,
    {   0x0B,  0x01,  0x03   },      //LED_CURRENT,
    {   0x10,  0x00,  0x10   },      //DISP_CONFIG,
    {   0,  0,  0   },      //TEMP_CONFIG,
    {   0,  0,  0   },      //TEMP_READ,
    {   0x1A,  0x16,  0x09   },      //MEM_CONTROL,
    {   0,  0,  0   },      //I2C_CONTROL,
    {   0x1A,  0x1A,  0x01   },      //LUT_VALID,
    {   0x1A,  0x1B,  0x01   },      //DISP_MODE,
    {   0x1A,  0x1D,  0x03   },      //TRIG_OUT1_CTL,
    {   0x1A,  0x1E,  0x02   },      //TRIG_OUT2_CTL,
    {   0x1A,  0x1F,  0x02   },      //RED_STROBE_DLY,
    {   0x1A,  0x20,  0x02   },      //GRN_STROBE_DLY,
    {   0x1A,  0x21,  0x02   },      //BLU_STROBE_DLY,
    {   0x1A,  0x22,  0x01   },      //PAT_DISP_MODE,
    {   0x1A,  0x23,  0x01   },      //PAT_TRIG_MODE,
    {   0x1A,  0x24,  0x01   },      //PAT_START_STOP,
    {   0,  0,  0   },      //BUFFER_SWAP,
    {   0,  0,  0   },      //BUFFER_WR_DISABLE,
    {   0,  0,  0   },      //CURRENT_RD_BUFFER,
    {   0x1A,  0x29,  0x08   },      //PAT_EXPO_PRD,
    {   0x1A,  0x30,  0x01   },      //INVERT_DATA,
    {   0x1A,  0x31,  0x04   },      //PAT_CONFIG,
    {   0x1A,  0x32,  0x01   },      //MBOX_ADDRESS,
    {   0x1A,  0x33,  0x01   },      //MBOX_CONTROL,
    {   0x1A,  0x34,  0x00   },      //MBOX_DATA,
    {   0x1A,  0x35,  0x04   },      //TRIG_IN1_DELAY,
    {   0x1A,  0x36,  0x01   },      //TRIG_IN2_CONTROL,
    {   0x1A,  0x39,  0x01   },      //IMAGE_LOAD,
    {   0x1A,  0x3A,  0x02   },      //IMAGE_LOAD_TIMING,
    {   0x1A,  0x3B,  0x00   },      //I2C0_CTRL,
    {   0x1A,  0x3E,  0x0C   },      //MBOX_EXP_DATA,
    {   0x1A,  0x3F,  0x02   },      //MBOX_EXP_ADDRESS,
    {   0x1A,  0x40,  0x06   },      //EXP_PAT_CONFIG
    {   0x1A,  0x42,  0x01   },      //NUM_IMAGE_IN_FLASH,
    {   0x1A,  0x43,  0x01   },      //I2C0_STAT,
    {   0x08,  0x07,  0x03   },      //GPCLK_CONFIG,
    {   0,  0,  0   },      //PULSE_GPIO_23,
    {   0,  0,  0   },      //ENABLE_DLPC350_DEBUG,
    {   0x12,  0x04,  0x0C   },      //TPG_COLOR,
    {   0x1A,  0x13,  0x05   },     //PWM_CAPTURE_READ,
    {   0x30,  0x01,  0x00   },     //PROG_MODE,
    {   0x00,  0x00,  0x00   },     //BL_STATUS
    {   0x00,  0x23,  0x01   },     //BL_SPL_MODE
    {   0x00,  0x15,  0x01   },     //BL_GET_MANID,
    {   0x00,  0x15,  0x01   },     //BL_GET_DEVID,
    {   0x00,  0x15,  0x01   },     //BL_GET_CHKSUM,
    {   0x00,  0x29,  0x04   },     //BL_SETSECTADDR,
    {   0x00,  0x28,  0x00   },     //BL_SECT_ERASE,
    {   0x00,  0x2C,  0x04   },     //BL_SET_DNLDSIZE,
    {   0x00,  0x25,  0x00   },     //BL_DNLD_DATA,
    {   0x00,  0x2F,  0x01   },     //BL_FLASH_TYPE,
    {   0x00,  0x26,  0x00   },     //BL_CALC_CHKSUM,
    {   0x00,  0x30,  0x01   }     //BL_PROG_MODE,
};



FuncDLPC350Api::FuncDLPC350Api(QObject *parent)
    : QObject{parent}
{

}

int FuncDLPC350Api::DLPC350_GetStatus(unsigned char *pHWStatus, unsigned char *pSysStatus, unsigned char *pMainStatus)
/**
 * This function is to be used to check the various status indicators from the controller.
 * Refer to DLPC350 Programmer's guide section 2.1 "DLPC350 Status Commands" for detailed description of each byte.
 *
 * @param pHWStatus - O - provides status information on the DLPC350's sequencer, DMD controller and initialization.
 * @param pSysStatus - O - provides DLPC350 status on internal memory tests..
 * @param pMainStatus - O - provides DMD park status and DLPC350 sequencer, frame buffer, and gamma correction status.
 *
 * @return 0 PASS <BR>
 *         -1 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(STATUS_HW);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *pHWStatus = msg.text.data[0];
    }
    else
        return -1;

    DLPC350_PrepReadCmd(STATUS_SYS);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *pSysStatus = msg.text.data[0];
    }
    else
        return -1;

    DLPC350_PrepReadCmd(STATUS_MAIN);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *pMainStatus = msg.text.data[0];
    }
    else
        return -1;

    return 0;
}

static int DLPC350_USB_Write()
{
    int bytesWritten;

    if(DeviceHandle == NULL)
        return -1;

    if((bytesWritten = hid_write(DeviceHandle, g_OutputBuffer, USB_MIN_PACKET_SIZE+1)) == -1)
    {
        hid_close(DeviceHandle);
        USBConnected = 0;
        return -1;
    }

    return bytesWritten;
}

static int DLPC350_USB_Read()
{
    int bytesRead;

    if(DeviceHandle == NULL)
        return -1;

    //clear out the input buffer
    memset((void*)&g_InputBuffer[0],0x00,USB_MIN_PACKET_SIZE+1);

    if((bytesRead = hid_read_timeout(DeviceHandle, g_InputBuffer, USB_MIN_PACKET_SIZE+1, 2000)) == -1)
    {
        hid_close(DeviceHandle);
        USBConnected = 0;
        return -1;
    }

    return bytesRead;
}

int FuncDLPC350Api::DLPC350_USB_Init()
{
    if(USBInit){
        return 0;
    }
    int res = hid_init();
    if(res == 0){
        USBInit = 1;
    }
    return res;
}

int FuncDLPC350Api::DLPC350_USB_IsConnected()
{
    return USBConnected;
}

int FuncDLPC350Api::DLPC350_USB_Open()
{
    // Open the device using the VID, PID,
    struct hid_device_info *hid_info;
    hid_info = hid_enumerate(MY_VID, MY_PID);
    if(hid_info == NULL)
    {
        USBConnected = 0;
        return -1;
    }

    DeviceHandle = NULL;

    if(0 == hid_info->interface_number)
    {
        DeviceHandle = hid_open_path(hid_info->path);
    }
    else
    {
        struct hid_device_info *hid_next_info = hid_info->next;

        if(hid_next_info != NULL)
        {
            DeviceHandle = hid_open_path(hid_next_info->path);
        }
    }

    if(DeviceHandle == NULL)
    {
        USBConnected = 0;
        return -1;
    }

    USBConnected = 1;

    return 0;
}

int FuncDLPC350Api::DLPC350_SetPatternTriggerMode(int trigMode)
/**
 * The Pattern Trigger Mode Selection command selects between one of the three pattern Trigger Modes.
 * Before executing this command, stop the current pattern sequence. After executing this command, send
 * the Validation command (I2C: 0x7D or USB: 0x1A1A) once before starting the pattern sequence.
 *
 * @param   trigMode  - I - 0 = Pattern Trigger Mode 0: VSYNC serves to trigger the pattern display sequence.
 *                          1 = Pattern Trigger Mode 1: Internally or Externally (through TRIG_IN1 and TRIG_IN2) generated trigger.
 *                          2 = Pattern Trigger Mode 2: TRIG_IN_1 alternates between two patterns,while TRIG_IN_2 advances to the next pair of patterns.
 *                          3 = Pattern Trigger Mode 3: Internally or externally generated trigger for Variable Exposure display sequence.
 *                          4 = Pattern Trigger Mode 4: VSYNC triggered for Variable Exposure display sequence.
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = trigMode;
    DLPC350_PrepWriteCmd(&msg, PAT_TRIG_MODE);

    return DLPC350_SendMsg(&msg,true);
    return 0;
}

int FuncDLPC350Api::DLPC350_SetPatternDisplayMode(bool external)
/**
 * The Pattern Display Data Input Source command selects the source of the data for pattern display:
 * streaming through the 24-bit RGB/FPD-link interface or stored data in the flash image memory area from
 * external Flash. Before executing this command, stop the current pattern sequence. After executing this
 * command, send the Validation command (I2C: 0x7D or USB: 0x1A1A) once before starting the pattern
 * sequence.
 *
 * @param   external  - I - TRUE = Pattern Display Data is streamed through the 24-bit RGB/FPD-link interface
 *                          FALSE = Pattern Display Data is fetched from flash memory
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    if(external)
        msg.text.data[2] = 0;
    else
        msg.text.data[2] = 3;

    DLPC350_PrepWriteCmd(&msg, PAT_DISP_MODE);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_ClearExpLut()
/**
 * This API does not send any commands to the controller.It clears the locally (in the GUI program) stored exposure LUT.
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    g_ExpLutIndex = 0;
    return 0;
}

int FuncDLPC350Api::DLPC350_EnterProgrammingMode()
/**
 * This function is to be called to put the unit in programming mode. Only programming mode APIs will work once
 * in this mode.
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = 1;

    DLPC350_PrepWriteCmd(&msg, PROG_MODE);

    return DLPC350_SendMsg(&msg,false);
}

int FuncDLPC350Api::DLPC350_GetFlashManID(unsigned short *pManID)
/**
 * This function works only in prorgamming mode.
 * This function returns the manufacturer ID of the flash part interfaced with the controller.
 *
 * @param pManID - O - Manufacturer ID of the flash part
 *
 * @return 0 PASS <BR>
 *         -1 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(BL_GET_MANID);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *pManID = msg.text.data[6];
        *pManID |= (unsigned short)msg.text.data[7] << 8;
        return 0;
    }
    return -1;
}

int FuncDLPC350Api::DLPC350_GetFlashDevID(unsigned long long *pDevID)
/**
 * This function works only in prorgamming mode.
 * This function returns the device ID of the flash part interfaced with the controller.
 *
 * @param pDevID - O - Device ID of the flash part
 *
 * @return 0 PASS <BR>
 *         -1 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(BL_GET_DEVID);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *pDevID = msg.text.data[6];
        *pDevID |= (unsigned long long)msg.text.data[7] << 8;
        *pDevID |= (unsigned long long)msg.text.data[8] << 16;
        *pDevID |= (unsigned long long)msg.text.data[9] << 24;
        *pDevID |= (unsigned long long)msg.text.data[12] << 32;
        *pDevID |= (unsigned long long)msg.text.data[13] << 40;
        *pDevID |= (unsigned long long)msg.text.data[14] << 48;
        *pDevID |= (unsigned long long)msg.text.data[15] << 56;
        return 0;
    }
    return -1;
}

int FuncDLPC350Api::DLPC350_SetFlashAddr(unsigned int Addr)
/**
 * This function works only in prorgamming mode.
 * This function is to be called to set the address prior to calling DLPC350_FlashSectorErase or DLPC350_DownloadData APIs.
 *
 * @param Addr - I - 32-bit absolute address.
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = Addr;
    msg.text.data[3] = Addr >> 8;
    msg.text.data[4] = Addr >> 16;
    msg.text.data[5] = Addr >> 24;

    DLPC350_PrepWriteCmd(&msg, BL_SET_SECTADDR);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_SetUploadSize(unsigned long dataLen)
/**
 * This function works only in prorgamming mode.
 * This function is to be called to set the payload size of data to be sent using DLPC350_DownloadData API.
 *
 * @param dataLen -I - length of download data payload in bytes.
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = dataLen;
    msg.text.data[3] = dataLen >> 8;
    msg.text.data[4] = dataLen >> 16;
    msg.text.data[5] = dataLen >> 24;

    DLPC350_PrepWriteCmd(&msg, BL_SET_DNLDSIZE);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_CalculateFlashChecksum()
/**
 * This function works only in prorgamming mode.
 * This function is to be issued to instruct the controller to calculate the flash checksum.
 * DLPC350_WaitForFlashReady() is then to be called to ensure that the controller is done and then call
 * DLPC350_GetFlashChecksum() API to retrieve the actual checksum from the controller.
 *
 * @return 0 = PASS <BR>
 *         -1 = FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepWriteCmd(&msg, BL_CALC_CHKSUM);

    if(DLPC350_SendMsg(&msg,false) <= 0)
        return -1;

    return 0;

}

void FuncDLPC350Api::DLPC350_WaitForFlashReady()
/**
 * This function works only in prorgamming mode.
 * This function polls the status bit and returns only when the controller is ready for next command.
 *
 */
{
    unsigned char BLstatus=STAT_BIT_FLASH_BUSY;

    do
    {
        DLPC350_GetBLStatus(&BLstatus);
    }
    while((BLstatus & STAT_BIT_FLASH_BUSY) == STAT_BIT_FLASH_BUSY);//Wait for flash busy flag to go off
}

int FuncDLPC350Api::DLPC350_GetBLStatus(unsigned char *BL_Status)
/**
 * This function works only in prorgamming mode.
 * This function returns the device ID of the flash part interfaced with the controller.
 *
 * @param BL_Status - O - BIT3 of the status byte when set indicates that the program is busy
 *                        with exectuing the previous command. When BIT3 is reset, it means the
 *                        program is ready for the next command.
 *
 * @return 0 PASS <BR>
 *         -1 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    /* For some reason BL_STATUS readback is not working properly.
     * However, after going through the bootloader code, I have ascertained that any
     * readback is fine - Byte 0 is always the bootloader status */
    DLPC350_PrepReadCmd(BL_GET_CHKSUM);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *BL_Status = msg.text.data[0];
        return 0;
    }
    return -1;
}

int FuncDLPC350Api::DLPC350_GetFlashChecksum(unsigned int *checksum)
/**
 * This function works only in prorgamming mode.
 * This function is to be used to retrieve the flash checksum from the controller.
 * DLPC350_CalculateFlashChecksum() and DLPC350_WaitForFlashReady() must be called before using this API.
 *
 * @param checksum - O - variable in which the flash checksum is to be returned
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;
#if 0
    DLPC350_PrepWriteCmd(&msg, BL_CALC_CHKSUM);

    if(DLPC350_SendMsg(&msg,true) <= 0)
        return -1;

    DLPC350_WaitForFlashReady();
#endif
    DLPC350_PrepReadCmd(BL_GET_CHKSUM);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);

        *checksum = msg.text.data[6];
        *checksum |= (unsigned int)msg.text.data[7] << 8;
        *checksum |= (unsigned int)msg.text.data[8] << 16;
        *checksum |= (unsigned int)msg.text.data[9] << 24;
        return 0;
    }
    return -1;
}

int FuncDLPC350Api::DLPC350_SetFlashType(unsigned char Type)
/**
 * This function works only in prorgamming mode.
 * This function is to be used to set the programming type of the flash device attached to the controller.
 *
 * @param Type - I - Type of the flash device.
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = Type;

    DLPC350_PrepWriteCmd(&msg, BL_FLASH_TYPE);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_FlashSectorErase()
/**
  * This function works only in prorgamming mode.
  * This function is to be called to erase a sector of flash. The address of the sector to be erased
  * is to be set by using the DLPC350_SetFlashAddr() API
  *
  * @return >=0 PASS <BR>
  *         <0 FAIL <BR>
  *
  */
{
    hidMessageStruct msg;

    DLPC350_PrepWriteCmd(&msg, BL_SECT_ERASE);
    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_UploadData(unsigned char *pByteArray, unsigned int dataLen)
{
    /**
     * This function works only in prorgamming mode.
     * This function sends one payload of data to the controller at a time. takes the total size of payload
     * in the parameter dataLen and returns the actual number of bytes that was sent in the return value.
     * This function needs to be called multiple times until all of the desired bytes are sent.
     *
     * @param pByteArray - I - Pointer to where the data to be downloaded is to be fetched from
     * @param dataLen -I - length in bytes of the total payload data to download.
     *
     * @return number of bytes actually downloaded <BR>
     *         <0 FAIL <BR>
     *
     */
    {
        hidMessageStruct msg;
        int retval;
        unsigned int sendSize;

        //The last -2 is to workaround a bug in bootloader.
        sendSize = HID_MESSAGE_MAX_SIZE - sizeof(msg.head)- sizeof(msg.text.cmd) - 2;

        if(dataLen > sendSize)
            dataLen = sendSize;

        CmdList[BL_DNLD_DATA].len = dataLen;
        memcpy(&msg.text.data[2], pByteArray, dataLen);

        DLPC350_PrepWriteCmd(&msg, BL_DNLD_DATA);

        retval = DLPC350_SendMsg(&msg,false);
        if(retval > 0)
            return dataLen;

        return -1;
    }
}

int FuncDLPC350Api::DLPC350_ExitProgrammingMode()
/**
 * This function works only in prorgamming mode.
 * This function is to be called to exit programming mode and resume normal operation with the new downloaded firmware.
 *
 * @return >=0 PASS <BR>
 *         <0 FAIL <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = 2;
    DLPC350_PrepWriteCmd(&msg, BL_PROG_MODE);

    return DLPC350_SendMsg(&msg,false);
}

int FuncDLPC350Api::DLPC350_AddToExpLut(int TrigType, int PatNum, int BitDepth, int LEDSelect, bool InvertPat, bool InsertBlack, bool BufSwap, bool trigOutPrev, unsigned int exp_time_us, unsigned int ptn_frame_period_us)
/**
 * This API does not send any commands to the controller.
 * It makes an entry (appends) in the locally stored (in the GUI program) pattern LUT as per the input arguments passed to this function.
 * See table 2-65 in programmer's guide for detailed desciprtion of pattern LUT entries.
 *
 * @param   TrigType  - I - Select the trigger type for the pattern
 *                          0 = Internal trigger
 *                          1 = External positive
 *                          2 = External negative
 *                          3 = No Input Trigger (Continue from previous; Pattern still has full exposure time)
 *                       0x3FF = Full Red Foreground color intensity
 * @param   PatNum  - I - Pattern number (0 based index). For pattern number 0x3F, there is no
 *                          pattern display. The maximum number supported is 24 for 1 bit-depth
 *                          patterns. Setting the pattern number to be 25, with a bit-depth of 1 will insert
 *                          a white-fill pattern. Inverting this pattern will insert a black-fill pattern. These w
 *                          patterns will have the same exposure time as defined in the Pattern Display
 *                          Exposure and Frame Period command. Table 2-66 in the programmer's guide illustrates which bit
 *                          planes are illuminated by each pattern number.
 * @param   BitDepth  - I - Select desired bit-depth
 *                          0 = Reserved
 *                          1 = 1-bit
 *                          2 = 2-bit
 *                          3 = 3-bit
 *                          4 = 4-bit
 *                          5 = 5-bit
 *                          6 = 6-bit
 *                          7 = 7-bit
 *                          8 = 8-bit
 * @param   LEDSelect  - I -  Choose the LEDs that are on: b0 = Red, b1 = Green, b2 = Blue
 *                          0 = No LED (Pass Through)
 *                          1 = Red
 *                          2 = Green
 *                          3 = Yellow (Green + Red)
 *                          4 = Blue
 *                          5 = Magenta (Blue + Red)
 *                          6 = Cyan (Blue + Green)
 *                          7 = White (Red + Blue + Green)
 * @param   InvertPat  - I - true = Invert pattern
 *                           false = do not invert pattern
 * @param   InsertBlack  - I - true = Insert black-fill pattern after current pattern. This setting requires 230 μs
 *                                      of time before the start of the next pattern
 *                           false = do not insert any post pattern
 * @param   BufSwap  - I - true = perform a buffer swap
 *                           false = do not perform a buffer swap
 * @param   trigOutPrev  - I - true = Trigger Out 1 will continue to be high. There will be no falling edge
 *                                       between the end of the previous pattern and the start of the current pattern. w
 *                                       Exposure time is shared between all patterns defined under a common
 *                                       trigger out). This setting cannot be combined with the black-fill pattern
 *                           false = Trigger Out 1 has a rising edge at the start of a pattern, and a falling
 *                                       edge at the end of the pattern
 * @param   exp_time_us     -I expsoure time in microseconds for this pattern
 * @param   ptn_frame_period_us     -I frame time in microseconds for this pattern
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    unsigned long int lutWord = 0;

    lutWord = TrigType & 3;
    if(PatNum > 24)
        return -1;

    lutWord |= ((PatNum & 0x3F) << 2);
    if( (BitDepth > 8) || (BitDepth <= 0))
        return -1;
    lutWord |= ((BitDepth & 0xF) << 8);
    if(LEDSelect > 7)
        return -1;
    lutWord |= ((LEDSelect & 0x7) << 12);
    if(InvertPat)
        lutWord |= BIT16;
    if(InsertBlack)
        lutWord |= BIT17;
    if(BufSwap)
        lutWord |= BIT18;
    if(trigOutPrev)
        lutWord |= BIT19;

    g_ExpLut[g_ExpLutIndex++] = lutWord;
    g_ExpLut[g_ExpLutIndex++] = exp_time_us;
    g_ExpLut[g_ExpLutIndex++] = ptn_frame_period_us;
    return 0;
}

int FuncDLPC350Api::DLPC350_SetVarExpPatternConfig(unsigned int numLutEntries, unsigned int numPatsForTrigOut2, unsigned int numImages, bool repeat)
/**
 * (I2C: 0x5B)
 * (USB: CMD2: 0x1A, CMD3: 0x40)
 * This API controls the execution of patterns stored in the lookup table.
 * Before using this API, stop the current pattern sequence using DLPC350_PatternDisplay() API
 * After calling this API, send the Validation command using the API DLPC350_ValidatePatLutData() before starting the pattern sequence
 *
 * @param   numLutEntries - I - Number of LUT entries
 * @param   repeat - I - 0 = execute the pattern sequence once; 1 = repeat the pattern sequnce.
 * @param   numPatsForTrigOut2 - I - Number of patterns to display(range 1 through 256).
 *                                   If in repeat mode, then this value dictates how often TRIG_OUT_2 is generated.
 * @param   numImages - I - Number of Image Index LUT Entries(range 1 through 64).
 *                          This Field is irrelevant for Pattern Display Data Input Source set to a value other than internal.
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = numLutEntries-1;             /* -1 because the firmware command takes 0-based indices (0 means 1) */
    msg.text.data[3] = (numLutEntries-1)>>8;
    msg.text.data[4] = numPatsForTrigOut2 - 1;      /* -1 because the firmware command takes 0-based indices (0 means 1) */
    msg.text.data[5] = (numPatsForTrigOut2-1)>>8;
    msg.text.data[6] = (numImages - 1) & 0xFF;               /* -1 because the firmware command takes 0-based indices (0 means 1) */
    (repeat == true) ? (msg.text.data[7] = 0x01) : (msg.text.data[7] = 0x00);
    DLPC350_PrepWriteCmd(&msg, EXP_PAT_CONFIG);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_SendVarExpPatLut()
/**
 * (I2C: 0x5c)
 * (USB: CMD2: 0x1A, CMD3: 0x3E)
 * This API sends the pattern LUT created by calling DLPC350_AddToExpLut() API to the DLPC350 controller.
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;
    unsigned int i;

#if 0
    printf("VarExpPatLut Send\n");
    for(i=0;i<g_ExpLutIndex;i++)
    {
        printf("VarExpPatLut[%04d] = 0x%X\n",i,g_ExpLut[i]);
    }
#endif

    if(DLPC350_OpenMailbox(3) < 0)
        return -1;

    DLPC350_PrepWriteCmd(&msg, MBOX_EXP_DATA);

    for(i=0; i<g_ExpLutIndex; i+=3)
    {
        if(DLPC350_SetVarExpMboxAddr(i/3) < 0)
            return -1;

        msg.text.data[2] = g_ExpLut[i];
        msg.text.data[3] = g_ExpLut[i]>>8;
        msg.text.data[4] = g_ExpLut[i]>>16;
        msg.text.data[5] = g_ExpLut[i]>>24;

        msg.text.data[6] = g_ExpLut[i+1];
        msg.text.data[7] = g_ExpLut[i+1]>>8;
        msg.text.data[8] = g_ExpLut[i+1]>>16;
        msg.text.data[9] = g_ExpLut[i+1]>>24;

        msg.text.data[10] = g_ExpLut[i+2];
        msg.text.data[11] = g_ExpLut[i+2]>>8;
        msg.text.data[12] = g_ExpLut[i+2]>>16;
        msg.text.data[13] = g_ExpLut[i+2]>>24;
        DLPC350_SendMsg(&msg,true);
    }

    DLPC350_CloseMailbox();

    return 0;
}

int FuncDLPC350Api::DLPC350_OpenMailbox(int MboxNum)
/**
 * (I2C: 0x77)
 * (USB: CMD2: 0x1A, CMD3: 0x33)
 * This API opens the specified Mailbox within the DLPC350 controller. This API must be called
 * before sending data to the mailbox/LUT using DLPC350_SendPatLut() or DLPC350_SendImageLut() APIs.
 *
 * @param MboxNum - I - 1 = Open the mailbox for image index configuration
 *                      2 = Open the mailbox for pattern definition.
 *                      3 = Open the mailbox for the Variable Exposure
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = MboxNum;
    DLPC350_PrepWriteCmd(&msg, MBOX_CONTROL);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_SetVarExpMboxAddr(int Addr)
/**
 * (I2C: 0x5D)
 * (USB: CMD2: 0x1A, CMD3: 0x32)
 * This API defines the offset location within the DLPC350 mailboxes to write data into or to read data from
 *
 * @param Addr - I - 0-1823 - Defines the offset within the selected (opened) LUT to write/read data to/from.
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    if(Addr > 1823)
        return -1;

    msg.text.data[2] = Addr;
    msg.text.data[3] = Addr >> 8;
    DLPC350_PrepWriteCmd(&msg, MBOX_EXP_ADDRESS);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_CloseMailbox()
/**
 * (I2C: 0x77)
 * (USB: CMD2: 0x1A, CMD3: 0x33)
 * This API is internally used by other APIs within this file. There is no need for user application to
 * call this API separately.
 * This API closes all the Mailboxes within the DLPC350 controller.
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = 0;
    DLPC350_PrepWriteCmd(&msg, MBOX_CONTROL);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_SendVarExpImageLut(unsigned char *lutEntries, unsigned int numEntries)
/**
 * (I2C: )
 * (USB: CMD2:  CMD3: )
 * This API sends the image LUT to the DLPC350 controller.
 *
 * @param   *lutEntries - I - Pointer to the array in which LUT entries to be sent are stored
 *
 * @param   numEntries - I - number of entries to be sent to the controller
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;
    unsigned int i;
    unsigned int bytes_sent = 0;

    if(numEntries < 1 || numEntries > MAX_VAR_EXP_IMAGE_LUT_ENTRIES)
        return -1;

#if 0
    printf("VarExpImageLut Send\n");
    for(i=0;i<numEntries;i++)
    {
        printf("varExpImageLut[%02d] = %d\n",i,lutEntries[i]);
    }
#endif

    DLPC350_OpenMailbox(1);
    DLPC350_SetVarExpMboxAddr(0);

    // Check for special case of 2 entries
    if( numEntries == 2)
    {
        msg.text.data[2] = lutEntries[1];
        msg.text.data[3] = lutEntries[0];
    }
    else
    {
        for(i=0; i<numEntries; i++)
        {
            msg.text.data[2+i] = lutEntries[i];
        }
    }

    CmdList[MBOX_DATA].len = numEntries;
    DLPC350_PrepWriteCmd(&msg, MBOX_DATA);
    bytes_sent = DLPC350_SendMsg(&msg,true);
    DLPC350_CloseMailbox();

    return bytes_sent;
}

int FuncDLPC350Api::DLPC350_PatternDisplay(unsigned int Action)
/**
 * (I2C: 0x65)
 * (USB: CMD2: 0x1A, CMD3: 0x24)
 * This API starts or stops the programmed patterns sequence.
 *
 * @param   Action - I - Pattern Display Start/Stop Pattern Sequence
 *                          0 = Stop Pattern Display Sequence. The next "Start" command will
 *                              restart the pattern sequence from the beginning.
 *                          1 = Pause Pattern Display Sequence. The next "Start" command will
 *                              start the pattern sequence by re-displaying the current pattern in the sequence.
 *                          2 = Start Pattern Display Sequence
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = (Action&0x03);
    DLPC350_PrepWriteCmd(&msg, PAT_START_STOP);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_GetPatternDisplay(unsigned int *pAction)
/**
 * (I2C: 0x65)
 * (USB: CMD2: 0x1A, CMD3: 0x24)
 * This API starts or stops the programmed patterns sequence.
 *
 * @param   *pAction - O - Returns the state of the Pattern Display
 *                          0 = Stop Pattern Display Sequence. The next "Start" command will
 *                              restart the pattern sequence from the beginning.
 *                          1 = Pause Pattern Display Sequence. The next "Start" command will
 *                              start the pattern sequence by re-displaying the current pattern in the sequence.
 *                          2 = Start Pattern Display Sequence
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

     DLPC350_PrepReadCmd(PAT_START_STOP);

      if(DLPC350_Read() > 0)
      {
          memcpy(&msg, g_InputBuffer, 65);
          *pAction = (msg.text.data[0] & 0xFF);
          return 0;
      }
      return -1;
}

int FuncDLPC350Api::DLPC350_StartPatLutValidate()
/**
 * (I2C: 0x7D)
 * (USB: CMD2: 0x1A, CMD3: 0x1A)
 * This API checks the programmed pattern display modes and indicates any invalid settings.
 * This command needs to be executed after all pattern display configurations have been completed.
 *
 * @param   *pStatus - O
 *                      BIT0 = Validity of exposure or frame period settings
 *                             1 = Selected exposure or frame period settings are invalid
 *                             0 =Selected exposure or frame period settings are valid
 *                      BIT1 = Validity of pattern numbers in lookup table (LUT)
 *                             1 = Selected pattern numbers in LUT are invalid
 *                             0 = Selected pattern numbers in LUT are valid
 *                      BIT2 = Status of Trigger Out1
 *                             1 = Warning, continuous Trigger Out1 request or overlapping black sectors
 *                             0 = Trigger Out1 settings are valid
 *                      BIT3 = Status of post sector settings
 *                             1 = Warning, post vector was not inserted prior to external triggered vector
 *                             0 = Post vector settings are valid
 *                      BIT4 = Status of frame period and exposure difference
 *                             1 = Warning, frame period or exposure difference is less than 230usec
 *                             0 = Frame period or exposure difference is valid
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepWriteCmd(&msg, LUT_VALID);
    if(DLPC350_SendMsg(&msg,true) < 0)
        return -1;

    //    DLPC350_PrepReadCmd(LUT_VALID);

    //    //Poll for completion of validation
    //    do
    //    {
    //        if(DLPC350_Read() > 0)
    //        {
    //            memcpy(&msg, InputBuffer, 65);
    //            if(((uint8)msg.text.data[0]&0x80) == 0)
    //            {
    //                *pStatus = msg.text.data[0];
    //                break;
    //            }
    //            else
    //                continue;
    //        }
    //        else{
    //            return -1;
    //        }
    //    } while(1);

    return 0;
}

int FuncDLPC350Api::DLPC350_CheckPatLutValidate(bool *ready, unsigned int *pStatus)
/**
 * (I2C: 0x7D)
 * (USB: CMD2: 0x1A, CMD3: 0x1A)
 * This API checks the programmed pattern display modes and indicates any invalid settings.
 * This command needs to be executed after all pattern display configurations have been completed.
 *
 * @param   *pStatus - O
 *                      BIT0 = Validity of exposure or frame period settings
 *                             1 = Selected exposure or frame period settings are invalid
 *                             0 =Selected exposure or frame period settings are valid
 *                      BIT1 = Validity of pattern numbers in lookup table (LUT)
 *                             1 = Selected pattern numbers in LUT are invalid
 *                             0 = Selected pattern numbers in LUT are valid
 *                      BIT2 = Status of Trigger Out1
 *                             1 = Warning, continuous Trigger Out1 request or overlapping black sectors
 *                             0 = Trigger Out1 settings are valid
 *                      BIT3 = Status of post sector settings
 *                             1 = Warning, post vector was not inserted prior to external triggered vector
 *                             0 = Post vector settings are valid
 *                      BIT4 = Status of frame period and exposure difference
 *                             1 = Warning, frame period or exposure difference is less than 230usec
 *                             0 = Frame period or exposure difference is valid
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(LUT_VALID);

    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);
        if(((uint8)msg.text.data[0]&0x80) == 0)
        {
            *pStatus = msg.text.data[0];
            *ready = true;
        }
        else
        {
            *ready = false;
        }
    }
    else{
        return -1;
    }


    return 0;
}

int FuncDLPC350Api::DLPC350_SetMode(bool SLmode)
/**
 * The Display Mode Selection Command enables the DLPC350 internal image processing functions for
 * video mode or bypasses them for pattern display mode. This command selects between video or pattern
 * display mode of operation.
 *
 * @param   SLmode  - I - TRUE = Pattern Display mode. Assumes a 1-bit through 8-bit image with a pixel
 *                              resolution of 912 x 1140 and bypasses all the image processing functions of DLPC350
 *                          FALSE = Video Display mode. Assumes streaming video image from the 30-bit
 *                              RGB or FPD-link interface with a pixel resolution of up to 1280 x 800 up to 120 Hz.
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    msg.text.data[2] = SLmode;
    DLPC350_PrepWriteCmd(&msg, DISP_MODE);

    return DLPC350_SendMsg(&msg,true);
}

int FuncDLPC350Api::DLPC350_GetMode(bool *pMode)
/**
 * The Display Mode Selection Command enables the DLPC350 internal image processing functions for
 * video mode or bypasses them for pattern display mode. This command selects between video or pattern
 * display mode of operation.
 *
 * @param   SLmode  - O - TRUE = Pattern Display mode. Assumes a 1-bit through 8-bit image with a pixel
 *                              resolution of 912 x 1140 and bypasses all the image processing functions of DLPC350
 *                        FALSE = Video Display mode. Assumes streaming video image from the 30-bit
 *                              RGB or FPD-link interface with a pixel resolution of up to 1280 x 800 up to 120 Hz.
 *
 * @return  0 = PASS    <BR>
 *          -1 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(DISP_MODE);
    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);
        *pMode = (msg.text.data[0] != 0);
        return 0;
    }
    return -1;
}

int FuncDLPC350Api::DLPC350_GetNumImagesInFlash(unsigned int *pNumImgInFlash)
/**
 * (I2C: 0x0C)
 * (USB: CMD2: 0x1A, CMD3: 0x42)
 * This command reads number of images in the firmware running in DLPC350 controller.
 *
 * @param   *pNumImgInFlash  - O - Number of Images in the Flash.
 *
 * @return  >=0 = PASS    <BR>
 *          <0 = FAIL  <BR>
 *
 */
{
    hidMessageStruct msg;

    DLPC350_PrepReadCmd(NUM_IMAGE_IN_FLASH);

    if(DLPC350_Read() > 0)
    {
        memcpy(&msg, g_InputBuffer, 65);
        *pNumImgInFlash = (msg.text.data[0]&0xFF);
        return 0;
    }

    return -1;
}

int FuncDLPC350Api::DLPC350_USB_Close()
{
    hid_close(DeviceHandle);
    USBConnected = 0;

    return 0;
}

int FuncDLPC350Api::DLPC350_USB_Exit()
{
    return hid_exit();
}

void FuncDLPC350Api::ShowError(QString err)
{
    qDebug()<<err;
    emit sigErr(err);
}

bool FuncDLPC350Api::ProcessFlashParamsLine(QString line)
{
    unsigned int MfgID, DevID, i;
    bool ok;

    line = line.trimmed();
    if(line.startsWith('/'))
        return false;

    if(line.isEmpty())
        return false;

    MfgID = line.section(',',1,1).trimmed().toUInt(&ok, 0);
    if(!ok)
        return false;
    DevID = line.section(',',3,3).trimmed().toUInt(&ok, 0);
    if(!ok)
        return false;

    if((MfgID == g_FlashDevice.Mfg_ID) && (DevID == g_FlashDevice.Dev_ID))
    {
        g_FlashDevice.Mfg = line.section(',', 0, 0).trimmed();
        g_FlashDevice.Dev = line.section(',', 2, 2).trimmed();
        g_FlashDevice.Size_MBit = line.section(',', 4, 4).trimmed().toUInt(&ok, 0);
        if(!ok)
            return false;
        g_FlashDevice.Type = line.section(',', 5, 5).trimmed().toUInt(&ok, 0);
        if(!ok)
            return false;
        g_FlashDevice.numSectors = line.section(',', 7, 7).trimmed().toUInt(&ok, 0);
        if(!ok)
            return false;

        for(i=0; i<g_FlashDevice.numSectors; i++)
        {
            g_FlashDevice.SectorArr[i] = line.section(',', 8+i, 8+i).trimmed().toUInt(&ok, 0);
            if(!ok)
                return false;
        }

        return true;
    }
    return false;
}

int FuncDLPC350Api::GetSectorNum(unsigned int Addr)
{
    unsigned int i;
    for(i=0; i < g_FlashDevice.numSectors; i++)
    {
        if(g_FlashDevice.SectorArr[i] > Addr)
            break;
    }

    return (i==0)? 0 : (i-1);
}

static int DLPC350_PrepReadCmd(DLPC350_CMD cmd)
/**
 * This function is private to this file. Prepares the read-control command packet for the given command code and copies it to g_OutputBuffer.
 *
 * @param   cmd  - I - USB command code.
 *
 * @return  0 = PASS
 *          -1 = FAIL
 *
 */
{
    hidMessageStruct msg;

    msg.head.flags.rw = 1; //Read
    msg.head.flags.reply = 1; //Host wants a reply from device
    msg.head.flags.dest = 0; //Projector Control Endpoint
    msg.head.flags.reserved = 0;
    msg.head.flags.nack = 0;
    msg.head.seq = 0;

    msg.text.cmd = (CmdList[cmd].CMD2 << 8) | CmdList[cmd].CMD3;
    msg.head.length = 2;

    if(cmd == BL_GET_MANID)
    {
        msg.text.data[2] = 0x0C;
        msg.head.length += 1;
    }
    else if (cmd == BL_GET_DEVID)
    {
        msg.text.data[2] = 0x0D;
        msg.head.length += 1;
    }
    else if (cmd == BL_GET_CHKSUM)
    {
        msg.text.data[2] = 0x00;
        msg.head.length += 1;
    }

    g_OutputBuffer[0]=0; // First byte is the report number
    memcpy(&g_OutputBuffer[1], &msg, (sizeof(msg.head)+sizeof(msg.text.cmd) + msg.head.length));
    return 0;
}

static int DLPC350_Read()
/**
 * This function is private to this file. This function is called to write the read control command and then read back 64 bytes over USB
 * to g_InputBuffer.
 *
 * @return  number of bytes read
 *          -2 = nack from target
 *          -1 = error reading
 *
 */
{
    int ret_val;
    hidMessageStruct *pMsg = (hidMessageStruct *)g_InputBuffer;
    if(DLPC350_USB_Write() > 0)
    {
        ret_val =  DLPC350_USB_Read();

        if((pMsg->head.flags.nack == 1) || (pMsg->head.length == 0))
            return -2;
        else
            return ret_val;
    }
    return -1;
}

static int DLPC350_SendMsg(hidMessageStruct *pMsg, bool ackRequired)
/**
 * This function is private to this file. This function is called to send a message over USB; in chunks of 64 bytes.
 *
 * @return  number of bytes sent
 *          -1 = FAIL
 *
 */
{
    int maxDataSize = USB_MAX_PACKET_SIZE-sizeof(pMsg->head);
    int dataBytesSent = MIN(pMsg->head.length, maxDataSize);    //Send all data or max possible

    // Default the DLPC350_PrepWriteCmd() update write message for ACK
    // if user not expecting adjust accordingly
    if(!ackRequired)
        pMsg->head.flags.reply = 0;

    g_OutputBuffer[0]=0; // First byte is the report number
    memcpy(&g_OutputBuffer[1], pMsg, (sizeof(pMsg->head) + dataBytesSent));

    //Single packet transaction
    if(dataBytesSent >= pMsg->head.length)
    {
        if(DLPC350_Write(ackRequired) < 0)
            return -1;
    }
    else
    {
        //Send ACK request only for the last packet
        if(DLPC350_Write(0) < 0)
            return -1;

        while(dataBytesSent < pMsg->head.length)
        {
            memcpy(&g_OutputBuffer[1], &pMsg->text.data[dataBytesSent], USB_MAX_PACKET_SIZE);

            if((dataBytesSent+USB_MAX_PACKET_SIZE) >= (pMsg->head.length))
            {
                //last packet request for ACK
                if(DLPC350_Write(ackRequired) < 0)
                    return -1;
            }
            else
            {
                //middle packet
                if(DLPC350_Write(0) < 0)
                    return -1;
            }

            dataBytesSent += USB_MAX_PACKET_SIZE;
        }
    }

    return dataBytesSent+sizeof(pMsg->head);
}

static int DLPC350_Write(bool ackRequired)
{
    int ret_val;
    hidMessageStruct *pMsg;

    if(ackRequired)
    {
        pMsg = (hidMessageStruct *)g_InputBuffer;
        if((ret_val = DLPC350_USB_Write()) > 0)
        {
            //Check for ACK or NACK response
            if(DLPC350_USB_Read() > 0)
            {
                if(pMsg->head.flags.nack == 1)
                    return -2;
                else
                    return ret_val;
            }
        }
    }
    else
    {
       ret_val = DLPC350_USB_Write();
    }

    return ret_val;
}

int DLPC350_PrepWriteCmd(hidMessageStruct *pMsg, DLPC350_CMD cmd)
/**
 * This function is private to this file. Prepares the write command packet with given command code in the message structure pointer passed.
 *
 * @param   cmd  - I - USB command code.
 * @param   pMsg - I - Pointer to the message.
 *
 * @return  0 = PASS
 *          -1 = FAIL
 *
 */
{
    pMsg->head.flags.rw = 0; //Write
    pMsg->head.flags.reply = 1; //Host wants a reply from device
    pMsg->head.flags.dest = 0; //Projector Control Endpoint
    pMsg->head.flags.reserved = 0;
    pMsg->head.flags.nack = 0;
    pMsg->head.seq = g_SeqNum++;

    pMsg->text.cmd = (CmdList[cmd].CMD2 << 8) | CmdList[cmd].CMD3;
    pMsg->head.length = CmdList[cmd].len + 2;

    return 0;
}

int FuncDLPC350Api::FWUpload(QString imgPath, bool fastLoad)
{
    unsigned long long i;
    unsigned short manID;
    unsigned long long devID;
    char displayStr[255];
    unsigned int startSector=0, BLsize=0, lastSectorToErase,tempChkSum = 0;;
    unsigned char *pByteArray=NULL;
    unsigned char *pByteArrayCache=NULL;
    unsigned char *pSecMatchList=NULL;
    long long dataLen, dataLen_full;
    long long dataLenCache;
    int bytesSent;
    unsigned int expectedChecksum=0, checksum;
    long long percent_completion = 0;
    bool isCacheFileExist = false;
    QFile imgFile(imgPath);
    QFile flashParamFile("FlashDeviceParameters.txt");
    //QFile flashParamFile("D:/__Hg/DLP_LightCrafter4500/GUI/Flash/FlashDeviceParameters.txt");
    QFile imgFileCached;
    QFileInfoList fileInfoList;
    QFileInfo imgFileInfo;
    QDir dirImgFile;

//    QTime execMeaTimer;
//    int timeLap;

//    execMeaTimer.start();

    //
    //1. Parse the supplied binary image for validity
    //

    emit sigInfo("1. Parse the supplied binary image for validity");
    if(!imgFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<("Unable to open image file. Copy image file to a folder with Admin/read/write permission and try again\n");
        QString err = imgFile.errorString();
        qDebug()<<(err.toLatin1().data());
        emit sigErr(QString("Unable to open image file. Copy image file to a folder with Admin/read/write permission and try again\n")
                    +err);
        return -1;
    }

    dataLen = imgFile.size();
    pByteArray = (unsigned char *)malloc(dataLen);
    if(pByteArray == NULL)
    {
        imgFile.close();
        qDebug()<<("Unable to get enough memory to read the firmware image on the system");
        emit sigErr("Unable to get enough memory to read the firmware image on the system");
        return -1;
    }

    imgFile.read((char *)pByteArray, dataLen);

    int ret = DLPC350_Frmw_CopyAndVerifyImage(pByteArray, dataLen);
    if (ret)
    {
        switch(ret)
        {
        case ERROR_FRMW_FLASH_TABLE_SIGN_MISMATCH:
            ShowError("ERROR: Flash Table Signature doesn't match! Bad Firmware Image!\n");
            free(pByteArray);
            imgFile.close();
            return -1;
        case ERROR_NO_MEM_FOR_MALLOC:
            ShowError("Fatal Error! System Run out of memory\n");
            free(pByteArray);
            imgFile.close();
            return -1;
        default:
            break;
        }
    }
    free(pByteArray);
    imgFile.close();
    //默认快速加载（Fast Load）
    //
    //1.1. Check if FlashLoaderCachce directory exist
    //
    emit sigInfo("1.1. Check if FlashLoaderCachce directory exist");
    imgFileInfo.setFile(imgFile.fileName());
    dirImgFile.setPath(imgFileInfo.absoluteDir().absolutePath());
    if(dirImgFile.exists(("FlashLoaderCache"))){
        dirImgFile.setPath(imgFileInfo.absoluteDir().absolutePath()+"/FlashLoaderCache");
        fileInfoList = dirImgFile.entryInfoList();
        if(fileInfoList.size() > 0)
        {
            //first two files in directory contain . and .. so pick
            //the file after these two
            imgFileCached.setFileName(imgFileInfo.absoluteDir().absolutePath()+"/FlashLoaderCache/"+fileInfoList.at(2).fileName());
            dataLenCache = imgFileCached.size();
            pByteArrayCache = (unsigned char *)malloc(dataLenCache);
            if(pByteArrayCache)
            {
                if(!imgFileCached.open(QIODevice::ReadOnly))
                {
                    ShowError("Unable to open cached firmware image file. Copy image file to a folder with Admin/read/write permission and try again\n");
                    QString err = imgFileCached.errorString();
                    ShowError(err.toLatin1().data());
                    return -1;
                }

                imgFileCached.read((char *)pByteArrayCache, dataLenCache);
                int ret =  DLPC350_Frmw_CopyAndVerifyImage(pByteArrayCache, dataLenCache);
                if (ret)
                {
                    switch(ret)
                    {
                    case ERROR_NO_MEM_FOR_MALLOC:
                        ShowError("ERROR: [Cached firmware image] Fatal Error! System Run out of memory\n");
                        free(pByteArrayCache);
                        imgFileCached.close();
                        return -1;
                    case ERROR_FRMW_FLASH_TABLE_SIGN_MISMATCH:
                        ShowError("ERROR: [Cached firmware image] Flash Table Signature doesn't match! Bad Firmware Image!\n");
                        free(pByteArrayCache);
                        imgFileCached.close();
                        return -1;
                    default:
                        break;
                    }
                }
                else
                {
                    isCacheFileExist = true;
                }
            }

            free(pByteArrayCache);
            imgFileCached.close();
        }
    }

    //
    //2. Check if user selected to update bootloader code
    //
    emit sigInfo("2. Check if user selected to update bootloader code");
    BLsize = 128 * 1024; //128KB is bootloader size

    //
    //3. Enter programming mode
    //
    emit sigInfo("3. Enter programming mode");
    emit sigInfo("Waiting to enter programming mode");
    if(DLPC350_EnterProgrammingMode() < 0)
    {
        ShowError("Unable to enter Programming mode");
        return -1;
    }
    if(DLPC350_GetFlashDevID(&devID) < 0)
    {
        ShowError("Unable to read Flash Device ID");
        return -1;
    }

    //
    //4. Read Flash information
    //
    emit sigInfo("4. Read Flash information");
    if(DLPC350_GetFlashManID(&manID) < 0)
    {
        ShowError("Unable to read Flash Manufacturer ID");
        return -1;
    }

    if(DLPC350_GetFlashDevID(&devID) < 0)
    {
        ShowError("Unable to read Flash Device ID");
        return -1;
    }
    devID &= 0xFFFF;

    g_FlashDevice.Mfg_ID = manID;
    g_FlashDevice.Dev_ID = devID;

    //
    //5. Populate flash layout from FlashDeviceParameters for given g_FlashDevice Mfg_ID & Dev_ID
    //
    emit sigInfo("5. Populate flash layout from FlashDeviceParameters for given g_FlashDevice Mfg_ID & Dev_ID");
    if (!flashParamFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ShowError("Unable to open FlashDeviceParameters.txt");
        QString err = imgFileCached.errorString();
        ShowError(err.toLatin1().data());
        return -1;
    }

    QTextStream in(&flashParamFile);
    bool found = false;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(ProcessFlashParamsLine(line))
        {
            found = true;
            break;
        }
    }

    if(found == false)
    {
        sprintf(displayStr, "Unsupported Flash Device : Manufacturer ID = 0x%x & Device ID = 0x%llx", manID, devID);
        ShowError(displayStr);
        return -1;
    }

    flashParamFile.close();

    //
    //5.1 Make sure the cached file is matching to the checksum of stored content on the flash
    //
    emit sigInfo("5.1 Make sure the cached file is matching to the checksum of stored content on the flash");
    if(isCacheFileExist)
    {
        expectedChecksum = 0;
        qint64 tempVar;
        imgFileCached.open(QIODevice::ReadOnly);
        if(BLsize>0) {
            pByteArrayCache = (unsigned char *)malloc(BLsize);
            imgFileCached.read((char *)pByteArrayCache, BLsize);
            free(pByteArrayCache);
        }

        //buffer size to 1KB
        pByteArrayCache = (unsigned char *)malloc(1024);

        while((tempVar=imgFileCached.read((char *)pByteArrayCache, 1024)) > 0)
        {
            for(i=0;qint64(i)<tempVar;i++)
                expectedChecksum += pByteArrayCache[i];
            QCoreApplication::processEvents(); //Update the GUI
        }

        //Compute the checksum on the Flash by running checksum sector by sector
        tempChkSum = 0;
        checksum = 0;
        i = 0;
        if(BLsize)
        {
            while(g_FlashDevice.SectorArr[i] < BLsize)
                i++;

            if(i>=g_FlashDevice.numSectors)
            {
                ShowError("Sector size is not a multiple of bootloader application size of 128KB");
                return -1;
            }
        }

        while((i < g_FlashDevice.numSectors-2) && (g_FlashDevice.SectorArr[i+1] < imgFileCached.size()))
        {
            DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
            DLPC350_SetUploadSize(g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i]);
            DLPC350_CalculateFlashChecksum();
            DLPC350_WaitForFlashReady();
            if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
            {
                checksum += tempChkSum;
                i++;
            }
            else
            {
                ShowError("Error during checksum computation of the flash with Cached firmware");
                return -1;
            }
        }

        if(i == (g_FlashDevice.numSectors-2))
        {
            if(g_FlashDevice.SectorArr[i+1] < imgFileCached.size())
            {
                //last -1 sector
                DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
                DLPC350_SetUploadSize(g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i]);
                DLPC350_CalculateFlashChecksum();
                DLPC350_WaitForFlashReady();
                if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
                {
                    checksum += tempChkSum;
                    i++;
                }
                else
                {
                    ShowError("Error during checksum computation of the flash with Cached firmware");
                    return -1;
                }

                //last sector
                if(g_FlashDevice.SectorArr[i] < imgFileCached.size())
                {
                    DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
                    DLPC350_SetUploadSize(imgFileCached.size()-g_FlashDevice.SectorArr[i]);
                    DLPC350_CalculateFlashChecksum();
                    DLPC350_WaitForFlashReady();
                    if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
                    {
                        checksum += tempChkSum;
                    }
                    else
                    {
                        ShowError("Error during checksum computation of the flash with Cached firmware");
                        return -1;
                    }
                }
            }
            else
            {
                //last sector
                DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
                DLPC350_SetUploadSize(imgFileCached.size()-g_FlashDevice.SectorArr[i]);
                DLPC350_CalculateFlashChecksum();
                DLPC350_WaitForFlashReady();
                if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
                {
                    checksum += tempChkSum;
                    i++;
                }
                else
                {
                    ShowError("Error during checksum computation of the flash with Cached firmware");
                    return -1;
                }
            }

        }
        else
        {
            //g_FlashDevice.SectorArr[i+1] may be >= imgFileCached.size()
            DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
            DLPC350_SetUploadSize(imgFileCached.size()-g_FlashDevice.SectorArr[i]);
            DLPC350_CalculateFlashChecksum();
            DLPC350_WaitForFlashReady();
            if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
            {
                checksum += tempChkSum;
            }
            else
            {
                ShowError("Error during checksum computation of the flash with Cached firmware");
                return -1;
            }
        }

        if(checksum != expectedChecksum)
        {
            isCacheFileExist = false;
        }

        free(pByteArrayCache);
        imgFileCached.close();
    }

    //
    // 5.2 If the cache file and firmware image file that is already programmed matching
    // then proceed with comparing the mismtached sectors b/w cached file and new firmware file
    //
    if(isCacheFileExist)
    {

        imgFile.open(QIODevice::ReadOnly);
        imgFileCached.open(QIODevice::ReadOnly);
        pSecMatchList = (unsigned char *)malloc(g_FlashDevice.numSectors);
        if(pSecMatchList == NULL)
        {
            imgFile.close();
            imgFileCached.close();
            ShowError("Unable to get enough memory create sector mismatch list");
            return -1;
        }

        //the mismatch sector will be later set to 0x01 for programming
        memset(pSecMatchList,0x00,g_FlashDevice.numSectors);

#ifdef DEBUG_LOG_EN
        //Sectors outside image size fill with 0xFF
        i=0;
        while(g_FlashDevice.SectorArr[i] < imgFile.size()) i++;
        do
        {
           pSecMatchList[i] = 0xFF;
           i++;
        } while(i < g_FlashDevice.numSectors);
#endif

        pByteArrayCache = (unsigned char *)malloc(256);
        pByteArray = (unsigned char *)malloc(256);

        long long int offset = 0;
        while(offset < imgFile.size())
        {
            int rdsize = 256;
            if((offset+256) > imgFile.size())
                rdsize = (imgFile.size() - offset);

            int rdFromImgFile = imgFile.read((char *)pByteArray, rdsize);
            int rdFromCacheFile = imgFileCached.read((char *)pByteArrayCache, rdsize);

            if((rdFromImgFile == rdFromCacheFile) && (rdFromImgFile == rdsize))
            {
                if(memcmp((void*) pByteArray,(void*) pByteArrayCache,rdsize) != 0)
                {
#ifdef DEBUG_LOG_EN
                    qDebug() << "Mismatch@" << QString("0x%1").arg(offset , 0, 16);
                    QByteArray arrayA((const char*)pByteArray, rdsize);
                    QByteArray arrayB((const char*) pByteArrayCache, rdsize);
                    qDebug() << "Img file:" << QString(arrayA.toHex());
                    qDebug() << "Cached file:"<< QString(arrayB.toHex());
#endif
                    pSecMatchList[GetSectorNum(offset)] = 0x01;
                }
            }
            else
            {
#ifdef DEBUG_LOG_EN
                qDebug() << "EOF Mismatch@" << QString("0x%1").arg(offset , 0, 16);
#endif
                pSecMatchList[GetSectorNum(offset)] = 0x01;
            }

            offset += rdsize;
            QCoreApplication::processEvents();
        }

        free(pByteArrayCache);
        free(pByteArray);

        //Close file access to cached file
        imgFile.close();
        imgFileCached.close();
    }

    //
    //6. Program Flash
    //
    imgFile.open(QIODevice::ReadOnly);
    dataLen = imgFile.size();
    pByteArray = (unsigned char *)malloc(dataLen);
    imgFile.read((char *)pByteArray, dataLen);

    //DLPC350_BLSpecialMode(BIT0); //Skip Software Write Response from Bootloader

#ifdef DEBUG_LOG_EN
    if(isCacheFileExist)
    {
        qDebug() << "Sectors needs to be updated: 1 - Update 0 - No update";
        for(i=0;(i<g_FlashDevice.numSectors) && (pSecMatchList[i] != 0xFF);i++)
        {
                qDebug() << QString("0x%1").arg(g_FlashDevice.SectorArr[i] , 0, 16) << ":" << pSecMatchList[i] ;
        }
    }
#endif

    if(isCacheFileExist)
    {
        //There may be mismatch in the first bootloader area also
        //since bootloader selected to be skipped so even if the mismatch
        //detected in this area we must skip erasing this one
        for(i=0;i<g_FlashDevice.numSectors;i++)
            if(g_FlashDevice.SectorArr[i] <= BLsize)
                pSecMatchList[i] = 0x00;
    }

    startSector = GetSectorNum(BLsize);
    lastSectorToErase = GetSectorNum(imgFile.size());
    if(imgFile.size() == g_FlashDevice.SectorArr[lastSectorToErase]) //If perfectly aligned with last sector start addr, no need to erase last sector.
        lastSectorToErase -= 1;

    DLPC350_SetFlashType(g_FlashDevice.Type);

    for(i=startSector; i <= lastSectorToErase; i++)
    {
        if(isCacheFileExist && pSecMatchList[i] == 0x00)
        {
            continue; //skip erasing the sector
        }
        else
        {
            DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
            DLPC350_FlashSectorErase();
            //DLPC350_WaitForFlashReady();    //Wait for flash busy flag to go off
        }
        QCoreApplication::processEvents(); //Update the GUI
    }

    if(isCacheFileExist == false)
    {
        dataLen -= BLsize;
        DLPC350_SetFlashAddr(BLsize);
        DLPC350_SetUploadSize(dataLen);

        dataLen_full = dataLen;

        while(dataLen > 0)
        {
            bytesSent = DLPC350_UploadData(pByteArray+BLsize+dataLen_full-dataLen, dataLen);

            if(bytesSent < 0)
            {
                ShowError("Flash Data Download Failed");
                imgFile.close();
                free(pByteArray);
                return -1;
            }

            dataLen -= bytesSent;

            if(percent_completion != (((dataLen_full-dataLen)*100)/dataLen_full))
            {
                percent_completion = (((dataLen_full-dataLen)*100)/dataLen_full);
            }

            QCoreApplication::processEvents(); //Update the GUI
        }
    }
    else
    {
        percent_completion = 0;

        int NumSectForPgm = 0, PgmSect = 0; //Number of sectors to be programmed
        unsigned long long bytesWritten = 0, totByteToBeWritten = 0;
        for(i=0;i<g_FlashDevice.numSectors;i++) {
            if(pSecMatchList[i] == 0x01) {
                NumSectForPgm++;
                if(g_FlashDevice.SectorArr[i+1] < imgFile.size())
                {
                    totByteToBeWritten += g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i];
                }
                else
                {
                    totByteToBeWritten += (imgFile.size() - g_FlashDevice.SectorArr[i]);
                }
            }
        }

        //Selectively erase and program the mismatch areas on the flash
        for(i=0;i<=(g_FlashDevice.numSectors-2)&& NumSectForPgm;i++)
        {
            //Was the sector erased for updating the flash
            if(pSecMatchList[i] == 0x01)
            {
                PgmSect++;

                DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
                if(g_FlashDevice.SectorArr[i+1] < imgFile.size())
                {
                    dataLen = g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i];
                }
                else
                {
                    dataLen =  (imgFile.size() - g_FlashDevice.SectorArr[i]);
                }

                dataLen_full = dataLen;
                DLPC350_SetUploadSize(dataLen);

                while(dataLen>0)
                {
                    bytesSent = DLPC350_UploadData(pByteArray+g_FlashDevice.SectorArr[i]+dataLen_full-dataLen, dataLen);
                    bytesWritten += bytesSent;
                    if(bytesSent < 0)
                    {
                        ShowError("Flash Data Download Failed");
                        imgFile.close();
                        free(pByteArray);
                        return -1;
                    }
                    dataLen -= bytesSent;

                    percent_completion = (bytesWritten*100)/totByteToBeWritten;
                    QCoreApplication::processEvents(); //Update the GUI
                }
            }
        }

        //check if there is still last sector remaining to be
        //programmed
        if((PgmSect<NumSectForPgm) && pSecMatchList[i] == 0x01)
        {
            PgmSect++;
            dataLen = imgFile.size() - g_FlashDevice.SectorArr[i];
            dataLen_full = dataLen;
            while(dataLen>0)
            {
                bytesSent = DLPC350_UploadData(pByteArray+g_FlashDevice.SectorArr[i]+dataLen_full-dataLen, dataLen);
                bytesWritten += bytesSent;
                if(bytesSent < 0)
                {
                    ShowError("Flash Data Download Failed");
                    imgFile.close();
                    free(pByteArray);
                    return -1;
                }
                dataLen -= bytesSent;
                percent_completion = (bytesWritten*100)/totByteToBeWritten;
                QCoreApplication::processEvents(); //Update the GUI
            }
        }
    }

    QCoreApplication::processEvents(); //Update the GUI

    //
    //7. Compute checksum
    //

    //Get final checksum computation on new firmware file
    free(pByteArray);
    imgFile.close();

    expectedChecksum = 0;
    int tempVar;
    imgFile.open(QIODevice::ReadOnly);
    if(BLsize>0) {
        pByteArray = (unsigned char *)malloc(BLsize);
        imgFile.read((char *)pByteArray, BLsize);
        free(pByteArray);
    }
    //buffer size to 1KB
    pByteArray = (unsigned char *)malloc(1024);
    while((tempVar=imgFile.read((char *)pByteArray, 1024)) > 0)
    {
        for(i=0;i<tempVar;i++)
            expectedChecksum += pByteArray[i];
        QCoreApplication::processEvents(); //Update the GUI
    }
    //Compute the checksum for stored firmware image
    DLPC350_SetFlashAddr(BLsize);
    DLPC350_SetUploadSize(imgFile.size()-BLsize);
    free(pByteArray);
    imgFile.close();

    //Compute the checksum on the programmed flash
    tempChkSum = 0;
    checksum = 0;
    i = 0;
    if(BLsize)
    {
        while(g_FlashDevice.SectorArr[i] < BLsize)
            i++;

        if(i>=g_FlashDevice.numSectors)
        {
            ShowError("Sector size is not a multiple of bootloader application size of 128KB");
            return -1;
        }
    }
    while((i < g_FlashDevice.numSectors-2) && (g_FlashDevice.SectorArr[i+1] < imgFile.size()))
    {
        DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
        DLPC350_SetUploadSize(g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i]);
        DLPC350_CalculateFlashChecksum();
        DLPC350_WaitForFlashReady();
        if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
        {
            checksum += tempChkSum;
            i++;
        }
        else
        {
            ShowError("Error during checksum computation of the flash with Cached firmware");
            return -1;
        }
    }

    if(i == (g_FlashDevice.numSectors-2))
    {
        if(g_FlashDevice.SectorArr[i+1] < imgFile.size())
        {
            //last -1 sector
            DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
            DLPC350_SetUploadSize(g_FlashDevice.SectorArr[i+1]-g_FlashDevice.SectorArr[i]);
            DLPC350_CalculateFlashChecksum();
            DLPC350_WaitForFlashReady();
            if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
            {
                checksum += tempChkSum;
                i++;
            }
            else
            {
                ShowError("Error during checksum computation of the flash with Cached firmware");
                return -1;
            }

            //last sector
            if(g_FlashDevice.SectorArr[i] < imgFile.size())
            {
                DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
                DLPC350_SetUploadSize(imgFile.size()-g_FlashDevice.SectorArr[i]);
                DLPC350_CalculateFlashChecksum();
                DLPC350_WaitForFlashReady();
                if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
                {
                    checksum += tempChkSum;
                }
                else
                {
                    ShowError("Error during checksum computation of the flash with Cached firmware");
                    return -1;
                }
            }
        }
        else
        {
            //last sector
            DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
            DLPC350_SetUploadSize(imgFile.size()-g_FlashDevice.SectorArr[i]);
            DLPC350_CalculateFlashChecksum();
            DLPC350_WaitForFlashReady();
            if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
            {
                checksum += tempChkSum;
                i++;
            }
            else
            {
                ShowError("Error during checksum computation of the flash with Cached firmware");
                return -1;
            }
        }
    }
    else
    {
        //g_FlashDevice.SectorArr[i+1] may be >= imgFile.size()
        DLPC350_SetFlashAddr(g_FlashDevice.SectorArr[i]);
        DLPC350_SetUploadSize(imgFile.size()-g_FlashDevice.SectorArr[i]);
        DLPC350_CalculateFlashChecksum();
        DLPC350_WaitForFlashReady();
        if(DLPC350_GetFlashChecksum(&tempChkSum) >= 0)
        {
            checksum += tempChkSum;
        }
        else
        {
            ShowError("Error during checksum computation of the flash with Cached firmware");
            return -1;
        }
    }


    if(checksum != expectedChecksum)
    {
        sprintf(displayStr, "Checksum mismatch: Expected %x; Received %x", expectedChecksum, checksum);
        ShowError(displayStr);
    }
    else
    {

        //if(ui->checkBox_FastFlashUpdate->isChecked())
        if(true)
        {
            imgFile.close();
            imgFile.open(QIODevice::ReadOnly);
            dataLen = imgFile.size();
            pByteArray = (unsigned char *)malloc(dataLen);
            imgFile.read((char *)pByteArray, dataLen);

            QString dirPath = imgFileInfo.absoluteDir().absolutePath();
            imgFileInfo.setFile(imgFile.fileName());
            dirImgFile.setPath(dirPath);
            if( dirImgFile.exists(("FlashLoaderCache")) == false)
            {
                dirImgFile.mkdir("FlashLoaderCache");
            }

            dirPath.append("/FlashLoaderCache");
            dirImgFile.setPath(dirPath);

            //Delete old files
            foreach(QString dirFile,dirImgFile.entryList())
            {
                //delete only the file name
                if(dirFile != "." && dirFile != ".." )
                    dirImgFile.remove(dirFile);
            }

            QDateTime t = QDateTime::currentDateTime();
            QString temp = t.toString("yyyyMMdd_hhmmss");
            dirPath.append("/");
            imgFileCached.setFileName(dirPath+temp);

            if(!imgFileCached.open(QIODevice::WriteOnly))
            {
                ShowError("Unable to create cached firmware image file for storing on the PC. Check folder with Admin/read/write permission and try again\n");
                QString err = imgFileCached.errorString();
                ShowError(err.toLatin1().data());
                imgFile.close();
                free(pByteArray);
                return -1;
            }

            //copy the contents from the image file
            if(imgFileCached.write((char *)pByteArray,imgFile.size())<0)
            {
                QString err = imgFileCached.errorString();
                ShowError(err.toLatin1().data());
                imgFile.close();
                free(pByteArray);
                imgFileCached.close();
                return -1;
            }

            imgFileCached.flush();
        }

//        timeLap = execMeaTimer.elapsed();

        DLPC350_ExitProgrammingMode(); //Exit programming mode; Start application.
    }

//    //Convert milliseconds to HH:MM:SS
//    timeLap /= 1000; //ms to seconds
//    int hh = (timeLap/3600); //number of hours
//    timeLap -= (hh*3600); //remaining seconds
//    int mm = (timeLap/60); // number of minutes
//    timeLap -= (mm*60); //remaining seconds
//    int ss = timeLap;


    return 0;
}

int FuncDLPC350Api::setColor()
{
    return 1;
}

int FuncDLPC350Api::setTrigger(TriggerType type)
{
    return type;
}
