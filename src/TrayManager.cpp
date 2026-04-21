#include "TrayManager.h"

#include "PomodoroController.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QStyle>

TrayManager::TrayManager(PomodoroController* pomodoro, QWidget* mainWindow, QObject* parent)
    : QObject(parent),
      m_pomodoro(pomodoro),
      m_mainWindow(mainWindow)
{
    m_tray.setToolTip("MomentWise");
    m_tray.setIcon(qApp->style()->standardIcon(QStyle::SP_ComputerIcon));
    m_menu = new QMenu();
    m_tray.setContextMenu(m_menu);

    connect(&m_tray, &QSystemTrayIcon::activated, this, &TrayManager::onActivated);
    connect(m_pomodoro, &PomodoroController::stateChanged, this, &TrayManager::rebuildMenu);
    connect(m_pomodoro, &PomodoroController::finished, this, &TrayManager::onPomodoroFinished);

    rebuildMenu();
}

void TrayManager::show()
{
    m_tray.show();
}

void TrayManager::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::Trigger)
        return;
    if (m_mainWindow && m_mainWindow->isVisible())
        emit requestHideMain();
    else
        emit requestShowMain();
}

void TrayManager::onPomodoroFinished()
{
    m_tray.showMessage(tr("Pomodoro finished"), tr("Time is up. Take a break!"), QSystemTrayIcon::Information, 5000);
}

void TrayManager::rebuildMenu()
{
    m_menu->clear();

    auto* showAct = m_menu->addAction(tr("Show"));
    connect(showAct, &QAction::triggered, this, &TrayManager::requestShowMain);

    auto* hideAct = m_menu->addAction(tr("Hide"));
    connect(hideAct, &QAction::triggered, this, &TrayManager::requestHideMain);

    m_menu->addSeparator();

    if (m_pomodoro->state() == PomodoroController::State::Running) {
        auto* pauseAct = m_menu->addAction(tr("Pause"));
        connect(pauseAct, &QAction::triggered, m_pomodoro, &PomodoroController::pause);
    } else if (m_pomodoro->state() == PomodoroController::State::Paused) {
        auto* resumeAct = m_menu->addAction(tr("Resume"));
        connect(resumeAct, &QAction::triggered, m_pomodoro, &PomodoroController::resume);
    } else {
        auto* startAct = m_menu->addAction(tr("Start"));
        connect(startAct, &QAction::triggered, m_pomodoro, &PomodoroController::start);
    }

    auto* stopAct = m_menu->addAction(tr("Stop"));
    connect(stopAct, &QAction::triggered, m_pomodoro, &PomodoroController::stop);

    m_menu->addSeparator();

    auto* quitAct = m_menu->addAction(tr("Quit"));
    connect(quitAct, &QAction::triggered, qApp, &QApplication::quit);
}

