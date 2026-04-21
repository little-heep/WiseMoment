#include "TodoModel.h"

#include "Database.h"

#include <QColor>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>

TodoModel::TodoModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int TodoModel::rowCount(const QModelIndex&) const
{
    return m_items.size();
}

int TodoModel::columnCount(const QModelIndex&) const
{
    return ColumnCount;
}

QVariant TodoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto& it = m_items.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case Title: return it.title;
        case Due:
            if (role == Qt::EditRole)
                return it.due;
            return it.due.isValid() ? it.due.toString("yyyy-MM-dd HH:mm") : QString();
        case Status: return it.done ? tr("Done") : tr("Todo");
        default: return {};
        }
    }

    if (role == Qt::ForegroundRole && index.column() != Status) {
        if (!it.done && it.due.isValid() && it.due < QDateTime::currentDateTime())
            return QColor(220, 60, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == Due || index.column() == Status)
            return Qt::AlignCenter;
    }

    return {};
}

QVariant TodoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    switch (section) {
    case Title: return tr("Title");
    case Due: return tr("Due");
    case Status: return tr("Status");
    default: return {};
    }
}

Qt::ItemFlags TodoModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    auto f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == Title || index.column() == Due)
        f |= Qt::ItemIsEditable;
    return f;
}

bool TodoModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    if (index.row() < 0 || index.row() >= m_items.size())
        return false;

    auto it = m_items.at(index.row());
    if (index.column() == Title) {
        const auto title = value.toString().trimmed();
        if (title.isEmpty())
            return false;
        it.title = title;
    } else if (index.column() == Due) {
        if (value.canConvert<QDateTime>()) {
            it.due = value.toDateTime();
        } else {
            const auto dt = QDateTime::fromString(value.toString(), Qt::ISODateWithMs);
            it.due = dt;
        }
    } else {
        return false;
    }

    if (!upsertRow(it))
        return false;

    beginResetModel();
    m_items[index.row()] = it;
    sortByUrgency();
    endResetModel();
    return true;
}

void TodoModel::load()
{
    beginResetModel();
    m_items.clear();

    Database::ensureSchema();
    auto db = Database::db();

    QSqlQuery q(db);
    q.prepare("SELECT id,title,due,done,created_at,done_at FROM todos");
    if (q.exec()) {
        while (q.next()) {
            TodoItem it;
            it.id = q.value(0).toString();
            it.title = q.value(1).toString();
            it.due = QDateTime::fromString(q.value(2).toString(), Qt::ISODateWithMs);
            it.done = q.value(3).toInt() != 0;
            it.createdAt = QDateTime::fromString(q.value(4).toString(), Qt::ISODateWithMs);
            it.doneAt = QDateTime::fromString(q.value(5).toString(), Qt::ISODateWithMs);
            m_items.push_back(it);
        }
    }

    sortByUrgency();
    endResetModel();
}

void TodoModel::add(const QString& title, const QDateTime& due)
{
    TodoItem it;
    it.id = newId();
    it.title = title.trimmed();
    it.due = due;
    it.done = false;
    it.createdAt = QDateTime::currentDateTime();

    if (!upsertRow(it))
        return;

    beginResetModel();
    m_items.push_front(it);
    sortByUrgency();
    endResetModel();
}

void TodoModel::removeRowAt(int row)
{
    if (row < 0 || row >= m_items.size())
        return;
    const auto id = m_items.at(row).id;
    if (!deleteRowById(id))
        return;
    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}

void TodoModel::toggleDone(int row)
{
    if (row < 0 || row >= m_items.size())
        return;
    auto it = m_items.at(row);
    it.done = !it.done;
    it.doneAt = it.done ? QDateTime::currentDateTime() : QDateTime{};
    if (!upsertRow(it))
        return;

    beginResetModel();
    m_items[row] = it;
    sortByUrgency();
    endResetModel();
}

int TodoModel::completedCountBetween(const QDate& from, const QDate& to) const
{
    int count = 0;
    for (const auto& it : m_items) {
        if (!it.done || !it.doneAt.isValid())
            continue;
        const auto d = it.doneAt.date();
        if (d >= from && d <= to)
            count++;
    }
    return count;
}

void TodoModel::sortByUrgency()
{
    std::sort(m_items.begin(), m_items.end(), [](const TodoItem& a, const TodoItem& b) {
        if (a.done != b.done)
            return a.done < b.done; // undone first

        const auto now = QDateTime::currentDateTime();
        const bool aOver = (!a.done && a.due.isValid() && a.due < now);
        const bool bOver = (!b.done && b.due.isValid() && b.due < now);
        if (aOver != bOver)
            return aOver > bOver; // overdue first

        if (a.due.isValid() != b.due.isValid())
            return a.due.isValid() > b.due.isValid(); // with due first

        if (a.due != b.due)
            return a.due < b.due; // earlier due first

        return a.createdAt < b.createdAt;
    });
}

QString TodoModel::newId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool TodoModel::upsertRow(const TodoItem& it)
{
    Database::ensureSchema();
    auto db = Database::db();
    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO todos(id,title,due,done,created_at,done_at) VALUES(?,?,?,?,?,?) "
        "ON CONFLICT(id) DO UPDATE SET "
        "title=excluded.title,"
        "due=excluded.due,"
        "done=excluded.done,"
        "created_at=excluded.created_at,"
        "done_at=excluded.done_at"
    );
    q.addBindValue(it.id);
    q.addBindValue(it.title);
    q.addBindValue(it.due.isValid() ? it.due.toString(Qt::ISODateWithMs) : QVariant(QVariant::String));
    q.addBindValue(it.done ? 1 : 0);
    q.addBindValue(it.createdAt.isValid() ? it.createdAt.toString(Qt::ISODateWithMs) : QDateTime::currentDateTime().toString(Qt::ISODateWithMs));
    q.addBindValue(it.doneAt.isValid() ? it.doneAt.toString(Qt::ISODateWithMs) : QVariant(QVariant::String));
    return q.exec();
}

bool TodoModel::deleteRowById(const QString& id)
{
    if (id.isEmpty())
        return false;
    Database::ensureSchema();
    auto db = Database::db();
    QSqlQuery q(db);
    q.prepare("DELETE FROM todos WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

