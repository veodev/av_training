#include "geoposition.h"
#include "debug.h"

#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>
#include <QAndroidJniEnvironment>
#include <QtAndroidExtras>

const int ANTENNA_STATUS_CHECK_INTERVAL_MS = 3000;

GeoPosition::GeoPosition(QObject* parent)
    : QObject(parent)
    , _prevStatus(AntennaStatusUnknown)
    , _antennaStatusTimer(this)
{
    QAndroidJniObject::callStaticMethod<void>("com/radioavionica/avicon15/PositionClass", "enableGps", "(Landroid/content/Context;)V", QtAndroid::androidContext().object());

    qRegisterMetaType<GeoPosition::AntennaStatus>("GeoPosition::AntennaStatus");

    QGeoPositionInfoSource* positionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
    ASSERT(connect(positionInfoSource, &QGeoPositionInfoSource::positionUpdated, this, &GeoPosition::positionUpdated));
    positionInfoSource->startUpdates();

    QGeoSatelliteInfoSource* satellitesInfoSource = QGeoSatelliteInfoSource::createDefaultSource(this);
    ASSERT(connect(satellitesInfoSource, &QGeoSatelliteInfoSource::satellitesInViewUpdated, this, &GeoPosition::satellitesInViewUpdated));
    ASSERT(connect(satellitesInfoSource, &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &GeoPosition::satellitesInUseUpdated));
    satellitesInfoSource->startUpdates();

    ASSERT(connect(&_antennaStatusTimer, &QTimer::timeout, this, &GeoPosition::antennaStatusTimeout));
    _antennaStatusTimer.start(ANTENNA_STATUS_CHECK_INTERVAL_MS);
}

GeoPosition::~GeoPosition()
{
    qDebug() << "Deleting geoposition...";
    _antennaStatusTimer.stop();
    disconnect(&_antennaStatusTimer, &QTimer::timeout, this, &GeoPosition::antennaStatusTimeout);
    qDebug() << "Geoposition deleted!";
}

GeoPosition* GeoPosition::instance()
{
    static GeoPosition _instance;
    return &_instance;
}

GeoPosition::AntennaStatus GeoPosition::antennaStatus()
{
    AntennaStatus status = AntennaStatusUnknown;

    bool value = QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon15/PositionClass", "isGpsEnabled", "(Landroid/content/Context;)Z", QtAndroid::androidContext().object());
    if (value) {
        status = AntennaStatusConnected;
    }
    else {
        status = AntennaStatusDisconnected;
    }

    return status;
}

void GeoPosition::antennaStatusTimeout()
{
    AntennaStatus status = antennaStatus();
    if (status != _prevStatus) {
        emit antennaStatusChanged(status);
    }
}
