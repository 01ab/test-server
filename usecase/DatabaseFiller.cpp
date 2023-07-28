
#include "DatabaseFiller.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QVariant>

DatabaseFiller::DatabaseFiller(IDatabase* db, QObject* parent)
    : QObject { parent }
    , _db { db }
{
    // создаём бд
    execQuery("CREATE TABLE IF NOT EXISTS source("
              "source_id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "path TEXT NOT NULL, "
              "lastWriteTime TEXT NOT NULL "
              ");");
    execQuery("CREATE TABLE IF NOT EXISTS block("
              "block_id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "source_id INTEGER NOT NULL, "
              "id TEXT NOT NULL, "
              "name TEXT NOT NULL, "
              "ip TEXT NOT NULL, "
              "FOREIGN KEY(source_id) REFERENCES source(source_id) ON DELETE CASCADE);");
    execQuery("CREATE TABLE IF NOT EXISTS board("
              "board_id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "block_id INTEGER NOT NULL, "
              "id TEXT NOT NULL, "
              "name TEXT NOT NULL, "
              "FOREIGN KEY(block_id) REFERENCES block(block_id) ON DELETE CASCADE);");
    execQuery("CREATE TABLE IF NOT EXISTS port("
              "port_id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "board_id INTEGER NOT NULL, "
              "id TEXT NOT NULL, "
              "FOREIGN KEY(board_id) REFERENCES board(board_id) ON DELETE CASCADE);");
}

void DatabaseFiller::execQuery(QString query)
{
    QSqlQuery q = _db->query(query);
    execQuery(q);
}

void DatabaseFiller::execQuery(QSqlQuery q)
{
    if (!q.exec()) {
        qDebug() << "Error execute sql query" << q.executedQuery() << q.lastError().databaseText() << q.lastError().text();
    }
}

QVariant DatabaseFiller::createSource(QString fileName)
{
    QSqlQuery query = _db->query();
    query.prepare("INSERT INTO source (path, lastWriteTime) VALUES (:path, :lastWriteTime)");
    query.bindValue(":path", fileName);
    query.bindValue(":lastWriteTime", QFileInfo { fileName }.lastModified().toString());
    execQuery(query);
    return query.lastInsertId();
}

QVariant DatabaseFiller::createBlock(QDomElement block, QVariant sourceId)
{
    QSqlQuery query = _db->query();
    query.prepare("INSERT INTO block (source_id, id, name, ip) VALUES (:source_id, :id, :name, :ip)");
    query.bindValue(":source_id", sourceId);
    query.bindValue(":id", block.attribute("id"));
    query.bindValue(":name", block.attribute("Name"));
    query.bindValue(":ip", block.attribute("IP"));
    execQuery(query);
    return query.lastInsertId();
}

QVariant DatabaseFiller::createBoard(QDomElement board, QVariant blockId)
{
    QSqlQuery query = _db->query();
    query.prepare("INSERT INTO board (block_id, id, name) VALUES (:block_id, :id, :name)");
    query.bindValue(":block_id", blockId);
    query.bindValue(":id", board.attribute("id"));
    query.bindValue(":name", board.attribute("Name"));
    execQuery(query);
    return query.lastInsertId();
}

QVariant DatabaseFiller::createPort(QDomElement port, QVariant boardId)
{
    QSqlQuery query = _db->query();
    query.prepare("INSERT INTO port (board_id, id) VALUES (:board_id, :id)");
    query.bindValue(":board_id", boardId);
    query.bindValue(":id", port.attribute("id"));
    execQuery(query);
    return query.lastInsertId();
}

void DatabaseFiller::removeXmlFile(QString fileName)
{
    QSqlQuery deleteRequest = _db->query();
    deleteRequest.prepare("DELETE FROM source WHERE path = :path");
    deleteRequest.bindValue(":path", fileName);
    execQuery(deleteRequest);
}

void DatabaseFiller::loadXmlFile(QString fileName)
{
    // сначала удалим всё что с таким же path
    removeXmlFile(fileName);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << QString("Error open %1").arg(fileName);
        return;
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << QString("%1:%2:%3 : parse error: %4").arg(fileName).arg(errorLine).arg(errorColumn).arg(errorMsg);
        return;
    }

    QVariant sourceId = createSource(fileName);
    if (!sourceId.isValid() || sourceId.isNull())
        return;

    QDomNodeList blocks = doc.elementsByTagName("block");
    // В тз один файл один блок, но сделаем для большего количества, xml же
    if (blocks.length() > 1)
        qDebug() << "File" << fileName << "has more than 1 block entry";

    for (int i = 0; i < blocks.length(); ++i) {
        QDomElement block = blocks.at(i).toElement();
        QVariant blockId = createBlock(block, sourceId);
        if (!blockId.isValid() || blockId.isNull())
            continue;
        QDomNodeList boards = block.childNodes();
        for (int bci = 0; bci < boards.length(); ++bci) {
            QDomElement board = boards.at(bci).toElement();
            if (board.tagName().compare("board", Qt::CaseInsensitive) == 0) {
                QVariant boardId = createBoard(board, blockId);
                if (!boardId.isValid() || boardId.isNull())
                    continue;
                QDomNodeList ports = board.childNodes();
                for (int pi = 0; pi < ports.length(); ++pi) {
                    QDomElement port = ports.at(pi).toElement();
                    if (port.tagName().compare("port", Qt::CaseInsensitive) == 0)
                        createPort(port, boardId);
                }
            } else {
                // вероятно нарушен формат, может быть надо вообще прекратить, todo аналитикам :-)
                qDebug() << "File" << fileName << "Unexpected content of the <board>";
            }
        }
    }
    qDebug() << "File" << fileName << "loaded";
}

// на самом деле тут такое себе решение, ибо не совсем солид и за загруженными в базу файлами должен следить кто то другой todo рефакторинг
QMap<QString, QDateTime> DatabaseFiller::loadedFiles()
{
    QMap<QString, QDateTime> result;
    QSqlQuery query("SELECT path, lastWriteTime FROM source");
    while (query.next()) {
        QString path = query.value(0).toString();
        QString lastWriteTime = query.value(1).toString();
        result[path] = QDateTime::fromString(lastWriteTime);
    }
    return result;
}
