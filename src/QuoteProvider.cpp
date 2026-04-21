#include "QuoteProvider.h"

#include <QDate>
#include <QStringList>

static QString pickFrom(const QStringList& list)
{
    if (list.isEmpty())
        return {};
    const int idx = qAbs(QDate::currentDate().dayOfYear()) % list.size();
    return list.at(idx);
}

QString QuoteProvider::quoteOfTheDay(const QString& localeName)
{
    if (localeName.startsWith("en")) {
        return pickFrom({
            "Small steps, every day.",
            "Focus is a superpower.",
            "Do the next right thing.",
            "Start, then refine.",
            "Make time visible."
        });
    }
    return pickFrom({
        "把注意力放在当下。",
        "小步快跑，持续积累。",
        "先开始，再变好。",
        "专注，是稀缺能力。",
        "让时间看得见。"
    });
}

