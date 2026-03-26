#ifndef ICAMERA_H
#define ICAMERA_H

#include "Prototype/prototypesensor.h"
#include <QImage>

class SENSORS_EXPORT  ICamera : public PrototypeSensor
{
    Q_OBJECT
public:
    virtual int init(QJsonObject param) = 0;
    virtual int open() = 0;
    virtual int close() = 0;
    virtual int startAcq() = 0;
    virtual int stopAcq() = 0;
    virtual int block() = 0;
    virtual int unblock() = 0;
    virtual int reset() = 0;
    virtual int getLastData(void* data) = 0;
    virtual int shutDown() = 0;
    virtual int restart() = 0;
    virtual int startRadio() = 0;
    virtual int stopRadio() = 0;

    virtual int setExpTime(int exp) = 0;
    virtual int setGain(int gain) = 0;
    virtual int setSource(int power) = 0;
    virtual int grabImage(QImage &image) = 0;
    virtual int grabImageRaw(unsigned char* buffer) = 0;
    virtual int openStream() = 0;
    virtual int closeStream() = 0;

    virtual int setLineRate(int lineRate) = 0;
    virtual int setImgWidth(int width) = 0;
    virtual int setImgHeight(int height) = 0;

protected:
    virtual int test() = 0;

protected:
    virtual int checkRes();
    virtual unsigned char* uint8_2_uchar(uint8_t *u8,size_t length);

signals:

protected:
    QString         m_pImgPath;
    QString         m_pImgDir;
    unsigned char   *m_pImgBuf;
    unsigned char   *m_pImgBufs[5];
    int             m_pImgHeight;
    int             m_pImgWidth;
    int             m_pPixelType;
    int             m_pCamType;
    int             m_pLineRate;

};

#endif // ICAMERA_H
