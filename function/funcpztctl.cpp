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
    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
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
    // 【修改前】：直接发送
    //pzt.write(tmpArr);

    // 【修改后】：压入队列，并尝试处理队列
    m_cmdQueue.enqueue(Arr);
    processQueue(); // 尝试触发发送


    QString hexMsguse = ByteArrayToHexString(tmpArr).toLatin1();
    //    qDebug()<<QString("Send ->  ")+hexMsguse.toUpper();
}

void FuncPZTCtl::processQueue()
{
    // 如果正在等回复，或者队列里没指令了，就什么也不做
    if (m_isWaiting || m_cmdQueue.isEmpty()) {
        return;
    }

    // 取出第一条指令发送
    QByteArray cmd = m_cmdQueue.dequeue();
    m_isWaiting = true; // 上锁，告诉程序：现在正占用着串口呢，下一条等会再发
    pzt.write(cmd);
    // 【新增】：信发出去的同时，开始倒计时 100ms
        m_timeoutTimer.start(100);
}

// 标准 Modbus RTU CRC16 校验算法
quint16 FuncPZTCtl::crc16(const QByteArray &data, int len)
{
    quint16 crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (quint8)data[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

//void FuncPZTCtl::onReadReady()
//{
//    QByteArray message;
//    message.append(pzt.readAll());
//    size_t dataLen = message.size();
//    if (dataLen > 256) {
//        // 处理数据过长的情况，例如截断或报错
//        dataLen = 256;
//    }
////    tmp_command = (unsigned char*)message.data();
//    std::memcpy(tmp_command.get(), message.data(), dataLen);
//    resCommand(tmp_command[3],tmp_command[4]);
//    QString messageHex;
//    messageHex = ByteArrayToHexString(message).toLatin1();
////    qDebug()<< "REC -> "+messageHex;
//}

void FuncPZTCtl::onReadReady()
{
    // 1. 把串口里刚收到的碎片全攒起来
    m_readBuffer.append(pzt.readAll());

    // 2. 循环处理缓存，直到不够一帧为止
    while (m_readBuffer.size() >= 5)
    {
        // 找包头 0x01，如果头不对，就把第一个字节扔了继续找
        if ((unsigned char)m_readBuffer[0] != 0x01) {
            m_readBuffer.remove(0, 1);
            continue;
        }

        // 获取功能码，决定这一帧该收多长
        unsigned char funcCode = m_readBuffer[1];
        int expectedLen = 0;

        if (funcCode == 0x03) {
            expectedLen = 9; // 协议：读位置返回 9 字节
        } else if (funcCode == 0x10) {
            expectedLen = 8; // 协议：写位置返回 8 字节
        } else {
            // 出现了意料之外的功能码，清除当前字节，重新对齐
            m_readBuffer.remove(0, 1);
            continue;
        }

        // 3. 判断攒够这一帧的长度了吗？
        if (m_readBuffer.size() >= expectedLen) {
            // 够了！把这完整的一包拿出来
            QByteArray frame = m_readBuffer.left(expectedLen);
            m_readBuffer.remove(0, expectedLen); // 从缓存里删掉已取走的

            // 【新增】：在规定的时间内完整收到了，赶紧把看门狗关掉！
            m_timeoutTimer.stop();

            // --- 这里是你原本的解析逻辑 ---
            if (funcCode == 0x03) {
                // 读到的位置就在 frame 的第 3, 4, 5, 6 字节
                double pos = calData(frame[3], frame[4], frame[5], frame[6]);
                emit sigPos(pos);
            }

            // --- 重点：该指令处理完了，解锁并触发下一条排队指令 ---
            m_isWaiting = false;
            processQueue();
        }
        else {
            // 长度还不够，说明数据还在路上，跳出循环等下次 readyRead
            break;
        }
    }
}

void FuncPZTCtl::onAcqTimeout()
{
    getPos(0);

}

void FuncPZTCtl::onTimeout()
{
    // 如果 100ms 过去了，下位机还是没回（或者没回全）
    // 强制解锁！不能让后续的指令被憋死
    m_isWaiting = false;

    // 把当前缓存里的残缺垃圾数据清空，防止污染下一条
    m_readBuffer.clear();

    // qDebug() << "通讯超时！强制丢弃并发送下一条";

    // 赶紧发队列里排队的下一条指令
    processQueue();
}
