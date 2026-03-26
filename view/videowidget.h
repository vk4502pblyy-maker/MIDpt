#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QImage>

class VideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override;

public slots:
    void updateImage(const QImage &image);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QImage m_currentImage;
    QSize m_lastTextureSize;

    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vbo;
    GLuint m_textureId;
};

#endif // VIDEOWIDGET_H
