#include "funcpztctl.h"
#include <QDataStream>
#include <QDebug>
#include <memory>
#include <QCoreApplication>
#include <QtMath>

QMutex FuncPZTCtl::mutex;
QScopedPointer<FuncPZTCtl> FuncPZTCtl::instance;

FuncPZTCtl *FuncPZTCtl::getInstance()
{
    if (instance.isNull()) {
        QMutexLocker locker(&mutex);
        if (instance.isNull()) {
            instance.reset(new FuncPZTCtl);
        }
    }
    return instance.data();
}

int FuncPZTCtl::initPZT(QString port)
{
    pzt.setPort(QSerialPortInfo(port));
    pzt.setBaudRate(QSerialPort::Baud115200);
    pzt.setDataBits(QSerialPort::Data8);
    pzt.setParity(QSerialPort::NoParity);
    pzt.setStopBits(QSerialPort::OneStop);
    bool ReData = false;
    ReData=pzt.open(QIODevice::ReadWrite);
    if(ReData==true)
    {
        emit sigInfo("PZT INIT SUCCESS PORT:"+port);
        m_pIsOpen = true;
        timeAcq.start(200);
    }else
    {
        emit sigError("PZT INIT FAIL PORT:"+port);
        return -1;
    }
    return 0;
}

double FuncPZTCtl::getPos(int channel)
{
    QByteArray Arr;
    int longtmp = 7;
    Arr.resize(longtmp);
    Arr[0] = 0xaa;//默认包头
    Arr[1] = 0x01;
    Arr[2] = longtmp;//包长
    Arr[3] = 0x06;//命令码 b3  05是开环 06是闭环
    Arr[4] = 0x00;//命令码 b4
    Arr[5] = channel;//通道号
    //抑或校验
    unsigned char tmpXor = 0x00;
    unsigned char * p = (unsigned char *)Arr.data();//转化为unsigned char 的数据 以备抑或校验使用
    for(int i = 0;i<p[2]-1;i++)
    {
         tmpXor = tmpXor^p[i];
//         qDebug()<<"校验中..."<<tmpXor<<i<<p[i]<<"长度"<<p[2];
    }
    Arr[longtmp-1] = tmpXor;
    if(pzt.isOpen())
    {
        SerSendArr(Arr,longtmp);
    }else
    {
        emit sigError("PORT NOT OPEN");
        return -1;
    }
    timerElp.restart();
    while(timerElp.elapsed()<5000){
        if(recFlag){
            recFlag = false;
            return m_pPos;
        }
        QCoreApplication::processEvents();
    }
    emit sigError("PZT Get Pos Time Out");
    return -1;
}

int FuncPZTCtl::setPos(int channel, double pos)
{
    qDebug()<<"PZT SET POS:"<<QString::number(pos);
    QByteArray Arr;
    unsigned char kk[4];
    int longtmp = 11;
    Arr.resize(longtmp);
    Arr[0] = 0xaa;//默认包头
    Arr[1] = 0x01;
    Arr[2] = longtmp;//包长
    Arr[3] = 0x01;//命令码 b3,01表示发送位移指令
    Arr[4] = 0x00;//命令码 b4
    Arr[5] = channel;//通道号

    DataAnla(pos,kk);
    Arr[6] =  kk[0];
    Arr[7] =  kk[1];
    Arr[8] =  kk[2];
    Arr[9] =  kk[3];

    //抑或校验
    unsigned char tmpXor = 0x00;
    unsigned char * p = (unsigned char *)Arr.data();//转化为unsigned char 的数据 以备抑或校验使用
    for(int i = 0;i<p[2]-1;i++)
    {
         tmpXor = tmpXor^p[i];
         qDebug()<<"校验中..."<<tmpXor<<i<<p[i]<<"长度"<<p[2];
    }
    Arr[longtmp-1] = tmpXor;
    if(pzt.isOpen())
    {
        SerSendArr(Arr,longtmp);
    }else
    {
        emit sigError("PORT NOT OPEN");
        return -1;
    }
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<10000){
        double posCur = getPos(0);
        if(posCur == -1)
            return -1;
        if(fabs(posCur-pos)<0.05)
            return 0;
        QCoreApplication::processEvents();
    }
    emit sigError("PZT Set Pos Time Out");
    return -1;
}

