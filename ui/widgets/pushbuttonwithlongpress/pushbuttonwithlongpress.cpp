#include <QMouseEvent>

#include "pushbuttonwithlongpress.h"

PushButtonWithLongPress::PushButtonWithLongPress(QWidget * parent)
    : QPushButton(parent)
    , _isTimerOut(false)
{
    _timer.setInterval(1000);
    _timer.setSingleShot(true);
    connect(&_timer, &QTimer::timeout, this, &PushButtonWithLongPress::timeout);
}

void PushButtonWithLongPress::mousePressEvent(QMouseEvent * e)
{
    QAbstractButton::mousePressEvent(e);
    _isTimerOut = false;
    _timer.start();
}

void PushButtonWithLongPress::mouseReleaseEvent(QMouseEvent * e)
{
    _timer.stop();
    if (_isTimerOut == false) {
        QAbstractButton::mouseReleaseEvent(e);
    }
    else {
        setDown(false);
        e->accept();
    }
}

void PushButtonWithLongPress::timeout()
{
    _isTimerOut = true;
    emit longPress();
}
