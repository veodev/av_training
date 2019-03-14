#ifndef UTSVU_POWERMANAGEMENT_H
#define UTSVU_POWERMANAGEMENT_H

#include "powermanagement.h"
#include <vector>

class UtsvuPowermanagement : public Powermanagement
{
public:
    UtsvuPowermanagement();
    ~UtsvuPowermanagement();

    static bool isSupported();
    bool update() override;

    double batteryVoltage() override;
    double batteryPercent() override;
    double voltage5v();
    double voltage12v();
    double voltage3v3();
    double temperatureSoc() override;

private:
    bool readPmicReg(int reg, int* pdata);
    bool writePmicReg(int reg, int data);
    bool bitopPmicReg(int reg, int cmd, int bitnum);
    bool processAdcConvert(void);

private:
    int _pmicFd;
    short _adcData[8];

    double _U_3V3;
    double _U_LICELL;
    double _U_5V;
    double _U_12V;
    double _U_VBAT;
    double _T_die;
    std::vector<double> _U_VBAT_LPF;
};

#endif  // UTSVU_POWERMANAGEMENT_H
