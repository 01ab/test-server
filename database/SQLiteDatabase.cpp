
#include "SQLiteDatabase.h"
#include <QDebug>

SQLiteDatabase::SQLiteDatabase(QString fileName, QObject* parent)
    : IDatabase { parent }
    , _fileName { fileName }
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(fileName);

    if (!_db.open())
        qDebug() << "Open database failed";
}

QSqlQuery SQLiteDatabase::query(QString request)
{
    return QSqlQuery { request, _db };
}
