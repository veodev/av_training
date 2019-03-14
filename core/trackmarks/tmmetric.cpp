#include "tmmetric.h"

TMMetric::TMMetric()
{
}

void TMMetric::reset()
{
    _meter = 0;
}

int TMMetric::getM() const
{
    return _meter;
}
