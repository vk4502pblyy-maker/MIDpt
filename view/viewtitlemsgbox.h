#ifndef VIEWTITLEMSGBOX_H
#define VIEWTITLEMSGBOX_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QCloseEvent>
#include "function/functionmovesl.h"
#include "function/funcpztctl.h"
#include "element/eleiconbtn.h"


class ViewTitleMsgBox : public QWidget
{
    Q_OBJECT
public:
    explicit ViewTitleMsgBox(QWidget *parent = nullptr);
    ~ViewTitleMsgBox();
    void writeMsg(QString msg);
    void setDisplay(QString msg);
    void cmdFrMainWindow(QString cmd);



private slots:
    void onTimeAcqOut();
    void onBtnShowAll();
    void onFloatMovement();
    void onFloatConfig();
    void onDevInfo(QString msg);

signals:
    void sigFloatMovement();
    void sigFloatConfig();

private:
    QLabel      *labelWarning;
    QLabel      *labelError;
    QListWidget *listMsg;
    QTimer      *timeAcq;
    QLabel      *labelPos;

    EleIconBtn  *btnMovement;
    EleIconBtn  *btnConfig;
    EleIconBtn  *btnDMD;
    EleIconBtn  *btnMove;
    EleIconBtn  *btnPZT;
    EleIconBtn  *btnCamera;

    EleIconBtn  *btnClear;

    FunctionMoveSL  *movement;
    FuncPZTCtl      *pzt;

    bool    zInitFlag;
    bool    xyInitFlag;
    double  xCur;
    double  yCur;
    double  zCur;
    double  PZTCur;


};

#endif // VIEWTITLEMSGBOX_H
