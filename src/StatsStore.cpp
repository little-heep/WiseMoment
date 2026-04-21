#include "StatsStore.h"

#include "Database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

namespace {

bool addDailyDelta(const QDate& day, int focusDelta, int todoDelta, const char* updateClause)
{
    if (!day.isValid() || (focusDelta <= 0 && todoDelta <= 0))
        return false;

    Database::ensureSchema();
    auto db = Database::db();
    QSqlQuery q(db);
    q.prepare(QString(
        "INSERT INTO daily_stats(day,focus_seconds,todo_completed) VALUES(?,?,?) "
        "ON CONFLICT(day) DO UPDATE SET %1").arg(QString::fromUtf8(updateClause)));
    q.addBindValue(QVariant(day.toString(Qt::ISODate)));
    q.addBindValue(QVariant(focusDelta));
    q.addBindValue(QVariant(todoDelta));
    return q.exec();
}

} // namespace

StatsStore::StatsStore(QObject* parent)
    : QObject(parent)
{
}

void StatsStore::addFocusSeconds(const QDate& day, int seconds)
{
    if (seconds <= 0)
        return;
    addDailyDelta(day, seconds, 0, "focus_seconds = focus_seconds + excluded.focus_seconds");
}

void StatsStore::addTodoCompleted(const QDate& day, int count)
{
    if (count <= 0)
        return;
    addDailyDelta(day, 0, count, "todo_completed = todo_completed + excluded.todo_completed");
}

DailyStats StatsStore::statsForDay(const QDate& day) const
{
    if (!day.isValid())
        return {};
    Database::ensureSchema();
    auto db = Database::db();
    QSqlQuery q(db);
    q.prepare("SELECT focus_seconds,todo_completed FROM daily_stats WHERE day=?");
    q.addBindValue(QVariant(day.toString(Qt::ISODate)));
    if (!q.exec() || !q.next())
        return {};
    DailyStats ds;
    ds.focusSeconds = q.value(0).toInt();
    ds.todoCompleted = q.value(1).toInt();
    return ds;
}

QMap<QDate, DailyStats> StatsStore::statsBetween(const QDate& from, const QDate& to) const
{
    QMap<QDate, DailyStats> out;
    if (!from.isValid() || !to.isValid() || from > to)
        return out;

    Database::ensureSchema();
    auto db = Database::db();
    QSqlQuery q(db);
    q.prepare("SELECT day,focus_seconds,todo_completed FROM daily_stats WHERE day>=? AND day<=? ORDER BY day ASC");
    q.addBindValue(QVariant(from.toString(Qt::ISODate)));
    q.addBindValue(QVariant(to.toString(Qt::ISODate)));
    if (!q.exec())
        return out;

    while (q.next()) {
        const auto d = QDate::fromString(q.value(0).toString(), Qt::ISODate);
        if (!d.isValid())
            continue;
        DailyStats ds;
        ds.focusSeconds = q.value(1).toInt();
        ds.todoCompleted = q.value(2).toInt();
        out.insert(d, ds);
    }

    return out;
}

