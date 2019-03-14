#include <QCoreApplication>

#include "geoposition.h"
#include "debug.h"
#include "nmeareader.h"

GeoPosition::GeoPosition(QObject* parent)
    : QObject(parent)
{
}

GeoPosition::~GeoPosition()
{
}

void GeoPosition::setSerialPortName(const QString& serialPortName)
{
    Q_UNUSED(serialPortName);
}

GeoPosition* GeoPosition::instance(const QString& serialPortName)
{
    Q_UNUSED(serialPortName);
    return NULL;
}

GeoPosition::AntennaStatus GeoPosition::antennaStatus()
{
    return AntennaStatusUnknown;
}

QIODevice* GeoPosition::getSerialPort()
{
    return NULL;
}
