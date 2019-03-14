#include "temperaturesensormanager.h"
#include <QThread>

TemperatureSensorManager::TemperatureSensorManager(int externalType, int internalType)
    : QObject(nullptr)
    , _internalSensor(nullptr)
    , _externalSensor(nullptr)
    , _updateTimer(nullptr)
    , _updateInterval(3000)
    , _externalType(externalType)
    , _internalType(internalType)
    , _internalTemp(273.0f)
    , _externalTemp(273.0f)
    , _internalWarningSent(false)
    , _externalWarningSent(false)
{
}

TemperatureSensorManager::~TemperatureSensorManager()
{
    qDebug() << "Deleting TemperatureSensorManager...";
}

void TemperatureSensorManager::sendSignals()
{
    emit internalTemperatureChanged(_internalTemp);
    emit externalTemperatureChanged(_externalTemp);
}

void TemperatureSensorManager::update()
{
    Q_ASSERT(thread() == QThread::currentThread());
    float internal = _internalSensor->getTemperature();
    float external = _externalSensor->getTemperature();
    if (internal != _internalTemp) {
        emit internalTemperatureChanged(internal);
    }
    if (external != _externalTemp) {
        emit externalTemperatureChanged(external);
    }
    _internalTemp = internal;
    _externalTemp = external;
    if (_internalSensor->getIsFailed()) {
        if (!_internalWarningSent) {
            emit sensorFailure(0);
            _internalWarningSent = true;
        }
    }
    else {
        _internalWarningSent = false;
    }
    if (_externalSensor->getIsFailed()) {
        if (!_externalWarningSent) {
            emit sensorFailure(1);
            _externalWarningSent = true;
        }
    }
    else {
        _externalWarningSent = false;
    }
}

void TemperatureSensorManager::init()
{
    Q_ASSERT(thread() == QThread::currentThread());

    switch (_internalType) {
    case 1:
        qDebug() << "Internal temperature sensor: fake";
        _internalSensor = new Temperature_fake(0);
        break;
    case 2:
        qDebug() << "Internal temperature sensor: LM75";
        _internalSensor = new LM75(1, "0x4f");
        break;
    case 3:
        qDebug() << "Internal temperature sensor: Generic";
        _internalSensor = new Ds18s20();
        break;
    default:
        qDebug() << "Internal temperature sensor: default";
        _internalSensor = new Temperature_fake(24.3f);
        break;
    }

    switch (_externalType) {
    case 1:
        qDebug() << "External temperature sensor: fake";
        _externalSensor = new Temperature_fake(0);
        break;
    case 2:
        qDebug() << "External temperature sensor: LM75";
        _externalSensor = new LM75(1, "0x4f");
        break;
    case 3:
        qDebug() << "External temperature sensor: Generic";
        _externalSensor = new Ds18s20();
        break;
    default:
        qDebug() << "External temperature sensor: default";
        _externalSensor = new Temperature_fake(24.3f);
        break;
    }

    _updateTimer = new QTimer(this);
    ASSERT(connect(_updateTimer, &QTimer::timeout, this, &TemperatureSensorManager::update));
    _updateTimer->setInterval(_updateInterval);
    _updateTimer->start();
}

void TemperatureSensorManager::stop()
{
    qDebug() << "Stopping TemperatureSensorManager...";
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_updateTimer != nullptr);
    _updateTimer->stop();
    disconnect(_updateTimer, &QTimer::timeout, this, &TemperatureSensorManager::update);
    delete _updateTimer;

    delete _internalSensor;
    delete _externalSensor;
}
