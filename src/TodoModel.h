#pragma once

#include <QAbstractTableModel>
#include <QDateTime>

struct TodoItem
{
    QString id;
    QString title;
    QDateTime due;
    bool done{false};
    QDateTime createdAt;
    QDateTime doneAt;
};

class TodoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { Title = 0, Due, Status, ColumnCount };

    explicit TodoModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    const QList<TodoItem>& items() const { return m_items; }

    void load();

    void add(const QString& title, const QDateTime& due);
    void removeRowAt(int row);
    void toggleDone(int row);

    int completedCountBetween(const QDate& from, const QDate& to) const;

private:
    void sortByUrgency();
    static QString newId();
    bool upsertRow(const TodoItem& it);
    bool deleteRowById(const QString& id);

    QList<TodoItem> m_items;
};

