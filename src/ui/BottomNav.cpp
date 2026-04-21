#include "BottomNav.h"

#include <QHBoxLayout>
#include <QToolButton>

static QToolButton* mk(const QString& txt, QWidget* parent)
{
    auto* b = new QToolButton(parent);
    b->setText(txt);
    b->setCheckable(true);
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    b->setMinimumHeight(52);
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return b;
}

BottomNav::BottomNav(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("BottomNav");
    auto* l = new QHBoxLayout(this);
    l->setContentsMargins(16, 12, 16, 16);
    l->setSpacing(12);

    m_btnPomo = mk(QStringLiteral("焚香  ") + tr("专注"), this);
    m_btnTask = mk(QStringLiteral("札记  ") + tr("任务"), this);
    m_btnStat = mk(QStringLiteral("观象  ") + tr("统计"), this);

    l->addWidget(m_btnPomo);
    l->addWidget(m_btnTask);
    l->addWidget(m_btnStat);

    connect(m_btnPomo, &QToolButton::clicked, this, [this] { emit indexChanged(0); });
    connect(m_btnTask, &QToolButton::clicked, this, [this] { emit indexChanged(1); });
    connect(m_btnStat, &QToolButton::clicked, this, [this] { emit indexChanged(2); });

    updateStyles();

    setStyleSheet(R"(
        QWidget#BottomNav {
            background: transparent;
        }
        QWidget#BottomNav QToolButton {
            font-size: 15px;
            font-weight: 600;
            padding: 12px;
            border-radius: 18px;
        }
    )");
}

void BottomNav::setCurrentIndex(int idx)
{
    m_idx = idx;
    updateStyles();
}

void BottomNav::updateStyles()
{
    const auto sel = "QToolButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(170,55,42,230),stop:1 rgba(132,40,34,215));"
                     "color:rgba(251,245,235,255);border:1px solid rgba(132,40,34,140);font-weight:700;}";
    const auto nor = "QToolButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(255,250,244,205),stop:1 rgba(237,227,211,195));"
                     "color:rgba(58,43,30,220);border:1px solid rgba(156,126,96,60);}"
                     "QToolButton:hover{background:rgba(247,238,225,235);border-color:rgba(170,55,42,85);}";
    m_btnPomo->setStyleSheet(m_idx == 0 ? sel : nor);
    m_btnTask->setStyleSheet(m_idx == 1 ? sel : nor);
    m_btnStat->setStyleSheet(m_idx == 2 ? sel : nor);
}

