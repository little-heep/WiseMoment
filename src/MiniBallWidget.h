#pragma once

#include <QWidget>

class QPropertyAnimation;

class MiniBallWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
public:
    explicit MiniBallWidget(QWidget* parent = nullptr);

    void setText(const QString& text);
    void showAnimated();
    void hideAnimated();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    qreal scale() const { return m_scale; }
    void setScale(qreal scale) { m_scale = scale; update(); }
    
    QString m_text;
    bool m_dragging{false};
    QPoint m_dragOffset;
    qreal m_scale{1.0};
    QPropertyAnimation* m_scaleAnimation{nullptr};
    QPropertyAnimation* m_opacityAnimation{nullptr};
};

