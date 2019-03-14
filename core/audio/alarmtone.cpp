#include <QThread>
#include <QApplication>

#include "alarmtone.h"
#include "alarmtone_p.h"
#include "settings.h"
#include "channelscontroller.h"
#include "debug.h"

AlarmTone::AlarmTone(QObject* parent)
    : QObject(parent)
    , _audioStarted(false)
    , _isSilentMode(false)
    , _channelsModel(nullptr)
    , _volume(getVolume())
{
    qRegisterMetaType<AudioGenerator::Tone>("AudioGenerator::Tone");
    qRegisterMetaType<QString>("QString&");
    qRegisterMetaType<SystemSounds>("SystemSounds");
    _alarmToneThread = new QThread(this);
    _alarmToneThread->setObjectName("_alarmToneThread");
    _alarmTonePrivate = new AlarmTonePrivate();
    Q_ASSERT(_alarmTonePrivate);
    _alarmTonePrivate->setObjectName("_alarmTonePrivate");
    _alarmTonePrivate->moveToThread(_alarmToneThread);

    ASSERT(connect(this, &AlarmTone::startAll, _alarmTonePrivate, &AlarmTonePrivate::init, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &AlarmTone::finishAll, _alarmTonePrivate, &AlarmTonePrivate::finish, Qt::BlockingQueuedConnection));
    ASSERT(connect(_alarmTonePrivate, &AlarmTonePrivate::destroyed, _alarmToneThread, &QThread::quit));
    ASSERT(connect(_alarmTonePrivate, &AlarmTonePrivate::audioStarted, this, &AlarmTone::onAudioStarted));
    ASSERT(connect(_alarmToneThread, &QThread::finished, _alarmTonePrivate, &AlarmTonePrivate::deleteLater));

    _alarmToneThread->start();
    emit startAll();

    ASSERT(connect(this, &AlarmTone::doTestTone, _alarmTonePrivate, &AlarmTonePrivate::testTone));
    ASSERT(connect(this, &AlarmTone::doStopTest, _alarmTonePrivate, &AlarmTonePrivate::stopTest));
    ASSERT(connect(this, &AlarmTone::doSetVolume, _alarmTonePrivate, &AlarmTonePrivate::setVolume));
    ASSERT(connect(this, &AlarmTone::doGetVolume, _alarmTonePrivate, &AlarmTonePrivate::onGetVolume));
    ASSERT(connect(this, &AlarmTone::doStop, _alarmTonePrivate, &AlarmTonePrivate::stop, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &AlarmTone::doResume, _alarmTonePrivate, &AlarmTonePrivate::resume, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &AlarmTone::doPlaySound, _alarmTonePrivate, &AlarmTonePrivate::onPlaySound));

    ASSERT(connect(this, &AlarmTone::doPlayTone, _alarmTonePrivate, &AlarmTonePrivate::playTone));
    ASSERT(connect(this, &AlarmTone::doStopTone, _alarmTonePrivate, &AlarmTonePrivate::stopTone));

    ASSERT(connect(_alarmTonePrivate, &AlarmTonePrivate::doVolume, this, &AlarmTone::onDoVolume));
}

AlarmTone::~AlarmTone()
{
    emit finishAll();
    qDebug() << "Deleting AlarmTone...";
    if (_alarmToneThread->isRunning()) {
        qDebug() << "Alarm tone thread running, stopping...";
        _alarmToneThread->quit();
        qDebug() << "Waiting for alarm tone...";
        if (_alarmToneThread->wait(2000)) {
            qDebug() << "Alarm tone thread stopped!";
        }
    }
}

void AlarmTone::setModel(QAbstractItemModel* channelsModel)
{
    Q_ASSERT(channelsModel != nullptr);

    if (_channelsModel != channelsModel) {
        if (_channelsModel != nullptr) {
            _channelsModel->disconnect(this);
        }
        _channelsModel = channelsModel;
        ASSERT(connect(_channelsModel, &QAbstractItemModel::dataChanged, this, &AlarmTone::dataChanged) /* , Qt::UniqueConnection */);
    }
}

