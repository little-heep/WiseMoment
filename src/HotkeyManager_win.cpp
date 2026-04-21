#include "HotkeyManager_win.h"

#include <QAbstractNativeEventFilter>
#include <QCoreApplication>

#ifdef _WIN32
#  include <windows.h>
#endif

namespace {
constexpr int HOTKEY_ID_TOGGLE_MAIN = 1001;
constexpr int HOTKEY_ID_TOGGLE_POMODORO = 1002;
}

HotkeyManagerWin::HotkeyManagerWin(QObject* parent)
    : QObject(parent)
{
}

HotkeyManagerWin::~HotkeyManagerWin()
{
    unregisterAll();
    if (m_installed)
        qApp->removeNativeEventFilter(this);
}

bool HotkeyManagerWin::registerDefaults()
{
#ifndef _WIN32
    return false;
#else
    if (!m_installed) {
        qApp->installNativeEventFilter(this);
        m_installed = true;
    }

    // Ctrl+Alt+M: toggle main window
    const bool ok1 = RegisterHotKey(nullptr, HOTKEY_ID_TOGGLE_MAIN, MOD_CONTROL | MOD_ALT, 0x4D);
    // Ctrl+Alt+P: start/pause/resume
    const bool ok2 = RegisterHotKey(nullptr, HOTKEY_ID_TOGGLE_POMODORO, MOD_CONTROL | MOD_ALT, 0x50);
    return ok1 && ok2;
#endif
}

void HotkeyManagerWin::unregisterAll()
{
#ifdef _WIN32
    UnregisterHotKey(nullptr, HOTKEY_ID_TOGGLE_MAIN);
    UnregisterHotKey(nullptr, HOTKEY_ID_TOGGLE_POMODORO);
#endif
}

bool HotkeyManagerWin::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);
#ifdef _WIN32
    MSG* msg = static_cast<MSG*>(message);
    if (msg && msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_ID_TOGGLE_MAIN) {
            emit toggleMainRequested();
            if (result) *result = 0;
            return true;
        }
        if (msg->wParam == HOTKEY_ID_TOGGLE_POMODORO) {
            emit togglePomodoroRequested();
            if (result) *result = 0;
            return true;
        }
    }
#else
    Q_UNUSED(message);
    Q_UNUSED(result);
#endif
    return false;
}

