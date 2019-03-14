#ifndef DEVICECONFIG_AVK31_H
#define DEVICECONFIG_AVK31_H

#include "DeviceConfig.h"

class cDeviceConfig_Avk31 : public cDeviceConfig
{
private:
    unsigned char UMUGain[81];

public:
    cDeviceConfig_Avk31(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_Avk31(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_AVK31_H */
