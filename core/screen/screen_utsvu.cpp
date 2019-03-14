#include "screen.h"
#include "gpio.h"

void screenPower(bool enable)
{
    GpioOutput gpio(2, enable);
}
