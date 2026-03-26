#ifndef ELEICONBTNSTATUS_H
#define ELEICONBTNSTATUS_H

#include <QPushButton>

class eleIconBtnStatus : public QPushButton
{
    Q_OBJECT
public:
    eleIconBtnStatus(QString iconOn,QString iconOff,
                     int w = 32,int h=32,
                     QString tipsOn = "",QString tipsOff = "",
                     QPushButton *parent = nullptr);
    void setSize(int w,int h);
    void setOn();
    void setOff();
    bool getStatus();

protected:
    // 重写点击事件
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QString m_pIconOn;
    QString m_pIconOff;
    QString m_pTipOn;
    QString m_pTipOff;
    bool    m_pStatus;
};

#endif // ELEICONBTNSTATUS_H
