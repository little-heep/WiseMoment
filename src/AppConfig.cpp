#include "AppConfig.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

static QJsonObject loadSettings(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())
        return {};
    return doc.object();
}

static void saveSettings(const QString& path, const QJsonObject& obj)
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

void AppConfig::ensureAppDataDir()
{
    QDir().mkpath(appDataDir());
    QDir().mkpath(audioDir());
}

QString AppConfig::appDataDir()
{
    const auto base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return base;
}

QString AppConfig::settingsPath()
{
    return appDataDir() + "/settings.json";
}

QString AppConfig::todosPath()
{
    return appDataDir() + "/todos.json";
}

QString AppConfig::statsPath()
{
    return appDataDir() + "/stats.json";
}

QString AppConfig::audioDir()
{
    return appDataDir() + "/audio";
}

QString AppConfig::uiLocale()
{
    const auto obj = loadSettings(settingsPath());
    const auto v = obj.value("uiLocale").toString();
    return v.isEmpty() ? QStringLiteral("zh_CN") : v;
}

void AppConfig::setUiLocale(const QString& localeName)
{
    auto obj = loadSettings(settingsPath());
    obj["uiLocale"] = localeName;
    saveSettings(settingsPath(), obj);
}

bool AppConfig::alwaysOnTop()
{
    const auto obj = loadSettings(settingsPath());
    return obj.value("alwaysOnTop").toBool(false);
}

void AppConfig::setAlwaysOnTop(bool on)
{
    auto obj = loadSettings(settingsPath());
    obj["alwaysOnTop"] = on;
    saveSettings(settingsPath(), obj);
}

double AppConfig::windowOpacity()
{
    const auto obj = loadSettings(settingsPath());
    const auto v = obj.value("windowOpacity").toDouble(1.0);
    if (v < 0.2) return 0.2;
    if (v > 1.0) return 1.0;
    return v;
}

void AppConfig::setWindowOpacity(double opacity01)
{
    auto obj = loadSettings(settingsPath());
    obj["windowOpacity"] = opacity01;
    saveSettings(settingsPath(), obj);
}

int AppConfig::pomodoroMinutes()
{
    const auto obj = loadSettings(settingsPath());
    const int v = obj.value("pomodoroMinutes").toInt(25);
    return qBound(1, v, 180);
}

void AppConfig::setPomodoroMinutes(int minutes)
{
    auto obj = loadSettings(settingsPath());
    obj["pomodoroMinutes"] = minutes;
    saveSettings(settingsPath(), obj);
}

bool AppConfig::playWhiteNoise()
{
    const auto obj = loadSettings(settingsPath());
    return obj.value("playWhiteNoise").toBool(false);
}

void AppConfig::setPlayWhiteNoise(bool on)
{
    auto obj = loadSettings(settingsPath());
    obj["playWhiteNoise"] = on;
    saveSettings(settingsPath(), obj);
}

QString AppConfig::whiteNoiseFile()
{
    const auto obj = loadSettings(settingsPath());
    return obj.value("whiteNoiseFile").toString();
}

void AppConfig::setWhiteNoiseFile(const QString& path)
{
    auto obj = loadSettings(settingsPath());
    obj["whiteNoiseFile"] = path;
    saveSettings(settingsPath(), obj);
}

