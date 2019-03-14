#include <QResizeEvent>

#include "controlswidget.h"

ControlsWidget::ControlsWidget(QWidget* parent)
    : QWidget(parent)
{
}

ControlsWidget::~ControlsWidget()
{
}


void ControlsWidget::resizeEvent(QResizeEvent* event)
{
    emit widthChanged(event->size().width());
    QWidget::resizeEvent(event);
}
