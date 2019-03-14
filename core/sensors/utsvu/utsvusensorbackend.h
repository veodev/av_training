#ifndef UTSVUSENSORBACKEND_H
#define UTSVUSENSORBACKEND_H

#include <qsensorbackend.h>

class UtsvuSensorBackendBase : public QSensorBackend
{
    Q_OBJECT

public:
    UtsvuSensorBackendBase(QSensor *sensor);

    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

protected:
    bool _started;
};

template<class SensorReading>
class UtsvuSensorBackend : public UtsvuSensorBackendBase
{
public:
    UtsvuSensorBackend(QSensor *sensor)
        : UtsvuSensorBackendBase(sensor)
    {
        setReading(&_reading);
    }

protected:
    SensorReading _reading;
};

#endif // UTSVUSENSORBACKEND_H