void AlarmTone::testTone(AudioGenerator::Tone tone, bool isPlay)
{
    emit doTestTone(tone, isPlay);
}

void AlarmTone::stopTest()
{
    emit doStopTest();
}

void AlarmTone::notification()
{
    emit doNotification();
}

void AlarmTone::setVolume(double value)
{
    emit doSetVolume(value);
    _volume = value;
}

double AlarmTone::volume()
{
    emit doGetVolume();
    return _volume;
}

void AlarmTone::setSilentMode(bool value)
{
    _isSilentMode = value;
    if (value) {
        UsedTonesMap::const_iterator iter = _isPlaingMap.constBegin();
        while (iter != _isPlaingMap.constEnd()) {
            if (iter.value() == true) {
                emit doStopTone(iter.key());
            }
            ++iter;
        }
    }
}

void AlarmTone::stop()
{
    Q_ASSERT(_alarmTonePrivate);
    emit doStop();
}

void AlarmTone::resume()
{
    Q_ASSERT(_alarmTonePrivate);
    emit doResume();
}

void AlarmTone::playSound(SystemSounds sound)
{
    emit doPlaySound(sound);
}

void AlarmTone::playTone(const QModelIndex& index)
{
    Q_ASSERT(thread() == QThread::currentThread());


    const QVariant& isPlayAlarmToneRole = _channelsModel->data(index, IsPlayAlarmToneRole);
    Q_ASSERT(isPlayAlarmToneRole.isValid() == true);

    if (isPlayAlarmToneRole.toBool() == true) {
        const QVariant& alarmTone = _channelsModel->data(index, AlarmToneRole);
        Q_ASSERT(alarmTone.isValid() == true);
        AudioGenerator::Tone tone = static_cast<AudioGenerator::Tone>(alarmTone.toInt());
        if (_isPlaingMap.value(tone, false) == false) {
            _isPlaingMap[tone] = true;
            emit doPlayTone(tone);
        }
    }
}

void AlarmTone::stopTone(const QModelIndex& index)
{
    Q_ASSERT(thread() == QThread::currentThread());

    QVariant value = _channelsModel->data(index, AlarmToneRole);
    Q_ASSERT(value.isValid() == true);
    AudioGenerator::Tone tone = static_cast<AudioGenerator::Tone>(value.toInt());
    if (_isPlaingMap.value(tone, false) == true) {
        _isPlaingMap[tone] = false;
        emit doStopTone(tone);
    }
}

void AlarmTone::onDoVolume(double volume)
{
    _volume = volume;
}

void AlarmTone::onAudioStarted()
{
    _audioStarted = true;
}

void AlarmTone::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_ASSERT(thread() == QThread::currentThread());
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        const QModelIndex& index = _channelsModel->index(i, 0);
        for (int role : roles) {
            switch (role) {
            case IsAlarmedRole:
                if (_isSilentMode == false) {
                    const QVariant& value = _channelsModel->data(index, role);
                    Q_ASSERT(value.isValid() == true);
                    if (value.isValid()) {
                        bool isAlarm = value.toBool();
                        if (isAlarm == true) {
                            playTone(index);
                        }
                        else {
                            stopTone(index);
                        }
                    }
                    else {
                        return;
                    }
                }
                break;
            case IsPlayAlarmToneRole:
                if (_isSilentMode == false) {
                    const QVariant& isPlayAlarmTone = _channelsModel->data(index, IsPlayAlarmToneRole);
                    const QVariant& isAlarmed = _channelsModel->data(index, IsAlarmedRole);

                    Q_ASSERT(isPlayAlarmTone.isValid() == true);
                    Q_ASSERT(isAlarmed.isValid() == true);

                    if (isAlarmed.toBool() && isPlayAlarmTone.toBool()) {
                        playTone(index);
                    }
                    else {
                        stopTone(index);
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}
