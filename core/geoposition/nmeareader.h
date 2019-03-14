#ifndef NMEAREADER_H
#define NMEAREADER_H

#include <QObject>
#include <QPointer>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

class QIODevice;
class NmeaPositionInfoSource;
class NmeaSatelliteInfoSource;

class NmeaReader : public QObject
{
    Q_OBJECT

public:
    explicit NmeaReader(QObject* parent = 0);

    void setDevice(QIODevice* device);
    QIODevice* device() const;

    QGeoPositionInfoSource* positionInfoSource() const;
    QGeoSatelliteInfoSource* satelliteInfoSource() const;

public Q_SLOTS:
    void readyRead();

private:
    QPointer<QIODevice> _device;
    mutable NmeaPositionInfoSource* _positionInfoSource;
    mutable NmeaSatelliteInfoSource* _satelliteInfoSource;
    QList<QGeoSatelliteInfo> _satellites;
};

#endif  // NMEAREADER_H
