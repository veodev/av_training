#include <QCoreApplication>
#include <QMouseEvent>

#include "stickypushbutton.h"
#include "styles.h"

StickyPushButton::StickyPushButton(QWidget* parent)
    : QPushButton(parent)
    , _isActive(false)
{
    this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
}

bool StickyPushButton::isActive()
{
    return _isActive;
}

void StickyPushButton::mousePressEvent(QMouseEvent* event)
{
    this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON));
    this->repaint();
    QCoreApplication::processEvents();
    QPushButton::mousePressEvent(event);
}

void StickyPushButton::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->x() < this->width()) && (event->y() < this->height())) {
        if (_isActive == true) {
            _isActive = false;
            this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
        }
        else {
            _isActive = true;
        }
        emit keyClicked();
        this->repaint();
        QCoreApplication::processEvents();
    }
    else {
        if (_isActive == false) {
            this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
        }
    }
    QPushButton::mouseReleaseEvent(event);
}

void StickyPushButton::getKeyPress()
{
}
