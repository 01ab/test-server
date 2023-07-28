#include "Server.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Server::Server(IDatabase* db, QObject* parent)
    : QObject { parent }
    , _db { db }
{
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
        answer["o1"] = "o2";
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
