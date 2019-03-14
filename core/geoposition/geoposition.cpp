#include <QSerialPort>
#include <QFile>
#include <QCoreApplication>
#include <QTimer>

#include "geoposition.h"
#include "debug.h"
#include "nmeareader.h"
#include "gpio.h"

GeoPosition::GeoPosition(QObject* parent)
    : QObject(parent)
    , _prevStatus(AntennaStatusUnknown)
    , _serialPortName("/dev/ttymxc1")
    , _antennaStatusTimer(this)
{
    qRegisterMetaType<GeoPosition::AntennaStatus>("GeoPosition::AntennaStatus");
#if defined(IMX_DEVICE)
    NmeaReader* nmeaReader = new NmeaReader(this);
    nmeaReader->setObjectName("nmeaReader");

    if (nmeaReader != nullptr) {
        QIODevice* serialPort = getSerialPort();

        if (serialPort != nullptr) {
            nmeaReader->setDevice(serialPort);

            QGeoPositionInfoSource* positionInfoSource = nmeaReader->positionInfoSource();
            ASSERT(connect(positionInfoSource, &QGeoPositionInfoSource::positionUpdated, this, &GeoPosition::positionUpdated));
            positionInfoSource->startUpdates();

            QGeoSatelliteInfoSource* satellitesInfoSource = nmeaReader->satelliteInfoSource();
            ASSERT(connect(satellitesInfoSource, &QGeoSatelliteInfoSource::satellitesInViewUpdated, this, &GeoPosition::satellitesInViewUpdated));
            ASSERT(connect(satellitesInfoSource, &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &GeoPosition::satellitesInUseUpdated));
            satellitesInfoSource->startUpdates();

            ASSERT(connect(&_antennaStatusTimer, &QTimer::timeout, this, &GeoPosition::antennaStatusTimeout));
            _antennaStatusTimer.setInterval(5000);
            _antennaStatusTimer.setSingleShot(false);
            _antennaStatusTimer.start();
        }
        else {
            nmeaReader->deleteLater();
        }
    }
#endif

    ASSERT(connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &GeoPosition::deleteLater));
}

GeoPosition::~GeoPosition()
{
    qDebug() << "Deleting geoposition...";
#if defined(IMX_DEVICE)
    QList<NmeaReader*> nmeaReaders = findChildren<NmeaReader*>("nmeaReader");
    if (nmeaReaders.count() >= 1) {
        NmeaReader* nmeaReader = nmeaReaders.at(0);
        QGeoPositionInfoSource* positionInfoSource = nmeaReader->positionInfoSource();
        positionInfoSource->stopUpdates();
        QGeoSatelliteInfoSource* satellitesInfoSource = nmeaReader->satelliteInfoSource();
        satellitesInfoSource->stopUpdates();
        nmeaReader->device()->close();
    }
    disconnect(&_antennaStatusTimer, &QTimer::timeout, this, &GeoPosition::antennaStatusTimeout);
    _antennaStatusTimer.stop();
#endif
    qDebug() << "Geoposition deleted!";
}

GeoPosition* GeoPosition::instance(const QString& serialPortName)
{
    static QMap<QString, GeoPosition*> _instances;

    GeoPosition* instance = nullptr;
    if (_instances.contains(serialPortName)) {
        instance = _instances.value(serialPortName);
    }
    else {
        instance = new GeoPosition();
        instance->setSerialPortName(serialPortName);
        _instances[serialPortName] = instance;
    }

    return instance;
}

void GeoPosition::setSerialPortName(const QString& serialPortName)
{
    _serialPortName = serialPortName;
}

GeoPosition::AntennaStatus GeoPosition::antennaStatus()
{
    static GpioInput gpio(1);
    AntennaStatus status = AntennaStatusUnknown;

    bool value = gpio.value();
    if (value) {
        status = AntennaStatusConnected;
    }
    else {
        status = AntennaStatusDisconnected;
    }

    return status;
}

QIODevice* GeoPosition::getSerialPort()
{
    QSerialPort* serialPort = new QSerialPort(this);
    serialPort->clearError();
    serialPort->setPortName(_serialPortName);
    QFile::remove(QString("/var/lock/LCK..%1").arg(serialPort->portName()));  // TODO: avoid this
    if (serialPort->open(QIODevice::ReadOnly) == true) {
        serialPort->setBaudRate(QSerialPort::Baud9600);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        serialPort->setParity(QSerialPort::NoParity);
    }
    else {
        QWARNING << "can't open'" << _serialPortName << "' serial port";
        serialPort->close();
        serialPort->deleteLater();
        serialPort = nullptr;
    }

    return serialPort;
}

void GeoPosition::antennaStatusTimeout()
{
    AntennaStatus status = antennaStatus();
    if (status != _prevStatus) {
        emit antennaStatusChanged(status);
    }
}
