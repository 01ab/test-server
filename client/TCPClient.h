
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "IClient.h"
#include <QTcpSocket>
#include <QThread>

// Очевидно что это реализация клиента по протоколу TCP/IP
class TCPClient : public IClient {
    Q_OBJECT
    qintptr _socketDescriptor;
    QTcpSocket* _socket = nullptr;
    QThread* _thread;

public:
    TCPClient(qintptr socketDescriptor, QObject* parent = nullptr);
    ~TCPClient();

public slots:
    virtual void reply(QByteArray data) override;

signals:
    void connectionClosed();

private slots:
    void init();
    void readyRead();
};

#endif // TCPCLIENT_H
