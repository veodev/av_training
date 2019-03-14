#include <cstdint>
#include "lm75.h"
#ifdef IMX_DEVICE
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#endif
#include "debug.h"

QString LM75::addr() const
{
    return _addr;
}

void LM75::setAddr(const QString& addr)
{
    _addr = addr;
}

int LM75::bus() const
{
    return _bus;
}

void LM75::setBus(int bus)
{
    _bus = bus;
}

float LM75::getTemperature()
{
    char buf[2];
    buf[0] = 0x00;
    buf[1] = 0x00;

#ifdef IMX_DEVICE
    int fileHandle = -1;
    int rc = 0;
    bool status = false;
    int addr = _addr.remove(0, 2).toUInt(&status, 16);
    fileHandle = open(_fileName.toStdString().c_str(), O_RDWR);
    if (fileHandle < 0) {
        int errsv = errno;
        qDebug() << "LM75 ERROR: device file open failes: device =" << _fileName << ", errno =" << strerror(errsv);
        _isFailed = true;
    }
    else {
        _isFailed = false;
    }

    if ((rc = ioctl(fileHandle, I2C_SLAVE, 0x4f)) < 0) {
        int errsv = errno;
        close(fileHandle);
        qDebug() << "LM75 ERROR: set slave address faled: address =" << addr << ", rc =" << rc << ", errno =" << strerror(errsv);
        _isFailed = true;
    }
    if (!_isFailed) {
        int rc = 0;
        if ((rc = read(fileHandle, buf, 2)) != 2) {
            int errsv = errno;
            close(fileHandle);
            qDebug() << "LM75 ERROR: i2c transaction failed: rc =" << rc << ", errno =" << strerror(errsv);
            _isFailed = true;
            return -99.9f;
        }
    }
    else {
        qDebug() << "LM75 in failure state!";
    }
    close(fileHandle);
#endif
    signed char temperature0 = static_cast<signed char>(buf[0]);
    signed char temperature1 = (buf[1] & 0x80) >> 7;
    float temp = static_cast<float>(temperature0) + 0.5f * static_cast<float>(temperature1);
    return temp;
}

LM75::LM75(int bus, const QString& hexAddr)
    : TemperatureSensor()
    , _bus(bus)
    , _addr(hexAddr)
    , _fileName("")
    , _failCount(0)
{
    qDebug() << "LM75 created! bus:" << _bus << "addr:" << _addr;
    _isFailed = false;
#ifdef IMX_DEVICE
    _fileName = "/dev/i2c-" + QString::number(_bus);
#endif
}

LM75::~LM75()
{
    _isFailed = true;
    qDebug() << "LM75 deleted!";
}
