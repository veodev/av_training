#ifndef GATEINTERVALSYMBOL_H
#define GATEINTERVALSYMBOL_H

#include <qwt_interval_symbol.h>

class GateIntervalSymbol : public QwtIntervalSymbol
{
public:
    GateIntervalSymbol();
    virtual ~GateIntervalSymbol();

    virtual void draw(QPainter*, Qt::Orientation, const QPointF& from, const QPointF& to) const;
};

#endif  // GATEINTERVALSYMBOL_H
