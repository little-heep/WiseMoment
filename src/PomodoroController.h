#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QTimer>

class PomodoroController : public QObject
{
    Q_OBJECT
public:
    enum class State { Idle, Running, Paused, Finished };
    Q_ENUM(State)

    explicit PomodoroController(QObject* parent = nullptr);

    State state() const { return m_state; }
    int totalSeconds() const { return m_totalSeconds; }
    int remainingSeconds() const { return m_remainingSeconds; }

    void setTotalMinutes(int minutes);

public slots:
    void start();
    void pause();
    void resume();
    void stop();

    void setWhiteNoiseEnabled(bool on);
    void setWhiteNoiseFile(const QString& path);

signals:
    void tick(int remainingSeconds, int totalSeconds);
    void stateChanged(PomodoroController::State state);
    void finished();

private slots:
    void onTimer();

private:
    void setState(State s);
    void updateWhiteNoisePlayback();

    State m_state{State::Idle};
    QTimer m_timer;
    int m_totalSeconds{25 * 60};
    int m_remainingSeconds{25 * 60};

    bool m_whiteNoiseEnabled{false};
    QString m_whiteNoiseFile;
    QMediaPlayer* m_player{nullptr};
};

