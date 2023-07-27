
#include "TCPClient.h"

TCPClient::TCPClient(qintptr socketDescriptor, QObject* parent)
    : IClient { parent }
    , _socketDescriptor { socketDescriptor }
{
    _thread = new QThread;
    connect(this, &TCPClient::connectionClosed, &QObject::deleteLater);
    connect(this, &TCPClient::connectionClosed, _thread, &QObject::deleteLater);
    connect(_thread, &QThread::started, this, &TCPClient::init);
    moveToThread(_thread);
    _thread->start();
}

void TCPClient::reply(QByteArray data)
{
    if (_socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Error, sending data when socket is not connected";
        return;
    }
    _socket->write(data);
}

void TCPClient::init()
{
    _socket = new QTcpSocket(this);
    if (_socket->setSocketDescriptor(_socketDescriptor)) {
        qDebug() << "Socket init error" << _socket->errorString();
        deleteLater();
        return;
    }
    connect(_socket, &QTcpSocket::disconnected, this, &TCPClient::connectionClosed);
    connect(_socket, &QTcpSocket::disconnected, _socket, &QObject::deleteLater);
    connect(_socket, &QIODevice::readyRead, this, &TCPClient::readyRead, Qt::DirectConnection);

    qDebug() << "New connection" << _socket->peerName();
}

void TCPClient::readyRead()
{
    QByteArray data = _socket->readAll();
    emit request(data);
}
