#include "BreathingRing.h"

#include <QDateTime>
#include <QPainter>
#include <QtMath>
#include <QEasingCurve>
#include <QTimer>

BreathingRing::BreathingRing(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(260, 260);
    
    // 使用定时器控制动画，避免每帧计算
    m_breathingTimer = new QTimer(this);
    m_breathingTimer->setInterval(50); // 20fps，更流畅
    connect(m_breathingTimer, &QTimer::timeout, this, [this] {
        m_breathingPhase += 0.15; // 调整呼吸速度
        if (m_breathingPhase > 2 * M_PI) {
            m_breathingPhase -= 2 * M_PI;
        }
        update();
    });
    m_breathingTimer->start();
}

void BreathingRing::setProgress(double p01)
{
    m_p = qBound(0.0, p01, 1.0);
    update();
}

void BreathingRing::setCenterText(const QString& text)
{
    m_text = text;
    update();
}

void BreathingRing::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF baseRect = QRectF(rect()).adjusted(16, 16, -16, -16);
    
    // 使用预计算的呼吸相位，避免实时计算
    const qreal pulse = 0.5 + 0.5 * qSin(m_breathingPhase);
    const qreal scale = 1.0 + 0.04 * pulse; // 更小的幅度，更流畅
    
    const QRectF r = QRectF(
        baseRect.center().x() - baseRect.width() * scale / 2,
        baseRect.center().y() - baseRect.height() * scale / 2,
        baseRect.width() * scale,
        baseRect.height() * scale
    );
    
    // 固定线条粗细
    const qreal w = 16.0;

    QRadialGradient halo(r.center(), r.width() * 0.68);
    halo.setColorAt(0.0, QColor(255, 245, 226, 46));
    halo.setColorAt(0.7, QColor(217, 188, 148, 24));
    halo.setColorAt(1.0, QColor(255, 245, 226, 0));
    p.setBrush(halo);
    p.setPen(Qt::NoPen);
    p.drawEllipse(r.adjusted(-18, -18, 18, 18));

    p.setPen(QPen(QColor(183, 157, 125, 72), w, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(r, 90 * 16, -360 * 16);

    QConicalGradient progress(r.center(), 90);
    progress.setColorAt(0.0, QColor(206, 123, 78, 235));
    progress.setColorAt(0.5, QColor(170, 55, 42, 225));
    progress.setColorAt(1.0, QColor(132, 40, 34, 235));
    p.setPen(QPen(QBrush(progress), w, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(r, 90 * 16, int(-360.0 * 16.0 * m_p));

    p.setPen(QColor(58, 43, 30, 232));
    auto f = font();
    f.setPointSize(30);
    f.setBold(true);
    p.setFont(f);
    p.drawText(rect().adjusted(0, -10, 0, 0), Qt::AlignCenter, m_text);

    auto subFont = font();
    subFont.setPointSize(11);
    p.setFont(subFont);
    p.setPen(QColor(116, 91, 68, 190));
    p.drawText(rect().adjusted(0, 92, 0, 0), Qt::AlignCenter, QStringLiteral("静心一刻"));
}

