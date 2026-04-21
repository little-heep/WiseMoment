#include "Database.h"

#include "AppConfig.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

static QString dbPath()
{
    return AppConfig::appDataDir() + "/momentwise.sqlite";
}

static bool execOk(QSqlQuery& q, const QString& sql)
{
    if (!q.exec(sql))
        return false;
    return true;
}

static int schemaVersion(QSqlDatabase& db)
{
    QSqlQuery q(db);
    if (!q.exec("PRAGMA user_version"))
        return 0;
    if (!q.next())
        return 0;
    return q.value(0).toInt();
}

static bool setSchemaVersion(QSqlDatabase& db, int v)
{
    QSqlQuery q(db);
    return q.exec(QString("PRAGMA user_version=%1").arg(v));
}

QSqlDatabase Database::db()
{
    return QSqlDatabase::database("momentwise");
}

bool Database::open()
{
    if (QSqlDatabase::contains("momentwise")) {
        auto d = db();
        if (d.isOpen())
            return true;
        return d.open();
    }

    AppConfig::ensureAppDataDir();
    const auto path = dbPath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    auto d = QSqlDatabase::addDatabase("QSQLITE", "momentwise");
    d.setDatabaseName(path);
    if (!d.open())
        return false;

    QSqlQuery q(d);
    q.exec("PRAGMA foreign_keys=ON");
    q.exec("PRAGMA journal_mode=WAL");
    return true;
}

bool Database::ensureSchema()
{
    if (!open())
        return false;

    auto d = db();
    QSqlQuery q(d);

    const int v = schemaVersion(d);
    if (v == 0) {
        if (!execOk(q,
            "CREATE TABLE IF NOT EXISTS todos ("
            "  id TEXT PRIMARY KEY,"
            "  title TEXT NOT NULL,"
            "  due TEXT,"
            "  done INTEGER NOT NULL DEFAULT 0,"
            "  created_at TEXT NOT NULL,"
            "  done_at TEXT"
            ");")) return false;

        if (!execOk(q, "CREATE INDEX IF NOT EXISTS idx_todos_done_due ON todos(done, due);"))
            return false;

        if (!execOk(q,
            "CREATE TABLE IF NOT EXISTS daily_stats ("
            "  day TEXT PRIMARY KEY,"
            "  focus_seconds INTEGER NOT NULL DEFAULT 0,"
            "  todo_completed INTEGER NOT NULL DEFAULT 0"
            ");")) return false;

        if (!setSchemaVersion(d, 1))
            return false;
        return true;
    }

    // future migrations here
    return true;
}

static bool tableHasRows(QSqlDatabase& db, const QString& table)
{
    QSqlQuery q(db);
    if (!q.exec(QString("SELECT 1 FROM %1 LIMIT 1").arg(table)))
        return false;
    return q.next();
}

static bool importTodosJson(QSqlDatabase& db)
{
    QFile f(AppConfig::todosPath());
    if (!f.open(QIODevice::ReadOnly))
        return true; // no file, skip

    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray())
        return true;

    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO todos(id,title,due,done,created_at,done_at) VALUES(?,?,?,?,?,?)");

    const auto arr = doc.array();
    for (const auto& v : arr) {
        if (!v.isObject())
            continue;
        const auto o = v.toObject();
        const auto id = o.value("id").toString();
        const auto title = o.value("title").toString();
        const auto due = o.value("due").toString();
        const auto done = o.value("done").toBool(false) ? 1 : 0;
        const auto createdAt = o.value("createdAt").toString();
        const auto doneAt = o.value("doneAt").toString();

        if (id.isEmpty() || title.trimmed().isEmpty())
            continue;

        q.addBindValue(id);
        q.addBindValue(title);
        q.addBindValue(due.isEmpty() ? QVariant(QVariant::String) : QVariant(due));
        q.addBindValue(done);
        q.addBindValue(createdAt.isEmpty() ? QDateTime::currentDateTime().toString(Qt::ISODateWithMs) : createdAt);
        q.addBindValue(doneAt.isEmpty() ? QVariant(QVariant::String) : QVariant(doneAt));
        if (!q.exec())
            return false;
        q.finish();
        q.prepare("INSERT OR IGNORE INTO todos(id,title,due,done,created_at,done_at) VALUES(?,?,?,?,?,?)");
    }
    return true;
}

static bool importStatsJson(QSqlDatabase& db)
{
    QFile f(AppConfig::statsPath());
    if (!f.open(QIODevice::ReadOnly))
        return true;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())
        return true;
    const auto root = doc.object();
    const auto days = root.value("days").toObject();

    QSqlQuery q(db);
    q.prepare("INSERT OR REPLACE INTO daily_stats(day,focus_seconds,todo_completed) VALUES(?,?,?)");
    for (auto it = days.begin(); it != days.end(); ++it) {
        if (!it.value().isObject())
            continue;
        const auto day = it.key();
        const auto o = it.value().toObject();
        const int focus = o.value("focusSeconds").toInt(0);
        const int done = o.value("todoCompleted").toInt(0);
        if (day.isEmpty())
            continue;
        q.addBindValue(day);
        q.addBindValue(focus);
        q.addBindValue(done);
        if (!q.exec())
            return false;
        q.finish();
        q.prepare("INSERT OR REPLACE INTO daily_stats(day,focus_seconds,todo_completed) VALUES(?,?,?)");
    }
    return true;
}

bool Database::migrateFromJsonIfNeeded()
{
    if (!ensureSchema())
        return false;

    auto d = db();
    if (tableHasRows(d, "todos") || tableHasRows(d, "daily_stats"))
        return true; // already using DB

    QSqlQuery q(d);
    if (!q.exec("BEGIN IMMEDIATE"))
        return false;

    const bool ok = importTodosJson(d) && importStatsJson(d);
    QSqlQuery endq(d);
    endq.exec(ok ? "COMMIT" : "ROLLBACK");
    return ok;
}

