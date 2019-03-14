#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#define CONFIG_MXC_PMIC_MC13892
#include <linux/pmic_external.h>

#define BITOP_BIT_CLEAR 0
#define BITOP_BIT_SET 1

#include "utsvupowermanagement.h"
#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif
#include "debug.h"

static const char* deviceFilename = "/dev/pmic";
#define BATTERY_VOLTAGE_USE_MIN 9
#define BATTERY_VOLTAGE_USE_MAX 13.8

#define BATTERY_VOLTAGE_COMPENSATION 1.16

UtsvuPowermanagement::UtsvuPowermanagement()
    : _pmicFd(-1)
    , _U_3V3(0)
    , _U_LICELL(0)
    , _U_5V(0)
    , _U_12V(0)
    , _U_VBAT(0)
    , _T_die(0)
{
    ::memset(_adcData, 8, sizeof(short));
}

UtsvuPowermanagement::~UtsvuPowermanagement()
{

}

bool UtsvuPowermanagement::isSupported()
{
    struct stat buf;
    if (::stat(deviceFilename, &buf) != -1) {
        return true;
    }
    return false;
}

bool UtsvuPowermanagement::update()
{
    bool rc = false;
    int mask0, mask1, data;
    _error.clear();

    _pmicFd = ::open(deviceFilename, O_RDWR);
    if (_pmicFd < 0) {
        _error = QString("unable open PMIC device - %1.").arg(deviceFilename);
        QWARNING << _error;
        return false;
    }

    ::memset(_adcData, 8, sizeof(short));

    // Disable all interrupt occurencies to block driver int handler
    if (readPmicReg(REG_INT_MASK0, &mask0) == false) {
        ::close(_pmicFd);
        return false;
    }
    if (readPmicReg(REG_INT_MASK1, &mask1) == false) {
        ::close(_pmicFd);
        return false;
    }
    if (writePmicReg(REG_INT_MASK0, 0xFFFFFF) == false) {
        goto exit_test;
    }
    if (writePmicReg(REG_INT_MASK1, 0xFFFFFF) == false) {
        goto exit_test;
    }

    // Set BUFFEN bit
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_SET, 3) == false) {
        goto exit_test;
    }

    // Measure 3V3 channel
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _U_3V3 = ((double) _adcData[0] / 1023.0) * 2.4 * 2.0;

    // Measure LICELL voltage
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_SET, 0) == false) {
        goto exit_test;
    }
    ::usleep(20000);
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _U_LICELL = ((double) _adcData[6] / 1023.0) * 3.6;

    // Measure die temperature
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_SET, 4) == false) {
        goto exit_test;
    }
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_CLEAR, 5) == false) {
        goto exit_test;
    }
    ::usleep(20000);
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _T_die = 25 + ((double) _adcData[7] - 680.0) * 0.4244;

    // Setup ADC ch7 1/1 divider
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_CLEAR, 4) == false) {
        goto exit_test;
    }
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_CLEAR, 5) == false) {
        goto exit_test;
    }
    if (bitopPmicReg(REG_ADC0, BITOP_BIT_CLEAR, 9) == false) {
        goto exit_test;
    }

    // Measure 5V channel
    if (readPmicReg(REG_POWER_MISC, &data) == false) {
        goto exit_test;
    }
    data = (data & ~0x203F00) | 0x100;
    if (writePmicReg(REG_POWER_MISC, data) == false) {
        goto exit_test;
    }
    ::usleep(20000);
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _U_5V = ((double) _adcData[7] / 1023.0) * 2.4 * (5.0 / 2.0);

    // Measure 12V channel
    if (readPmicReg(REG_POWER_MISC, &data) == false) {
        goto exit_test;
    }
    data = (data & ~0x203F00) | 0x400;
    if (writePmicReg(REG_POWER_MISC, data) == false) {
        goto exit_test;
    }
    ::usleep(20000);
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _U_12V = ((double) _adcData[7] / 1023.0) * 2.4 * 6.0;

    // Measure VBAT channel
    if (readPmicReg(REG_POWER_MISC, &data) == false) {
        goto exit_test;
    }
    data = (data & ~0x203F00) | 0x1000;
    if (writePmicReg(REG_POWER_MISC, data) == false) {
        goto exit_test;
    }
    ::usleep(20000);
    if (processAdcConvert() == false) {
        goto exit_test;
    }
    _U_VBAT = ((double) _adcData[7] / 1024.0) * 2.4 * (321.0 / 20.0) * BATTERY_VOLTAGE_COMPENSATION;

    QDEBUG << QString("U(+3.3V) = %1 V, U(LICELL) = %2 V, T(die) = %3C, U(+5V) = %4 V, U(+12V) = %5 V, U(VBAT) = %6 V").arg(_U_3V3).arg(_U_LICELL).arg(_T_die).arg(_U_5V).arg(_U_12V).arg(_U_VBAT);

    // Voltage analisys
    if ((_U_3V3 < (0.95 * 3.367)) || (_U_3V3 > (1.05 * 3.367))) {
        QWARNING << QString("invalid +3.3V voltage = %1 V, but must be >= %2 V and <= %3 V").arg(_U_3V3).arg(0.95 * 3.367).arg(1.05 * 3.367);
    }
    if (_U_LICELL < 2.4) {
        QWARNING << QString("invalid LICELL voltage = %1 V, but must be >= %2 V").arg(_U_LICELL).arg(2.4);
    }
    if ((_U_5V < (0.91 * 4.988)) || (_U_5V > (1.09 * 4.988))) {
        QWARNING << QString("invalid +5V voltage = %1 V, but must be >= %2 V and <= %3 V").arg(_U_5V).arg(0.91 * 4.988).arg(1.09 * 4.988);
    }
    if ((_U_12V < (0.91 * 11.99)) || (_U_12V > (1.09 * 11.99))) {
        QWARNING << QString("invalid +12V voltage = %1 V, but must be >= %2 V and <= %3 V").arg(_U_12V).arg(0.91 * 11.99).arg(1.09 * 11.99);
    }
    if ((_U_VBAT < BATTERY_VOLTAGE_USE_MIN) || (_U_VBAT > BATTERY_VOLTAGE_USE_MAX)) {
        QWARNING << QString("invalid VBAT voltage = %1 V, but must be >= %2 V and <= %3 V").arg(_U_VBAT).arg(BATTERY_VOLTAGE_USE_MIN).arg(BATTERY_VOLTAGE_USE_MAX);
    }

    rc = true;
