#ifndef ADVANTECHPOWERMANAGEMENT_H
#define ADVANTECHPOWERMANAGEMENT_H

#include "powermanagement.h"

class AdvantechPowermanagement : public Powermanagement
{
public:
    AdvantechPowermanagement();

    static bool isSupported();
    bool update();

    double batteryVoltage();
    double batteryPercent();

private:
    double _batteryVoltage;
    double _batteryPercent;
};

#endif  // ADVANTECHPOWERMANAGEMENT_H
