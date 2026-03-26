#include "storagethread.h"
#include <QDebug>
#include <stdio.h>

StorageThread::StorageThread(QObject *parent)
    : QThread(parent),
      m_threadRunning(false),
      m_continuousMode(false),
      m_snapshotRequested(false)
{
}

StorageThread::~StorageThread()
{
    closeStorage();
}

bool StorageThread::openStorage(const QString &filePath)
{
    if (m_threadRunning) return false; // 已经打开了

    m_filePath = filePath;
    m_threadRunning = true;
    m_continuousMode = false;
    m_snapshotRequested = false;

    start(QThread::HighPriority);
    return true;
}

void StorageThread::closeStorage()
{
    if (m_threadRunning) {
        m_threadRunning = false;
        m_cond.wakeAll();
        wait();
    }
}

void StorageThread::setContinuousRecord(bool enable)
{
    m_continuousMode = enable;
}

void StorageThread::triggerSnapshot()
{
    m_snapshotRequested = true; // 外部发个信号：我要下一张！
}

void StorageThread::addFrame(const Pylon::CGrabResultPtr &grabResult)
{
    if (!m_threadRunning) return;

    // 【核心拦截逻辑】：如果不是连续录制，又没请求单帧，直接丢弃（回收内存）
    if (!m_continuousMode && !m_snapshotRequested) return;

    QMutexLocker locker(&m_mutex);
    if (m_queue.size() > 200) {
        return; // 防爆内存
    }

    m_queue.enqueue(grabResult);

    // 【关键】：如果是单帧抓拍，收下这张后立马“关门”，保证只存一张
    if (m_snapshotRequested) {
        m_snapshotRequested = false;
    }

    m_cond.wakeOne();
}

void StorageThread::run()
{
    FILE* file = fopen(m_filePath.toLocal8Bit().constData(), "wb");
    if (!file) {
        qDebug() << "Failed to open file for writing.";
        return;
    }

    setvbuf(file, nullptr, _IOFBF, 32 * 1024 * 1024);

    // 大循环：只要文件还要求开着，或者队列里还有没写完的数据，就一直跑
    while (m_threadRunning || !m_queue.isEmpty()) {
        Pylon::CGrabResultPtr frame;

        {
            QMutexLocker locker(&m_mutex);
            // 队列为空时，线程进入完全休眠，0 CPU 占用，直到有新一帧被放进来
            while (m_queue.isEmpty() && m_threadRunning) {
                m_cond.wait(&m_mutex);
            }
            if (m_queue.isEmpty()) break;
            frame = m_queue.dequeue();
        }

        if (frame.IsValid() && frame->GrabSucceeded()) {
            fwrite(frame->GetBuffer(), 1, frame->GetPayloadSize(), file);
        }
    }

    fflush(file);
    fclose(file);
    qDebug() << "Storage file closed successfully.";
}
