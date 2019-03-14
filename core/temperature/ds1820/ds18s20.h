#ifndef DS18S20_H
#define DS18S20_H

#include <QFile>
#include <QString>
#include "temperature/temperaturesensor.h"

class Ds18s20 : public TemperatureSensor
{
public:
    Ds18s20();
    ~Ds18s20();

    bool init();
    void finit();
    float getTemperature();

private:
    QFile _w1DevFile;
    QString _w1DevFileName;
    bool _isInited;
};

#endif
