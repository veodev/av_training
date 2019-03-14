#include "nmeapositioninfosource.h"

NmeaPositionInfoSource::NmeaPositionInfoSource(QObject* parent)
    : QGeoPositionInfoSource(parent)
    , _isUpdateStarted(false)
    , _error(QGeoPositionInfoSource::NoError)
{
}

NmeaPositionInfoSource::~NmeaPositionInfoSource()
{
}

QGeoPositionInfoSource::Error NmeaPositionInfoSource::error() const
{
    return _error;
}

QGeoPositionInfo NmeaPositionInfoSource::lastKnownPosition(bool) const
{
    return _lastUpdate;
}

int NmeaPositionInfoSource::minimumUpdateInterval() const
{
    return 100;
}

QGeoPositionInfoSource::PositioningMethods NmeaPositionInfoSource::supportedPositioningMethods() const
{
    return SatellitePositioningMethods;
}

void NmeaPositionInfoSource::requestUpdate(int)
{
}

void NmeaPositionInfoSource::startUpdates()
{
    _isUpdateStarted = true;
}

void NmeaPositionInfoSource::stopUpdates()
{
    _isUpdateStarted = false;
}

void NmeaPositionInfoSource::notifyNewUpdate(QGeoPositionInfo& update, bool)
{
    if (_isUpdateStarted) {
        _lastUpdate = update;
        emit positionUpdated(update);
    }
}
