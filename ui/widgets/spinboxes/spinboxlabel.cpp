#include "spinboxlabel.h"

SpinBoxLabel::SpinBoxLabel(QWidget* parent)
    : QLabel(parent)
{
}

SpinBoxLabel::~SpinBoxLabel()
{
}

void SpinBoxLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    QLabel::mousePressEvent(event);
}