void FuncPZTCtl::startAcq(int channel)
{
    Q_UNUSED(channel)
    timeAcq.start(500);
}

void FuncPZTCtl::stopAcq(int channel)
{
    Q_UNUSED(channel)
    timeAcq.stop();
}

bool FuncPZTCtl::pztIsOpen()
{
    return m_pIsOpen;
}

FuncPZTCtl::FuncPZTCtl(QObject *parent)
    : QObject{parent}
{
//    tmp_command = new unsigned char[256];
    tmp_command = std::make_unique<unsigned char[]>(256);
    recFlag = false;
    m_pPos = -1;
    connect(&pzt,&QSerialPort::readyRead,this,&FuncPZTCtl::onReadReady);
    connect(&timeAcq,&QTimer::timeout,this,&FuncPZTCtl::onAcqTimeout);
}

FuncPZTCtl::~FuncPZTCtl()
{
    pzt.close();
}

void FuncPZTCtl::resCommand(unsigned char T_D_3, unsigned char T_D_4)
{
//    unsigned char FlagCloseOrOpen='O';//默认开环
//    unsigned char flagCh = 0;
    switch (T_D_3) {
    case 6:
        //flagCh = tmp_command[5];//通道号
        m_pPos = calData(tmp_command[6],tmp_command[7],
                    tmp_command[8],tmp_command[9]);
        recFlag = true;
        emit sigPos(m_pPos);
        break;
    default:
        break;
    }
}

//数据解码
double FuncPZTCtl::calData(unsigned char kk0, unsigned char kk1, unsigned char kk2, unsigned char kk3)
{
    double d;
    if(kk0&0x80)
    {kk0-=0x80;
      d=(double)(kk0*256+kk1+(kk2*256+kk3)*0.0001);
      d*=(-1);
    }
    else
    {
        d=(double)(kk0*256+kk1+(kk2*256+kk3)*0.0001);
    }
    return d;
}

//数据编码
unsigned char *FuncPZTCtl::DataAnla(double f, unsigned char kk[])
{
    //	if(chenfeng==1)sprintf(buf,"%d",addr_save);
        if(f<0)//f中的内容为负数
        {
         f*=(-1);//将F中的内容转换为正数
         int a=int(f);
          kk[0]=a/256+0x80;//将F中的内容转换为负数
          kk[1]=a%256;
          a=int((f-a)*10000);
          kk[2]=a/256;
          kk[3]=a%256;
        }
        else
        {
          int a=int(f);
          kk[0]=a/256;
          kk[1]=a%256;
          a=int((f-a+0.000001)*10000);
          kk[2]=a/256;
          kk[3]=a%256;
        }
        return kk;
}

QString FuncPZTCtl::ByteArrayToHexString(QByteArray &ba)
{
    QDataStream out(&ba,QIODevice::ReadWrite);   //将str的数据 读到out里面去
    QString buf;
    while(!out.atEnd())
    {
        qint8 outChar = 0;
        out >> outChar;   //每次一个字节的填充到 outchar
        QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0')).toUpper() + QString(" ");   //2 字符宽度
        buf += str;
    }
    return buf;
}

void FuncPZTCtl::SerSendArr(QByteArray tmpArr, int longData)
{
    QByteArray Arr;
    Arr.resize(longData);
    for(int i=0;i<longData;i++)
    {
        Arr[i]= tmpArr[i];
    }
    //SerialUse->write(Arr);
    pzt.write(tmpArr);
    QString hexMsguse = ByteArrayToHexString(tmpArr).toLatin1();
//    qDebug()<<QString("Send ->  ")+hexMsguse.toUpper();
}

void FuncPZTCtl::onReadReady()
{
    QByteArray message;
    message.append(pzt.readAll());
    size_t dataLen = message.size();
    if (dataLen > 256) {
        // 处理数据过长的情况，例如截断或报错
        dataLen = 256;
    }
//    tmp_command = (unsigned char*)message.data();
    std::memcpy(tmp_command.get(), message.data(), dataLen);
    resCommand(tmp_command[3],tmp_command[4]);
    QString messageHex;
    messageHex = ByteArrayToHexString(message).toLatin1();
//    qDebug()<< "REC -> "+messageHex;
}

void FuncPZTCtl::onAcqTimeout()
{
    getPos(0);

}
