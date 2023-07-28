
#ifndef DATABASEWATCHER_H
#define DATABASEWATCHER_H

#include "DatabaseFiller.h"

#include <QObject>
#include <QTimer>

class DatabaseWatcher : public QObject {
    Q_OBJECT
    DatabaseFiller* _filler;
    QString _path;
    QTimer _timer;

public:
    explicit DatabaseWatcher(DatabaseFiller* filler, QString directory, QObject* parent = nullptr);

signals:

private slots:
    void syncData();

private:
    QMap<QString, QDateTime> existsFiles();
};

#endif // DATABASEWATCHER_H
