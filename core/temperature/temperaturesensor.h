#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

class TemperatureSensor
{
protected:
    bool _isFailed;

public:
    TemperatureSensor()
        : _isFailed(false)
    {
    }
    virtual ~TemperatureSensor() {}
    virtual float getTemperature() = 0;
    bool getIsFailed() const
    {
        return _isFailed;
    }
};


#endif  // TEMPERATURESENSOR_H
