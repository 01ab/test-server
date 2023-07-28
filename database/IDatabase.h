
#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QSqlQuery>

class IDatabase : public QObject {
    Q_OBJECT

public:
    explicit IDatabase(QObject* parent = nullptr)
        : QObject { parent } {};

public:
    virtual QSqlQuery query(QString request = QString()) = 0;
};

#endif // IDATABASE_H
