#include <QIODevice>
#include <QGeoPositionInfo>
#include <QGeoSatelliteInfo>

#include "locationutils.h"
#include "nmeareader.h"
#include "nmeapositioninfosource.h"
#include "nmeasatelliteinfosource.h"
#include "debug.h"

NmeaReader::NmeaReader(QObject* parent)
    : QObject(parent)
    , _device(nullptr)
    , _positionInfoSource(nullptr)
    , _satelliteInfoSource(nullptr)
{
}

void NmeaReader::setDevice(QIODevice* device)
{
    if (device != _device) {
        if (_device.isNull() == false) {
            qWarning("QNmeaPositionInfoSource: source device has already been set");
            QObject::disconnect(_device, &QIODevice::readyRead, this, &NmeaReader::readyRead);
        }
        _device = device;
        ASSERT(QObject::connect(_device, &QIODevice::readyRead, this, &NmeaReader::readyRead));
    }
}

QIODevice* NmeaReader::device() const
{
    return _device;
}

QGeoPositionInfoSource* NmeaReader::positionInfoSource() const
{
    if (_positionInfoSource == nullptr) {
        _positionInfoSource = new NmeaPositionInfoSource(const_cast<NmeaReader*>(this));
    }

    return _positionInfoSource;
}

QGeoSatelliteInfoSource* NmeaReader::satelliteInfoSource() const
{
    if (_satelliteInfoSource == nullptr) {
        _satelliteInfoSource = new NmeaSatelliteInfoSource(const_cast<NmeaReader*>(this));
    }

    return _satelliteInfoSource;
}

void NmeaReader::readyRead()
{
    while (_device->canReadLine()) {
        QGeoPositionInfo posInfo;
        QList<QGeoSatelliteInfo> satellites;
        bool hasFix = false;
        bool isCompleted = false;
        bool isInUse = false;

        const QByteArray& buff = _device->readLine(1024);

        if (buff.size() <= 0) {
            continue;
        }
        if (_positionInfoSource != nullptr && LocationUtils::getPosInfoFromNmea(buff.data(), buff.size(), &posInfo, &hasFix)) {
            _positionInfoSource->notifyNewUpdate(posInfo, hasFix);
        }
        else if (_satelliteInfoSource != nullptr && LocationUtils::getSatInfoFromNmea(buff.data(), buff.size(), satellites, isCompleted, isInUse)) {
            if (satellites.count() > 0) {
                if (isInUse == true) {
                    _satelliteInfoSource->notifyNewInUseUpdate(satellites);
                }
                else {
                    _satellites.append(satellites);
                    if (isCompleted) {
                        _satelliteInfoSource->notifyNewInViewUpdate(_satellites);
                        _satellites.clear();
                    }
                }
            }
        }
    }
}
