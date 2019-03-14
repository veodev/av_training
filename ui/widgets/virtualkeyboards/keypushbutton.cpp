#include <QCoreApplication>

#include "keypushbutton.h"
#include "styles.h"

KeyPushButton::KeyPushButton(QWidget* parent)
    : QPushButton(parent)
{
    this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
}

void KeyPushButton::mousePressEvent(QMouseEvent* event)
{
    this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON));
    this->repaint();
    QPushButton::mousePressEvent(event);
    QCoreApplication::processEvents();
}

void KeyPushButton::mouseReleaseEvent(QMouseEvent* event)
{
    this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
    this->repaint();
    QPushButton::mouseReleaseEvent(event);
    QCoreApplication::processEvents();
}
