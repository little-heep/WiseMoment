#pragma once

#include <QObject>
#include <QSystemTrayIcon>

class PomodoroController;

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(PomodoroController* pomodoro, QWidget* mainWindow, QObject* parent = nullptr);

    void show();

signals:
    void requestShowMain();
    void requestHideMain();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onPomodoroFinished();
    void rebuildMenu();

private:
    PomodoroController* m_pomodoro{nullptr};
    QWidget* m_mainWindow{nullptr};
    QSystemTrayIcon m_tray;
    QMenu* m_menu{nullptr};
};

