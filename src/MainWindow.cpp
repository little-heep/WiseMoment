#include "MainWindow.h"

#include "AppConfig.h"
#include "HotkeyManager_win.h"
#include "MiniBallWidget.h"
#include "PomodoroController.h"
#include "QuoteProvider.h"
#include "TodoModel.h"
#include "TodoCardDelegate.h"
#include "TodoDueDelegate.h"
#include "StatsStore.h"
#include "TrayManager.h"
#include "ui/BreathingRing.h"
#include "ui/HeaderWidget.h"
#include "ui/BottomNav.h"
#include "ui/AcrylicBlur_win.h"

#include <QAction>
#include <QCloseEvent>
#include <QDate>
#include <QDateTimeEdit>
#include <QDateEdit>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QFile>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QToolButton>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPalette>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    // 移除透明背景设置，让我们的宣纸背景色生效
    // setAttribute(Qt::WA_TranslucentBackground, true);

    m_pomodoro = new PomodoroController(this);
    m_ball = new MiniBallWidget();
    m_todos = new TodoModel(this);
    m_stats = new StatsStore(this);
    m_tray = new TrayManager(m_pomodoro, this, this);
    m_hotkeys = new HotkeyManagerWin(this);

    m_todos->load();

    buildUi();

    // 禁用AcrylicBlur效果，让我们的宣纸背景色生效
    // AcrylicBlurWin::enable(this);

    applyWindowPins();
    applyWindowOpacity();

    connect(m_pomodoro, &PomodoroController::tick, this, &MainWindow::onPomodoroTick);
    connect(m_pomodoro, &PomodoroController::stateChanged, this, &MainWindow::onPomodoroStateChanged);
    connect(m_pomodoro, &PomodoroController::finished, this, &MainWindow::onPomodoroFinished);
    connect(m_ball, &MiniBallWidget::clicked, this, &MainWindow::toggleMainVisibility);

    connect(m_tray, &TrayManager::requestShowMain, this, [this] { show(); raise(); activateWindow(); });
    connect(m_tray, &TrayManager::requestHideMain, this, [this] { hide(); });
    m_tray->show();

    m_hotkeys->registerDefaults();
    connect(m_hotkeys, &HotkeyManagerWin::toggleMainRequested, this, &MainWindow::toggleMainVisibility);
    connect(m_hotkeys, &HotkeyManagerWin::togglePomodoroRequested, this, &MainWindow::togglePomodoroStartPauseResume);

    m_pomodoro->setTotalMinutes(AppConfig::pomodoroMinutes());
    m_pomodoro->setWhiteNoiseEnabled(AppConfig::playWhiteNoise());
    m_pomodoro->setWhiteNoiseFile(AppConfig::whiteNoiseFile());

    updatePomodoroUi();
    updateStatsUi();
}

MainWindow::~MainWindow()
{
    if (m_ball)
        m_ball->hide();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    // Keep app in tray by default
    hide();
    e->ignore();
}

