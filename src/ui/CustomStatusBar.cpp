#include "CustomStatusBar.h"

#include <QHBoxLayout>
#include <QLabel>

CustomStatusBar::CustomStatusBar(QWidget* parent)
    : QWidget(parent)
{
    auto* l = new QHBoxLayout(this);
    l->setContentsMargins(12, 6, 12, 6);
    l->setSpacing(10);

    m_left = new QLabel(this);
    m_left->setStyleSheet("color:rgba(100, 90, 80, 180);");
    m_right = new QLabel(this);
    m_right->setStyleSheet("color:rgba(100, 90, 80, 180);");

    l->addWidget(m_left, 1);
    l->addWidget(m_right, 0, Qt::AlignRight);

    setStyleSheet("background: rgba(120, 100, 80, 10); border-top: 1px solid rgba(180, 170, 150, 30);");
}

void CustomStatusBar::setLeftText(const QString& text)
{
    m_left->setText(text);
}

void CustomStatusBar::setRightText(const QString& text)
{
    m_right->setText(text);
}

