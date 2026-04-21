#pragma once

#include <QColor>
#include <QString>

namespace Theme {

// 东方古典配色：宣纸、木色、黛墨、朱砂、鎏金
static const QColor Bg0         = QColor(242, 235, 220);
static const QColor Bg1         = QColor(233, 223, 205);
static const QColor Paper       = QColor(252, 248, 239);
static const QColor CardBg      = QColor(255, 251, 243, 228);
static const QColor CardStroke  = QColor(146, 117, 84, 92);
static const QColor CardStrokeSoft = QColor(164, 137, 105, 54);
static const QColor Shadow      = QColor(85, 61, 37, 28);

static const QColor TextMain    = QColor(58, 43, 30);
static const QColor TextSub     = QColor(108, 83, 61);
static const QColor TextMuted   = QColor(133, 111, 91);
static const QColor Accent      = QColor(170, 55, 42);
static const QColor AccentDeep  = QColor(132, 40, 34);
static const QColor AccentLight = QColor(201, 110, 73);
static const QColor InkWash     = QColor(84, 66, 49, 28);
static const QColor GoldAccent  = QColor(176, 129, 67);
static const QColor Moss        = QColor(109, 120, 83);

QString appStyleSheet();
QString preferredFontFamily();

}

