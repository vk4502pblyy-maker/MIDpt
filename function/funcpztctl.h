#ifndef FUNCPZTCTL_H
#define FUNCPZTCTL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTimer>
#include <QMutex>
#include <QElapsedTimer>
#include <memory>
#include <QQueue>

class FuncPZTCtl : public QObject
{
    Q_OBJECT
public:
    static FuncPZTCtl*  getInstance();
    ~FuncPZTCtl();
    int                 initPZT(QString port);
    double              getPos(int channel);
    int                 setPos(int channel, double pos);
    void                startAcq(int channel);
    void                stopAcq(int channel);
    bool                pztIsOpen();

signals:
    void sigInfo(QString msg);
    void sigError(QString msg);
    void sigPos(double pos);

private:
    explicit        FuncPZTCtl(QObject *parent = nullptr);
    void            dataEncoading(double f,unsigned char kk[4]);
    void            resCommand(unsigned char T_D_3,unsigned char T_D_4);
    double          calData(unsigned char kk0,unsigned char kk1,
                        unsigned char kk2,unsigned char kk3);
    unsigned char*  DataAnla(double f,unsigned char kk[4]);
    QString         ByteArrayToHexString(QByteArray &ba);
    void            SerSendArr(QByteArray tmpArr,int longData);

    // 添加一个新的私有函数，用来处理队列
    void processQueue();
    quint16 crc16(const QByteArray &data, int len); // 添加标准 Modbus CRC 校验函数

private slots:
    void onReadReady();
    void onAcqTimeout();
    void onTimeout();

private:
    static QMutex                       mutex;
    static QScopedPointer<FuncPZTCtl>   instance;

    QSerialPort     pzt;
    QTimer          timeAcq;
    QTimer          m_timeoutTimer;
//    unsigned char*  tmp_command;
    std::unique_ptr<unsigned char[]> tmp_command;
    double          m_pPos;
    bool            recFlag;
    QElapsedTimer   timerElp;
    bool            m_pIsOpen = false;

    QQueue<QByteArray> m_cmdQueue;          // 指令队列
    bool               m_isWaiting = false; // 状态机：当前是否正在等待设备回复
    QByteArray         m_readBuffer;        // 用于拼接半包的缓存区（解决粘包）


    Q_DISABLE_COPY(FuncPZTCtl)
};

#endif // FUNCPZTCTL_H
