#ifndef CAMBASLERC_H
#define CAMBASLERC_H

//ver 1.0.0.1
/*
 * 巴斯勒相机dll
 * 版本更新说明：
 * ver 1.0.0.0 10月25日测试通过。
 * ver 1.0.0.1 11.8日，跟随基类增加新的功能 getLastData(void* data)
 * ver 1.0.1.0 11.20日，跟随基类增加新的功能 shutDown()和Restart()，这两个功能暂时没有作用
 * ver 1.0.1.1 11.20日，跟随基类增加新的功能 openRadio()和closeRadio()，这两个功能暂时没有作用
 */

#include "icamera.h"
#include "pylonc/PylonC.h"
#include <QTimer>

#define NUM_BUFFERS     5
#define STREAM_INVAL    80
class SENSORS_EXPORT CamBaslerC : public ICamera
{
    Q_OBJECT
public:
    CamBaslerC();
    virtual int     init(QJsonObject param) override;
                    //int camNum
    virtual int     open() override;
    virtual int     close() override;
    virtual int     startAcq() override;
    virtual int     stopAcq() override;
    virtual int     block() override;
    virtual int     unblock() override;
    virtual int     reset() override;
    virtual int     getLastData(void* data) override;
    virtual int     shutDown() override;
    virtual int     restart() override;
    virtual int     startRadio() override;
    virtual int     stopRadio() override;

    virtual int     setExpTime(int exp) override;
    virtual int     setGain(int gain) override;
    virtual int     setSource(int power) override;
    virtual int     grabImage(QImage &image) override;
    virtual int     grabImageRaw(unsigned char* buffer) override;
    virtual int     openStream() override;
    virtual int     closeStream() override;

    virtual int     setLineRate(int lineRate) override;
    virtual int     setImgWidth(int width) override;
    virtual int     setImgHeight(int height) override;
            int     grabImgMSVC(QImage &image);
            int     setPacketSize(int packetSize);

protected:
    virtual int     test() override;

public:
    QImage          getLastImg();
    unsigned char*  getLastImgRaw();    ////不要对指针进行任何操作
    QJsonObject     getDevInfo();       ////QString ModelName,QString SerualNumber,QString FullName,int payloadSize
    void            getImageStatus(int &width,int &height,int &format);

private:
    int             initStream();
    int             cameraConfig();
    virtual int     checkRes() override;
    int             grabBySteram();
    int             push2ImgList(unsigned char* buffer);
    QImage          uc2Img(unsigned char* buffer);
    int             freshStreamBuffer();
    void            functionStart(QString functionName);
    void            functionOver(QString functionName);


private slots:
    void            onStreamTimeOut();

private:
    PYLON_DEVICE_HANDLE         hDev;
    PYLON_STREAMGRABBER_HANDLE  hGrabber;
    PYLON_WAITOBJECT_HANDLE     hWait;
    HRESULT                     m_pRes;
    PylonDeviceInfo_t           m_pDevInfo;

    GENAPIC_RESULT              m_pResult;
    size_t                      m_pPayloadSize;
    unsigned char               *m_pStreamBuffers[NUM_BUFFERS];
    PYLON_STREAMBUFFER_HANDLE   m_pStreamBufferHandles[NUM_BUFFERS];
    PylonGrabResult_t           m_pStreamGrabResult;
    QJsonObject                 m_pInitParam;
    QImage                      m_pLastImg;

    bool                        m_pIsAvail;
    bool                        m_pCamIsOpen;
    bool                        m_pStreamInit;
    bool                        m_pStreamOpen;
    QTimer                      streamTimer;
    QImage                      *m_pListImage[NUM_BUFFERS];
};

#endif // CAMBASLERC_H
