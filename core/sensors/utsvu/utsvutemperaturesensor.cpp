#include <QDebug>

#include "debug.h"
#include "utsvutemperaturesensor.h"

void Worker::doInit()
{
    QString error;
    bool isInited = temperatureSensor.init(error);
    emit initResult(isInited, error);
}

void Worker::doWork()
{
    QString error;
    float rez = temperatureSensor.temperature(error);
    emit resultReady(rez, error);
}

UtsvuTemperatureSensor::UtsvuTemperatureSensor(QSensor *sensor)
    : UtsvuSensorBackend<QAmbientTemperatureReading>(sensor)
{
    setDescription(QLatin1String("Temperature in degrees Celsius"));
}

UtsvuTemperatureSensor::~UtsvuTemperatureSensor()
{
    addOutputRange(-55.0, 125.0, 0.5);
}

void UtsvuTemperatureSensor::start()
{
    if (_started) {
        stop();
    }

    Worker *worker = new Worker;
    worker->moveToThread(&_workerThread);

    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&_workerThread, &QThread::started, worker, &Worker::doInit);
    connect(worker, &Worker::initResult, this, &UtsvuTemperatureSensor::initResult);
    connect(this, &UtsvuTemperatureSensor::operate, worker, &Worker::doWork);
    connect(worker, &Worker::resultReady, this, &UtsvuTemperatureSensor::handleResults);
    _workerThread.start();
}

void UtsvuTemperatureSensor::stop()
{
    _timer.stop();
    _workerThread.quit();
    _workerThread.wait();

    UtsvuSensorBackendBase::stop();
}

void UtsvuTemperatureSensor::initResult(bool isInited, const QString &error)
{
    if (isInited == true) {
        _timer.setInterval(1000);
        connect(&_timer, SIGNAL(timeout()), this, SIGNAL(operate()));
        _timer.start();

        UtsvuSensorBackendBase::start();
    }
    else {
        QWARNING << error;
    }
}

void UtsvuTemperatureSensor::handleResults(qreal temperature, const QString &error)
{
    if (error.isEmpty()) {
        QDEBUG << "temperature:" << temperature;
        _reading.setTemperature(temperature);
        newReadingAvailable();
    }
    else {
        QWARNING << error;
    }
}
