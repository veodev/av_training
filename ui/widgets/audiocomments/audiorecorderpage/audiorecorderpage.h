#ifndef AUDIORECORDERPAGE_H
#define AUDIORECORDERPAGE_H

#include <QWidget>

#include <QMediaRecorder>
#include <QUrl>

namespace Ui
{
class AudioRecorderPage;
}

class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;

class AudioLevel;

class AudioRecorderPage : public QWidget
{
    Q_OBJECT

public:
    explicit AudioRecorderPage(QWidget* parent = nullptr);
    ~AudioRecorderPage();
    void setRecordingEnabled(bool value);
    // void supports();

public slots:
    void setVisible(bool visible);
    void processBuffer(const QAudioBuffer& buffer);

protected:
    bool event(QEvent* e);

private slots:
    void togglePause();
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void updateState(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage(QMediaRecorder::Error error);

    void on_recordButton_released();

    void on_pauseButton_released();

private:
    void stopRecording();
    void clearAudioLevels();

    Ui::AudioRecorderPage* ui;

    QAudioRecorder* _audioRecorder;
    QAudioProbe* _audioProbe;
    QList<AudioLevel*> _audioLevels;
};

#endif  // AUDIORECORDERPAGE_H
