#ifndef NMEAPOSITIONINFOSOURCE_H
#define NMEAPOSITIONINFOSOURCE_H

#include <QGeoPositionInfoSource>

#include "nmeareader.h"

class NmeaPositionInfoSource : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    explicit NmeaPositionInfoSource(QObject* parent = 0);
    ~NmeaPositionInfoSource();

    Error error() const;
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    int minimumUpdateInterval() const;
    PositioningMethods supportedPositioningMethods() const;

public slots:
    void requestUpdate(int timeout = 0);
    void startUpdates();
    void stopUpdates();

private:
    friend class NmeaReader;
    void notifyNewUpdate(QGeoPositionInfo& update, bool fixStatus);

private:
    bool _isUpdateStarted;
    QGeoPositionInfo _lastUpdate;
    QGeoPositionInfoSource::Error _error;
};

#endif  // NMEAPOSITIONINFOSOURCE_H
