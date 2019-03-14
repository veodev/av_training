#ifndef TMMETRIC_H
#define TMMETRIC_H

#include "trackmarks/trackmarks.h"

class TMMetric : public TrackMarks
{
public:
    TMMetric();
    virtual void reset();
    virtual void addMeter(int meter) = 0;
    int getM() const;
};

#endif  // TMMETRIC_H
