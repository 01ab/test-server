#include "Server.h"
#include <QDebug>

Server::Server(QObject* parent)
    : QObject { parent }
{
}

void Server::processClientRequest(QByteArray data)
{
    IClient* client = qobject_cast<IClient*>(sender());
    if (client == nullptr)
        return;

    qDebug() << data;
    client->reply("OK");
}

void Server::newClient(IClient* client)
{
    // настраиваем бизнес логику для нового клиента
    connect(client, &IClient::request, this, &Server::processClientRequest);
}
