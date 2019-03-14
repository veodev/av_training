#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

#include <QString>

class Powermanagement
{
public:
    Powermanagement();
    virtual ~Powermanagement() {}

    virtual bool update()
    {
        return true;
    }

    QString errorString() const;

    virtual double batteryVoltage()
    {
        return .0;
    }
    virtual double batteryPercent()
    {
        return .0;
    }
    virtual double temperatureSoc()
    {
        return .0;
    }

    void setbatteryVoltageRange(double min, double max);
    double batteryVoltageMinimum() const;
    double batteryVoltageMaximum() const;

protected:
    double _batteryVoltageMinimum;
    double _batteryVoltageMaximum;
    QString _error;
};

#endif  // POWERMANAGEMENT_H
