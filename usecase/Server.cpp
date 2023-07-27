#include "Server.h"

Server::Server(QObject* parent)
    : QObject { parent }
{
}

void Server::newClient(IClient* client)
{
    // настраиваем бизнес логику для нового клиента
}
