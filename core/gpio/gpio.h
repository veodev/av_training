#ifndef GPIO_H
#define GPIO_H

#include <QString>

class GpioOutput
{
public:
    GpioOutput();
    GpioOutput(int portNumber, bool initValue);
    void setPortNumber(int portNumber);
    void setValue(bool value);
    bool getValue();

protected:
    bool _currentValue;
    QString _fileName;
};

class GpioInput
{
public:
    GpioInput();
    explicit GpioInput(int portNumber);
    void setPortNumber(int portNumber);
    bool value();
    bool value(bool& isOk);

protected:
    QString _fileName;
};

#endif  // GPIO_H
