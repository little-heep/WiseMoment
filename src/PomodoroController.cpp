#include "PomodoroController.h"

#include <QAudioOutput>
#include <QDateTime>
#include <QFileInfo>

PomodoroController::PomodoroController(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(250);
    connect(&m_timer, &QTimer::timeout, this, &PomodoroController::onTimer);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // not used in this Qt5 project
#else
    m_player = new QMediaPlayer(this);
#endif
}

void PomodoroController::setTotalMinutes(int minutes)
{
    minutes = qBound(1, minutes, 180);
    m_totalSeconds = minutes * 60;
    if (m_state == State::Idle || m_state == State::Finished) {
        m_remainingSeconds = m_totalSeconds;
        emit tick(m_remainingSeconds, m_totalSeconds);
    }
}

void PomodoroController::start()
{
    if (m_state == State::Running)
        return;
    m_remainingSeconds = m_totalSeconds;
    setState(State::Running);
    updateWhiteNoisePlayback();
    m_timer.start();
    emit tick(m_remainingSeconds, m_totalSeconds);
}

void PomodoroController::pause()
{
    if (m_state != State::Running)
        return;
    m_timer.stop();
    setState(State::Paused);
    updateWhiteNoisePlayback();
}

void PomodoroController::resume()
{
    if (m_state != State::Paused)
        return;
    setState(State::Running);
    updateWhiteNoisePlayback();
    m_timer.start();
}

void PomodoroController::stop()
{
    if (m_state == State::Idle)
        return;
    m_timer.stop();
    setState(State::Idle);
    updateWhiteNoisePlayback();
    m_remainingSeconds = m_totalSeconds;
    emit tick(m_remainingSeconds, m_totalSeconds);
}

void PomodoroController::setWhiteNoiseEnabled(bool on)
{
    m_whiteNoiseEnabled = on;
    updateWhiteNoisePlayback();
}

void PomodoroController::setWhiteNoiseFile(const QString& path)
{
    m_whiteNoiseFile = path;
    updateWhiteNoisePlayback();
}

void PomodoroController::onTimer()
{
    if (m_state != State::Running)
        return;

    static qint64 lastMs = -1;
    if (lastMs < 0)
        lastMs = QDateTime::currentMSecsSinceEpoch();

    const auto now = QDateTime::currentMSecsSinceEpoch();
    const auto delta = now - lastMs;
    if (delta < 950)
        return;

    const int dec = int(delta / 1000);
    lastMs = now;

    m_remainingSeconds = qMax(0, m_remainingSeconds - dec);
    emit tick(m_remainingSeconds, m_totalSeconds);

    if (m_remainingSeconds <= 0) {
        m_timer.stop();
        setState(State::Finished);
        updateWhiteNoisePlayback();
        emit finished();
    }
}

void PomodoroController::setState(State s)
{
    if (m_state == s)
        return;
    m_state = s;
    emit stateChanged(m_state);
}

void PomodoroController::updateWhiteNoisePlayback()
{
    if (!m_player)
        return;

    const bool shouldPlay = (m_state == State::Running) && m_whiteNoiseEnabled && QFileInfo::exists(m_whiteNoiseFile);
    if (!shouldPlay) {
        m_player->stop();
        return;
    }

    if (m_player->media().canonicalUrl().toLocalFile() != m_whiteNoiseFile) {
        m_player->setMedia(QUrl::fromLocalFile(m_whiteNoiseFile));
    }
    m_player->setVolume(50);
    m_player->play();
}

