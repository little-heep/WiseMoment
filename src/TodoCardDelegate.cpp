#include "TodoCardDelegate.h"

#include "TodoModel.h"

#include <QApplication>
#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

namespace {

QRect cardRectForOption(const QStyleOptionViewItem& option)
{
    const bool hovered = (option.state & QStyle::State_MouseOver);
    if (hovered)
        return option.rect.adjusted(8, 4, -8, -10);
    return option.rect.adjusted(10, 8, -10, -8);
}

}

TodoCardDelegate::TodoCardDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void TodoCardDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!painter || !index.isValid())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QRect cardRect = cardRectForOption(option);
    const bool hovered = (option.state & QStyle::State_MouseOver);
    const QRect shadowRect = cardRect.translated(0, hovered ? 6 : 5);
    const bool selected = (option.state & QStyle::State_Selected);

    const QString title = index.data(Qt::DisplayRole).toString();
    const QModelIndex dueIndex = index.sibling(index.row(), TodoModel::Due);
    const QModelIndex statusIndex = index.sibling(index.row(), TodoModel::Status);
    const QDateTime due = dueIndex.data(Qt::EditRole).toDateTime();
    const bool done = (statusIndex.data(Qt::DisplayRole).toString() == QApplication::translate("TodoModel", "Done"));
    const bool overdue = (!done && due.isValid() && due < QDateTime::currentDateTime());

    painter->setPen(Qt::NoPen);
    painter->setBrush(hovered ? QColor(71, 49, 31, 38) : QColor(71, 49, 31, 24));
    painter->drawRoundedRect(shadowRect, 22, 22);

    QColor fillStart(255, 252, 246, selected ? 244 : (hovered ? 238 : 228));
    QColor fillEnd(245, 236, 221, selected ? 238 : (hovered ? 228 : 215));
    QColor border = selected ? QColor(170, 55, 42, 124) : (hovered ? QColor(170, 55, 42, 106) : QColor(152, 124, 95, 72));
    if (done) {
        fillStart = QColor(243, 239, 230, 215);
        fillEnd = QColor(233, 228, 217, 205);
        border = QColor(130, 122, 108, 64);
    } else if (overdue) {
        border = QColor(170, 55, 42, 132);
    }

    QLinearGradient background(cardRect.topLeft(), cardRect.bottomRight());
    background.setColorAt(0.0, fillStart);
    background.setColorAt(1.0, fillEnd);
    painter->setBrush(background);
    painter->setPen(QPen(border, 1));
    painter->drawRoundedRect(cardRect, 22, 22);

    QRect accentRect = cardRect.adjusted(12, 14, -cardRect.width() + 16, -14);
    accentRect.setWidth(4);
    painter->setPen(Qt::NoPen);
    painter->setBrush(done ? QColor(109, 120, 83, 170) : QColor(170, 55, 42, 185));
    painter->drawRoundedRect(accentRect, 2, 2);

    QRect contentRect = cardRect.adjusted(24, 16, -18, -16);
    const QRect toggleRect = toggleButtonRect(option);
    const QRect deleteRect = deleteButtonRect(option);
    QRect titleRect = contentRect;
    titleRect.setRight(toggleRect.left() - 10);
    titleRect.setBottom(contentRect.top() + 20);

    QFont titleFont = option.font;
    titleFont.setPixelSize(18);
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->setPen(done ? QColor(117, 104, 91, 210) : QColor(58, 43, 30, 242));
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title);

    const auto drawActionButton = [&](const QRect& rect, const QString& text, const QColor& fill, const QColor& border, const QColor& fg) {
        painter->setBrush(fill);
        painter->setPen(QPen(border, 1));
        painter->drawRoundedRect(rect, 12, 12);
        QFont f = option.font;
        f.setPixelSize(14);
        f.setBold(true);
        painter->setFont(f);
        painter->setPen(fg);
        painter->drawText(rect, Qt::AlignCenter, text);
    };

    drawActionButton(toggleRect,
                     done ? QStringLiteral("重开") : QStringLiteral("完成"),
                     done ? QColor(109, 120, 83, 28) : QColor(170, 55, 42, 24),
                     done ? QColor(109, 120, 83, 82) : QColor(170, 55, 42, 92),
                     done ? QColor(90, 102, 71, 220) : QColor(140, 46, 38, 228));
    drawActionButton(deleteRect,
                     QStringLiteral("删除"),
                     QColor(101, 78, 56, 18),
                     QColor(132, 108, 84, 60),
                     QColor(97, 74, 52, 220));

    QRect chipRect = QRect(contentRect.left(), contentRect.bottom() - 20, 88, 20);
    QString dueText;
    QColor chipFill;
    QColor chipText;
    if (done) {
        dueText = QStringLiteral("已完成");
        chipFill = QColor(109, 120, 83, 34);
        chipText = QColor(90, 102, 71, 220);
    } else if (!due.isValid()) {
        dueText = QStringLiteral("未设期限");
        chipFill = QColor(176, 129, 67, 26);
        chipText = QColor(143, 103, 51, 220);
    } else if (overdue) {
        dueText = QStringLiteral("已逾期");
        chipFill = QColor(170, 55, 42, 32);
        chipText = QColor(155, 46, 38, 230);
    } else {
        dueText = due.date() == QDate::currentDate()
            ? QStringLiteral("今天截止")
            : due.date() == QDate::currentDate().addDays(1)
                ? QStringLiteral("明日截止")
                : QStringLiteral("待处理");
        chipFill = QColor(170, 55, 42, 24);
        chipText = QColor(140, 46, 38, 225);
    }

    painter->setBrush(chipFill);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(chipRect, 12, 12);
    QFont chipFont = option.font;
    chipFont.setPixelSize(12);
    chipFont.setBold(true);
    painter->setFont(chipFont);
    painter->setPen(chipText);
    painter->drawText(chipRect, Qt::AlignCenter, dueText);

    QRect dueRect = QRect(chipRect.right() + 10, contentRect.bottom() - 23,
                          contentRect.right() - chipRect.right() - 10, 24);
    QFont dueFont = option.font;
    dueFont.setPixelSize(13);
    painter->setFont(dueFont);
    painter->setPen(QColor(116, 91, 68, 214));
    const QString dueLabel = due.isValid() ? due.toString("yyyy年MM月dd日  HH:mm") : QStringLiteral("可在下方设置日期");
    painter->drawText(dueRect, Qt::AlignLeft | Qt::AlignVCenter, dueLabel);

    painter->restore();
}

bool TodoCardDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (!index.isValid())
        return false;

    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    const auto* mouseEvent = static_cast<QMouseEvent*>(event);
    if (toggleButtonRect(option).contains(mouseEvent->pos())) {
        emit toggleRequested(index.row());
        return true;
    }
    if (deleteButtonRect(option).contains(mouseEvent->pos())) {
        emit deleteRequested(index.row());
        return true;
    }
    return false;
}

QSize TodoCardDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    return QSize(0, 90);
}

QRect TodoCardDelegate::toggleButtonRect(const QStyleOptionViewItem& option) const
{
    const QRect cardRect = cardRectForOption(option);
    return QRect(cardRect.right() - 146, cardRect.top() + 14, 62, 28);
}

QRect TodoCardDelegate::deleteButtonRect(const QStyleOptionViewItem& option) const
{
    const QRect cardRect = cardRectForOption(option);
    return QRect(cardRect.right() - 76, cardRect.top() + 14, 54, 28);
}
