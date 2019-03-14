#include "powermanagement.h"

#define BATTERY_VOLTAGE_USE_MIN 9
#define BATTERY_VOLTAGE_USE_MAX 13.8

Powermanagement::Powermanagement()
    : _batteryVoltageMinimum(BATTERY_VOLTAGE_USE_MIN)
    , _batteryVoltageMaximum(BATTERY_VOLTAGE_USE_MAX)
    , _error(QString())
{
}

void Powermanagement::setbatteryVoltageRange(double min, double max)
{
    _batteryVoltageMinimum = min;
    _batteryVoltageMaximum = max;
}

double Powermanagement::batteryVoltageMinimum() const
{
    return _batteryVoltageMinimum;
}

double Powermanagement::batteryVoltageMaximum() const
{
    return _batteryVoltageMaximum;
}


QString Powermanagement::errorString() const
{
    return _error;
}
