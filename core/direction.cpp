#include "direction.h"
#include "settings.h"


Direction directionByProbe(int value)
{
    static bool isInvertEncoder = invertEncoder();
    Direction dir = UnknownDirection;

    if (isInvertEncoder == false) {
        if (value < 0) {
            dir = BackwardDirection;
        }
        else {
            dir = ForwardDirection;
        }
    }
    else {
        if (value > 0) {
            dir = BackwardDirection;
        }
        else {
            dir = ForwardDirection;
        }
    }
    return dir;
}

Direction directionByEChannelDir(eChannelDir value)
{
    Direction dir = UnknownDirection;

    switch (value) {
    case cdZoomIn:
        dir = ForwardDirection;
        break;
    case cdZoomOut:
        dir = BackwardDirection;
        break;
    case cdNone:
        dir = UnknownDirection;
        break;
    default:
        break;
    }

    return dir;
}
