#include "tmrussian.h"

QString TMRussian::getString()
{
    return QString::number(_km) + " км " + QString::number(_pk) + " пк " + QString::number(_meter) + " м";
}

QString TMRussian::getHmString()
{
    int totalMeters = _km * 1000 + _pk * 100 + _meter;
    return QString("%1+%2").arg(totalMeters / 100).arg(totalMeters % 100);
}

QString TMRussian::getKmString()
{
    int totalMeters = _km * 1000 + _pk * 100 + _meter;
    return QString("%1,%2 км").arg(_km).arg(totalMeters % 1000);
}

QString TMRussian::getMeterString()
{
    int totalMeters = _km * 1000 + _pk * 100 + _meter;
    return QString("%1 м").arg(totalMeters);
}

void TMRussian::syncPole(int a, int b)
{
    _km = a;
    _pk = b;
}

void TMRussian::addMeter(int meter)
{
    if (_direction == ForwardDirection) {
        _meter += meter;
    }
    else if (_direction == BackwardDirection) {
        _meter -= meter;
    }
}

TMRussian::TMRussian()
{
    _pk = 0;
    _km = 0;
    _postKm[0] = 0;
    _postKm[1] = 0;

    _postPk[0] = 0;
    _postPk[1] = 0;
}

TMRussian::~TMRussian() {}

int TMRussian::getPk() const
{
    return _pk;
}

int TMRussian::getKm() const
{
    return _km;
}

int TMRussian::getM() const
{
    return _meter;
}

void TMRussian::setPk(int pk)
{
    _pk = pk;
}

void TMRussian::setKm(int km)
{
    _km = km;
}

void TMRussian::setM(int m)
{
    _meter = m;
}

bool TMRussian::checkNotification(const Notification& notification, int distanceMeter)
{
    if (notification.getKm() == _km) {
        if (notification.getPk() == _pk) {
            if (notification.getMetre() < (_meter + distanceMeter) && notification.getMetre() > (_meter - distanceMeter)) {
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

void TMRussian::updatePost()
{
    if (_direction == ForwardDirection) {
        if (_pk == 1) {
            _postKm[0] = _km - 1;
            _postPk[0] = 10;
            _postPk[1] = 1;
        }
        else {
            _postKm[0] = _km;
            _postPk[0] = _pk - 1;
            _postPk[1] = _pk;
        };
    }
    else if (_direction == BackwardDirection) {
        if (_pk == 10) {
            _postKm[0] = _km + 1;
            _postPk[0] = 1;
            _postPk[1] = 10;
        }
        else {
            _postKm[0] = _km;
            _postPk[0] = _pk + 1;
            _postPk[1] = _pk;
        };
    };
    _postKm[1] = _km;
}


void TMRussian::setPostKm(unsigned char n, int value)
{
    if (n < 2) {
        _postKm[n] = value;
    }
    else {
        return;
    }
}

void TMRussian::setPostPk(unsigned char n, int value)
{
    if (n < 2) {
        _postPk[n] = value;
    }
    else {
        return;
    }
}

int TMRussian::getPostKm(unsigned char n) const
{
    if (n < 2) {
        return _postKm[n];
    }
    else {
        return 0;
    }
}

int TMRussian::getPostPk(unsigned char n) const
{
    if (n < 2) {
        return _postPk[n];
    }
    else {
        return 0;
    }
}

void TMRussian::resetMeter()
{
    if (_direction == ForwardDirection) {
        _meter = 0;
    }
    else if (_direction == BackwardDirection) {
        _meter = 100;
    }
}

void TMRussian::reset()
{
    _meter = 0;
    _km = 0;
    _pk = 0;
    _postKm[0] = 0;
    _postKm[1] = 0;

    _postPk[0] = 0;
    _postPk[1] = 0;
}

void TMRussian::next()
{
    if (_direction == ForwardDirection) {
        _pk++;
        if (_pk == 11) {
            _pk = 1;
            _km++;
        }
    }
    else if (_direction == BackwardDirection) {
        _pk--;
        if (_pk == 0) {
            _pk = 10;
            _km--;
        }
    };
    updatePost();
}

void TMRussian::prev()
{
    if (_direction == ForwardDirection) {
        _pk--;
        if (_pk == 0) {
            _pk = 10;
            _km--;
        }
    }
    else if (_direction == BackwardDirection) {
        _pk++;
        if (_pk == 11) {
            _pk = 1;
            _km++;
        }
    };
    updatePost();
}
