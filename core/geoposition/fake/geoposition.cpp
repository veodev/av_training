#include "geoposition.h"
#include "debug.h"

#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

GeoPosition::GeoPosition(QObject* parent)
    : QObject(parent)
{
}

GeoPosition::~GeoPosition() {}

GeoPosition* GeoPosition::instance()
{
    static GeoPosition _instance;
    return &_instance;
}

GeoPosition::AntennaStatus GeoPosition::antennaStatus()
{
    return AntennaStatus{AntennaStatusUnknown};
}
