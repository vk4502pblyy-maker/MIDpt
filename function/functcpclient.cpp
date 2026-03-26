#include "functcpclient.h"

FuncTcpClient::FuncTcpClient(QObject *parent)
    : QObject{parent}
{
    m_socket = new QTcpSocket(this);

    // 连接信号槽
    connect(m_socket, &QTcpSocket::connected, this, &FuncTcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &FuncTcpClient::onDisconnected);
//    connect(m_socket, &QTcpSocket::readyRead, this, &FuncTcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::readyRead, this, &FuncTcpClient::onReadyReadEz);
//    connect(m_socket, &QTcpSocket::errorOccurred, this, &FuncTcpClient::onError);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &FuncTcpClient::onError);
}

FuncTcpClient::~FuncTcpClient()
{
    disconnectFromServer();
}

bool FuncTcpClient::connectToServer(const QString &host, quint16 port)
{
    if (isConnected())
        disconnectFromServer();

    m_socket->connectToHost(host, port);

    // 等待连接建立（非阻塞）
    if (!m_socket->waitForConnected(3000)) {
        emit errorOccurred(m_socket->errorString());
        return false;
    }

    return true;
}

void FuncTcpClient::disconnectFromServer()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
    }
}

bool FuncTcpClient::isConnected() const
{
    if(m_socket == nullptr){
        return false;
    }
    QTcpSocket::SocketState state = m_socket->state();
    return state == QTcpSocket::ConnectedState;
}

void FuncTcpClient::sendMessage(const QString &message)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return;
    }

    // 序列化消息（带长度头部）
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);

    // 先写入占位长度
    stream << (quint16)0;
    // 写入实际消息
    stream << message;
    // 返回流开始位置，写入实际长度
    stream.device()->seek(0);
    stream << (quint16)(data.size() - sizeof(quint16));

    // 发送数据
    m_socket->write(data);
    m_socket->flush();
}

void FuncTcpClient::onConnected()
{
    emit connected();
    qDebug() << "Connected to server";
}

void FuncTcpClient::onDisconnected()
{
    emit disconnected();
    qDebug() << "Disconnected from server";
}

void FuncTcpClient::onReadyRead()
{
    QDataStream stream(m_socket);
    stream.setVersion(QDataStream::Qt_5_12);

    // 循环读取所有完整消息
    while (m_socket->bytesAvailable() >= qint64(sizeof(quint16))) {
        stream.startTransaction();

        // 读取消息长度
        quint16 messageLength = 0;
        stream >> messageLength;

        // 检查是否有完整消息
        if (m_socket->bytesAvailable() < messageLength) {
            stream.rollbackTransaction();
            break;
        }

        // 读取消息内容
//        QString message;
//        stream >> message;
        // 如果发送端的message是其他类型（如QByteArray），则使用对应类型接收
    }
    QByteArray message;
    stream >> message;

   if (stream.commitTransaction()) {
       emit messageReceived(message);
       qDebug() << "Received:" << message;
   }
}

void FuncTcpClient::onReadyReadEz()
{
    // 读取所有可用数据
    QByteArray receivedData = m_socket->readAll();

    // 准备解析数据流
    QDataStream stream(&receivedData, QIODevice::ReadOnly);
    // 关键：必须使用与发送端相同的QDataStream版本
    stream.setVersion(QDataStream::Qt_5_12);

    // 根据发送端写入的message类型进行读取
    // 假设发送端的message是QString类型
    QString message;
    stream >> message;

    // 如果发送端的message是其他类型（如QByteArray），则使用对应类型接收
//     QByteArray message;
//     stream >> message;

    qDebug() << "RECEIVE:" << message;
    emit messageReceived(message);
}

void FuncTcpClient::onError(QAbstractSocket::SocketError socketError)
{

}
