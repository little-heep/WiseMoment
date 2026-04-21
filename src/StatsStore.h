#pragma once

#include <QObject>
#include <QDate>
#include <QMap>

struct DailyStats
{
    int focusSeconds{0};
    int todoCompleted{0};
};

class StatsStore : public QObject
{
    Q_OBJECT
public:
    explicit StatsStore(QObject* parent = nullptr);

    void addFocusSeconds(const QDate& day, int seconds);
    void addTodoCompleted(const QDate& day, int count = 1);

    DailyStats statsForDay(const QDate& day) const;
    QMap<QDate, DailyStats> statsBetween(const QDate& from, const QDate& to) const;
};

