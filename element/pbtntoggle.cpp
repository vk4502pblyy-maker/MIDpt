#include "pbtntoggle.h"

PBtnToggle::PBtnToggle(QString iconOn, QString iconOff, int w, int h, QString tipsOn, QString tipsOff, QPushButton *parent) : QPushButton(parent)
{
    this->setCheckable(true);
    m_pIconOn = iconOn;
    m_pIconOff = iconOff;
    m_pTipOn = tipsOn;
    m_pTipOff = tipsOff;

    this->setIcon(QIcon(iconOff));
    this->setIconSize(QSize(int(w*0.9),int(h*0.9)));
    this->setFixedSize(w,h);
    this->setCheckable(true);
}

void PBtnToggle::checkStateSet()
{
    QPushButton::checkStateSet(); // 必须调用基类实现
    bool checked = isChecked();
    QString icon = checked ? m_pIconOn:m_pIconOff;
    QString tip = checked ? m_pTipOn:m_pTipOff;
    this->setIcon(QIcon(icon));
    this->setToolTip(tip);
}

void PBtnToggle::enterEvent(QEvent *event)
{
    bool checked = isChecked();
    QString icon = checked ? m_pIconOff:m_pIconOn;
    QString tip = checked ? m_pTipOff:m_pTipOn;
    this->setIcon(QIcon(icon));
    this->setToolTip("单击后:"+tip);
}

void PBtnToggle::leaveEvent(QEvent *event)
{
    bool checked = isChecked();
    QString icon = checked ? m_pIconOn:m_pIconOff;
    QString tip = checked ? m_pTipOn:m_pTipOff;
    this->setIcon(QIcon(icon));
    this->setToolTip(tip);
}
