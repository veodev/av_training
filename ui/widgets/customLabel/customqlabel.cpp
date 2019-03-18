#include "customqlabel.h"
#include <QDebug>

CustomQLabel::CustomQLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent)
{
}

CustomQLabel::~CustomQLabel() {}

void CustomQLabel::setIndexAndSide(int index, int side)
{
    _index = index;
    _side = side;
}

int CustomQLabel::getIndex()
{
    return _index;
}

int CustomQLabel::getSide()
{
    return _side;
}

void CustomQLabel::mousePressEvent(QMouseEvent* me)
{
    Q_UNUSED(me);
    emit clicked();
}
