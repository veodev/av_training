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
    static GeoPosition* instance(const QString& serialPortName);

    void setSerialPortName(const QString& serialPortName);
    AntennaStatus antennaStatus();

signals:
    void positionUpdated(const QGeoPositionInfo& info);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& satellites);
    void antennaStatusChanged(GeoPosition::AntennaStatus antennaStatus);

private:
    explicit GeoPosition(QObject* parent = 0);
    ~GeoPosition();

    QIODevice* getSerialPort();

private slots:
    void antennaStatusTimeout();

private:
    AntennaStatus _prevStatus;
    QString _serialPortName;
    QTimer _antennaStatusTimer;
};

#endif  // GEOPOSITION_H
