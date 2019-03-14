#ifndef BSCANGATES_H
#define BSCANGATES_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <array>

class QColor;
class QPointF;

#define HORIZONTAL_OFFSET 2
#define HORIZONTAL_SPACING 5
#define VERTICAL_SPACING 1
#define LINEWIDTH 1

class BScanGates : public QWidget
{
    Q_OBJECT

    struct GatePoint
    {
        unsigned short first;
        unsigned short second;
        GatePoint(unsigned short f, unsigned short s)
            : first(f)
            , second(s)
        {
        }
        GatePoint()
            : first(0)
            , second(0)
        {
        }
    };

public:
    enum Location
    {
        InLine,
        Parallel
    };

public:
    explicit BScanGates(QWidget* parent = nullptr);
    ~BScanGates();

    void setGateLocation(Location value = InLine);
    void setGate(unsigned short start, unsigned short end, int gateIndex, const QColor& color = Qt::black, const QColor& individualColor = Qt::black);
    void setMaxGateSize(unsigned short size);
    void resetGates();
    void setBackgroundColor(const QColor& color);
    int getMaxGateSize();

    void setFirstGateColor(const QColor& firstGateColor);
    void setSecondGateColor(const QColor& secondGateColor);

    void setIsIndividualColors(bool isIndividualColors);

    void setFirstGateIndividualColor(const QColor& firstGateIndividualColor);
    void setSecondGateIndividualColor(const QColor& secondGateIndividualColor);

protected:
    void paintEvent(QPaintEvent* event);

private:
    bool _isIndividualColors;
    unsigned char _betweenOffset;
    unsigned char _horizontalOffset;
    unsigned short _maxGateSize;

    QColor _firstGateColor;
    QColor _secondGateColor;
    QColor _firstGateIndividualColor;
    QColor _secondGateIndividualColor;
    QColor _bgColor;
    std::array<std::vector<GatePoint>, 2> _gates;
};

#endif  // BSCANGATES_H
