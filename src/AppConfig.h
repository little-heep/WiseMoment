#pragma once

#include <QString>

class AppConfig
{
public:
    static void ensureAppDataDir();
    static QString appDataDir();
    static QString settingsPath();
    static QString todosPath();
    static QString statsPath();
    static QString audioDir();

    static QString uiLocale(); // "zh_CN" / "en_US"
    static void setUiLocale(const QString& localeName);

    static bool alwaysOnTop();
    static void setAlwaysOnTop(bool on);

    static double windowOpacity();
    static void setWindowOpacity(double opacity01);

    static int pomodoroMinutes();
    static void setPomodoroMinutes(int minutes);

    static bool playWhiteNoise();
    static void setPlayWhiteNoise(bool on);

    static QString whiteNoiseFile();
    static void setWhiteNoiseFile(const QString& path);
};

