#include "camerathread.h"
#include <QDebug>
#include <QElapsedTimer>

using namespace Pylon;
using namespace GenApi;

CameraThread::CameraThread(QObject *parent) : QThread(parent), m_isGrabbing(false), m_camera(nullptr)
{
    PylonInitialize();
    m_storageThread = new StorageThread(this);
}

CameraThread::~CameraThread()
{
    stopAcquisition();
    disconnectCamera();
    PylonTerminate();
}

bool CameraThread::connectCamera()
{
    try {
        if (!m_camera) {
            m_camera = new CInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        }
        if (!m_camera->IsOpen()) {
            m_camera->Open();
            qDebug() << "Connected to:" << m_camera->GetDeviceInfo().GetModelName();
        }
        emit sigCameraConnected();
        m_isOpen = true;
        return true;
    } catch (const GenericException &e) {
        emit cameraError(QString("连接相机失败: %1").arg(e.GetDescription()));
        return false;
    }
}

void CameraThread::disconnectCamera()
{
    stopAcquisition();
    if (m_camera && m_camera->IsOpen()) {
        m_camera->Close();
        delete m_camera;
        m_camera = nullptr;
        m_isOpen = false;
    }
}

void CameraThread::startAcquisition()
{
    if (m_camera && m_camera->IsOpen() && !m_isGrabbing) {
        m_isGrabbing = true;
        start(); // 启动 QThread 的 run() 函数
    }
}

void CameraThread::stopAcquisition()
{
    if (m_isGrabbing) {
        m_isGrabbing = false;
        wait(); // 等待线程安全退出
    }
}

void CameraThread::run()
{
    try {
        // 【关键修改 1】：扩大相机的底层缓存池！默认只有 10 帧，
                // 在 367fps 下，SSD 哪怕有 30 毫秒的系统抖动就会把 10 帧耗尽。我们将其扩大到 150 帧。
        m_camera->MaxNumBuffer = 150;
        m_camera->StartGrabbing(GrabStrategy_OneByOne);
        CGrabResultPtr ptrGrabResult;
        int frameCounter = 0; // 用于 UI 抽帧计数
        QElapsedTimer uiTimer;
        uiTimer.start();

        while (m_isGrabbing && m_camera->IsGrabbing()) {
            m_camera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if (ptrGrabResult->GrabSucceeded()) {
                if (m_storageThread) {
                                    m_storageThread->addFrame(ptrGrabResult);
                                }
                if (uiTimer.hasExpired(16)) {
                QImage img((const uchar*)ptrGrabResult->GetBuffer(),
                           ptrGrabResult->GetWidth(),
                           ptrGrabResult->GetHeight(),
                           ptrGrabResult->GetWidth(),
                           QImage::Format_Grayscale8);

                emit imageReady(img.copy());
                uiTimer.restart();
            }
            }
        }
        m_camera->StopGrabbing();
    } catch (const GenericException &e) {
        emit cameraError(QString("采集错误: %1").arg(e.GetDescription()));
        m_isGrabbing = false;
    }
}

// ---------------- 参数读写部分 (做了多版本相机的兼容) ----------------

void CameraThread::setExposureTime(double value)
{
    if (!m_camera || !m_camera->IsOpen()) return;
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        CFloatParameter exposure(nodemap, "ExposureTime");
        if (!exposure.IsValid()) exposure.Attach(nodemap, "ExposureTimeAbs"); // 兼容旧型号
        if (exposure.IsValid() && exposure.IsWritable()) {
            exposure.SetValue(value);
        }
    } catch (...) { /* 忽略不支持的异常 */ }
}

void CameraThread::setGain(double value)
{
    if (!m_camera || !m_camera->IsOpen()) return;
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        CFloatParameter gain(nodemap, "Gain");
        if (!gain.IsValid()) gain.Attach(nodemap, "GainRaw"); // 兼容旧型号
        if (gain.IsValid() && gain.IsWritable()) {
            gain.SetValue(value);
        }
    } catch (...) { }
}

double CameraThread::getCurrentExposureTime()
{
    if (!m_camera || !m_camera->IsOpen()) return 0.0;
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        CFloatParameter exposure(nodemap, "ExposureTime");
        if (!exposure.IsValid()) exposure.Attach(nodemap, "ExposureTimeAbs");
        if (exposure.IsValid() && exposure.IsReadable()) return exposure.GetValue();
    } catch (...) {}
    return 0.0;
}

double CameraThread::getCurrentGain()
{
    if (!m_camera || !m_camera->IsOpen()) return 0.0;
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        CFloatParameter gain(nodemap, "Gain");
        if (!gain.IsValid()) gain.Attach(nodemap, "GainRaw");
        if (gain.IsValid() && gain.IsReadable()) return gain.GetValue();
    } catch (...) {}
    return 0.0;
}

void CameraThread::setPacketSize(int value)
{
    if (!m_camera || !m_camera->IsOpen()) return;
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        // GevSCPSPacketSize 是 GigE 相机特有的包大小控制节点
        CIntegerParameter packetSize(nodemap, "GevSCPSPacketSize");
        if (packetSize.IsValid() && packetSize.IsWritable()) {
            packetSize.SetValue(value);
        }
    } catch (...) { /* 忽略不支持的异常 */ }
}

int CameraThread::getCurrentPacketSize()
{
    if (!m_camera || !m_camera->IsOpen()) return 1500; // 默认返回标准以太网包大小
    try {
        INodeMap& nodemap = m_camera->GetNodeMap();
        CIntegerParameter packetSize(nodemap, "GevSCPSPacketSize");
        if (packetSize.IsValid() && packetSize.IsReadable()) {
            return static_cast<int>(packetSize.GetValue());
        }
    } catch (...) {}
    return 1500;
}

bool CameraThread::isGigECamera()
{
    if (!m_camera || !m_camera->IsOpen()) return false;
    // 获取设备类名称，GigE 相机会返回 "BaslerGigE"
    return m_camera->GetDeviceInfo().GetDeviceClass() == "BaslerGigE";
}
