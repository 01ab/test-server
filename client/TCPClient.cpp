
#include "TCPClient.h"
#include <QHostAddress>

TCPClient::TCPClient(qintptr socketDescriptor, QObject* parent)
    : IClient { parent }
    , _socketDescriptor { socketDescriptor }
{
    _thread = new QThread;
    connect(this, &TCPClient::connectionClosed, this, &QObject::deleteLater);
    connect(_thread, &QThread::started, this, &TCPClient::init);
    connect(_thread, &QThread::finished, _thread, &QThread::deleteLater);
    moveToThread(_thread);
    _thread->start();
}

TCPClient::~TCPClient()
{
    if (_socket) {
        _socket->close();
        delete _socket;
    }
    _thread->quit();
}

void TCPClient::reply(QByteArray data)
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "reply", Q_ARG(QByteArray, data));
        return;
    }
    if (!_socket || _socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Error, sending data when socket is not connected";
        return;
    }
    _socket->write(data);
}

void TCPClient::init()
{
    _socket = new QTcpSocket(this);

    connect(_socket, &QTcpSocket::disconnected, this, &TCPClient::connectionClosed);
    connect(_socket, &QIODevice::readyRead, this, &TCPClient::readyRead, Qt::DirectConnection);

    if (!_socket->setSocketDescriptor(_socketDescriptor)) {
        qDebug() << "Socket init error" << _socket->errorString();
        deleteLater();
        return;
    }

    qDebug() << "New connection" << _socket->peerAddress().toString() << "to" << _socket->localAddress().toString();
}

void TCPClient::readyRead()
{
    if (!_socket->bytesAvailable())
        return;

    QByteArray data = _socket->readAll();
    emit request(data);
}
