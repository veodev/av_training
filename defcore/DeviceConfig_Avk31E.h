#ifndef DEVICECONFIG_AVK31E_H
#define DEVICECONFIG_AVK31E_H

#include "DeviceConfig.h"

class cDeviceConfig_Avk31E : public cDeviceConfig
{
private:
    unsigned char UMUGain[81];

public:
    cDeviceConfig_Avk31E(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_Avk31E(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_AVK31E_H */
