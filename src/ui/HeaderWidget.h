#pragma once

#include <QWidget>

class QLabel;
class QToolButton;
class QMenu;
class QSlider;
class QComboBox;

class HeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderWidget(QWidget* parent = nullptr);

    void setPinned(bool on);
    void setQuote(const QString& text);

signals:
    void pinToggled(bool on);
    void minimizeRequested();
    void closeRequested();
    void ballRequested();

    void opacityChanged(int percent);      // 20..100
    void languageChanged(const QString& locale); // zh_CN/en_US

private:
    void buildMenu();

    QLabel* m_title{nullptr};
    QLabel* m_quote{nullptr};
    QToolButton* m_pin{nullptr};
    QToolButton* m_ball{nullptr};
    QToolButton* m_settings{nullptr};
    QToolButton* m_min{nullptr};
    QToolButton* m_close{nullptr};

    QMenu* m_menu{nullptr};
    QSlider* m_opacity{nullptr};
    QComboBox* m_lang{nullptr};
};

