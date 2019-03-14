#include "genericTempSensor.h"
#include "debug.h"
#include "settings.h"

GenericTempSensor::GenericTempSensor()
    : TemperatureSensor()
{
    bool isFound = false;
    QList<QByteArray> sensorTypes = QSensor::sensorTypes();
    for (const QByteArray& sensorType : sensorTypes) {
        if (sensorType.size() > 0) {
            QString name(sensorType);
            if (name == QString("QAmbientTemperatureSensor")) {
                qDebug() << "Sensor found:" << sensorType.toStdString().c_str();
                isFound = true;
                break;
            }
        }
    }

    if (isFound == false) {
        qDebug() << "Temperature sensor class not found";
        _isFailed = true;
        _sensor = nullptr;
    }
    else {
        _sensor = new QSensor("QAmbientTemperatureSensor");
        if (_sensor->start()) {
            if (_sensor->connectToBackend()) {
                _sensor->reading();
            }
            else {
                qDebug() << "QSensor: unable to connect to backend!";
                _sensor = nullptr;
                _isFailed = true;
            }
        }
        else {
            qDebug() << "QSensor: unable to start!";
            _sensor = nullptr;
            _isFailed = true;
        }
    }
}

GenericTempSensor::~GenericTempSensor()
{
    _isFailed = true;
    if (_sensor != nullptr) {
        delete _sensor;
    }
}

float GenericTempSensor::getTemperature()
{
#ifdef IMX_DEVICE
    if (_sensor != nullptr) {
        QSensorReading* reading = _sensor->reading();
        if (reading != nullptr) {
            return reading->property("temperature").toFloat();
        }
    }
    else {
        _isFailed = true;
    }
#endif
    return -0;
}