exit_test:
    // Restore driver interrupt masks
    writePmicReg(REG_INT_MASK0, mask0);
    writePmicReg(REG_INT_MASK1, mask1);
    ::close(_pmicFd);

    return rc;
}

double UtsvuPowermanagement::batteryVoltage()
{
    if (_U_VBAT_LPF.size() < 16) {
        _U_VBAT_LPF.push_back(_U_VBAT);
        return _U_VBAT;
    }
    else {
        _U_VBAT_LPF.erase(_U_VBAT_LPF.begin());
        _U_VBAT_LPF.push_back(_U_VBAT);
        return std::accumulate(_U_VBAT_LPF.begin(), _U_VBAT_LPF.end(), 0.0, [](double prev, double cur) { return prev + cur; }) / 16.0;
    }
}

double UtsvuPowermanagement::batteryPercent()
{
    double percents = 0;
    if (batteryVoltage() >= _batteryVoltageMaximum) {
        percents = 100;
    }
    else if (batteryVoltage() <= _batteryVoltageMinimum) {
        percents = 0;
    }
    else {
        percents = ((batteryVoltage() - _batteryVoltageMinimum) / (_batteryVoltageMaximum - _batteryVoltageMinimum)) * 100.0;
    }
    return percents;
}

double UtsvuPowermanagement::voltage5v()
{
    return _U_5V;
}

double UtsvuPowermanagement::voltage12v()
{
    return _U_12V;
}

double UtsvuPowermanagement::voltage3v3()
{
    return _U_3V3;
}

double UtsvuPowermanagement::temperatureSoc()
{
    return _T_die;
}

bool UtsvuPowermanagement::readPmicReg(int reg, int* pdata)
{
    register_info reginfo;
    ::memset(&reginfo, 0, sizeof(register_info));
    reginfo.reg = reg;
    reginfo.reg_value = 0;
    if (::ioctl(_pmicFd, PMIC_READ_REG, &reginfo) < 0) {
        _error = QString("Could not read PMIC register - %1").arg(::strerror(errno));
        QWARNING << _error;
        return false;
    }
    *pdata = reginfo.reg_value;
    return true;
}

bool UtsvuPowermanagement::writePmicReg(int reg, int data)
{
    register_info reginfo;
    ::memset(&reginfo, 0, sizeof(register_info));
    reginfo.reg = reg;
    reginfo.reg_value = data;
    if (::ioctl(_pmicFd, PMIC_WRITE_REG, &reginfo) < 0) {
        _error = QString("Could not write PMIC register - %1").arg(::strerror(errno));
        QWARNING << _error;
        return false;
    }
    return true;
}

bool UtsvuPowermanagement::bitopPmicReg(int reg, int cmd, int bitnum)
{
    int data;

    if (readPmicReg(reg, &data) == false) {
        return false;
    }

    switch (cmd) {
    case BITOP_BIT_CLEAR:
        data = data & ~(1 << bitnum);
        break;
    case BITOP_BIT_SET:
        data = data | (1 << bitnum);
        break;
    default:
        return false;
    }
    if (writePmicReg(reg, data) == false) {
        return false;
    }

    return true;
}

bool UtsvuPowermanagement::processAdcConvert(void)
{
    int adc_ctl, status, i, data;

    // Clear all status bits
    if (readPmicReg(REG_INT_STATUS0, &status) == false) {
        return false;
    }
    if (writePmicReg(REG_INT_STATUS0, status) == false) {
        return false;
    }

    adc_ctl = 0x300801;
    // Enable ADC and start 8-channel conversion
    if (writePmicReg(REG_ADC1, adc_ctl) == false) {
        return false;
    }
    ::usleep(10000);
    // Read status bit
    if (readPmicReg(REG_INT_STATUS0, &status) == false) {
        return false;
    }

    if ((status & 0x01) == 0) {
        _error = QString("ADC conversion not complete. Status = %1").arg(status);
        QWARNING << _error;
        return false;
    }

    for (i = 0; i < 4; ++i) {
        // Setup ADA1 and ADA2 values
        adc_ctl = 0x1 | (2 * i) << 5 | (2 * i + 1) << 8;
        if (writePmicReg(REG_ADC1, adc_ctl) == false) {
            return false;
        }
        if (readPmicReg(REG_ADC2, &data) == false) {
            return false;
        }
        _adcData[2 * i] = static_cast<short>(((data & 0xFFF) >> 2));
        _adcData[(2 * i) + 1] = static_cast<short>(((data & 0xFFF000) >> 14));
    }

    return true;
}
