#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "screen.h"

#include <QDebug>

static const float BRIGHTNESS_MIN = 48.0f;
static const float BRIGHTNESS_MAX = 255.0f;

bool setScreenBrightness(double value)
{
    unsigned char brightness = round(BRIGHTNESS_MIN + (((BRIGHTNESS_MAX - BRIGHTNESS_MIN) * static_cast<float>(value)) / 100.0f));
    qDebug() << "UTSVU brightness byte:" << brightness;

    int file;
    int adapter_nr = 0;
    char filename[20];
    int rc = 0;

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        int errsv = errno;
        fprintf(stderr, "ERROR: device file open failes: device = '%s', errno = %s\n", filename, strerror(errsv));
        return false;
    }

    int addr = 0x2c; /* The I2C address */
    if ((rc = ioctl(file, I2C_SLAVE, addr)) < 0) {
        int errsv = errno;
        close(file);
        fprintf(stderr, "ERROR: set slave address faled: address = 0x%x, rc = %d, errno = %s\n", addr, rc, strerror(errsv));
        return false;
    }

    char buf[10];
    buf[0] = 0x00;
    buf[1] = brightness;
    if ((rc = write(file, buf, 2)) != 2) {
        int errsv = errno;
        close(file);
        fprintf(stderr, "ERROR: i2c transaction failed: rc = %d, errno = %s\n", rc, strerror(errsv));
        return false;
    }

    close(file);

    return true;
}

double screenBrightness()
{
    int file;
    int adapter_nr = 0;
    char filename[20];
    int rc = 0;

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        int errsv = errno;
        fprintf(stderr, "ERROR: device file open failes: device = '%s', errno = %s\n", filename, strerror(errsv));
        return -1;
    }

    int addr = 0x2c; /* The I2C address */
    if ((rc = ioctl(file, I2C_SLAVE, addr)) < 0) {
        int errsv = errno;
        close(file);
        fprintf(stderr, "ERROR: set slave address faled: address = 0x%x, rc = %d, errno = %s\n", addr, rc, strerror(errsv));
        return -1;
    }

    char buf[10];
    buf[0] = 0x00;
    buf[1] = 0x00;
    if ((rc = read(file, buf, 2)) != 2) {
        int errsv = errno;
        close(file);
        fprintf(stderr, "ERROR: i2c transaction failed: rc = %d, errno = %s\n", rc, strerror(errsv));
        return -1;
    }

    close(file);

    usleep(10);

    return buf[1];
}
