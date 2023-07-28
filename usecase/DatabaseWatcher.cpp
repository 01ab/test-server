
#include "DatabaseWatcher.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMap>

DatabaseWatcher::DatabaseWatcher(DatabaseFiller* filler, QString directory, QObject* parent)
    : QObject { parent }
    , _filler { filler }
    , _path { directory }

{
    // в тз у нас обновление по таймеру, но, таки есть ещё QFileSystemWatcher
    connect(&_timer, &QTimer::timeout, this, &DatabaseWatcher::syncData);
    _timer.setInterval(60000); // раз в минуту
    _timer.start();

    // немного ватчера мимо кассы
    QFileSystemWatcher* fsw = new QFileSystemWatcher({ directory }, this);
    connect(fsw, &QFileSystemWatcher::directoryChanged, this, &DatabaseWatcher::syncData);

    syncData();
}

QMap<QString, QDateTime> DatabaseWatcher::existsFiles()
{
    QDir directory(_path);
    QStringList files = directory.entryList({ "*.xml" }, QDir::Files);
    QMap<QString, QDateTime> result;
    foreach (QString file, files) {
        QString fullName = directory.filePath(file);
        result[fullName] = QFileInfo { fullName }.lastModified();
    }
    return result;
}

void DatabaseWatcher::syncData()
{
    // получаем данные из бд, смотрим какие у нас есть, ищем разницу, применяем новые данные
    QMap<QString, QDateTime> loaded = _filler->loadedFiles();
    QMap<QString, QDateTime> exists = existsFiles();
    // удаляем файлы которых уже нет
    foreach (QString file, loaded.keys()) {
        if (!exists.contains(file)) {
            _filler->removeXmlFile(file);
            qDebug() << "File" << file << "removed";
        }
    }
    // Загружаем файлы новые, или обновлённые
    foreach (QString file, exists.keys()) {
        if (!loaded.contains(file) || loaded[file] != exists[file])
            _filler->loadXmlFile(file);
    }
}
