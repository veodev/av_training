#include "tmmetric1km.h"

int TMMetric1KM::getKm() const
{
    return _km;
}

void TMMetric1KM::setKm(int km)
{
    _km = km;
}

bool TMMetric1KM::checkNotification(const Notification& notification, int distanceMeter)
{
    Q_ASSERT(false);
    Q_UNUSED(notification);
    Q_UNUSED(distanceMeter);
    return false;
}

TMMetric1KM::TMMetric1KM()
{
    _km = 0;
}

QString TMMetric1KM::getString()
{
    QString xxx = QString::number(qAbs(_km));

    if (xxx.size() < 3) {
        int znum = 3 - xxx.size();
        if (znum == 2) {
            xxx.prepend("0");
            xxx.prepend("0");
        }
        else if (znum == 1) {
            xxx.prepend("0");
        }
    }
    if (_km < 0) {
        xxx.prepend("-");
    }

    QString yyy = QString::number(_meter);
    if (yyy.size() < 3) {
        int znum = 3 - yyy.size();
        if (znum == 2) {
            yyy.prepend("0");
            yyy.prepend("0");
        }
        else if (znum == 1) {
            yyy.prepend("0");
        }
    }

    return xxx + "." + yyy;
}

QString TMMetric1KM::getHmString()
{
    return QString();
}

QString TMMetric1KM::getKmString()
{
    return QString();
}

QString TMMetric1KM::getMeterString()
{
    return QString();
}

void TMMetric1KM::addMeter(int meter)
{
    if (_direction == ForwardDirection || _direction == UnknownDirection) {
        _meter += meter;
        if (_meter >= 1000) {
            _meter = 1000 - _meter;
            _km += 1;
        }
        if (_meter < 0) {
            _meter = 1000 + _meter;
            _km -= 1;
        }
    }
    else if (_direction == BackwardDirection) {
        _meter -= meter;
        if (_meter >= 1000) {
            _meter = 1000 - _meter;
            _km += 1;
        }
        if (_meter < 0) {
            _meter = 1000 + _meter;
            _km -= 1;
        }
    }
}

void TMMetric1KM::syncPole(int a, int b)
{
    _km = a;
    _meter = b;
}

void TMMetric1KM::reset()
{
    _km = 0;
    _meter = 0;
}
