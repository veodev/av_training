#include "temperature_fake.h"

Temperature_fake::Temperature_fake(float temp) : TemperatureSensor()
{
    _temp = temp;
}

void Temperature_fake::setTemperature(float temp)
{
    _temp = temp;
}

Temperature_fake::~Temperature_fake()
{

}

float Temperature_fake::getTemperature()
{
    return _temp;
}
