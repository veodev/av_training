#include "advantechpowermanagement.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

const QString batteryCapacityPath = "/sys/class/power_supply/BAT0/capacity";

AdvantechPowermanagement::AdvantechPowermanagement()
{
    _batteryVoltage = 0;
    _batteryPercent = 0;
}

bool AdvantechPowermanagement::isSupported()
{
    QDir batteryDir(QString("/sys/class/power_supply/BAT0"));
    if (batteryDir.exists()) {
        return true;
    }
    return false;
}

bool AdvantechPowermanagement::update()
{
    QFile batteryCapacityFile(batteryCapacityPath);
    if (!batteryCapacityFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream batteryCapacityIn(&batteryCapacityFile);
    int batteryCapacity = batteryCapacityIn.readLine().toInt();
    batteryCapacityFile.close();

    _batteryPercent = batteryCapacity;
    return true;
}

double AdvantechPowermanagement::batteryVoltage()
{
    return _batteryVoltage;
}

double AdvantechPowermanagement::batteryPercent()
{
    return _batteryPercent;
}
