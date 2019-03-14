#ifndef TMMETRIC1KM_H
#define TMMETRIC1KM_H


#include <trackmarks/tmmetric.h>

class TMMetric1KM : public TMMetric
{
    int _km;

public:
    TMMetric1KM();
    QString getString();
    QString getHmString();
    QString getKmString();
    QString getMeterString();
    void addMeter(int meter);
    void syncPole(int a, int b);
    void reset();
    int getKm() const;
    void setKm(int km);

    bool checkNotification(const Notification& notification, int distanceMeter);
};

#endif  // TMMETRIC1KM_H
