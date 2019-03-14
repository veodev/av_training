#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <QAudioOutput>
#include <QPointer>
#include <QFile>
#include <QBuffer>

#include "audiogenerator.h"
#include "sounddata.h"

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    explicit AudioOutput(QObject* parent = 0);
    ~AudioOutput();

    bool isStarted() const;

public slots:
    void play();
    void stop();
    void resume();
    void setVolume(double value);
    double volume();
    void setTone(AudioGenerator::Tone tone);
    void clearTone(AudioGenerator::Tone tone);

    void playSound(SystemSounds sound);
    void playSound(const QString& soundPath);

private slots:
    void notified();
    void handleStateChanged(QAudio::State state);

private:
    void init();
    void startPlay();
    void stopPlay();
    void loadSounds();

private:
    QPointer<QAudioOutput> _audioOutput;
    QBuffer _buffer;
    std::vector<SoundData> _sounds;
    AudioGenerator* _generator;
    bool _isStarted;
};

#endif  // AUDIO_OUTPUT_H
