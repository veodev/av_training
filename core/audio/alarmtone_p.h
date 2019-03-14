#ifndef ALARMTONE_PRIVATE_H
#define ALARMTONE_PRIVATE_H

#include <QObject>
#include <QVector>

#include "audiogenerator.h"
#include "coredefinitions.h"

class QTimer;
class QAbstractItemModel;
class AudioOutput;

class AlarmTonePrivate : public QObject
{
    Q_OBJECT

public:
    explicit AlarmTonePrivate(QObject* parent = 0);
    ~AlarmTonePrivate();
    bool isStarted() const;
public slots:
    void init();
    void finish();
    void testTone(AudioGenerator::Tone tone, bool isPlay = true);
    void stopTest();
    void setVolume(double value);
    double volume() const;
    void stop();
    void resume();
    void onGetVolume();

    void onPlaySound(SystemSounds sound);
    void playTone(AudioGenerator::Tone tone);
    void stopTone(AudioGenerator::Tone tone);

signals:
    void doVolume(double volume);
    void audioStarted();

private slots:
    void notification500HzTimeout();
    void notification1000Hz();
    void notification1000HzTimeout();
    void notification2000Hz();
    void notification2000HzTimeout();

private:
    AudioOutput* _audioOutput;
    bool _isNotificationPlaing;
    typedef QMap<AudioGenerator::Tone, bool> UsedTonesMap;
    UsedTonesMap _isTestingMap;
};

#endif  // ALARMTONE_PRIVATE_H
