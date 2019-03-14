#include <QColor>
#include <QPointF>
#include <QVector>
#include <QRect>
#include <QDebug>

#include "bscangates.h"

BScanGates::BScanGates(QWidget* parent)
    : QWidget(parent)
    , _isIndividualColors(false)
    , _betweenOffset(0)
    , _horizontalOffset(0)
    , _maxGateSize(0)
    , _firstGateColor(Qt::black)
    , _secondGateColor(Qt::black)
    , _firstGateIndividualColor(Qt::black)
    , _secondGateIndividualColor(Qt::black)
    , _bgColor(Qt::white)
{
    setGateLocation();
}

BScanGates::~BScanGates() {}

void BScanGates::setGateLocation(BScanGates::Location value)
{
    switch (value) {
    case InLine:
        _betweenOffset = 0;
        _horizontalOffset = HORIZONTAL_OFFSET;
        break;
    case Parallel:
        _betweenOffset = HORIZONTAL_SPACING;
        _horizontalOffset = HORIZONTAL_OFFSET;
        break;
    }
}

void BScanGates::setGate(unsigned short start, unsigned short end, int gateIndex, const QColor& color, const QColor& individualColor)
{
    if (gateIndex == 0) {
        _gates.at(0).clear();
        _gates.at(0).emplace_back(start, end);
        _firstGateIndividualColor = individualColor;
        _firstGateColor = color;
    }
    else if (gateIndex == 1) {
        _gates.at(1).clear();
        _gates.at(1).emplace_back(start, end);
        _secondGateIndividualColor = individualColor;
        _secondGateColor = color;
    }
    else {
        qDebug() << "Unknown gate index" << gateIndex;
        Q_ASSERT(false);
    }
    update();
}

void BScanGates::setMaxGateSize(unsigned short size)
{
    _maxGateSize = size;
}

void BScanGates::resetGates()
{
    for (auto& gate : _gates) {
        gate.clear();
    }
    update();
}

void BScanGates::setBackgroundColor(const QColor& color)
{
    _bgColor = color;
    update();
}

int BScanGates::getMaxGateSize()
{
    return _maxGateSize;
}

void BScanGates::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    int height = this->height();
    int width = this->width();
    painter.fillRect(QRect(0, 0, width, height), _bgColor);

    painter.setPen(_isIndividualColors ? _firstGateIndividualColor : _firstGateColor);

    for (const auto& gate : _gates.at(0)) {
        float heightStart = height - static_cast<float>(gate.first) / static_cast<float>(_maxGateSize) * height;
        float heightEnd = height - static_cast<float>(gate.second) / static_cast<float>(_maxGateSize) * height;
        QLineF line(_horizontalOffset, heightStart, _horizontalOffset, heightEnd);
        QLineF lineStart(_horizontalOffset - 1, heightStart, _horizontalOffset + 1, heightStart);
        QLineF lineEnd(_horizontalOffset - 1, heightEnd, _horizontalOffset + 1, heightEnd);
        painter.drawLine(line);
        painter.drawLine(lineStart);
        painter.drawLine(lineEnd);
    }

    painter.setPen(_isIndividualColors ? _secondGateIndividualColor : _secondGateColor);

    for (const auto& gate : _gates.at(1)) {
        float heightStart = height - static_cast<float>(gate.first) / static_cast<float>(_maxGateSize) * height;
        float heightEnd = height - static_cast<float>(gate.second) / static_cast<float>(_maxGateSize) * height;
        QLineF line(_horizontalOffset + _betweenOffset, heightStart, _horizontalOffset + _betweenOffset, heightEnd);
        QLineF lineStart(_horizontalOffset - 1 + _betweenOffset, heightStart, _horizontalOffset + 1 + _betweenOffset, heightStart);
        QLineF lineEnd(_horizontalOffset - 1 + _betweenOffset, heightEnd, _horizontalOffset + 1 + _betweenOffset, heightEnd);
        painter.drawLine(line);
        painter.drawLine(lineStart);
        painter.drawLine(lineEnd);
    }

    painter.end();
}

void BScanGates::setSecondGateIndividualColor(const QColor& secondGateIndividualColor)
{
    _secondGateIndividualColor = secondGateIndividualColor;
}

void BScanGates::setFirstGateIndividualColor(const QColor& firstGateIndividualColor)
{
    _firstGateIndividualColor = firstGateIndividualColor;
}

void BScanGates::setIsIndividualColors(bool isIndividualColors)
{
    _isIndividualColors = isIndividualColors;
    update();
}

void BScanGates::setSecondGateColor(const QColor& secondGateColor)
{
    _secondGateColor = secondGateColor;
}

void BScanGates::setFirstGateColor(const QColor& firstGateColor)
{
    _firstGateColor = firstGateColor;
}
