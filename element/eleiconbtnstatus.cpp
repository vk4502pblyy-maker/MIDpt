#include "eleiconbtnstatus.h"
#include <QMouseEvent>

#pragma execution_character_set("utf-8")

eleIconBtnStatus::eleIconBtnStatus(QString iconOn, QString iconOff, int w, int h,
                                   QString tipsOn, QString tipsOff,
                                   QPushButton *parent)
{
    m_pIconOn = iconOn;
    m_pIconOff = iconOff;
    m_pTipOn = tipsOn;
    m_pTipOff = tipsOff;
    this->setIcon(QIcon(iconOff));
    this->setIconSize(QSize(w*0.9,h*0.9));
    this->setFixedSize(w,h);
    setOff();
}

void eleIconBtnStatus::setSize(int w, int h)
{
    this->setIconSize(QSize(w*0.9,h*0.9));
    this->setFixedSize(w,h);
}

void eleIconBtnStatus::setOn()
{
    this->setIcon(QIcon(m_pIconOn));
//    this->setStyleSheet(R"(
//        QPushButton {
//            border: none;               /* 去除边框 */
//            background-color: rgba(87, 255, 115, 1);
//            padding: 0px;               /* 去除内边距 */
//        }
//        QPushButton:hover {
//            background-color: rgba(255, 162, 87, 1); /* hover时轻微背景色 */
//            border-radius: 4px;         /* 圆角效果 */
//        }
//        QPushButton:pressed {
//            background-color: rgba(150, 150, 150, 80); /* 按下时背景色 */
//        }
//        QPushButton:disabled {
//            background-color: #4E5969; /* 按钮禁用 */
//        }
//    )");
    this->setToolTip(m_pTipOff);
    m_pStatus = true;
}

void eleIconBtnStatus::setOff()
{
    this->setIcon(QIcon(m_pIconOff));
//    this->setStyleSheet(R"(
//        QPushButton {
//            border: none;               /* 去除边框 */
//            background-color: rgba(255, 87, 87, 1);    /* 透明背景 */
//            padding: 0px;               /* 去除内边距 */
//        }
//        QPushButton:hover {
//            background-color: rgba(255, 162, 87, 1); /* hover时轻微背景色 */
//            border-radius: 4px;         /* 圆角效果 */
//        }
//        QPushButton:pressed {
//            background-color: rgba(150, 150, 150, 80); /* 按下时背景色 */
//        }
//        QPushButton:disabled {
//            background-color: #4E5969; /* 按钮禁用 */
//        }
//    )");
    this->setToolTip(m_pTipOn);
    m_pStatus = false;
}

bool eleIconBtnStatus::getStatus()
{
    return m_pStatus;
}

void eleIconBtnStatus::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if(m_pStatus){
            setOff();
        }
        else{
            setOn();
        }
    }
    QPushButton::mouseReleaseEvent(e);
}
