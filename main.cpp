#include <QCoreApplication>
#include <QDebug>

#include <client/TCPClient.h>
#include <database/SQLiteDatabase.h>
#include <server/TCPServer.h>
#include <usecase/DatabaseWatcher.h>
#include <usecase/Server.h>

TCPServer* ipServer;
Server* server;
IDatabase* db;

DatabaseWatcher* dbWatch;

const int ListenPort = 2020;

// Тут конфигурирование и связывание элементов системы
int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "test-server";
    db = new SQLiteDatabase("data.db");
    server = new Server(db);
    ipServer = new TCPServer();
    QObject::connect(ipServer, &TCPServer::newClient, server, [](qintptr socket) {
        server->newClient(new TCPClient(socket));
    });

    if (ipServer->listen(QHostAddress::Any, ListenPort)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Server start error" << ipServer->errorString();
    }

    dbWatch = new DatabaseWatcher(new DatabaseFiller(db), QCoreApplication::applicationDirPath() + "/data/source");

    return a.exec();
}
