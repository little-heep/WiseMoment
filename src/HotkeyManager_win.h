#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

class HotkeyManagerWin : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit HotkeyManagerWin(QObject* parent = nullptr);
    ~HotkeyManagerWin() override;

    bool registerDefaults();
    void unregisterAll();

signals:
    void toggleMainRequested();
    void togglePomodoroRequested();

protected:
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

private:
    bool m_installed{false};
};

