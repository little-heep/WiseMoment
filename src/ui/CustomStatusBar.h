#pragma once

#include <QWidget>

class QLabel;

class CustomStatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomStatusBar(QWidget* parent = nullptr);

public slots:
    void setLeftText(const QString& text);
    void setRightText(const QString& text);

private:
    QLabel* m_left{nullptr};
    QLabel* m_right{nullptr};
};

