#pragma once

#include <QObject>

class QAudioRecorder;

class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorder(QObject* parent = nullptr);
    ~AudioRecorder() override;

    bool isAvailable() const;
    bool isRecording() const;

    QString currentOutputFile() const;

public slots:
    bool startWavToFile(const QString& absoluteFilePath);
    void stop();

signals:
    void recordingStarted(const QString& filePath);
    void recordingStopped(const QString& filePath);
    void errorOccurred(const QString& errorString);

private:
    QAudioRecorder* m_rec{nullptr};
    QString m_currentFile;
};

