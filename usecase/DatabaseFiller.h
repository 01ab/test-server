
#ifndef DATABASEFILLER_H
#define DATABASEFILLER_H

#include <QDomElement>
#include <QObject>
#include <database/IDatabase.h>

// заполняет базу данных на основе xml файлов
class DatabaseFiller : public QObject {
    Q_OBJECT

    IDatabase* _db;

public:
    explicit DatabaseFiller(IDatabase* db, QObject* parent = nullptr);
    void removeXmlFile(QString fileName);
    void loadXmlFile(QString fileName);
    QMap<QString, QDateTime> loadedFiles();

private:
    QVariant createSource(QString fileName);
    QVariant createBlock(QDomElement block, QVariant sourceId);
    QVariant createBoard(QDomElement board, QVariant blockId);
    QVariant createPort(QDomElement port, QVariant boardId);
    void execQuery(QString query);
    void execQuery(QSqlQuery query);
};

#endif // DATABASEFILLER_H
