#include <QPainter>

#include "qwt_painter.h"
#include "gateintervalsymbol.h"

GateIntervalSymbol::GateIntervalSymbol()
    : QwtIntervalSymbol(UserSymbol)
{
}

GateIntervalSymbol::~GateIntervalSymbol()
{
}

void GateIntervalSymbol::draw(QPainter* painter, Qt::Orientation orientation, const QPointF& from, const QPointF& to) const
{
    Q_UNUSED(orientation);

    QPointF p1 = from;
    QPointF p2 = to;
    if (QwtPainter::roundingAlignment(painter)) {
        p1 = p1.toPoint();
        p2 = p2.toPoint();
    }

    QwtPainter::drawLine(painter, p1, p2);

    const qreal halfWidth = width() / 2;
    QPointF point;

    if (p1.x() > p2.x()) {
        QPointF tmp(p1);
        p1 = p2;
        p2 = tmp;
    }

    point.setX(p1.x() - halfWidth);
    point.setY(p1.y() - halfWidth);
    QwtPainter::drawLine(painter, point, p1);
    point.setY(p1.y() + halfWidth);
    QwtPainter::drawLine(painter, point, p1);

    point.setX(p2.x() + halfWidth);
    point.setY(p2.y() - halfWidth);
    QwtPainter::drawLine(painter, point, p2);
    point.setY(p2.y() + halfWidth);
    QwtPainter::drawLine(painter, point, p2);
}
