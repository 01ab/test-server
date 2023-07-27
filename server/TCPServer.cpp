#include "TCPServer.h"

TCPServer::TCPServer(QObject* parent)
    : QTcpServer { parent }
{
}

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    emit newClient(socketDescriptor);
}
