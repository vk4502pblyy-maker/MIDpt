#ifndef RAWIMGWIDGET_H
#define RAWIMGWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QCloseEvent>
#include <QTimer>
#include "view/videowidget.h"
#include "element/pbtnsingleshot.h"
#include "function/rawreader.h"

class RawImgWidget : public QDialog
{
    Q_OBJECT
public:
    explicit RawImgWidget(QWidget *parent = nullptr);
    void setFilePath(QString filePath);
    void setFrameWH(int width,int height);

protected:
    void showEvent(QShowEvent *) override;
    void closeEvent(QCloseEvent *event) override;

signals:

public slots:

private:
    void initRawReader(QString filePath);
    void refreshRawReader(QString filePath);


private slots:
    void onShowTimerOut();
    void onBtnAlbum();
    void onBtnForward();
    void onBtnBackward();
    void onBtnSave();
    void onBtnPlay();

private:
    PBtnSingleShot  *btnAlbum;
    PBtnSingleShot  *btnForward;
    PBtnSingleShot  *btnBackward;
    PBtnSingleShot  *btnSave;
    PBtnSingleShot  *btnPlay;
    QSpinBox        *boxIndex;

    QTimer          showTimer;

    VideoWidget     *videoImage;
    RawReader       *rawReader = nullptr;

    int     m_frameWidth = 0;
    int     m_frameHeight = 0;
    qint64  m_ImgCount = 0;
    QString m_style = "";
    QString m_lastSaveDir = "";
    QString m_lastAlbumDir = "";
    QString m_filePath = "";

};

#endif // RAWIMGWIDGET_H
