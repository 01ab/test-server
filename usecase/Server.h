
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <client/IClient.h>
#include <database/IDatabase.h>

class Server : public QObject {
    Q_OBJECT

    IDatabase* _db;

public:
    explicit Server(IDatabase* db, QObject* parent = nullptr);

public slots:
    void newClient(IClient* client);

protected slots:
    void processClientRequest(QByteArray data);
private:
    QJsonArray dumpBlocksToJson();
    QJsonArray dumpBoardsToJson(int blockId);
    QJsonArray dumpPortsToJson(int boardId);
};

#endif // SERVER_H
