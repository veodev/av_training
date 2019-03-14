#ifndef DEVICECONFIG_MANUALSET_H
#define DEVICECONFIG_MANUALSET_H

#include "DeviceConfig.h"


class cDeviceConfig_ManualSet: public cDeviceConfig
{

public:
    cDeviceConfig_ManualSet(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_ManualSet(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_MANUALSET_H */
