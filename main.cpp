#include <QCoreApplication>
#include <QDebug>

#include <client/TCPClient.h>
#include <server/TCPServer.h>
#include <usecase/Server.h>

TCPServer* ipServer;
Server* server;

const int ListenPort = 2020;

// Тут конфигурирование и связывание элементов системы
int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "test-server";
    server = new Server;
    ipServer = new TCPServer();
    QObject::connect(ipServer, &TCPServer::newClient, server, [](qintptr socket) {
        server->newClient(new TCPClient(socket));
    });

    if (ipServer->listen(QHostAddress::Any, ListenPort)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Server start error" << ipServer->errorString();
    }

    return a.exec();
}
