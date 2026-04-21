#include "MiniBallWidget.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QPropertyAnimation>
#include <QEasingCurve>

MiniBallWidget::MiniBallWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFixedSize(88, 88);

    const auto screen = QGuiApplication::primaryScreen();
    const auto geo = screen ? screen->availableGeometry() : QRect(0, 0, 1280, 720);
    move(geo.right() - width() - 20, geo.top() + 120);
    
    // 初始化动画
    m_scaleAnimation = new QPropertyAnimation(this, "scale", this);
    m_scaleAnimation->setDuration(300);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
    
    m_opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_opacityAnimation->setDuration(300);
    m_opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void MiniBallWidget::setText(const QString& text)
{
    if (m_text == text)
        return;
    m_text = text;
    update();
}

void MiniBallWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    
    // 应用缩放
    p.translate(rect().center());
    p.scale(m_scale, m_scale);
    p.translate(-rect().center());

    const QRectF r = rect().adjusted(4, 4, -4, -4);
    
    // 外层光晕
    QRadialGradient outerGlow(r.center(), r.width() * 0.8);
    outerGlow.setColorAt(0.0, QColor(255, 244, 226, 40));
    outerGlow.setColorAt(0.7, QColor(225, 190, 148, 22));
    outerGlow.setColorAt(1.0, QColor(255, 248, 235, 0));
    p.setBrush(outerGlow);
    p.setPen(Qt::NoPen);
    p.drawEllipse(r.adjusted(-8, -8, 8, 8));
    
    // 主背景：漆红印章质感
    QRadialGradient mainGradient(r.center(), r.width() * 0.6);
    mainGradient.setColorAt(0.0, QColor(204, 103, 70, 220));
    mainGradient.setColorAt(0.55, QColor(170, 55, 42, 210));
    mainGradient.setColorAt(1.0, QColor(120, 37, 31, 180));
    p.setBrush(mainGradient);
    p.setPen(QPen(QColor(244, 227, 197, 90), 2));
    p.drawEllipse(r);
    p.setPen(QPen(QColor(238, 208, 163, 70), 1));
    p.drawEllipse(r.adjusted(5, 5, -5, -5));
    
    // 内层高光
    QRadialGradient highlight(r.center() * 0.7, r.width() * 0.3);
    highlight.setColorAt(0.0, QColor(255, 255, 255, 40));
    highlight.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.setBrush(highlight);
    p.setPen(Qt::NoPen);
    p.drawEllipse(r.adjusted(r.width() * 0.2, r.height() * 0.2, -r.width() * 0.4, -r.height() * 0.4));

    // 绘制文字
    p.setPen(QColor(252, 245, 235, 255));
    auto f = font();
    f.setBold(true);
    f.setPointSize(12);
    p.setFont(f);
    p.drawText(rect(), Qt::AlignCenter, m_text);
}

void MiniBallWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragOffset = e->globalPos() - frameGeometry().topLeft();
        e->accept();
        return;
    }
    QWidget::mousePressEvent(e);
}

void MiniBallWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_dragging) {
        move(e->globalPos() - m_dragOffset);
        e->accept();
        return;
    }
    QWidget::mouseMoveEvent(e);
}

void MiniBallWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && m_dragging) {
        const bool click = (e->globalPos() - (frameGeometry().topLeft() + m_dragOffset)).manhattanLength() < 4;
        m_dragging = false;
        if (click)
            emit clicked();
        e->accept();
        return;
    }
    QWidget::mouseReleaseEvent(e);
}

void MiniBallWidget::showAnimated()
{
    setWindowOpacity(0.0);
    setScale(0.3);
    show();
    
    // 缩放动画
    m_scaleAnimation->setStartValue(0.3);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
    
    // 透明度动画
    m_opacityAnimation->setStartValue(0.0);
    m_opacityAnimation->setEndValue(1.0);
    m_opacityAnimation->start();
}

void MiniBallWidget::hideAnimated()
{
    // 缩放动画
    m_scaleAnimation->setStartValue(1.0);
    m_scaleAnimation->setEndValue(0.3);
    m_scaleAnimation->start();
    
    // 透明度动画
    m_opacityAnimation->setStartValue(1.0);
    m_opacityAnimation->setEndValue(0.0);
    m_opacityAnimation->start();
    
    // 动画结束后隐藏
    connect(m_opacityAnimation, &QPropertyAnimation::finished, this, [this] {
        hide();
        m_opacityAnimation->disconnect();
    });
}

