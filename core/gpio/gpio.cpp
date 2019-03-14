#include "gpio.h"

#include <QFile>
#include <QDebug>

GpioOutput::GpioOutput()
    : _currentValue(false)
{
}

GpioOutput::GpioOutput(int portNumber, bool initValue)
{
    Q_ASSERT(portNumber < 4);
    setPortNumber(portNumber);
    qDebug() << "GPIO" << _fileName << "init:" << initValue;
    QFile file(_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        qWarning() << "Error of open file '" << _fileName << "':" << file.errorString();
        return;
    }

    file.putChar(initValue ? '1' : '0');
    file.close();
    _currentValue = initValue;
}

void GpioOutput::setPortNumber(int portNumber)
{
    Q_ASSERT(portNumber < 4);
    _fileName = QString("/proc/gpio/gpo-%1").arg(portNumber);
}

void GpioOutput::setValue(bool value)
{
    if (_currentValue != value) {
        qDebug() << "GPIO" << _fileName << "Set to" << value;
        QFile file(_fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
            qWarning() << "Error of open file '" << _fileName << "':" << file.errorString();
            return;
        }

        file.putChar(value ? '1' : '0');
        file.close();
        _currentValue = value;
    }
}

bool GpioOutput::getValue()
{
    QFile file(_fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        qWarning() << "Error of open file '" << _fileName << "':" << file.errorString();
        return false;
    }

    char value = 0;
    bool retVal = false;
    file.getChar(&value);
    retVal = value != '0';
    file.close();
    qDebug() << "GPIO" << _fileName << " read value:" << value << " bool:" << retVal;
    return retVal;
}

GpioInput::GpioInput() {}

GpioInput::GpioInput(int portNumber)
{
    setPortNumber(portNumber);
}

void GpioInput::setPortNumber(int portNumber)
{
    Q_ASSERT(portNumber < 4);
    _fileName = QString("/proc/gpio/gpi-%1").arg(portNumber);
}

bool GpioInput::value()
{
    bool isOk;
    return value(isOk);
}

bool GpioInput::value(bool& isOk)
{
    bool rc = false;
    isOk = false;

    QFile file(_fileName);
    if (file.open(QIODevice::ReadOnly) == false) {
        qWarning() << "Error of open file '" << _fileName << "':" << file.errorString();
        return false;
    }
    const QByteArray& data = file.readAll();
    if (data.length() > 0) {
        if (data.at(0) == '1') {
            rc = true;
            isOk = true;
        }
        else if (data.at(0) == '0') {
            rc = false;
            isOk = true;
        }
        else {
            qDebug() << "GPIOInput unexpected:" << data.at(0);
        }
    }
    else {
        qDebug() << "GPIOInput bad size:" << data.size();
    }
    file.close();

    return rc;
}
