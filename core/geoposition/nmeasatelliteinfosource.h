#ifndef NMEASATELLITEINFOSOURCE_H
#define NMEASATELLITEINFOSOURCE_H

#include <QGeoSatelliteInfoSource>

class NmeaSatelliteInfoSource : public QGeoSatelliteInfoSource
{
    Q_OBJECT

public:
    explicit NmeaSatelliteInfoSource(QObject* parent = 0);

    Error error() const;
    int minimumUpdateInterval() const;

public slots:
    void requestUpdate(int timeout = 0);
    void startUpdates();
    void stopUpdates();

private:
    friend class NmeaReader;
    void notifyNewInViewUpdate(const QList<QGeoSatelliteInfo>& update);
    void notifyNewInUseUpdate(const QList<QGeoSatelliteInfo>& update);

private:
    bool _isUpdateStarted;
    QGeoSatelliteInfoSource::Error _error;
};

#endif  // NMEASATELLITEINFOSOURCE_H
