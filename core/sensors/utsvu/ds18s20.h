#ifndef DS18S20_H
#define DS18S20_H

#include <QFile>
#include <QString>

class Ds18s20
{
public:
    Ds18s20();
    ~Ds18s20();

    bool init(QString &error);
    void finit();
    float temperature(QString &error);

private:
    QFile _w1DevFile;
    QString _w1DevFileName;
    bool _isInited;
};

#endif // DS18S20_H
