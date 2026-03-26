#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_textureId(0)
{
}

VideoWidget::~VideoWidget()
{
    makeCurrent();
    m_vbo.destroy();
    if (m_textureId != 0) {
        glDeleteTextures(1, &m_textureId);
    }
    doneCurrent();
}

void VideoWidget::updateImage(const QImage &image)
{
    m_currentImage = image;
    update(); // 触发 paintGL 进行重绘
}

void VideoWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 顶点着色器
    const char *vsrc =
        "attribute vec2 aPos;\n"
        "attribute vec2 aTexCoord;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "  vTexCoord = aTexCoord;\n"
        "}\n";

    // 片段着色器 (直接对纹理采样)
    const char *fsrc =
        "uniform sampler2D uTexture;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "  gl_FragColor = texture2D(uTexture, vTexCoord);\n"
        "}\n";

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
    m_program.link();

    // 矩形顶点与纹理坐标 (Y轴翻转以纠正图像倒立)
    float vertices[] = {
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 0.0f
    };

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // 线性滤波保证画面平滑缩放
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void VideoWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_currentImage.isNull()) {
        return;
    }

    m_program.bind();
    m_vbo.bind();

    int posLoc = m_program.attributeLocation("aPos");
    m_program.enableAttributeArray(posLoc);
    m_program.setAttributeBuffer(posLoc, GL_FLOAT, 0, 2, 4 * sizeof(float));

    int texLoc = m_program.attributeLocation("aTexCoord");
    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // QImage 的 .copy() 保证了每行数据必定是 4 字节对齐
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (m_currentImage.size() != m_lastTextureSize) {
        // 使用 GL_LUMINANCE 解析单通道灰度图
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_currentImage.width(), m_currentImage.height(),
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_currentImage.constBits());
        m_lastTextureSize = m_currentImage.size();
    } else {
        // 尺寸不变时，直接局部刷新显存，极大提升帧率
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_currentImage.width(), m_currentImage.height(),
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, m_currentImage.constBits());
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_vbo.release();
    m_program.release();
}
