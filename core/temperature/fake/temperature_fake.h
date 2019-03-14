#ifndef TEMPERATURE_FAKE_H
#define TEMPERATURE_FAKE_H
#include "temperature/temperaturesensor.h"

class Temperature_fake : public TemperatureSensor
{
    float _temp;
public:
    explicit Temperature_fake(float temp);
    void setTemperature(float temp);
    virtual ~Temperature_fake();
    virtual float getTemperature();
};

#endif // TEMPERATURE_FAKE_H
