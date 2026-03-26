#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <atomic>
#include <pylon/PylonIncludes.h>
#include "storagethread.h"

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr);
    ~CameraThread() override;

    // 相机生命周期控制
    bool connectCamera();
    void disconnectCamera();
    void startAcquisition();
    void stopAcquisition();

    // 参数设置接口
    void setExposureTime(double value);
    void setGain(double value);
    void setPacketSize(int value);

    // 获取当前相机参数用于更新UI
    double getCurrentExposureTime();
    double getCurrentGain();
    int getCurrentPacketSize();
    bool isGigECamera();

    void openStorage(const QString &filePath) { if (m_storageThread) m_storageThread->openStorage(filePath); }
    void closeStorage() { if (m_storageThread) m_storageThread->closeStorage(); }
    void setContinuousRecord(bool enable) { if (m_storageThread) m_storageThread->setContinuousRecord(enable); }
    void triggerSnapshot() { if (m_storageThread) m_storageThread->triggerSnapshot(); }

    bool getCameraOpen(){return m_isOpen;}


signals:
    void imageReady(const QImage &image);
    void sigCameraConnected();
    void sigInfo(const QString &info);
    void cameraError(const QString &errorMsg); // 抛出异常信号

protected:
    void run() override;

private:
    std::atomic<bool> m_isGrabbing;
    Pylon::CInstantCamera *m_camera; // 改为指针，以便在不同函数中操作
    StorageThread *m_storageThread;
    bool m_isOpen = false;
};

#endif // CAMERATHREAD_H
