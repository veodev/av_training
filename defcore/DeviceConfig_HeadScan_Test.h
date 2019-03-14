#ifndef DEVICECONFIG_HEADSCAN_TEST_H
#define DEVICECONFIG_HEADSCAN_TEST_H

#include "DeviceConfig.h"

class cDeviceConfig_HeadScan_Test: public cDeviceConfig
{
private:
    unsigned char UMUGain[81];

public:
    cDeviceConfig_HeadScan_Test(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_HeadScan_Test(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_HEADSCAN_H */
