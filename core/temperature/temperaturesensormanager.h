#ifndef TEMPERATURESENSORMANAGER_H
#define TEMPERATURESENSORMANAGER_H

#include <QObject>
#include <QTimer>
#include "debug.h"
#include "temperature/temperaturesensor.h"
#include "temperature/fake/temperature_fake.h"
#include "temperature/lm75/lm75.h"
#include "temperature/ds1820/ds18s20.h"

class TemperatureSensorManager : public QObject
{
    Q_OBJECT

    TemperatureSensor* _internalSensor;
    TemperatureSensor* _externalSensor;

    QTimer* _updateTimer;
    int _updateInterval;
    int _externalType;
    int _internalType;
    float _internalTemp;
    float _externalTemp;

    bool _internalWarningSent;
    bool _externalWarningSent;

public:
    TemperatureSensorManager(int externalType, int internalType);
    ~TemperatureSensorManager();


signals:
    void internalTemperatureChanged(qreal value);
    void externalTemperatureChanged(qreal value);
    void sensorFailure(int id);

public slots:
    void sendSignals();
    void update();
    void init();
    void stop();
};

#endif  // TEMPERATURESENSORMANAGER_H
