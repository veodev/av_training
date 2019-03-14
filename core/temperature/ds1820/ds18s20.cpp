#include <QDir>
#include <QDebug>

#include "ds18s20.h"
#include "debug.h"

#define ONEWIRE_DEVICES_DIR "/sys/bus/w1/devices/"
#define DS18S20_FAMILY_CODE "10-"
#define ONEWIRE_DEVICES_NAME "/w1_slave"

Ds18s20::Ds18s20()
    : _isInited(false)
{
    init();
}

Ds18s20::~Ds18s20()
{
    finit();
}

bool Ds18s20::init()
{
#ifdef IMX_DEVICE
    QDir w1Dir(QLatin1String(ONEWIRE_DEVICES_DIR));
    QFileInfoList list = w1Dir.entryInfoList();
    for (const auto& fileInfo : list) {
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

    qDebug() << QLatin1String("device file not found");
#endif
    return false;
}

void Ds18s20::finit()
{
    if (_isInited) {
        _w1DevFile.close();
        _isInited = false;
    }
}

float Ds18s20::getTemperature()
{
    if (!_isInited) {
        if (!init()) {
            _isFailed = true;
            return 0;
        }
    }

    if (_w1DevFile.isOpen()) {
        _w1DevFile.close();
    }

    if (!_w1DevFile.open(QFile::ReadOnly)) {
        _isFailed = true;
        qDebug() << "can't open device file name:'" << _w1DevFileName << "' reason:" << _w1DevFile.errorString();
        return 0;
    }

    const QByteArray& data = _w1DevFile.readAll();
    if (data.size() < 5) {
        _isFailed = true;
        qDebug() << QString("too little (%1 bytes) data readed from file %2").arg(data.size()).arg(_w1DevFileName);
        return 0;
    }

    if (data.indexOf("YES") == -1) {
        _isFailed = true;
        qDebug() << "CRC YES not found, or bad CRC";
        return 0;
    }

    int index = -1;
    if ((index = data.indexOf("t=")) == -1) {
        _isFailed = true;
        qDebug() << "temperature value not found";
        return 0;
    }
    index += 2;
    bool ok = false;
    float t = data.mid(index).simplified().toFloat(&ok) / 1000.0f;
    if (!ok) {
        _isFailed = true;
        qDebug() << QLatin1String("a conversion error occurs");
        return 0;
    }

    _isFailed = false;
    return t;
}