void MainWindow::buildUi()
{
    setWindowTitle("MomentWise");
    resize(760, 620);

    auto* central = new QFrame(this);
    central->setObjectName("WindowSurface");
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(0);

    m_header = new HeaderWidget(this);
    m_header->setQuote(QuoteProvider::quoteOfTheDay(AppConfig::uiLocale()));
    root->addWidget(m_header);

    m_stack = new QStackedWidget(this);
    root->addWidget(m_stack, 1);

    // 初始化卷轴舒展动画效果
    m_stackOpacityEffect = new QGraphicsOpacityEffect(m_stack);
    m_stack->setGraphicsEffect(m_stackOpacityEffect);
    m_pageTransitionAnimation = new QPropertyAnimation(m_stackOpacityEffect, "opacity", this);
    m_pageTransitionAnimation->setDuration(800); // 800ms动画时长
    m_pageTransitionAnimation->setEasingCurve(QEasingCurve::OutCubic); // 舒展缓动效果

    m_nav = new BottomNav(this);
    root->addWidget(m_nav);

    connect(m_header, &HeaderWidget::minimizeRequested, this, &MainWindow::showMinimized);
    connect(m_header, &HeaderWidget::closeRequested, this, [this] { hide(); });
    connect(m_header, &HeaderWidget::ballRequested, this, [this] {
        hide();
        if (m_ball) m_ball->showAnimated();
    });
    connect(m_header, &HeaderWidget::pinToggled, this, [this](bool on) {
        AppConfig::setAlwaysOnTop(on);
        m_header->setPinned(on);
        applyWindowPins();
    });
    connect(m_header, &HeaderWidget::opacityChanged, this, [this](int v) {
        AppConfig::setWindowOpacity(double(v) / 100.0);
        applyWindowOpacity();
    });
    connect(m_header, &HeaderWidget::languageChanged, this, [this](const QString& locale) {
        if (locale.isEmpty())
            return;
        AppConfig::setUiLocale(locale);
        applyLocale();
    });

    connect(m_nav, &BottomNav::indexChanged, this, [this](int idx) {
        const int currentIdx = m_stack->currentIndex();
        if (currentIdx != idx) {
            animatePageTransition(currentIdx, idx);
        }
        m_nav->setCurrentIndex(idx);
    });

    const auto makeSectionTitle = [this](const QString& eyebrow, const QString& title, const QString& subtitle) {
        auto* box = new QFrame(this);
        auto* layout = new QVBoxLayout(box);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);
        Q_UNUSED(subtitle);

        auto* eyebrowLabel = new QLabel(eyebrow, box);
        eyebrowLabel->setProperty("role", "eyebrow");
        layout->addWidget(eyebrowLabel);

        auto* titleLabel = new QLabel(title, box);
        titleLabel->setProperty("role", "sectionTitle");
        layout->addWidget(titleLabel);
        return box;
    };

    const auto makePanel = [this]() {
        auto* frame = new QFrame(this);
        frame->setObjectName("Card");
        return frame;
    };

    const QString lightCalendarStyle = QStringLiteral(R"(
        QCalendarWidget {
            background: rgba(255,251,242,252);
            border: 1px solid rgba(221,173,106,112);
            border-radius: 16px;
        }
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(255,245,223,255),
                stop:1 rgba(255,235,198,252));
            border-top-left-radius: 16px;
            border-top-right-radius: 16px;
            border-bottom: 1px solid rgba(221,173,106,120);
        }
        QCalendarWidget QToolButton {
            color: rgba(122,76,36,250);
            background: transparent;
            border: none;
            font-size: 13px;
            font-weight: 700;
            min-width: 30px;
            min-height: 28px;
            border-radius: 10px;
        }
        QCalendarWidget QToolButton:hover {
            background: rgba(255,255,255,158);
        }
        QCalendarWidget QMenu {
            background: rgba(255,250,236,252);
            color: rgba(80,53,32,246);
            border: 1px solid rgba(221,173,106,128);
            border-radius: 12px;
            padding: 6px;
        }
        QCalendarWidget QSpinBox {
            background: rgba(255,252,242,252);
            color: rgba(93,60,34,245);
            border: 1px solid rgba(221,173,106,136);
            border-radius: 10px;
            padding: 4px 8px;
            min-width: 80px;
        }
        QCalendarWidget QAbstractItemView {
            background: rgba(255,251,242,252);
            color: rgba(78,51,30,250);
            selection-background-color: rgba(240,182,94,138);
            selection-color: rgba(103,61,22,255);
            alternate-background-color: rgba(254,242,216,136);
            border: 1px solid rgba(221,173,106,112);
            border-bottom-left-radius: 16px;
            border-bottom-right-radius: 16px;
            outline: none;
            font-size: 12px;
        }
        QCalendarWidget QTableView {
            background: rgba(255,251,242,252);
            color: rgba(78,51,30,250);
            alternate-background-color: rgba(254,242,216,136);
            selection-background-color: rgba(240,182,94,138);
            selection-color: rgba(103,61,22,255);
            gridline-color: rgba(221,173,106,80);
        }
        QCalendarWidget QWidget#qt_calendar_calendarview {
            background: rgba(255,251,242,252);
            color: rgba(78,51,30,250);
            alternate-background-color: rgba(254,242,216,136);
            selection-background-color: rgba(240,182,94,138);
            selection-color: rgba(103,61,22,255);
            outline: none;
        }
    )");

    const auto applyLightCalendar = [&lightCalendarStyle](QCalendarWidget* calendar) {
        if (!calendar)
            return;
        calendar->setStyleSheet(lightCalendarStyle);
        QPalette pal = calendar->palette();
        pal.setColor(QPalette::Window, QColor(255, 251, 242));
        pal.setColor(QPalette::Base, QColor(255, 251, 242));
        pal.setColor(QPalette::Text, QColor(78, 51, 30));
        pal.setColor(QPalette::ButtonText, QColor(122, 76, 36));
        pal.setColor(QPalette::Highlight, QColor(240, 182, 94, 138));
        pal.setColor(QPalette::HighlightedText, QColor(103, 61, 22));
        calendar->setPalette(pal);
        calendar->setAutoFillBackground(true);

        const auto polishCalendarView = [calendar]() {
            auto* view = calendar->findChild<QTableView*>(QStringLiteral("qt_calendar_calendarview"));
            if (!view)
                return;
            view->setStyleSheet(QStringLiteral(
                "QTableView#qt_calendar_calendarview {"
                " background: rgba(255,251,242,252);"
                " color: rgba(78,51,30,250);"
                " gridline-color: rgba(221,173,106,80);"
                " selection-background-color: rgba(240,182,94,138);"
                " selection-color: rgba(103,61,22,255);"
                " alternate-background-color: rgba(254,242,216,136);"
                " border: 0px;"
                " }"
                "QTableView#qt_calendar_calendarview::item {"
                " background: rgba(255,251,242,252);"
                " color: rgba(78,51,30,250);"
                " }"));
            QPalette vp = view->palette();
            vp.setColor(QPalette::Base, QColor(255, 251, 242));
            vp.setColor(QPalette::Window, QColor(255, 251, 242));
            vp.setColor(QPalette::Text, QColor(78, 51, 30));
            vp.setColor(QPalette::Highlight, QColor(240, 182, 94, 138));
            vp.setColor(QPalette::HighlightedText, QColor(103, 61, 22));
            view->setPalette(vp);
            view->viewport()->setAutoFillBackground(true);
            view->setAlternatingRowColors(false);
        };
        polishCalendarView();
        QTimer::singleShot(0, calendar, polishCalendarView);
        QObject::connect(calendar, &QCalendarWidget::currentPageChanged, calendar, polishCalendarView);
    };

    // --- Page 0: Pomodoro ---
    auto* pomo = new QWidget(this);
    auto* pomoLayout = new QVBoxLayout(pomo);
    pomoLayout->setContentsMargins(24, 18, 24, 18);
    pomoLayout->setSpacing(16);

    pomoLayout->addWidget(makeSectionTitle(tr("焚香入定"), tr("专注时刻"),
                                           tr("以更安静的视觉层次组织时间、声音与状态切换。")));

    auto* focusPanel = makePanel();
    auto* focusLayout = new QVBoxLayout(focusPanel);
    focusLayout->setContentsMargins(24, 24, 24, 24);
    focusLayout->setSpacing(18);

    auto* ring = new BreathingRing(this);
    ring->setCenterText("25:00");
    ring->setProgress(0.0);
    focusLayout->addWidget(ring, 0, Qt::AlignCenter);

    auto* row1 = new QHBoxLayout();
    row1->setSpacing(12);
    m_pomoMinutes = new QSpinBox(this);
    m_pomoMinutes->setRange(1, 180);
    m_pomoMinutes->setValue(AppConfig::pomodoroMinutes());
    m_pomoMinutes->setMinimumHeight(44);

    auto* timeLabel = new QLabel(tr("专注时长（分钟）"), this);
    timeLabel->setProperty("role", "muted");
    timeLabel->setAlignment(Qt::AlignVCenter);

    row1->addWidget(timeLabel);
    row1->addWidget(m_pomoMinutes);
    row1->addStretch(1);

    m_btnStart = new QPushButton(tr("开始专注"), this);
    m_btnStart->setMinimumHeight(44);
    m_btnStart->setMinimumWidth(118);
    m_btnStart->setObjectName("Primary");

    m_btnPause = new QPushButton(tr("暂停"), this);
    m_btnPause->setMinimumHeight(44);
    m_btnPause->setMinimumWidth(80);

    m_btnResume = new QPushButton(tr("继续"), this);
    m_btnResume->setMinimumHeight(44);
    m_btnResume->setMinimumWidth(80);
    m_btnResume->setObjectName("accentGhost");
    m_btnStop = new QPushButton(tr("停止"), this);
    m_btnStop->setMinimumHeight(44);
    m_btnStop->setMinimumWidth(80);
    m_btnStop->setObjectName("accentGhost");
    row1->addWidget(m_btnStart);
    row1->addWidget(m_btnPause);
    row1->addWidget(m_btnResume);
    row1->addWidget(m_btnStop);
    focusLayout->addLayout(row1);

    auto* soundPanel = makePanel();
    auto* soundLayout = new QVBoxLayout(soundPanel);
    soundLayout->setContentsMargins(22, 20, 22, 20);
    soundLayout->setSpacing(12);

    auto* soundHeading = new QLabel(tr("伴读与氛围"), soundPanel);
    soundHeading->setProperty("role", "eyebrow");
    soundLayout->addWidget(soundHeading);

    auto* row2 = new QHBoxLayout();
    row2->setSpacing(16);
    m_chkWhiteNoise = new QCheckBox(tr("白噪音"), this);
    m_chkWhiteNoise->setChecked(AppConfig::playWhiteNoise());

    m_noiseCombo = new QComboBox(this);
    m_noiseCombo->setMinimumWidth(200);
    m_noiseCombo->setEditable(false);

    m_btnPickNoise = new QPushButton(tr("添加..."), this);
    m_btnPickNoise->setObjectName("accentGhost");
    m_btnRemoveNoise = new QPushButton(tr("删除"), this);
    m_btnRemoveNoise->setObjectName("accentGhost");
    auto* vol = new QSlider(Qt::Horizontal, this);
    m_volSlider = vol;
    vol->setRange(0, 100);
    vol->setValue(50);
    vol->setMinimumWidth(120);

    row2->addWidget(m_chkWhiteNoise);
    auto* audioLabel = new QLabel(tr("音频:"), this);
    audioLabel->setProperty("role", "muted");
    row2->addWidget(audioLabel);
    row2->addWidget(m_noiseCombo, 1);
    row2->addWidget(m_btnPickNoise);
    row2->addWidget(m_btnRemoveNoise);
    row2->addWidget(vol, 1);
    soundLayout->addLayout(row2);

    m_chkShrink = new QCheckBox(tr("运行时自动收缩为小球"), this);
    m_chkShrink->setChecked(true);
    soundLayout->addWidget(m_chkShrink);

    pomoLayout->addWidget(focusPanel);
    pomoLayout->addWidget(soundPanel);
    pomoLayout->addStretch(1);

    m_stack->addWidget(pomo);

    connect(m_pomoMinutes, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) {
        AppConfig::setPomodoroMinutes(v);
        m_pomodoro->setTotalMinutes(v);
    });
    connect(m_btnStart, &QPushButton::clicked, m_pomodoro, &PomodoroController::start);
    connect(m_btnPause, &QPushButton::clicked, m_pomodoro, &PomodoroController::pause);
    connect(m_btnResume, &QPushButton::clicked, m_pomodoro, &PomodoroController::resume);
    connect(m_btnStop, &QPushButton::clicked, m_pomodoro, &PomodoroController::stop);
    
    connect(m_chkWhiteNoise, &QCheckBox::toggled, this, [this](bool on) {
        AppConfig::setPlayWhiteNoise(on);
        m_pomodoro->setWhiteNoiseEnabled(on);
    });

    auto refreshNoiseList = [this] {
        m_noiseCombo->clear();
        m_noiseCombo->addItem(tr("无"), "");

        const QDir dir(AppConfig::audioDir());
        const auto files = dir.entryInfoList(QStringList() << "*.mp3" << "*.wav" << "*.m4a" << "*.aac", QDir::Files, QDir::Time);
        for (const auto& fi : files) {
            m_noiseCombo->addItem(fi.fileName(), fi.absoluteFilePath());
        }

        const auto cur = AppConfig::whiteNoiseFile();
        if (cur.isEmpty()) {
            m_noiseCombo->setCurrentIndex(0);
        } else {
            const int idx = m_noiseCombo->findData(cur);
            if (idx >= 0) {
                m_noiseCombo->setCurrentIndex(idx);
            } else {
                m_noiseCombo->setCurrentIndex(0);
            }
        }
    };

    refreshNoiseList();

    connect(m_btnPickNoise, &QPushButton::clicked, this, [this, refreshNoiseList] {
        const auto file = QFileDialog::getOpenFileName(this, tr("Choose audio"), QString(),
                                                      tr("Audio files (*.mp3 *.wav *.m4a *.aac);;All files (*.*)"));
        if (file.isEmpty())
            return;

        const QDir audioDir(AppConfig::audioDir());
        if (!audioDir.exists()) {
            audioDir.mkpath(".");
        }

        const QFileInfo fileInfo(file);
        const QString destFile = audioDir.filePath(fileInfo.fileName());

        if (!QFile::exists(destFile)) {
            QFile::copy(file, destFile);
        }

        AppConfig::setWhiteNoiseFile(destFile);
        m_pomodoro->setWhiteNoiseFile(destFile);
        refreshNoiseList();
    });

    connect(m_btnRemoveNoise, &QPushButton::clicked, this, [this, refreshNoiseList] {
        const int idx = m_noiseCombo->currentIndex();
        if (idx <= 0)
            return;
        const QString path = m_noiseCombo->itemData(idx).toString();
        if (path.isEmpty())
            return;
        const bool removed = QFile::remove(path);
        if (!removed && QFile::exists(path)) {
            QMessageBox::warning(this, tr("删除失败"), tr("当前音频正在被占用或无权限删除。"));
            return;
        }
        if (AppConfig::whiteNoiseFile() == path) {
            AppConfig::setWhiteNoiseFile(QString());
            m_pomodoro->setWhiteNoiseFile(QString());
        }
        refreshNoiseList();
    });
    
    connect(m_noiseCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int idx) {
        const auto path = m_noiseCombo->itemData(idx).toString();
        AppConfig::setWhiteNoiseFile(path);
        m_pomodoro->setWhiteNoiseFile(path);
    });

    connect(m_pomodoro, &PomodoroController::tick, this, [ring](int remaining, int total) {
        ring->setCenterText(QString("%1:%2")
                                .arg(remaining / 60, 2, 10, QChar('0'))
                                .arg(remaining % 60, 2, 10, QChar('0')));
        const double p = total > 0 ? double(total - remaining) / double(total) : 0.0;
        ring->setProgress(p);
    });

    // --- Page 1: Tasks (temporary table-based) ---
    auto* todo = new QWidget(this);
    auto* todoLayout = new QVBoxLayout(todo);
    todoLayout->setContentsMargins(24, 18, 24, 18);
    todoLayout->setSpacing(16);

    todoLayout->addWidget(makeSectionTitle(tr("执简记事"), tr("任务清单"),
                                           tr("把待办、期限与完成状态收进同一页，减少视觉噪音。")));

    auto* todoPanel = makePanel();
    auto* todoPanelLayout = new QVBoxLayout(todoPanel);
    todoPanelLayout->setContentsMargins(20, 20, 20, 20);
    todoPanelLayout->setSpacing(14);

    m_todoView = new QTableView(this);
    m_todoView->setModel(m_todos);
    auto* todoDelegate = new TodoCardDelegate(m_todoView);
    m_todoView->setItemDelegateForColumn(TodoModel::Title, todoDelegate);
    m_todoView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_todoView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_todoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_todoView->setMinimumHeight(340);
    m_todoView->setAlternatingRowColors(true);
    m_todoView->verticalHeader()->hide();
    m_todoView->setShowGrid(false);
    m_todoView->horizontalHeader()->hide();
    m_todoView->setColumnHidden(TodoModel::Due, true);
    m_todoView->setColumnHidden(TodoModel::Status, true);
    m_todoView->horizontalHeader()->setSectionResizeMode(TodoModel::Title, QHeaderView::Stretch);
    m_todoView->verticalHeader()->setDefaultSectionSize(92);
    m_todoView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_todoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_todoView->setMouseTracking(true);
    m_todoView->viewport()->setMouseTracking(true);
    m_todoView->viewport()->setAttribute(Qt::WA_Hover, true);
    m_todoView->setStyleSheet("QTableView { padding: 4px 2px 4px 2px; font-size: 11px; }");
    todoPanelLayout->addWidget(m_todoView, 1);

    auto* composerPanel = new QFrame(this);
    composerPanel->setObjectName("Card");
    auto* composerLayout = new QVBoxLayout(composerPanel);
    composerLayout->setContentsMargins(20, 18, 20, 18);
    composerLayout->setSpacing(14);

    auto* composerHint = new QLabel(tr("新任务"), composerPanel);
    composerHint->setProperty("role", "eyebrow");
    composerLayout->addWidget(composerHint);

    auto* titleEdit = new QLineEdit(this);
    titleEdit->setPlaceholderText(tr("新建待办事项..."));
    titleEdit->setMinimumHeight(48);
    composerLayout->addWidget(titleEdit);

    auto* todoRow = new QHBoxLayout();
    todoRow->setSpacing(12);
    m_dueEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(1), this);
    m_dueEdit->setCalendarPopup(true);
    m_dueEdit->setDisplayFormat("yyyy年MM月dd日  HH:mm");
    m_dueEdit->setMinimumHeight(48);
    auto* todoCalendar = new QCalendarWidget(this);
    todoCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    todoCalendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    todoCalendar->setFirstDayOfWeek(Qt::Monday);
    applyLightCalendar(todoCalendar);
    m_dueEdit->setCalendarWidget(todoCalendar);
    auto* addBtn = new QPushButton(tr("添加"), this);
    addBtn->setMinimumHeight(48);
    addBtn->setMinimumWidth(92);
    addBtn->setObjectName("Primary");

    auto* dueLabel = new QLabel(tr("期限"), this);
    dueLabel->setProperty("role", "muted");
    todoRow->addWidget(dueLabel);
    todoRow->addWidget(m_dueEdit);
    todoRow->addStretch(1);
    todoRow->addWidget(addBtn);
    composerLayout->addLayout(todoRow);

    todoLayout->addWidget(todoPanel, 1);
    todoLayout->addWidget(composerPanel);

    m_stack->addWidget(todo);

    connect(addBtn, &QPushButton::clicked, this, [this, titleEdit] {
        const auto title = titleEdit->text().trimmed();
        if (title.isEmpty())
            return;
        m_todos->add(title, m_dueEdit->dateTime());
        titleEdit->clear();
        updateStatsUi();
    });
    connect(todoDelegate, &TodoCardDelegate::deleteRequested, this, [this](int row) {
        if (row < 0 || row >= m_todos->items().size())
            return;
        m_todos->removeRowAt(row);
        updateStatsUi();
    });
    connect(todoDelegate, &TodoCardDelegate::toggleRequested, this, [this](int row) {
        if (row < 0 || row >= m_todos->items().size())
            return;
        const bool wasDone = m_todos->items().at(row).done;
        m_todos->toggleDone(row);
        const bool nowDone = m_todos->items().at(row).done;
        if (!wasDone && nowDone) {
            m_stats->addTodoCompleted(QDate::currentDate(), 1);
            updateStatsUi();
        }
    });

    // --- Page 2: Stats ---
    auto* stats = new QWidget(this);
    auto* statsLayout = new QVBoxLayout(stats);
    statsLayout->setContentsMargins(24, 18, 24, 18);
    statsLayout->setSpacing(16);

    statsLayout->addWidget(makeSectionTitle(tr("观象知时"), tr("专注统计"),
                                            tr("用更清晰的卡片层级查看区间表现、日均效率与完成密度。")));

    auto* rangeBox = makePanel();
    auto* rangeLayout = new QVBoxLayout(rangeBox);
    rangeLayout->setContentsMargins(22, 20, 22, 20);
    rangeLayout->setSpacing(14);
    auto* rangeTitle = new QLabel(tr("时间范围"), rangeBox);
    rangeTitle->setProperty("role", "eyebrow");
    rangeLayout->addWidget(rangeTitle);

    m_statsRangePeriod = new QLabel("-", rangeBox);
    m_statsRangePeriod->setStyleSheet("font-size: 17px; font-weight: 700; color: rgba(95,58,30,245);");
    rangeLayout->addWidget(m_statsRangePeriod);

    auto* rangeControls = new QHBoxLayout();
    rangeControls->setSpacing(14);
    m_statsFrom = new QDateEdit(QDate::currentDate().addDays(-6), this);
    m_statsFrom->setCalendarPopup(true);
    m_statsFrom->setMinimumHeight(44);
    m_statsTo = new QDateEdit(QDate::currentDate(), this);
    m_statsTo->setCalendarPopup(true);
    m_statsTo->setMinimumHeight(44);
    m_statsApply = new QPushButton(tr("应用"), this);
    m_statsApply->setMinimumHeight(44);
    m_statsApply->setMinimumWidth(92);
    m_statsApply->setObjectName("Primary");
    auto* statsFromCalendar = new QCalendarWidget(this);
    statsFromCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    statsFromCalendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    statsFromCalendar->setFirstDayOfWeek(Qt::Monday);
    applyLightCalendar(statsFromCalendar);
    m_statsFrom->setCalendarWidget(statsFromCalendar);
    auto* statsToCalendar = new QCalendarWidget(this);
    statsToCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    statsToCalendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    statsToCalendar->setFirstDayOfWeek(Qt::Monday);
    applyLightCalendar(statsToCalendar);
    m_statsTo->setCalendarWidget(statsToCalendar);
    rangeControls->addWidget(new QLabel(tr("开始日期"), this));
    rangeControls->addWidget(m_statsFrom);
    rangeControls->addWidget(new QLabel(tr("结束日期"), this));
    rangeControls->addWidget(m_statsTo);
    rangeControls->addWidget(m_statsApply);
    rangeControls->addStretch(1);
    rangeLayout->addLayout(rangeControls);
    statsLayout->addWidget(rangeBox);

    auto* metricsBox = makePanel();
    auto* metricsRoot = new QVBoxLayout(metricsBox);
    metricsRoot->setContentsMargins(22, 20, 22, 20);
    metricsRoot->setSpacing(14);
    auto* metricsTitle = new QLabel(tr("区间核心指标"), metricsBox);
    metricsTitle->setProperty("role", "eyebrow");
    metricsRoot->addWidget(metricsTitle);

    auto* metricsGrid = new QGridLayout();
    metricsGrid->setHorizontalSpacing(14);
    metricsGrid->setVerticalSpacing(14);
    const auto makeMetricCard = [metricsBox](const QString& title, QLabel** outValue) {
        auto* card = new QFrame(metricsBox);
        card->setObjectName("Card");
        card->setStyleSheet("QFrame#Card { border-radius: 18px; background: rgba(255,248,233,210); border: 1px solid rgba(170,128,85,64); }");
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 14, 16, 14);
        cardLayout->setSpacing(6);
        auto* titleLabel = new QLabel(title, card);
        titleLabel->setProperty("role", "muted");
        auto* valueLabel = new QLabel("-", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: rgba(133,64,36,248);");
        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(valueLabel);
        *outValue = valueLabel;
        return card;
    };
    metricsGrid->addWidget(makeMetricCard(tr("专注总时长"), &m_statsRangeFocus), 0, 0);
    metricsGrid->addWidget(makeMetricCard(tr("完成任务"), &m_statsRangeDone), 0, 1);
    metricsGrid->addWidget(makeMetricCard(tr("有效天数"), &m_statsRangeDays), 1, 0);
    metricsGrid->addWidget(makeMetricCard(tr("日均任务"), &m_statsDonePerDay), 1, 1);
    metricsRoot->addLayout(metricsGrid);
    statsLayout->addWidget(metricsBox);

    auto* insightBox = makePanel();
    auto* insightLayout = new QHBoxLayout(insightBox);
    insightLayout->setContentsMargins(22, 20, 22, 20);
    insightLayout->setSpacing(16);
    auto* insightTag = new QLabel(tr("效率洞察"), insightBox);
    insightTag->setProperty("role", "eyebrow");
    m_statsFocusPerDay = new QLabel("-", insightBox);
    m_statsFocusPerDay->setStyleSheet("font-size: 16px; font-weight: 700; color: rgba(95,58,30,245);");
    insightLayout->addWidget(insightTag);
    insightLayout->addWidget(m_statsFocusPerDay, 1);
    statsLayout->addWidget(insightBox);

    connect(m_statsApply, &QPushButton::clicked, this, &MainWindow::updateStatsUi);
    connect(m_statsFrom, &QDateEdit::dateChanged, this, &MainWindow::updateStatsUi);
    connect(m_statsTo, &QDateEdit::dateChanged, this, &MainWindow::updateStatsUi);

    statsLayout->addStretch(1);
    m_stack->addWidget(stats);

    setCentralWidget(central);
    m_stack->setCurrentIndex(0);
    m_nav->setCurrentIndex(0);
}

