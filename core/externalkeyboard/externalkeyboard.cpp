#include <QFile>
#include <QDebug>

#include "externalkeyboard.h"
#include "debug.h"
#include "gpio.h"

ExternalKeyboard::ExternalKeyboard(QObject * parent)
    : QObject(parent)
{
    qDebug() << "External keyboard created!";
    GpioKey * key = new GpioKey();
    key->gpio = new GpioInput();
    key->gpio->setPortNumber(0);
    key->isPressedPrev = false;
    key->keyCode = 0;
    _keys.push_back(key);
    key = new GpioKey();
    key->gpio = new GpioInput();
    key->gpio->setPortNumber(2);
    key->isPressedPrev = false;
    key->keyCode = 1;
    _keys.push_back(key);
    key = new GpioKey();
    key->gpio = new GpioInput();
    key->gpio->setPortNumber(3);
    key->isPressedPrev = false;
    key->keyCode = 2;
    _keys.push_back(key);

    _timer.setInterval(250);
    ASSERT(connect(&_timer, &QTimer::timeout, this, &ExternalKeyboard::tick));
    _timer.start();
}

ExternalKeyboard::~ExternalKeyboard() {
    qDebug() << "Deleting external keyboard...";
    disconnect(&_timer, &QTimer::timeout, this, &ExternalKeyboard::tick);
    _timer.stop();
    for(GpioKey * key : _keys) {
        delete key;
    }
    qDebug() << "External keyboard deleted!";
}

void ExternalKeyboard::tick()
{
    for(GpioKey * key : _keys) {
        bool value = key->gpio->value();
        if (value == true && key->isPressedPrev == true) {
            key->isPressedPrev = false;
            emit toggled(false, key->keyCode);
        }
        else if (value == false && key->isPressedPrev == false) {
            key->isPressedPrev = true;
            emit toggled(true, key->keyCode);
        }
    }
}

ExternalKeyboard::GpioKey::~GpioKey()
{
    delete gpio;
}
