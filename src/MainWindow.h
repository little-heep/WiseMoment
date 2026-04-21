#pragma once

#include <QMainWindow>

class QLabel;
class QStackedWidget;
class QTableView;
class QSpinBox;
class QPushButton;
class QCheckBox;
class QSlider;
class QComboBox;
class QDateTimeEdit;
class QDateEdit;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

class AudioRecorder;
class PomodoroController;
class MiniBallWidget;
class TrayManager;
class TodoModel;
class StatsStore;
class HotkeyManagerWin;
class HeaderWidget;
class BottomNav;
class CustomStatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* e) override;

private slots:
    void onPomodoroTick(int remaining, int total);
    void onPomodoroStateChanged();
    void onPomodoroFinished();

    void onAddTodo();
    void onRemoveTodo();
    void onToggleTodoDone();

    void applyWindowPins();
    void applyWindowOpacity();
    void applyLocale();

    void toggleMainVisibility();
    void togglePomodoroStartPauseResume();

private:
    void buildUi();
    void updatePomodoroUi();
    void updateStatsUi();
    void animatePageTransition(int fromIndex, int toIndex);
    static QString fmtTime(int seconds);

    PomodoroController* m_pomodoro{nullptr};
    MiniBallWidget* m_ball{nullptr};
    TrayManager* m_tray{nullptr};
    TodoModel* m_todos{nullptr};
    StatsStore* m_stats{nullptr};
    HotkeyManagerWin* m_hotkeys{nullptr};

    HeaderWidget* m_header{nullptr};
    QStackedWidget* m_stack{nullptr};
    BottomNav* m_nav{nullptr};

    // Pomodoro tab
    QSpinBox* m_pomoMinutes{nullptr};
    QPushButton* m_btnStart{nullptr};
    QPushButton* m_btnPause{nullptr};
    QPushButton* m_btnResume{nullptr};
    QPushButton* m_btnStop{nullptr};
    QCheckBox* m_chkShrink{nullptr};
    QCheckBox* m_chkWhiteNoise{nullptr};
    QComboBox* m_noiseCombo{nullptr};
    QPushButton* m_btnPickNoise{nullptr};
    QPushButton* m_btnRemoveNoise{nullptr};
    QSlider* m_volSlider{nullptr};

    // Task page (temporary keeps table)
    QTableView* m_todoView{nullptr};
    QDateTimeEdit* m_dueEdit{nullptr};

    // Stats tab
    QDateEdit* m_statsFrom{nullptr};
    QDateEdit* m_statsTo{nullptr};
    QPushButton* m_statsApply{nullptr};
    QLabel* m_statsRangePeriod{nullptr};
    QLabel* m_statsRangeFocus{nullptr};
    QLabel* m_statsRangeDone{nullptr};
    QLabel* m_statsRangeDays{nullptr};
    QLabel* m_statsFocusPerDay{nullptr};
    QLabel* m_statsDonePerDay{nullptr};

    // Settings tab
    QCheckBox* m_chkTop{nullptr};
    QSlider* m_opacity{nullptr};
    QComboBox* m_locale{nullptr};

    // Animation effects
    QPropertyAnimation* m_pageTransitionAnimation{nullptr};
    QGraphicsOpacityEffect* m_stackOpacityEffect{nullptr};
};

