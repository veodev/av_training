#include "colorscheme.h"
#include <QtXml/QDomElement>

QString ColorScheme::getName() const
{
    return _name;
}

void ColorScheme::setName(const QString& name)
{
    _name = name;
}

ColorScheme::ColorType ColorScheme::getForwardColor() const
{
    return _forwardColor;
}

void ColorScheme::setForwardColor(const ColorType& forwardColor)
{
    _forwardColor = forwardColor;
}

ColorScheme::ColorType ColorScheme::getBackwardColor() const
{
    return _backwardColor;
}

void ColorScheme::setBackwardColor(const ColorType& backwardColor)
{
    _backwardColor = backwardColor;
}

ColorScheme::ColorScheme()
    : _forwardColor(0x0000)
    , _backwardColor(0xFFE0)
{
}

bool ColorScheme::loadFromFile(QFile& file)
{
    Q_UNUSED(file);
    return false;
}

bool ColorScheme::saveToFile(QFile& file)
{
    Q_UNUSED(file);
    return false;
}

ColorScheme::ColorType ColorScheme::getColorForKey(ColorScheme::ColorKey key)
{
    if (_colors.find(key) != _colors.end()) {
        return _colors[key];
    }

    return 0xF00F;
}

void ColorScheme::setColorForKey(ColorScheme::ColorKey key, ColorScheme::ColorType color)
{
    _colors[key] = color;
}

ColorScheme::ColorType ColorScheme::getIndividualColorForKey(CID cid)
{
    if (_individualColors.find(cid) != _individualColors.end()) {
        return _individualColors[cid];
    }

    return 0xF00F;
}

void ColorScheme::setIndividualColorForKey(CID cid, ColorScheme::ColorType color)
{
    _individualColors[cid] = color;
}
