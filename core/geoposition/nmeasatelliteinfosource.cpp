#include "nmeasatelliteinfosource.h"

NmeaSatelliteInfoSource::NmeaSatelliteInfoSource(QObject* parent)
    : QGeoSatelliteInfoSource(parent)
    , _isUpdateStarted(false)
    , _error(QGeoSatelliteInfoSource::NoError)
{
}

QGeoSatelliteInfoSource::Error NmeaSatelliteInfoSource::error() const
{
    return _error;
}

int NmeaSatelliteInfoSource::minimumUpdateInterval() const
{
    return 100;
}

void NmeaSatelliteInfoSource::requestUpdate(int)
{
}

void NmeaSatelliteInfoSource::startUpdates()
{
    _isUpdateStarted = true;
}

void NmeaSatelliteInfoSource::stopUpdates()
{
    _isUpdateStarted = false;
}

void NmeaSatelliteInfoSource::notifyNewInViewUpdate(const QList<QGeoSatelliteInfo>& update)
{
    if (_isUpdateStarted) {
        emit satellitesInViewUpdated(update);
    }
}

void NmeaSatelliteInfoSource::notifyNewInUseUpdate(const QList<QGeoSatelliteInfo>& update)
{
    if (_isUpdateStarted) {
        emit satellitesInUseUpdated(update);
    }
}
