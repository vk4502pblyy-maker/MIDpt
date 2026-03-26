#ifndef VIEWFLOATMOVEMENT_H
#define VIEWFLOATMOVEMENT_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QCloseEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "function/funcpztctl.h"
#include "function/funcmovewm.h"
#include "element/eleiconbtn.h"

class ViewFloatMovement : public QWidget
{
    Q_OBJECT
public:
    explicit ViewFloatMovement(QWidget *parent = nullptr);
    void setPosMsg(QString type,QString posMsg);
    void setPZTPos(double pos);
    void setMoveEnable(QString nodes);
    void setPZTEnable();

signals:

protected:
    // 重写关闭事件，改为隐藏窗口
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTimerAcqTimeout();
    void onPztSliderChanged();
    void onPztBoxChanged();
    void onDevInfo(QString msg);
    void onBtnPZTU();
    void onBtnPZTD();
    void onBtnPZTMove();
    void onBtnInc();
    void onBtnDec();
    void onBtnMove();

private:
    void setStepBtnsEnabled(bool enabled);

private:
    EleIconBtn  *btnXF;
    EleIconBtn  *btnXB;
    EleIconBtn  *btnYF;
    EleIconBtn  *btnYB;
    EleIconBtn  *btnZU;
    EleIconBtn  *btnZD;
    EleIconBtn  *btnPZTU;
    EleIconBtn  *btnPZTD;
    EleIconBtn  *btnMoveX;
    EleIconBtn  *btnMoveY;
    EleIconBtn  *btnMoveZ;
    EleIconBtn  *btnMovePZT;

    QLabel      *labelPos;
    QLabel      *labelPZT;

    QDoubleSpinBox  *boxX;
    QDoubleSpinBox  *boxY;
    QDoubleSpinBox  *boxZ;
    QDoubleSpinBox  *boxPZT;
    QDoubleSpinBox  *boxStepX;
    QDoubleSpinBox  *boxStepY;
    QDoubleSpinBox  *boxStepZ;
    QDoubleSpinBox  *boxStepPZT;

    FuncPZTCtl  *pzt;
    FuncMoveWM  *moveWM;
    QTimer      timerAcq;
    QElapsedTimer   etimerStepBtns;
    QString     m_pStyle;
    bool        pztEnable;
    bool        moveEnable;


};

#endif // VIEWFLOATMOVEMENT_H
