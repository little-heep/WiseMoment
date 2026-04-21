#include "Theme.h"

#include <QFontDatabase>

namespace Theme {

QString preferredFontFamily()
{
    // 东方美学字体优先级：楷体、宋体、思源宋体等
    const QStringList candidates = {
        "KaiTi",                    // 楷体
        "SimSun",                   // 宋体
        "Source Han Serif SC",      // 思源宋体
        "Noto Serif CJK SC",        // Noto Serif
        "FangSong",                 // 仿宋
        "Microsoft YaHei UI",       // 微软雅海（备选）
        "Microsoft YaHei",
        "Segoe UI"
    };

    const auto families = QFontDatabase().families();
    for (const auto& c : candidates) {
        if (families.contains(c))
            return c;
    }
    return QString();
}

QString appStyleSheet()
{
    QString style = QString(R"(
        QWidget {
            color: rgba(%1,%2,%3,255);
            font-family: "%FONT%";
            font-size: 15px;
            background: transparent;
        }

        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(%4,%5,%6,255),
                stop:0.45 rgba(%7,%8,%9,255),
                stop:1 rgba(%10,%11,%12,255));
        }

        QFrame#WindowSurface {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(%13,%14,%15,242),
                stop:0.55 rgba(250,245,236,236),
                stop:1 rgba(247,239,226,244));
            border: 1px solid rgba(%16,%17,%18,%19);
            border-radius: 30px;
        }

        QFrame#WindowSurface[role="panel"] {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(255,252,246,235),
                stop:1 rgba(247,238,223,225));
            border: 1px solid rgba(%20,%21,%22,%23);
            border-radius: 22px;
        }

        QFrame#Card, QFrame[role="panel"], QGroupBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(255,252,246,228),
                stop:0.55 rgba(252,247,238,220),
                stop:1 rgba(243,234,220,208));
            border: 1px solid rgba(%16,%17,%18,%19);
            border-radius: 24px;
        }

        QPushButton, QToolButton {
            border: 1px solid rgba(%20,%21,%22,64);
            border-radius: 18px;
            padding: 12px 18px;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(255,251,244,228),
                stop:1 rgba(236,226,210,205));
            color: rgba(%1,%2,%3,255);
            font-weight: 600;
            font-size: 15px;
        }
        QPushButton:hover, QToolButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(255,253,248,240),
                stop:1 rgba(241,231,215,220));
            border-color: rgba(%24,%25,%26,92);
        }
        QPushButton:pressed, QToolButton:pressed {
            background: rgba(%27,%28,%29,70);
            border-color: rgba(%27,%28,%29,110);
        }
        QPushButton#Primary {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(%24,%25,%26,235),
                stop:1 rgba(%27,%28,%29,220));
            color: rgba(252, 247, 239, 255);
            font-weight: 700;
            font-size: 16px;
            border: 1px solid rgba(%27,%28,%29,150);
        }
        QPushButton#Primary:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(%24,%25,%26,245),
                stop:1 rgba(%27,%28,%29,235));
        }
        QPushButton#accentGhost {
            background: rgba(%24,%25,%26,24);
            color: rgba(%27,%28,%29,235);
            border: 1px solid rgba(%24,%25,%26,72);
        }

        QLineEdit, QDateTimeEdit, QDateEdit, QComboBox, QSpinBox {
            border: 1px solid rgba(%20,%21,%22,62);
            border-radius: 18px;
            padding: 12px 16px;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(255,252,247,215),
                stop:1 rgba(242,233,218,205));
            color: rgba(%1,%2,%3,255);
            selection-background-color: rgba(%24,%25,%26,110);
            font-size: 16px;
            font-weight: 500;
        }
        QLineEdit:hover, QDateTimeEdit:hover, QDateEdit:hover, QComboBox:hover, QSpinBox:hover {
            border-color: rgba(%24,%25,%26,85);
        }
        QLineEdit:focus, QDateTimeEdit:focus, QDateEdit:focus, QComboBox:focus, QSpinBox:focus {
            border-color: rgba(%24,%25,%26,120);
            border-width: 2px;
        }

        QDateTimeEdit::drop-down, QDateEdit::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: center right;
            width: 30px;
            margin-right: 6px;
            border: 1px solid rgba(%24,%25,%26,58);
            border-radius: 10px;
            background: rgba(%24,%25,%26,20);
        }
        QDateTimeEdit::drop-down:hover, QDateEdit::drop-down:hover {
            background: rgba(%24,%25,%26,30);
            border-color: rgba(%24,%25,%26,88);
        }
        QDateTimeEdit::down-arrow, QDateEdit::down-arrow {
            image: none;
            width: 0;
            height: 0;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid rgba(%27,%28,%29,190);
        }
        QDateTimeEdit::down-arrow:hover, QDateEdit::down-arrow:hover {
            border-top-color: rgba(%27,%28,%29,230);
        }

        QSpinBox::up-button, QSpinBox::down-button {
            subcontrol-origin: border;
            width: 32px;
            border: none;
            background: transparent;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: rgba(%24,%25,%26,18);
            border-radius: 6px;
        }
        QSpinBox::up-button:pressed, QSpinBox::down-button:pressed {
            background: rgba(%24,%25,%26,32);
            border-radius: 6px;
        }
        QSpinBox::up-arrow {
            image: none;
            width: 0;
            height: 0;
            border-left: 6px solid transparent;
            border-right: 6px solid transparent;
            border-bottom: 6px solid rgba(%1,%2,%3,160);
            margin: 4px;
        }
        QSpinBox::down-arrow {
            image: none;
            width: 0;
            height: 0;
            border-left: 6px solid transparent;
            border-right: 6px solid transparent;
            border-top: 6px solid rgba(%1,%2,%3,160);
            margin: 4px;
        }
        QSpinBox::up-arrow:hover, QSpinBox::down-arrow:hover {
            border-bottom-color: rgba(%24,%25,%26,190);
            border-top-color: rgba(%24,%25,%26,190);
        }

        QComboBox::drop-down {
            border: none;
            width: 28px;
            background: rgba(%27,%28,%29,36);
            border-radius: 10px;
            margin-right: 4px;
        }
        QComboBox::down-arrow {
            image: none;
            width: 0;
            height: 0;
        }

        QSlider::groove:horizontal {
            height: 8px;
            border-radius: 4px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(%20,%21,%22,28),
                stop:1 rgba(%16,%17,%18,42));
        }
        QSlider::handle:horizontal {
            width: 20px;
            margin: -7px 0;
            border-radius: 10px;
            background: qradialgradient(cx:0.3, cy:0.3, radius:1.0,
                stop:0 rgba(%24,%25,%26,220),
                stop:1 rgba(%27,%28,%29,180));
            border: 2px solid rgba(250, 242, 230, 80);
        }
        QSlider::handle:horizontal:hover {
            border-color: rgba(%31,%32,%33,120);
        }

        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(%34,%35,%36,58),
                stop:1 rgba(%34,%35,%36,38));
            color: rgba(%1,%2,%3,230);
            border: none;
            border-bottom: 1px solid rgba(%16,%17,%18,70);
            padding: 12px;
            font-weight: 600;
            font-size: 14px;
        }
        QTableView {
            background: transparent;
            alternate-background-color: rgba(%34,%35,%36,18);
            gridline-color: rgba(%20,%21,%22,22);
            selection-background-color: rgba(%24,%25,%26,44);
            selection-color: rgba(250, 244, 236, 255);
            border: none;
        }
        QTableView::item {
            padding: 0;
            border: none;
            font-size: 15px;
        }

        QCalendarWidget QWidget {
            background: rgba(252,248,240,250);
        }
        QCalendarWidget QToolButton {
            min-width: 30px;
            min-height: 28px;
            border-radius: 12px;
            padding: 6px 10px;
        }
        QCalendarWidget QMenu {
            padding: 8px;
        }
        QCalendarWidget QSpinBox {
            min-width: 84px;
        }
        QCalendarWidget QAbstractItemView {
            selection-background-color: rgba(%24,%25,%26,52);
            selection-color: rgba(250,244,236,255);
            alternate-background-color: rgba(%34,%35,%36,14);
            border-radius: 14px;
            outline: none;
        }

        QMenu {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(255,250,242,246),
                stop:1 rgba(240,230,214,242));
            border: 1px solid rgba(%16,%17,%18,80);
            border-radius: 18px;
            padding: 14px;
        }
        QMenu::item {
            padding: 12px 18px;
            border-radius: 14px;
            color: rgba(%1,%2,%3,230);
            font-size: 15px;
        }
        QMenu::item:selected {
            background: rgba(%24,%25,%26,26);
        }
        QMenu::separator {
            height: 1px;
            background: rgba(%20,%21,%22,28);
            margin: 8px 10px;
        }

        QGroupBox {
            font-weight: 600;
            color: rgba(%1,%2,%3,240);
            margin-top: 20px;
            padding-top: 18px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 18px;
            padding: 0 10px;
            font-size: 16px;
            color: rgba(%24,%25,%26,220);
        }

        QCheckBox {
            spacing: 12px;
            color: rgba(%1,%2,%3,230);
            font-size: 15px;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border-radius: 6px;
            border: 1px solid rgba(%20,%21,%22,72);
            background: rgba(255,251,246,210);
        }
        QCheckBox::indicator:hover {
            border-color: rgba(%24,%25,%26,84);
        }
        QCheckBox::indicator:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(%24,%25,%26,220),
                stop:1 rgba(%27,%28,%29,200));
            border-color: rgba(%27,%28,%29,105);
        }

        QLabel {
            color: rgba(%1,%2,%3,238);
            font-size: 15px;
            background: transparent;
        }

        QLabel[role="eyebrow"] {
            color: rgba(%31,%32,%33,220);
            font-size: 13px;
            font-weight: 700;
            letter-spacing: 1px;
        }

        QLabel[role="sectionTitle"] {
            color: rgba(%1,%2,%3,255);
            font-size: 22px;
            font-weight: 700;
        }

        QLabel[role="metric"] {
            color: rgba(%27,%28,%29,245);
            font-size: 28px;
            font-weight: 700;
        }

        QLabel[role="muted"] {
            color: rgba(%37,%38,%39,228);
        }

        QAbstractScrollArea {
            background: transparent;
        }

        QScrollBar:vertical {
            background: rgba(%20,%21,%22,12);
            width: 10px;
            border-radius: 5px;
            margin: 4px;
        }
        QScrollBar::handle:vertical {
            background: rgba(%24,%25,%26,80);
            min-height: 24px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
            border: none;
        }
    )")
        .arg(TextMain.red()).arg(TextMain.green()).arg(TextMain.blue())
        .arg(Bg0.red()).arg(Bg0.green()).arg(Bg0.blue())
        .arg(Bg1.red()).arg(Bg1.green()).arg(Bg1.blue())
        .arg(Paper.red()).arg(Paper.green()).arg(Paper.blue())
        .arg(CardBg.red()).arg(CardBg.green()).arg(CardBg.blue())
        .arg(CardStroke.red()).arg(CardStroke.green()).arg(CardStroke.blue()).arg(CardStroke.alpha())
        .arg(CardStrokeSoft.red()).arg(CardStrokeSoft.green()).arg(CardStrokeSoft.blue()).arg(CardStrokeSoft.alpha())
        .arg(Accent.red()).arg(Accent.green()).arg(Accent.blue())
        .arg(AccentDeep.red()).arg(AccentDeep.green()).arg(AccentDeep.blue())
        .arg(GoldAccent.red()).arg(GoldAccent.green()).arg(GoldAccent.blue())
        .arg(InkWash.red()).arg(InkWash.green()).arg(InkWash.blue())
        .arg(TextMuted.red()).arg(TextMuted.green()).arg(TextMuted.blue());
    style.replace("%FONT%", preferredFontFamily());
    return style;
}

}

