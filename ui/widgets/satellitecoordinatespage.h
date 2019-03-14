#ifndef SATELLITECOORDINATESPAGE_H
#define SATELLITECOORDINATESPAGE_H

#include <QWidget>
#include <QGeoPositionInfo>
#include <QGeoSatelliteInfo>
#include "geoposition.h"

namespace Ui
{
class SatelliteCoordinatesPage;
}

class SatelliteCoordinatesPage : public QWidget
{
    Q_OBJECT

public:
    explicit SatelliteCoordinatesPage(QWidget* parent = 0);
    ~SatelliteCoordinatesPage();

    void setAntennaStatus(GeoPosition::AntennaStatus antennaStatus);
    void setSatellitesInUse(int countSatellites);
    void setGeoPositionInfo(const QGeoPositionInfo& info);

public slots:
    void setVisible(bool visible);

signals:
    void switchToVisibleSatellitesPage();

protected:
    bool event(QEvent* e);

private slots:
    void positionUpdated(const QGeoPositionInfo& info);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& satellites);
    void antennaStatusChanged(GeoPosition::AntennaStatus antennaStatus);
    void on_visibleSatellitesButton_released();

private:
    Ui::SatelliteCoordinatesPage* ui;
    GeoPosition* _geoPosition;
    GeoPosition::AntennaStatus _antennaStatus;
};

#endif  // SATELLITECOORDINATESPAGE_H
