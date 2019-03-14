#ifndef LM75_H
#define LM75_H
#include "temperature/temperaturesensor.h"
#include <QString>

class LM75 : public TemperatureSensor
{
    int _bus;
    QString _addr;
    QString _fileName;

    int _failCount;

public:
    LM75(int bus, const QString& hexAddr);
    virtual ~LM75();
    QString addr() const;
    void setAddr(const QString& addr);
    int bus() const;
    void setBus(int bus);
    virtual float getTemperature();
};

#endif  // LM75_H
