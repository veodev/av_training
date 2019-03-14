#ifndef GENERICTEMPSENSOR_H
#define GENERICTEMPSENSOR_H

#include <QObject>
#include <QSensor>
#include "temperature/temperaturesensor.h"

class GenericTempSensor : public TemperatureSensor
{
public:
    GenericTempSensor();
    virtual ~GenericTempSensor();
    virtual float getTemperature();

private:
    QSensor * _sensor;
};

#endif // GENERICTEMPSENSOR_H
