#include "txtio.h"
#include "QDebug"

TxtIO::TxtIO(QObject *parent) : QObject(parent)
{

}

QStringList TxtIO::ReadATxt(QString path)
{
    QStringList content;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return content;
    QTextStream in(&file);
    QString line = in.readLine();
    content.append(line);
    while (!line.isNull()) {
        line = in.readLine();
        content.append(line);
    }
    file.close();
    return content;
}

QStringList TxtIO::readACSV(QString path)
{
    QStringList content;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return content;
    QTextStream read(&file);
    content = read.readAll().split('\n',QString::SkipEmptyParts);
    file.close();
    return content;
}

void TxtIO::WriteATxt(const QString &path, const QStringList &txt)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return;
    QTextStream stream(&file);
    qDebug() << "[Text] TxtIOWtxtlistCount = " <<txt.count();
    for(int i=0;i<txt.count();i++)
    {
        if(i == txt.count()-1)
            stream << txt.at(i);
        else
            stream << txt.at(i) << '\n';
    }
    file.close();
}

void TxtIO::WriteATxtAppend(const QString &path, const QStringList &txt)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        return;
    QTextStream stream(&file);
    qDebug() << "[Text] TxtIOWtxtlistCount = " <<txt.count();
    for(int i=0;i<txt.count();i++)
    {
        if(i == txt.count()-1)
            stream << txt.at(i);
        else
            stream << txt.at(i) << '\n';
    }
    file.close();
}

QMap<QString, QStringList> TxtIO::ReadMutiTxtFromDir(QString dirPath, QStringList &keyList, QStringList &filterKeyWords)
{
    QDir dir(dirPath);
    QStringList filter = QStringList() << "*.txt";
    dir.setNameFilters(filter);
    dir.setFilter(QDir::Files|QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    QMap<QString,QStringList> res;
    bool hasFilterKeyWord = true;
    if(filterKeyWords.isEmpty())
        hasFilterKeyWord = false;
    for(int i=0;i<list.count();i++)
    {
        bool isContainKey = false;
        for (int j=0; j<filterKeyWords.size(); j++) {
            if(list.at(i).fileName().contains(filterKeyWords.at(j)) && hasFilterKeyWord){
                isContainKey = true;
                break;
            }
        }
        if(isContainKey) continue;

        QStringList content = ReadATxt(list.at(i).filePath());
        QString fileName = list.at(i).fileName();
        res.insert(fileName,content);
        keyList.append(fileName);
    }
    return res;
}

bool TxtIO::txtIsExist(QString dirPath, QString fileName)
{
    bool exist = false;
    QDir dir(dirPath);
    QStringList filter = QStringList() << "*.txt";
    dir.setNameFilters(filter);
    QFileInfoList list = dir.entryInfoList(filter);
    for(int i=0;i<list.count();i++)
    {
        if(list.at(i).fileName().contains(fileName, Qt::CaseInsensitive))
        {
            exist = true;
            break;
        }
    }
    return exist;
}

void TxtIO::qureyTxt(QString dirPath, QString keyword,QStringList &pathList)
{
    QDir dir(dirPath);
    QStringList filter = QStringList() << "*.txt";
    dir.setNameFilters(filter);
    QFileInfoList list = dir.entryInfoList(filter);
    for(int i=0;i<list.count();i++)
    {
//        qDebug() << "[Text] fileName = " << list.at(i).fileName();
        if(list.at(i).fileName().contains(keyword))
        {
            pathList.append(list.at(i).filePath());
            qDebug() << "[Text] filePath = " << list.at(i).filePath();
        }
    }
}

int TxtIO::copyAllTxtFromDir(QString dirPath, QString newDirPath)
{
    QDir dir(dirPath);
    if(!dir.exists())
        return -1;
    QStringList filter = QStringList() << "*.txt";
    dir.setNameFilters(filter);
    QFileInfoList list = dir.entryInfoList(filter);
    foreach (QFileInfo info, list)
    {
        QString newPath = newDirPath+"/"+info.fileName();
        QFile::copy(info.filePath(),newPath);
    }
    return 0;
}



int TxtIO::processPrintSingle(QString dirPath, int num, int column)
{
    QStringList pathList;
    QStringList txt;
    qureyTxt(dirPath,"firepsall_1_1_",pathList);

    if(pathList.isEmpty())
        return -1;
    for(int i=0;i<pathList.count();i++)
    {
        qDebug() << "[Text] path = " << pathList.at(i);
        txt = ReadATxt(pathList.at(i));
        processNozzleTxt(txt,num,column);
        WriteATxt(pathList.at(i),txt);
    }
    qureyTxt(dirPath,"firepsall",pathList);
    for(int i=0;i<pathList.count();i++)
    {
        if(pathList.at(i).contains("firepsall_1_1_"))
            continue;
        QFile::remove(pathList.at(i));
    }
    QString path = dirPath + "/Movestep1.txt";
    QStringList str = ReadATxt(path);
    str[2] = "[0]";
    WriteATxt(path,str);
    return 0;

}

int TxtIO::readPrintNozzles(QString txtPath, QVector<int> &nozzles)
{
    QStringList str = ReadATxt(txtPath);
    for(int i=0;i<str.count();i++)
    {
        QStringList s = str.at(i).split('\t');
        if(!s.isEmpty())
        {
            for(int j=0;j<s.count();j++)
            {
                if(s.at(j) != '0')
                    nozzles.append(s.at(j).toInt());
            }
        }
    }
    return 0;
}

void TxtIO::change2Zero4Print(QString &str, int column)
{
    str.clear();
    for(int i=0;i<column;i++)
    {
        str.append(QString::number(0));
        str.append('\t');
    }
//    str.append('\n');
}

void TxtIO::processNozzleTxt(QStringList &txt, int num, int column)
{
    int j=2;
    QVector<int> snz = getStartNozzle();
    qDebug() << "[Text] num = " << num;
    while(j<txt.count())
    {
        if(snz.isEmpty())
            break;
        if(j-1 == snz.first())
        {
            qDebug() << "[Text] nozzles = " << j;
            for(int k=0;k<8;k++)
            {
                if(k != num-1)
                    change2Zero4Print(txt[j],column);
                j++;
            }
            qDebug() << "[Text] nozzlee = " << j;
            if(!snz.isEmpty())
                snz.removeFirst();
        }
        else
            j++;
    }
}

QVector<int> TxtIO::getStartNozzle()
{
    return {2,16,30,45,59,73,87,101,115,130,144,158,172,186,200,215,229,243,257,271,286,300,314,328,342,356,371,385,399,413,
        427,
        441,
        456,
        470,
        484,
        498,
        512,
        526,
        541,
        555,
        569,
        583,
        597,
        611,
        626};
}
