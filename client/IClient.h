#ifndef ICLIENT_H
#define ICLIENT_H

#include <QObject>

// Базовый клиент, умеет присылать запрос (request), умеет получать ответ (reply)
class IClient : public QObject {
    Q_OBJECT

public:
    explicit IClient(QObject* parent = nullptr)
        : QObject { parent } {};

public slots:
    virtual void reply(QByteArray data) = 0;

signals:
    void request(QByteArray data);
};

#endif // ICLIENT_H
