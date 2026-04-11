#ifndef RAWIMAGESTITCHER_H
#define RAWIMAGESTITCHER_H

#include <QString>
#include <QSize>
#include <QFile>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>

class RawImageStitcher {
public:
    /**
     * @brief 构造函数
     * @param rawWidth 单帧图像的宽度
     * @param rawHeight 单帧图像的高度
     * @param rawType 单帧图像的 OpenCV 类型 (例如: CV_8UC1 为灰度, CV_8UC3 为RGB)
     * @param targetResolution 最终输出的固定分辨率大小
     */
    RawImageStitcher(int rawWidth, int rawHeight, int rawType, const QSize& targetResolution);

    /**
     * @brief 从 RAW 文件中读取指定索引的图像
     * @param filePath RAW 文件的绝对或相对路径
     * @param index 帧索引 (从 0 开始)
     * @return 读取到的 OpenCV Mat 对象。如果失败则返回空 Mat
     */
    cv::Mat readRawFrame(const QString& filePath, int index);

    /**
     * @brief 将多张图像按指定的行数和列数拼接，并压缩到固定分辨率
     * @param images 按从左到右、从上到下顺序排列的图像集合
     * @param rows 拼接结果的行数
     * @param cols 拼接结果的列数
     * @return 拼接并缩放后的结果图
     */
    cv::Mat stitchAndResize(const std::vector<cv::Mat>& images, int rows, int cols);

    /**
     * @brief 综合工作流：读取多个指定索引的帧，拼接并输出固定分辨率图像
     * @param filePath RAW 文件路径
     * @param indices 需要读取的帧索引列表
     * @param rows 拼接行数
     * @param cols 拼接列数
     * @return 最终结果
     */
    cv::Mat processWorkFlow(const QString& filePath, const std::vector<int>& indices, int rows, int cols);

private:
    int m_rawWidth;
    int m_rawHeight;
    int m_rawType;
    QSize m_targetResolution;
};

#endif // RAWIMAGESTITCHER_H
