#include "pbtnsingleshot.h"

PBtnSingleShot::PBtnSingleShot(QString icon, int w, int h, QString tipsOn, QString iconHover, QPushButton *parent) :
    QPushButton(parent),m_pIcon(icon),m_pIconHover(iconHover)
{
    this->setIcon(QIcon(icon));
    this->setIconSize(QSize(int(w*0.9),int(h*0.9)));
    this->setFixedSize(w,h);
    this->setToolTip("点一下"+tipsOn);
}

void PBtnSingleShot::enterEvent(QEvent *event)
{
    if(!m_pIconHover.isEmpty()){
        this->setIcon(QIcon(m_pIconHover));
    }
}

void PBtnSingleShot::leaveEvent(QEvent *event)
{
    if(!m_pIconHover.isEmpty()){
        this->setIcon(QIcon(m_pIcon));
    }
}
