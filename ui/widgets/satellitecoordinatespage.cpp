#include <QtNumeric>

#include "satellitecoordinatespage.h"
#include "ui_satellitecoordinatespage.h"

#include "debug.h"

SatelliteCoordinatesPage::SatelliteCoordinatesPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SatelliteCoordinatesPage)
#if defined TARGET_AVICON31
    , _geoPosition(GeoPosition::instance("/dev/ttymxc1"))
#elif defined TARGET_AVICON15 || defined TARGET_AVICONDB
    , _geoPosition(GeoPosition::instance())
#endif
{
    ui->setupUi(this);
    setAntennaStatus(GeoPosition::AntennaStatusDisconnected);
#if defined TARGET_AVICON31 || defined TARGET_AVICON15
    ASSERT(connect(_geoPosition, &GeoPosition::antennaStatusChanged, this, &SatelliteCoordinatesPage::antennaStatusChanged));
#endif
}

SatelliteCoordinatesPage::~SatelliteCoordinatesPage()
{
    delete ui;
}

void SatelliteCoordinatesPage::setAntennaStatus(GeoPosition::AntennaStatus antennaStatus)
{
    switch (antennaStatus) {
    case GeoPosition::AntennaStatusConnected:
        ui->antannaStatus->setText(tr("Connected"));
        break;
    case GeoPosition::AntennaStatusDisconnected:
        ui->antannaStatus->setText(tr("Disconnected"));
        ui->satellites->clear();
        ui->timestamp->clear();
        ui->speed->clear();
        ui->direction->clear();
        ui->coordinates->clear();
        break;
    default:
        ui->antannaStatus->setText(tr("Unknown status"));
        break;
    }
}

void SatelliteCoordinatesPage::setSatellitesInUse(int countSatellites)
{
    ui->satellites->setText(QString::number(countSatellites));
}

void SatelliteCoordinatesPage::setGeoPositionInfo(const QGeoPositionInfo& info)
{
    positionUpdated(info);
}

void SatelliteCoordinatesPage::setVisible(bool visible)
{
    if (visible == true) {
        ASSERT(connect(_geoPosition, &GeoPosition::positionUpdated, this, &SatelliteCoordinatesPage::positionUpdated));
        ASSERT(connect(_geoPosition, &GeoPosition::satellitesInUseUpdated, this, &SatelliteCoordinatesPage::satellitesInUseUpdated));
    }
    else {
        disconnect(_geoPosition, &GeoPosition::positionUpdated, this, &SatelliteCoordinatesPage::positionUpdated);
        disconnect(_geoPosition, &GeoPosition::satellitesInUseUpdated, this, &SatelliteCoordinatesPage::satellitesInUseUpdated);
    }

    QWidget::setVisible(visible);
}

bool SatelliteCoordinatesPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void SatelliteCoordinatesPage::positionUpdated(const QGeoPositionInfo& info)
{
    QGeoCoordinate coordinate = info.coordinate();
    ui->coordinates->setText(coordinate.toString(QGeoCoordinate::DegreesMinutesSecondsWithHemisphere));

    qreal direction = info.attribute(QGeoPositionInfo::Direction);
    if (::qIsNaN(direction) == false) {
        ui->direction->setText(QString::number(direction));
    }

    qreal speed = info.attribute(QGeoPositionInfo::GroundSpeed);
    if (::qIsNaN(speed) == false) {
        ui->speed->setText(QString::number(speed));
    }

    if (info.timestamp().toString() != "") {
        ui->timestamp->setText(info.timestamp().toString(Qt::LocaleDate));
    }
}

void SatelliteCoordinatesPage::satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& satellites)
{
    if ((satellites.count() > 0) && (satellites.at(0).satelliteSystem() == QGeoSatelliteInfo::GPS)) {
        ui->satellites->setText(QString::number(satellites.count()));
    }
}

void SatelliteCoordinatesPage::antennaStatusChanged(GeoPosition::AntennaStatus antennaStatus)
{
    if (_antennaStatus == antennaStatus) {
        return;
    }
    _antennaStatus = antennaStatus;
    switch (antennaStatus) {
    case GeoPosition::AntennaStatusConnected:
        ui->antannaStatus->setText(tr("Connected"));
        break;
    case GeoPosition::AntennaStatusDisconnected:
        ui->antannaStatus->setText(tr("Disconnected"));
        break;
    default:
        ui->antannaStatus->setText(tr("Unknown status"));
    }
}

void SatelliteCoordinatesPage::on_visibleSatellitesButton_released()
{
    emit switchToVisibleSatellitesPage();
}
