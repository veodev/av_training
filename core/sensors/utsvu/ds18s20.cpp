#include <QDir>
#include <QDebug>

#include "ds18s20.h"
#include "debug.h"

#define ONEWIRE_DEVICES_DIR  "/sys/bus/w1/devices/"
#define DS18S20_FAMILY_CODE  "10-"
#define ONEWIRE_DEVICES_NAME "/w1_slave"

Ds18s20::Ds18s20()
    : _isInited(false)
{
}

Ds18s20::~Ds18s20()
{
    finit();
}

bool Ds18s20::init(QString &error)
{
    QDir w1Dir(QLatin1String(ONEWIRE_DEVICES_DIR));
    QFileInfoList list = w1Dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString fileName = fileInfo.fileName();
        if (fileName == QLatin1String(".") || fileName == QLatin1String("..")) {
            continue;
        }
        if (fileName.startsWith(QLatin1String(DS18S20_FAMILY_CODE))) {
            _w1DevFileName = fileInfo.absoluteFilePath() + QLatin1String(ONEWIRE_DEVICES_NAME);
            _w1DevFile.setFileName(_w1DevFileName);
            _isInited = true;
            return true;
        }
    }

    error = QLatin1String("device file not found");
    QWARNING << error;
    return false;
}

void Ds18s20::finit()
{
    if (_isInited) {
        _w1DevFile.close();
        _isInited = false;
    }
}

float Ds18s20::temperature(QString &error)
{
    if (_isInited == false) {
        if (init(error) == false) {
            return 0;
        }
    }

    if (_w1DevFile.isOpen()) {
        _w1DevFile.close();
    }

    if (_w1DevFile.open(QFile::ReadOnly) == false) {
        error = QLatin1String("can't open device file name:'") + _w1DevFileName + QLatin1String("' reason:") + _w1DevFile.errorString();
        QCRITICAL << error;
        return 0;
    }

    QByteArray data = _w1DevFile.readAll();
    if (data.size() < 5) {
        error = QString(QLatin1String("too little (%1 bytes) data readed from file %2")).arg(data.size()).arg(_w1DevFileName);
        QWARNING << error;
        return 0;
    }

    if (data.indexOf("YES") == -1) {
        error = QLatin1String("CRC YES not found, or bad CRC");
        QWARNING << error;
        return 0;
    }

    int index = -1;
    if ((index = data.indexOf("t=")) == -1) {
        error = QLatin1String("temperature value not found");
        QWARNING << error;
        return 0;
    }
    index += 2;
    bool ok = false;
    float t = data.mid(index).simplified().toFloat(&ok)/1000;
    if (ok == false) {
        error = QLatin1String("a conversion error occurs");
        QWARNING << error;
        return 0;
    }

    return t;
}
