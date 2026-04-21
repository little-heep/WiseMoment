#pragma once

#include <QWidget>

class QTimer;

class BreathingRing : public QWidget
{
    Q_OBJECT
public:
    explicit BreathingRing(QWidget* parent = nullptr);

    void setProgress(double p01); // 0..1
    void setCenterText(const QString& text);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    double m_p{0.0};
    QString m_text;
    QTimer* m_breathingTimer{nullptr};
    qreal m_breathingPhase{0.0};
};

