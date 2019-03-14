#include <QMouseEvent>

#include "statusbar.h"

StatusBar::StatusBar(QWidget* parent)
    : QWidget(parent)
{
}

StatusBar::~StatusBar()
{
}

void StatusBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->y() > this->height()) {
        emit clicked();
    }
}
