#include <QTimer>
#include <QDateTime>
#include "clock.h"
#include "debug.h"

Clock::Clock(QWidget* parent)
    : QLCDNumber(parent)
    , _even(false)
    , _timer(new QTimer(this))
{
    ASSERT(connect(_timer, &QTimer::timeout, this, &Clock::update));
    _timer->setInterval(1000);
    _timer->setSingleShot(false);
    _timer->start();

    update();
}


Clock::~Clock()
{
    disconnect(_timer, &QTimer::timeout, this, &Clock::update);
    _timer->stop();
}

void Clock::update()
{
    const QTime& time = QTime::currentTime();
    const QString& text = _even ? time.toString("hh:mm") : time.toString("hh mm");
    _even = !_even;
    display(text);
}
