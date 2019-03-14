#ifndef DEVICECONFIG_BUMTest_H
#define DEVICECONFIG_BUMTest_H

#include "DeviceConfig.h"

class cDeviceConfig_BUMTest: public cDeviceConfig
{
private:
    unsigned char UMUGain[81];

public:
    cDeviceConfig_BUMTest(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_BUMTest(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_AVK31_H */
