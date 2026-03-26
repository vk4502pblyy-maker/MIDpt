#ifndef FUNCTCPCLIENT_H
#define FUNCTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

class FuncTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit FuncTcpClient(QObject *parent = nullptr);
    ~FuncTcpClient();

    bool connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    void sendMessage(const QString &message);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onReadyReadEz();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;

};

#endif // FUNCTCPCLIENT_H
