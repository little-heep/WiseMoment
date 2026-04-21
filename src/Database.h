#pragma once

#include <QSqlDatabase>

class Database
{
public:
    static bool open();
    static QSqlDatabase db();

    static bool ensureSchema();
    static bool migrateFromJsonIfNeeded();
};

