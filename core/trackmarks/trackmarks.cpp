#include "trackmarks.h"


Direction TrackMarks::getDirection() const
{
    return _direction;
}

void TrackMarks::setDirection(const Direction& direction)
{
    _direction = direction;
}

TrackMarks::TrackMarks()
{
    _meter = 0;
    _centimeter = 0;
    _direction = ForwardDirection;
}

TrackMarks::~TrackMarks()
{
}

int TrackMarks::getTotalMeter()
{
    return _meter;
}

void TrackMarks::addMeter(int meter)
{
    if (_direction == ForwardDirection) {
        _meter += meter;
    }
    else if (_direction == BackwardDirection) {
        _meter -= meter;
    }
    else {
        _meter += meter;
    }
}

void TrackMarks::addCentimeter(int cm)
{
    if (_direction == ForwardDirection) {
        _centimeter += cm;
    }
    else if (_direction == BackwardDirection) {
        _centimeter -= cm;
    }
    else {
        _centimeter += cm;
    }
}
