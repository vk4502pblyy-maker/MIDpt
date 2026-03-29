#ifndef RAWREADER_H
#define RAWREADER_H

#include <QObject>
#include <QFile>
#include <QImage>
#include <QString>

class RawReader : public QObject
{
    Q_OBJECT

public:
    explicit RawReader(QObject *parent = nullptr);
    ~RawReader();

    // 1. 动态设置分辨率（随时可以调用）
    void setResolution(int width, int height);
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    // 2. 打开和关闭文件
    bool open(const QString &filePath);
    void close();

    // 3. 状态获取
    qint64 getTotalFrames() const;
    qint64 getCurrentIndex() const;

    // 4. 核心回放接口
    QImage getFrame(qint64 index);
    QImage getNextFrame();
    QImage getPrevFrame();

private:
    QFile m_file;
    int m_width;
    int m_height;
    qint64 m_frameSizeBytes; // 单帧所占字节数
    qint64 m_totalFrames;    // 总帧数
    qint64 m_currentIndex;   // 当前帧索引
};

#endif // RAWREADER_H
