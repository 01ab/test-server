
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <client/IClient.h>

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);

public slots:
    void newClient(IClient* client);

signals:
protected slots:
    void processClientRequest(QByteArray data);
};

#endif // SERVER_H