#include "spinboxcolor.h"

SpinBoxColor::SpinBoxColor(QWidget* parent)
    : SpinBoxBase(parent)
{
    _index = 0;
    enableCaption(true);
    displayValue();
}

SpinBoxColor::~SpinBoxColor() {}

void SpinBoxColor::addColor(const QColor& color)
{
    _colors.push_back(color);
}

void SpinBoxColor::clearColors()
{
    _colors.clear();
}

void SpinBoxColor::setIndex(int index)
{
    _index = index;
    setColor(_colors[_index]);
}

void SpinBoxColor::displayValue()
{
    if (_index >= 0 && _index < _colors.size()) {
        setColor(_colors[_index]);
        emit colorChanged(_colors[_index], _index);
    }
}

void SpinBoxColor::nextValue()
{
    if (_index < _colors.size()) {
        _index++;
    }
    displayValue();
}

void SpinBoxColor::prevValue()
{
    if (_index >= 0) {
        _index--;
    }
    displayValue();
}

bool SpinBoxColor::isMinimumReached()
{
    return _index <= 0;
}

bool SpinBoxColor::isMaximumReached()
{
    return _index >= _colors.size();
}
