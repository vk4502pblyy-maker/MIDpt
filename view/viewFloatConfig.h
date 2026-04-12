#ifndef VIEWFLOATCONFIG_H
#define VIEWFLOATCONFIG_H

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QCloseEvent>
#include "element/eleiconbtn.h"
#include "element/pbtntoggle.h"


#pragma execution_character_set(push, "utf-8")
class viewFloatConfig : public QWidget
{
    Q_OBJECT
public:
    explicit viewFloatConfig(QWidget *parent = nullptr);
    void cloWid();

protected:
    // 重写关闭事件，改为隐藏窗口
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void sigCommand(QString cmd);
    void sigOpenCamera(int camNum);
    void sigCloseCamera(int camNum);
    void sigcamPacketSize(int camNum,int size);
    void sigcamExpTime(int camNum,int exp);
    void sigInfo(QString msg);

private slots:
    void onBtnDMD();
    void onBtnCam();
    void onBtnMov();
    void onBtnPZT();

private:
    void readDevInfo();
    void registDevSigSlot();
    void openMotor();

private:
    PBtnToggle    *esBtnDMD;
    PBtnToggle    *esBtnCam;
    PBtnToggle    *esBtnMov;
    PBtnToggle    *esBtnPZT;

    QString m_pDMD;
    QString m_pCam;
    QString m_pMotor;
    QString m_pPZT;
    QString m_pStyle;

    bool    m_pCamInit;


};

#endif // VIEWFLOATCONFIG_H
