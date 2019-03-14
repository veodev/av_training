#ifndef TMRUSSIAN_H
#define TMRUSSIAN_H

#include "trackmarks.h"

class TMRussian : public TrackMarks
{
    int _pk;
    int _km;
    int _postKm[2];
    int _postPk[2];

public:
    void updatePost();
    QString getString();
    QString getHmString();
    QString getKmString();
    QString getMeterString();
    void syncPole(int a, int b);
    void addMeter(int meter);
    void setPostKm(unsigned char n, int value);
    void setPostPk(unsigned char n, int value);
    int getPostKm(unsigned char n) const;
    int getPostPk(unsigned char n) const;
    void resetMeter();
    void reset();
    void next();
    void prev();
    TMRussian();
    ~TMRussian();
    int getPk() const;
    int getKm() const;
    int getM() const;
    void setPk(int pk);
    void setKm(int km);
    void setM(int m);
    TMRussian& operator=(const TMRussian& other)
    {
        if (this == &other) {
            return *this;
        }
        _km = other._km;
        _pk = other._pk;
        _postKm[0] = other._postKm[0];
        _postKm[1] = other._postKm[1];
        _postPk[0] = other._postPk[0];
        _postPk[1] = other._postPk[1];
        _direction = other._direction;
        return *this;
    }

    bool checkNotification(const Notification& notification, int distanceMeter);
};

#endif  // TMRUSSIAN_H
