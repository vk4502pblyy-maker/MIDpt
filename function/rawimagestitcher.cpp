#include "RawImageStitcher.h"

RawImageStitcher::RawImageStitcher(int rawWidth, int rawHeight, int rawType, const QSize& targetResolution)
    : m_rawWidth(rawWidth)
    , m_rawHeight(rawHeight)
    , m_rawType(rawType)
    , m_targetResolution(targetResolution)
{
}

cv::Mat RawImageStitcher::readRawFrame(const QString& filePath, int index)
{
    // 计算单帧图像占据的字节数
    int elemSize = CV_ELEM_SIZE(m_rawType);
    qint64 frameBytes = static_cast<qint64>(m_rawWidth) * m_rawHeight * elemSize;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件:" << filePath;
        return cv::Mat();
    }

    qint64 offset = index * frameBytes;

    // 检查文件大小是否足够包含指定的帧
    if (file.size() < offset + frameBytes) {
        qWarning() << "索引超出文件范围！Index:" << index;
        file.close();
        return cv::Mat();
    }

    // 移动文件指针到对应索引的位置
    if (!file.seek(offset)) {
        qWarning() << "文件定位失败！";
        file.close();
        return cv::Mat();
    }

    // 创建 OpenCV 矩阵，并直接将数据读入 Mat 的内存缓冲区
    cv::Mat frame(m_rawHeight, m_rawWidth, m_rawType);
    qint64 bytesRead = file.read(reinterpret_cast<char*>(frame.data), frameBytes);

    file.close();

    if (bytesRead != frameBytes) {
        qWarning() << "读取的数据不完整！预期:" << frameBytes << "实际:" << bytesRead;
        return cv::Mat();
    }

    return frame;
}

cv::Mat RawImageStitcher::stitchAndResize(const std::vector<cv::Mat>& images, int rows, int cols)
{
    if (images.empty()) {
        qWarning() << "传入的图像列表为空！";
        return cv::Mat();
    }

    if (images.size() != static_cast<size_t>(rows * cols)) {
        qWarning() << "图像数量与行列数不匹配！ Images size:" << images.size()
                   << " Expected:" << rows * cols;
        return cv::Mat();
    }

    // 计算拼图所需的整体大图的尺寸
    int totalWidth = cols * m_rawWidth;
    int totalHeight = rows * m_rawHeight;

    // 创建一张全黑的背景大图 (类型与单张图保持一致)
    cv::Mat stitchedMat = cv::Mat::zeros(totalHeight, totalWidth, m_rawType);

    // 将单张图拷贝到大图的对应区域 (ROI)
    for (int i = 0; i < images.size(); ++i) {
        int r = i / cols; // 当前图所在的行
        int c = i % cols; // 当前图所在的列

        // 计算当前图应该放在大图的哪个区域
        cv::Rect roi(c * m_rawWidth, r * m_rawHeight, m_rawWidth, m_rawHeight);

        // 提取大图中的 ROI 区域，并将当前图拷贝进去
        images[i].copyTo(stitchedMat(roi));
    }

    // 将拼接好的大图缩放(压缩)到指定的固定分辨率
    cv::Mat finalMat;
    cv::Size targetCvSize(m_targetResolution.width(), m_targetResolution.height());

    // cv::INTER_AREA 在缩小图像时能提供更好的无莫尔条纹效果
    cv::resize(stitchedMat, finalMat, targetCvSize, 0, 0, cv::INTER_AREA);

    return finalMat;
}

cv::Mat RawImageStitcher::processWorkFlow(const QString& filePath, const std::vector<int>& indices, int rows, int cols)
{
    std::vector<cv::Mat> imagesToStitch;

    for (int idx : indices) {
        cv::Mat frame = readRawFrame(filePath, idx);
        if (frame.empty()) {
            qWarning() << "处理中断：未能读取到索引为" << idx << "的图像。";
            return cv::Mat();
        }
        imagesToStitch.push_back(frame);
    }

    return stitchAndResize(imagesToStitch, rows, cols);
}