void MainWindow::onPomodoroTick(int remaining, int total)
{
    Q_UNUSED(total);
    m_ball->setText(fmtTime(remaining));
}

void MainWindow::onPomodoroStateChanged()
{
    updatePomodoroUi();

    const auto st = m_pomodoro->state();
    const bool running = (st == PomodoroController::State::Running);

    if (running && m_chkShrink->isChecked()) {
        hide();
        if (m_ball) m_ball->showAnimated();
    } else if (st == PomodoroController::State::Paused || st == PomodoroController::State::Finished || st == PomodoroController::State::Idle) {
        if (m_ball) m_ball->hideAnimated();
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::onPomodoroFinished()
{
    m_stats->addFocusSeconds(QDate::currentDate(), m_pomodoro->totalSeconds());
    updateStatsUi();
}

void MainWindow::onAddTodo()
{
    // Not used (wired inline)
}

void MainWindow::onRemoveTodo()
{
    const auto idx = m_todoView->currentIndex();
    if (!idx.isValid())
        return;
    m_todos->removeRowAt(idx.row());
    updateStatsUi();
}

void MainWindow::onToggleTodoDone()
{
    const auto idx = m_todoView->currentIndex();
    if (!idx.isValid())
        return;

    const bool wasDone = m_todos->items().at(idx.row()).done;
    m_todos->toggleDone(idx.row());
    const bool nowDone = m_todos->items().at(idx.row()).done;

    if (!wasDone && nowDone) {
        m_stats->addTodoCompleted(QDate::currentDate(), 1);
        updateStatsUi();
    }
}

void MainWindow::applyWindowPins()
{
    const bool onTop = AppConfig::alwaysOnTop();
    setWindowFlag(Qt::WindowStaysOnTopHint, onTop);
    show();
}

void MainWindow::applyWindowOpacity()
{
    setWindowOpacity(AppConfig::windowOpacity());
}

void MainWindow::applyLocale()
{
    QMessageBox::information(this, tr("Restart required"),
                             tr("Language will fully apply after restart.\n\n"
                                "If you want full translations, build .qm files using lrelease."));
}

void MainWindow::toggleMainVisibility()
{
    if (isVisible()) {
        hide();
        if (m_ball) m_ball->showAnimated();
        return;
    }
    if (m_ball) m_ball->hideAnimated();
    show();
    raise();
    activateWindow();
}

void MainWindow::togglePomodoroStartPauseResume()
{
    const auto st = m_pomodoro->state();
    if (st == PomodoroController::State::Running) {
        m_pomodoro->pause();
    } else if (st == PomodoroController::State::Paused) {
        m_pomodoro->resume();
    } else {
        m_pomodoro->start();
    }
}

void MainWindow::updatePomodoroUi()
{
    const auto st = m_pomodoro->state();
    const bool running = (st == PomodoroController::State::Running);
    const bool paused = (st == PomodoroController::State::Paused);

    m_btnStart->setVisible(!running && !paused);
    m_btnPause->setVisible(running);
    m_btnResume->setVisible(paused);
    m_btnStop->setVisible(running || paused);
    m_pomoMinutes->setEnabled(!running && !paused);
}

void MainWindow::updateStatsUi()
{
    const auto from = m_statsFrom ? m_statsFrom->date() : QDate::currentDate().addDays(-6);
    const auto to = m_statsTo ? m_statsTo->date() : QDate::currentDate();
    const QDate realFrom = from <= to ? from : to;
    const QDate realTo = from <= to ? to : from;
    const auto range = m_stats->statsBetween(realFrom, realTo);
    int focusSum = 0;
    int doneSum = 0;
    int activeDays = 0;
    for (auto it = range.begin(); it != range.end(); ++it) {
        focusSum += it.value().focusSeconds;
        doneSum += it.value().todoCompleted;
        if (it.value().focusSeconds > 0 || it.value().todoCompleted > 0)
            ++activeDays;
    }
    const int totalDays = qMax(1, static_cast<int>(realFrom.daysTo(realTo) + 1));
    const double focusPerDay = static_cast<double>(focusSum) / static_cast<double>(totalDays);
    const double donePerDay = static_cast<double>(doneSum) / static_cast<double>(totalDays);

    if (m_statsRangePeriod) {
        m_statsRangePeriod->setText(QString("%1 ~ %2")
            .arg(realFrom.toString("yyyy.MM.dd"))
            .arg(realTo.toString("yyyy.MM.dd")));
    }
    if (m_statsRangeFocus) m_statsRangeFocus->setText(fmtTime(focusSum));
    if (m_statsRangeDone) m_statsRangeDone->setText(QString::number(doneSum));
    if (m_statsRangeDays) m_statsRangeDays->setText(QString("%1 / %2").arg(activeDays).arg(totalDays));
    if (m_statsDonePerDay) m_statsDonePerDay->setText(QString::number(donePerDay, 'f', 1));
    if (m_statsFocusPerDay) {
        m_statsFocusPerDay->setText(tr("日均专注 %1 ，平均每天完成 %2 项任务")
            .arg(fmtTime(static_cast<int>(focusPerDay)))
            .arg(QString::number(donePerDay, 'f', 1)));
    }
}

QString MainWindow::fmtTime(int seconds)
{
    seconds = qMax(0, seconds);
    const int h = seconds / 3600;
    const int m = (seconds % 3600) / 60;
    const int s = seconds % 60;
    if (h > 0)
        return QString("%1h %2m %3s").arg(h).arg(m).arg(s);
    if (m > 0)
        return QString("%1m %2s").arg(m).arg(s);
    return QString("%1s").arg(s);
}

void MainWindow::animatePageTransition(int fromIndex, int toIndex)
{
    // 卷轴舒展动画：淡出当前页面 -> 切换页面 -> 淡入新页面
    m_pageTransitionAnimation->stop();
    
    // 淡出动画
    m_pageTransitionAnimation->setStartValue(1.0);
    m_pageTransitionAnimation->setEndValue(0.0);
    
    connect(m_pageTransitionAnimation, &QPropertyAnimation::finished, this, [this, toIndex]() {
        // 切换页面
        m_stack->setCurrentIndex(toIndex);
        
        // 淡入动画
        m_pageTransitionAnimation->disconnect();
        m_pageTransitionAnimation->setStartValue(0.0);
        m_pageTransitionAnimation->setEndValue(1.0);
        m_pageTransitionAnimation->start();
    });
    
    m_pageTransitionAnimation->start();
}

