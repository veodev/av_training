#ifndef ALARMTONE_H
#define ALARMTONE_H

#include <QObject>
#include <QVector>

#include "audiogenerator.h"
#include "coredefinitions.h"

class AlarmTonePrivate;
class QAbstractItemModel;

class AlarmTone : public QObject
{
    Q_OBJECT

public:
    explicit AlarmTone(QObject* parent = 0);
    ~AlarmTone();

    void setModel(QAbstractItemModel* channelsModel);

    void testTone(AudioGenerator::Tone tone, bool isPlay = true);
    void stopTest();
    void notification();
    void setVolume(double value);
    double volume();
    void setSilentMode(bool value);
    void stop();
    void resume();
    void playMedia(const QString& filePath);


signals:
    void doTestTone(AudioGenerator::Tone tone, bool isPlay);
    void doStopTest();
    void doNotification();
    void doSetVolume(double value);
    void doGetVolume();
    void doStop();
    void doResume();
    void doPlaySound(SystemSounds sound);
    void doPlayTone(AudioGenerator::Tone tone);
    void doStopTone(AudioGenerator::Tone tone);
    void finishAll();
    void startAll();

private slots:
    void onDoVolume(double volume);
    void onAudioStarted();
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
public slots:
    void playSound(SystemSounds sound);

private:
    void playTone(const QModelIndex& index);
    void stopTone(const QModelIndex& index);

private:
    bool _audioStarted;
    bool _isSilentMode;
    AlarmTonePrivate* _alarmTonePrivate;
    QAbstractItemModel* _channelsModel;
    QThread* _alarmToneThread;
    double _volume;
    typedef QMap<AudioGenerator::Tone, bool> UsedTonesMap;
    UsedTonesMap _isPlaingMap;
};

#endif  // ALARMTONE_H
