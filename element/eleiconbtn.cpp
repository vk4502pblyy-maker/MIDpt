#include "eleiconbtn.h"
#include <QMouseEvent>

#pragma execution_character_set("utf-8")

EleIconBtn::EleIconBtn(QString iconPath, int w, int h, QString toolTips, QPushButton *parent)
{
    this->setIcon(QIcon(iconPath));
    this->setIconSize(QSize(w,h));
    m_w = w;
    m_h = h;
    m_isTrig = true;
    m_Status = 0;
    m_isCycle = false;
    m_isToggled = false;
    this->setFixedSize(w,h);
    this->setToolTip(toolTips);
    offIcon = QIcon(iconPath);
//    this->setStyleSheet(R"(
//    QPushButton {
//        background-color: #F5F5F5; /* 主色 - 按钮正常 */
//        color: #000000;
//        border: none;
//        border-radius: 3px;
//        padding: 6px 14px;
//        font-size: 18px;
//    }

//    QPushButton:hover {
//        background-color: #ADD8E6; /* 按钮悬停 */
//        border: 2px solid #4E5969;
//    }

//    QPushButton:pressed {
//        background-color: #0A2491; /* 按钮点击 */
//    }

//    QPushButton:disabled {
//        background-color: #DCDCDC; /* 按钮禁用 */
//    }

//    )");
}

void EleIconBtn::setSize(int w, int h)
{
    this->setIconSize(QSize(w,h));
    this->setFixedSize(w,h);
}

void EleIconBtn::changeIcon(QString iconPath, QString toolTips)
{
    this->setIcon(QIcon(iconPath));
    this->setIconSize(QSize(m_w,m_h));
    this->setFixedSize(m_w,m_h);
    this->setToolTip(toolTips);
}

void EleIconBtn::setStatusIcon(QStringList iconPaths, QStringList toolTips, bool isCycle, bool isTrig)
{
    iconStatusPaths = iconPaths;
    iconStatusToolTips = toolTips;
    m_isCycle = isCycle;
    m_isTrig = isTrig;
    if(iconPaths.count() == 2){
        offIcon = QIcon(iconPaths.first());
        onIcon = QIcon(iconPaths.last());
    }
}

void EleIconBtn::changeStatus(int status)
{
    if(status<=iconStatusPaths.count()-1){
        QString iconPath = iconStatusPaths.at(status);
        QString toolTips = iconStatusToolTips.at(status);
        m_Status = status;
        changeIcon(iconPath,toolTips);
    }
}

int EleIconBtn::getStatus()
{
    return m_Status;
}

void EleIconBtn::setToggleBtn(bool isToggle)
{
    m_isToggled = isToggle;
    m_toggleStatus = false;
}

void EleIconBtn::changeToggleStatus()
{
    m_toggleStatus = !m_toggleStatus;
}

bool EleIconBtn::getToggleStatus()
{
    return m_toggleStatus;
}

void EleIconBtn::setHoverIcon(QString icon)
{
    hoverIcon = QIcon(icon);
    m_hoverIcon = true;
}

void EleIconBtn::mousePressEvent(QMouseEvent *event)
{
    if(!m_isTrig){
        event->ignore();
        return;
    }
    if(m_isCycle){
        if(m_Status == iconStatusPaths.count()-1){
            m_Status = 0;
        }
        else
            m_Status++;
        changeStatus(m_Status);
    }
    if(m_isToggled){
        changeToggleStatus();
    }
    QPushButton::mousePressEvent(event);
}

void EleIconBtn::enterEvent(QEvent *event)
{
    if(m_hoverIcon){
        this->setIcon(hoverIcon);
        this->setIconSize(QSize(int(this->width()),int(this->height())));
    }
    if(!m_isTrig){
        event->ignore();
        return;
    }
    if(m_isCycle){
        int tempStatus = 0;
        if(m_Status == iconStatusPaths.count()-1){
            tempStatus = 0;
        }
        else
            tempStatus = m_Status+1;
        tempChangeStatus(tempStatus);
        this->setToolTip(QString("单击后")+iconStatusToolTips.at(m_Status));
    }

}

void EleIconBtn::leaveEvent(QEvent *event)
{
    if(m_hoverIcon){
        this->setIcon(offIcon);
        this->setIconSize(QSize(int(this->width()),int(this->height())));
    }
    if(!m_isTrig){
        event->ignore();
        return;
    }
    changeStatus(m_Status);
}

void EleIconBtn::tempChangeStatus(int status)
{
    if(status<=iconStatusPaths.count()-1){
        QString iconPath = iconStatusPaths.at(status);
        QString toolTips = iconStatusToolTips.at(status);
        changeIcon(iconPath,toolTips);
    }
}
