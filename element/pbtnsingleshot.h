#ifndef PBTNSINGLESHOT_H
#define PBTNSINGLESHOT_H

#include <QPushButton>

class PBtnSingleShot : public QPushButton
{
public:
    PBtnSingleShot(QString icon,
                   int w = 32,int h=32,
                   QString tipsOn = "", QString iconHover = "",
                   QPushButton *parent = nullptr);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString m_pIcon = "";
    QString m_pIconHover = "";
};

#endif // PBTNSINGLESHOT_H
