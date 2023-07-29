
#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QTcpServer>

// Класс помошник, что бы можно было получить socketDescriptor, при подключении клиента
class TCPServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TCPServer(QObject* parent = nullptr);

signals:
    void newClient(qintptr socketDescriptor);

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;
};

#endif // TCPSERVER_H
