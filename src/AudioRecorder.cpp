#include "AudioRecorder.h"

#include <QDir>
#include <QFileInfo>

#include <QAudioEncoderSettings>
#include <QAudioRecorder>
#include <QMultimedia>
#include <QUrl>

AudioRecorder::AudioRecorder(QObject* parent)
    : QObject(parent)
{
    m_rec = new QAudioRecorder(this);

    connect(m_rec, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this, [this](QMediaRecorder::Error) {
        emit errorOccurred(m_rec->errorString());
    });
    connect(m_rec, &QAudioRecorder::stateChanged, this, [this](QMediaRecorder::State s) {
        if (s == QMediaRecorder::RecordingState)
            emit recordingStarted(m_currentFile);
        if (s == QMediaRecorder::StoppedState)
            emit recordingStopped(m_currentFile);
    });
}

AudioRecorder::~AudioRecorder() = default;

bool AudioRecorder::isAvailable() const
{
    return m_rec && !m_rec->audioInputs().isEmpty();
}

bool AudioRecorder::isRecording() const
{
    return m_rec && m_rec->state() == QMediaRecorder::RecordingState;
}

QString AudioRecorder::currentOutputFile() const
{
    return m_currentFile;
}

bool AudioRecorder::startWavToFile(const QString& absoluteFilePath)
{
    if (!m_rec)
        return false;

    const auto outInfo = QFileInfo(absoluteFilePath);
    QDir().mkpath(outInfo.absolutePath());

    m_currentFile = outInfo.absoluteFilePath();
    m_rec->setOutputLocation(QUrl::fromLocalFile(m_currentFile));

    QAudioEncoderSettings audio;
    audio.setCodec("audio/pcm");
    audio.setQuality(QMultimedia::NormalQuality);
    audio.setChannelCount(1);
    audio.setSampleRate(44100);

    m_rec->setAudioSettings(audio);
    m_rec->setContainerFormat("wav");

    m_rec->record();
    return true;
}

void AudioRecorder::stop()
{
    if (!m_rec)
        return;
    m_rec->stop();
}

