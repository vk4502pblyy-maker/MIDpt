#ifndef TXTIO_H
#define TXTIO_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QTextStream>

class TxtIO : public QObject
{
    Q_OBJECT
public:
    explicit TxtIO(QObject *parent = nullptr);
    static QStringList ReadATxt(QString path);
    static QStringList readACSV(QString path);
    static void WriteATxt(const QString &path, const QStringList &txt);
    static void WriteATxtAppend(const QString &path, const QStringList &txt);
    static QMap<QString,QStringList> ReadMutiTxtFromDir(QString dirPath, QStringList &keyList, QStringList &filterKeyWords);
    static bool txtIsExist(QString dirPath,QString fileName);
    static void qureyTxt(QString dirPath,QString keyword,QStringList &pathList);
    static int copyAllTxtFromDir(QString dirPath,QString newDirPath);


public:
    static int processPrintSingle(QString dirPath,int num,int column);
    //
    static int readPrintNozzles(QString txtPath,QVector<int> &nozzles);
    //这个txtPath就是喷嘴-像素槽映射表，返回的就是可用的喷嘴

signals:

public slots:

private:
    static void change2Zero4Print(QString &str,int column);
    static void processNozzleTxt(QStringList &txt,int num,int column);
    static QVector<int> getStartNozzle();
};

#endif // TXTIO_H
