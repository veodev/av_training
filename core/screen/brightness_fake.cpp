#include "screen.h"

static double _screenBrightness = 100;

bool setScreenBrightness(double value)
{
    _screenBrightness = value;

    return true;
}

double screenBrightness()
{
    return _screenBrightness;
}
