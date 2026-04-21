#pragma once

#include <QString>

class QuoteProvider
{
public:
    static QString quoteOfTheDay(const QString& localeName);
};

