#ifndef STORAGETHREAD_H
#define STORAGETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QString>
#include <atomic>
#include <pylon/PylonIncludes.h>

class StorageThread : public QThread
{
    Q_OBJECT
public:
    explicit StorageThread(QObject *parent = nullptr);
    ~StorageThread() override;

    // 1. 文件生命周期管理 (控制文件的开启和关闭)
    bool openStorage(const QString &filePath);
    void closeStorage();

    // 2. 模式控制
    void setContinuousRecord(bool enable); // 开启/关闭连续录制
    void triggerSnapshot();                // 触发一次关键帧抓拍

    // 3. 接收底层指针
    void addFrame(const Pylon::CGrabResultPtr &grabResult);

protected:
    void run() override;

private:
    std::atomic<bool> m_threadRunning;     // 控制线程和文件大循环
    std::atomic<bool> m_continuousMode;    // 是否处于连续录制状态
    std::atomic<bool> m_snapshotRequested; // 是否请求了单帧抓拍

    QString m_filePath;
    QQueue<Pylon::CGrabResultPtr> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
};

#endif // STORAGETHREAD_H
