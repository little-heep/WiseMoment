#pragma once

#include <QStyledItemDelegate>

class TodoCardDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TodoCardDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

signals:
    void toggleRequested(int row);
    void deleteRequested(int row);

private:
    QRect toggleButtonRect(const QStyleOptionViewItem& option) const;
    QRect deleteButtonRect(const QStyleOptionViewItem& option) const;
};
