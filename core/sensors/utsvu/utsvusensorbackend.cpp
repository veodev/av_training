#include "utsvusensorbackend.h"

#include <QtCore/QDebug>

UtsvuSensorBackendBase::UtsvuSensorBackendBase(QSensor *sensor)
    : QSensorBackend(sensor)
{
}


void UtsvuSensorBackendBase::start()
{
    _started = true;
}

void UtsvuSensorBackendBase::stop()
{
    _started = false;
}
