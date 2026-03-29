#include "rawreader.h"
#include <QDebug>

RawReader::RawReader(QObject *parent)
    : QObject(parent), m_width(0), m_height(0),
      m_frameSizeBytes(0), m_totalFrames(0), m_currentIndex(-1)
{
}

RawReader::~RawReader()
{
    close();
}

void RawReader::setResolution(int width, int height)
{
    if (width <= 0 || height <= 0) return;

    m_width = width;
    m_height = height;
    m_frameSizeBytes = static_cast<qint64>(m_width) * static_cast<qint64>(m_height);

    // 极其智能的细节：如果此时已经打开了一个文件，分辨率改变后自动重新计算总帧数
    if (m_file.isOpen() && m_frameSizeBytes > 0) {
        m_totalFrames = m_file.size() / m_frameSizeBytes;
        // 如果当前索引超出了新的总帧数，将其拉回最后一帧
        if (m_currentIndex >= m_totalFrames) {
            m_currentIndex = m_totalFrames > 0 ? m_totalFrames - 1 : 0;
        }
    }
}

bool RawReader::open(const QString &filePath)
{
    close();

    m_totalFrames = 0;
    if (m_frameSizeBytes <= 0) {
        qDebug() << "Error: Resolution not set! Call setResolution() first.";
        return false;
    }

    m_file.setFileName(filePath);
    if (!m_file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open RAW file for reading:" << filePath;
        return false;
    }

    m_totalFrames = m_file.size() / m_frameSizeBytes;
    m_currentIndex = 0;

    return m_totalFrames > 0;
}

void RawReader::close()
{
    if (m_file.isOpen()) {
        m_file.close();
    }
    m_totalFrames = 0;
    m_currentIndex = -1;
}

qint64 RawReader::getTotalFrames() const
{
    return m_totalFrames;
}

qint64 RawReader::getCurrentIndex() const
{
    return m_currentIndex;
}

QImage RawReader::getFrame(qint64 index)
{
    if (!m_file.isOpen() || index < 0 || index >= m_totalFrames || m_frameSizeBytes <= 0) {
        return QImage();
    }

    qint64 offset = index * m_frameSizeBytes;
    if (!m_file.seek(offset)) {
        return QImage();
    }

    QByteArray data = m_file.read(m_frameSizeBytes);
    if (data.size() != m_frameSizeBytes) {
        return QImage();
    }

    QImage img((const uchar*)data.constData(),
               m_width,
               m_height,
               m_width,
               QImage::Format_Grayscale8);

    m_currentIndex = index;
    return img.copy();
}

QImage RawReader::getNextFrame()
{
    return getFrame(m_currentIndex + 1);
}

QImage RawReader::getPrevFrame()
{
    return getFrame(m_currentIndex - 1);
}
