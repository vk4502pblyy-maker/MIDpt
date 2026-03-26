#ifndef ELEICONBTN_H
#define ELEICONBTN_H

#include <QPushButton>
#include <QMouseEvent>

class EleIconBtn : public QPushButton
{
    Q_OBJECT
public:
    EleIconBtn(QString iconPath,
               int w = 32,int h = 32,
               QString toolTips = "",
               QPushButton *parent = nullptr);
    void setSize(int w,int h);
    void changeIcon(QString iconPath,QString toolTips = "");
    void setStatusIcon(QStringList iconPaths,QStringList toolTips,bool isCycle = true,bool isTrig = true);
    void changeStatus(int status);
    int  getStatus();
    void setToggleBtn(bool isToggle);
    void changeToggleStatus();
    bool getToggleStatus();
    void setHoverIcon(QString icon);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    // 鼠标离开控件（悬停结束）
    void leaveEvent(QEvent *event) override;

private:
    void tempChangeStatus(int status);


private:
    QStringList iconStatusPaths;
    QStringList iconStatusToolTips;
    int m_w;
    int m_h;
    int m_Status;
    bool m_isCycle;
    bool m_isTrig;
    bool m_isToggled;
    bool m_toggleStatus;
    bool m_hoverIcon = false;
    QIcon onIcon;
    QIcon offIcon;
    QIcon hoverIcon;

};

#endif // ELEICONBTN_H
