#include <QTimer>

#include "settings.h"
#include "alarmtone_p.h"
#include "audiooutput.h"
#include "channelscontroller.h"
#include "debug.h"
#include "roles.h"
#include "core.h"

#include <QThread>

#define NOTIFICATION_PLAY_INTERVAL 200
#define NOTIFICATION_PAUSE_INTERVAL 50

AlarmTonePrivate::AlarmTonePrivate(QObject* parent)
    : QObject(parent)
    , _audioOutput(nullptr)
    , _isNotificationPlaing(false)


{
    qRegisterMetaType<QVector<int>>("QVector<int>");
}

AlarmTonePrivate::~AlarmTonePrivate()
{
    qDebug() << "Deleting AlarmTonePrivate...";
}

bool AlarmTonePrivate::isStarted() const
{
    Q_ASSERT(_audioOutput != nullptr);
    return _audioOutput->isStarted();
}

void AlarmTonePrivate::init()
{
    Q_ASSERT(thread() == QThread::currentThread());
    _audioOutput = new AudioOutput(this);
    _audioOutput->setVolume(getVolume());
    _audioOutput->play();
    emit audioStarted();
}

void AlarmTonePrivate::finish()
{
    qDebug() << "AlarmTonePrivate finish...";
    if (_audioOutput != nullptr) {
        delete _audioOutput;
    }
}

void AlarmTonePrivate::testTone(AudioGenerator::Tone tone, bool isPlay)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    if (isPlay) {
        if (!_isTestingMap.value(tone, false)) {
            _isTestingMap[tone] = true;
            _audioOutput->setTone(tone);
            _audioOutput->play();
        }
    }
    else {
        if (_isTestingMap.value(tone, false)) {
            _isTestingMap[tone] = false;
            _audioOutput->clearTone(tone);
            _audioOutput->play();
        }
    }
}

void AlarmTonePrivate::stopTest()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    UsedTonesMap::const_iterator iter = _isTestingMap.constBegin();
    while (iter != _isTestingMap.constEnd()) {
        if (iter.value()) {
            _audioOutput->clearTone(iter.key());
        }
        ++iter;
    }
    _isTestingMap.clear();
    _audioOutput->play();
}

void AlarmTonePrivate::setVolume(double value)
{
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->setVolume(value);
}

double AlarmTonePrivate::volume() const
{
    Q_ASSERT(_audioOutput != nullptr);

    return _audioOutput->volume();
}

void AlarmTonePrivate::stop()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);
    _audioOutput->stop();
}

void AlarmTonePrivate::resume()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);
    _audioOutput->resume();
}

void AlarmTonePrivate::onGetVolume()
{
    emit doVolume(_audioOutput->volume());
}

void AlarmTonePrivate::onPlaySound(SystemSounds sound)
{
    Q_ASSERT(thread() == QThread::currentThread());
    _audioOutput->playSound(sound);
}

void AlarmTonePrivate::playTone(AudioGenerator::Tone tone)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);
    _audioOutput->setTone(tone);
    _audioOutput->play();
}

void AlarmTonePrivate::stopTone(AudioGenerator::Tone tone)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);
    _audioOutput->clearTone(tone);
    _audioOutput->play();
}
void AlarmTonePrivate::notification500HzTimeout()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->clearTone(AudioGenerator::left500Hz);
    _audioOutput->clearTone(AudioGenerator::right500Hz);
    _audioOutput->play();
    QTimer::singleShot(NOTIFICATION_PAUSE_INTERVAL, this, &AlarmTonePrivate::notification1000Hz);
}

void AlarmTonePrivate::notification1000Hz()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->setTone(AudioGenerator::left1000Hz);
    _audioOutput->setTone(AudioGenerator::right1000Hz);
    _audioOutput->play();
    QTimer::singleShot(NOTIFICATION_PLAY_INTERVAL, this, &AlarmTonePrivate::notification1000HzTimeout);
}

void AlarmTonePrivate::notification1000HzTimeout()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->clearTone(AudioGenerator::left1000Hz);
    _audioOutput->clearTone(AudioGenerator::right1000Hz);
    _audioOutput->play();
    QTimer::singleShot(NOTIFICATION_PAUSE_INTERVAL, this, &AlarmTonePrivate::notification2000Hz);
}

void AlarmTonePrivate::notification2000Hz()
{
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->setTone(AudioGenerator::left2000Hz);
    _audioOutput->setTone(AudioGenerator::right2000Hz);
    _audioOutput->play();
    QTimer::singleShot(NOTIFICATION_PLAY_INTERVAL, this, &AlarmTonePrivate::notification2000HzTimeout);
}

void AlarmTonePrivate::notification2000HzTimeout()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_audioOutput != nullptr);

    _audioOutput->clearTone(AudioGenerator::left2000Hz);
    _audioOutput->clearTone(AudioGenerator::right2000Hz);
    _audioOutput->play();
    _isNotificationPlaing = false;
}
