#pragma once

#include <QWidget>

class QToolButton;

class BottomNav : public QWidget
{
    Q_OBJECT
public:
    explicit BottomNav(QWidget* parent = nullptr);

    void setCurrentIndex(int idx);

signals:
    void indexChanged(int idx);

private:
    void updateStyles();

    QToolButton* m_btnPomo{nullptr};
    QToolButton* m_btnTask{nullptr};
    QToolButton* m_btnStat{nullptr};
    int m_idx{0};
};

