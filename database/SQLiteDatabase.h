
#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include "IDatabase.h"

class SQLiteDatabase : public IDatabase {
    Q_OBJECT
    QString _fileName;
    QSqlDatabase _db;

public:
    explicit SQLiteDatabase(QString fileName, QObject* parent = nullptr);

    virtual QSqlQuery query(QString request = QString()) override;
};

#endif // SQLITEDATABASE_H
