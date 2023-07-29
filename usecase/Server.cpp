#include "Server.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>

Server::Server(IDatabase* db, QObject* parent)
    : QObject { parent }
    , _db { db }
{
}

QJsonArray Server::dumpPortsToJson(int boardId)
{
    QJsonArray result;
    QSqlQuery ports = _db->query();
    ports.prepare("SELECT * FROM port WHERE board_id=:board_id");
    ports.bindValue(":board_id", boardId);
    ports.exec();
    while (ports.next()) {
        QJsonObject port;
        QSqlRecord record = ports.record();
        for (int i = 0; i < record.count(); ++i)
            port[record.fieldName(i)] = record.field(i).value().toString();
        result.append(port);
    }
    return result;
}

QJsonArray Server::dumpBoardsToJson(int blockId)
{
    QJsonArray result;
    QSqlQuery boards = _db->query();
    boards.prepare("SELECT * FROM board WHERE block_id=:block_id");
    boards.bindValue(":block_id", blockId);
    boards.exec();
    while (boards.next()) {
        QJsonObject board;
        QSqlRecord record = boards.record();
        for (int i = 0; i < record.count(); ++i)
            board[record.fieldName(i)] = record.field(i).value().toString();
        board["ports"] = dumpPortsToJson(record.field(record.indexOf("board_id")).value().toInt());
        result.append(board);
    }
    return result;
}

QJsonArray Server::dumpBlocksToJson()
{
    QJsonArray result;
    QSqlQuery blocks = _db->query("SELECT * FROM block");
    while (blocks.next()) {
        QJsonObject block;
        QSqlRecord record = blocks.record();
        for (int i = 0; i < record.count(); ++i)
            block[record.fieldName(i)] = record.field(i).value().toString();
        block["boards"] = dumpBoardsToJson(record.field(record.indexOf("block_id")).value().toInt());
        result.append(block);
    }
    return result;
}

void Server::processClientRequest(QByteArray data)
{
    IClient* client = qobject_cast<IClient*>(sender());
    if (client == nullptr)
        return;

    // итак, основной обмен данными
    QString requestBody = QString::fromUtf8(data);

    QJsonParseError error;
    QJsonDocument requestJson = QJsonDocument::fromJson(data, &error);
    if (requestJson.isNull()) {
        client->reply(QString("Error %1").arg(error.errorString()).toUtf8());
        return;
    }

    if (!requestJson.isObject()) {
        client->reply("Request is not object");
        return;
    }

    QJsonObject request = requestJson.object();
    // например наш запрос такой {"request"="database"}
    if (request.contains("request") && request["request"].toString().compare("database", Qt::CaseInsensitive) == 0) {
        QJsonObject answer;
        answer["blocks"] = dumpBlocksToJson();
        client->reply(QJsonDocument { answer }.toJson());
        return;
    }

    qDebug() << data;
    client->reply("OK");
}

void Server::newClient(IClient* client)
{
    // настраиваем бизнес логику для нового клиента
    connect(client, &IClient::request, this, &Server::processClientRequest);
}
