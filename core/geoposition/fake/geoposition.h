#ifndef GEOPOSITION_H
#define GEOPOSITION_H

#include <QObject>
#include <QGeoPositionInfo>
#include <QGeoSatelliteInfo>
#include <QTimer>

class QIODevice;

class GeoPosition : public QObject
{
    Q_OBJECT

public:
    enum AntennaStatus
    {
        AntennaStatusUnknown,
        AntennaStatusConnected,
        AntennaStatusDisconnected
    };

public:
    static GeoPosition* instance();
    AntennaStatus antennaStatus();

signals:
    void positionUpdated(const QGeoPositionInfo& info);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& satellites);
    void antennaStatusChanged(GeoPosition::AntennaStatus antennaStatus);

private:
    explicit GeoPosition(QObject* parent = 0);
    ~GeoPosition();
};

#endif  // GEOPOSITION_H
