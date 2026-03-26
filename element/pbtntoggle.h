#ifndef PBTNTOGGLE_H
#define PBTNTOGGLE_H

#include <QPushButton>

class PBtnToggle : public QPushButton
{
    Q_OBJECT
public:
    explicit PBtnToggle(QString iconOn,QString iconOff,
                        int w = 32,int h=32,
                        QString tipsOn = "",QString tipsOff = "",
                        QPushButton *parent = nullptr);

signals:

protected:
    void checkStateSet() override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

public slots:

private:
    QString m_pIconOn;
    QString m_pIconOff;
    QString m_pTipOn;
    QString m_pTipOff;
};

#endif // PBTNTOGGLE_H
