#include "TodoDueDelegate.h"

#include <QDateTimeEdit>

TodoDueDelegate::TodoDueDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* TodoDueDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* ed = new QDateTimeEdit(parent);
    ed->setCalendarPopup(true);
    ed->setDisplayFormat("yyyy-MM-dd HH:mm");
    return ed;
}

void TodoDueDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto* ed = qobject_cast<QDateTimeEdit*>(editor);
    if (!ed)
        return;
    const auto v = index.model()->data(index, Qt::EditRole);
    const auto dt = v.canConvert<QDateTime>() ? v.toDateTime() : QDateTime{};
    ed->setDateTime(dt.isValid() ? dt : QDateTime::currentDateTime().addDays(1));
}

void TodoDueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* ed = qobject_cast<QDateTimeEdit*>(editor);
    if (!ed || !model)
        return;
    model->setData(index, ed->dateTime(), Qt::EditRole);
}

void TodoDueDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
    if (editor)
        editor->setGeometry(option.rect);
}

