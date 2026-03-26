#ifndef VIEWDLPC350SCAN_H
#define VIEWDLPC350SCAN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include "function/funcdlp3500ctl.h"

class ViewDLPC350Scan : public QWidget
{
    Q_OBJECT
public:
    explicit ViewDLPC350Scan(QWidget *parent = nullptr);

signals:

private slots:
    void onBtnConnect();
    void onBtnSetMode();
    void onBtnUpload();
    void onBtnAddPattern();
    void onBtnSendPattern();
    void onBtnValidateSeq();
    void onBtnPlay();
    void onBtnStop();
    void onBtnTest();
    void onBtnClear();
    void onDLPInfo(QString msg);

private:
    QPushButton *btnConnect;
    QPushButton *btnSetMode;
    QPushButton *btnUpload;
    QPushButton *btnAddPattern;
    QPushButton *btnSendPattern;
    QPushButton *btnValidateSeq;
    QPushButton *btnPlay;
    QPushButton *btnStop;
    QPushButton *btnTest;
    QPushButton *btnclear;
    QSpinBox    *spImg;
    QListWidget *listLog;

    FuncDLP3500Ctl *dlp;

    QStringList m_pListImg;

};

#endif // VIEWDLPC350SCAN_H
