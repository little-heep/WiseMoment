#include "HeaderWidget.h"

#include "../AppConfig.h"

#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

static QToolButton* makeIconButton(const QString& text, QWidget* parent)
{
    auto* b = new QToolButton(parent);
    b->setText(text);
    b->setCursor(Qt::PointingHandCursor);
    b->setToolButtonStyle(Qt::ToolButtonTextOnly);
    b->setFixedSize(36, 36);
    return b;
}

HeaderWidget::HeaderWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("HeaderWidget");
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 10);
    root->setSpacing(10);

    auto* top = new QHBoxLayout();
    top->setSpacing(10);

    m_title = new QLabel(QStringLiteral("MomentWise"), this);
    m_title->setProperty("role", "sectionTitle");
    top->addWidget(m_title);
    top->addStretch(1);

    m_pin = makeIconButton(QStringLiteral("⌂"), this);
    m_pin->setToolTip(tr("置顶窗口"));
    top->addWidget(m_pin);

    m_ball = makeIconButton(QStringLiteral("◉"), this);
    m_ball->setToolTip(tr("切换小窗模式"));
    top->addWidget(m_ball);

    m_settings = makeIconButton(QStringLiteral("☰"), this);
    m_settings->setToolTip(tr("更多设置"));
    top->addWidget(m_settings);

    m_min = makeIconButton(QStringLiteral("—"), this);
    m_min->setToolTip(tr("最小化窗口"));
    top->addWidget(m_min);

    m_close = makeIconButton(QStringLiteral("×"), this);
    m_close->setToolTip(tr("关闭到托盘"));
    top->addWidget(m_close);

    root->addLayout(top);

    m_quote = new QLabel(this);
    m_quote->setAlignment(Qt::AlignCenter);
    m_quote->setWordWrap(true);
    m_quote->setProperty("role", "muted");
    root->addWidget(m_quote);

    setStyleSheet(R"(
        QWidget#HeaderWidget {
            background: transparent;
        }
        QWidget#HeaderWidget QToolButton {
            font-size: 16px;
            font-weight: 600;
            border-radius: 18px;
            color: rgba(86, 63, 44, 220);
            background: rgba(255, 249, 240, 180);
            border: 1px solid rgba(152, 124, 95, 58);
        }
        QWidget#HeaderWidget QToolButton:hover {
            background: rgba(252, 242, 230, 230);
            border-color: rgba(170, 55, 42, 90);
        }
        QWidget#HeaderWidget QToolButton:pressed {
            background: rgba(170, 55, 42, 40);
        }
        QWidget#HeaderWidget QToolButton[text="◉"] {
            color: rgba(170, 55, 42, 230);
        }
        QWidget#HeaderWidget QLabel[role="muted"] {
            font-size: 14px;
            font-style: italic;
            color: rgba(110, 86, 65, 220);
            padding: 0 10px 6px 10px;
        }
    )");

    buildMenu();
    m_settings->setMenu(m_menu);
    m_settings->setPopupMode(QToolButton::InstantPopup);

    connect(m_pin, &QToolButton::clicked, this, [this] {
        emit pinToggled(!AppConfig::alwaysOnTop());
    });
    connect(m_ball, &QToolButton::clicked, this, &HeaderWidget::ballRequested);
    connect(m_min, &QToolButton::clicked, this, &HeaderWidget::minimizeRequested);
    connect(m_close, &QToolButton::clicked, this, &HeaderWidget::closeRequested);

    setPinned(AppConfig::alwaysOnTop());
}

void HeaderWidget::setPinned(bool on)
{
    m_pin->setText(on ? QStringLiteral("⬆") : QStringLiteral("⌂"));
    m_pin->setToolTip(on ? tr("已置顶窗口") : tr("置顶窗口"));
}

void HeaderWidget::setQuote(const QString& text)
{
    m_quote->setText(text);
}

void HeaderWidget::buildMenu()
{
    m_menu = new QMenu(this);

    // Opacity
    auto* opacityRow = new QWidget(this);
    auto* opacityLayout = new QHBoxLayout(opacityRow);
    opacityLayout->setContentsMargins(10, 8, 10, 8);
    opacityLayout->setSpacing(10);
    auto* opacityLabel = new QLabel(tr("窗影"), opacityRow);
    opacityLabel->setProperty("role", "eyebrow");
    opacityLayout->addWidget(opacityLabel);
    m_opacity = new QSlider(Qt::Horizontal, opacityRow);
    m_opacity->setRange(20, 100);
    m_opacity->setValue(int(AppConfig::windowOpacity() * 100));
    m_opacity->setFixedWidth(180);
    opacityLayout->addWidget(m_opacity, 1);

    auto* opacityAct = new QWidgetAction(this);
    opacityAct->setDefaultWidget(opacityRow);
    m_menu->addAction(opacityAct);

    // Language
    auto* langRow = new QWidget(this);
    auto* langLayout = new QHBoxLayout(langRow);
    langLayout->setContentsMargins(10, 8, 10, 8);
    langLayout->setSpacing(10);
    auto* langLabel = new QLabel(tr("文字"), langRow);
    langLabel->setProperty("role", "eyebrow");
    langLayout->addWidget(langLabel);
    m_lang = new QComboBox(langRow);
    m_lang->addItem(QStringLiteral("中文"), QStringLiteral("zh_CN"));
    m_lang->addItem(QStringLiteral("English"), QStringLiteral("en_US"));
    const auto cur = AppConfig::uiLocale();
    const int idx = m_lang->findData(cur);
    if (idx >= 0) m_lang->setCurrentIndex(idx);
    langLayout->addWidget(m_lang);

    auto* langAct = new QWidgetAction(this);
    langAct->setDefaultWidget(langRow);
    m_menu->addAction(langAct);

    m_menu->addSeparator();
    m_menu->addAction(tr("快捷键: Ctrl+Alt+M / Ctrl+Alt+P"))->setEnabled(false);

    connect(m_opacity, &QSlider::valueChanged, this, &HeaderWidget::opacityChanged);
    connect(m_lang, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) {
        emit languageChanged(m_lang->currentData().toString());
    });
}

