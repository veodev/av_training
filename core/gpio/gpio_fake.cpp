#include "gpio.h"

#include <QFile>
#include <QDebug>

GpioOutput::GpioOutput()
{
}

GpioOutput::GpioOutput(int portNumber, bool initValue)
{
    Q_UNUSED(portNumber);
    Q_UNUSED(initValue);
}

void GpioOutput::setPortNumber(int portNumber)
{
    Q_UNUSED(portNumber);
}

void GpioOutput::setValue(bool value)
{
    Q_UNUSED(value);
}

GpioInput::GpioInput()
{
}

GpioInput::GpioInput(int portNumber)
{
    Q_UNUSED(portNumber);
}

void GpioInput::setPortNumber(int portNumber)
{
    Q_UNUSED(portNumber);
}

bool GpioInput::value()
{
    return false;
}

bool GpioInput::value(bool& isOk)
{
    Q_UNUSED(isOk);
    return false;
}
