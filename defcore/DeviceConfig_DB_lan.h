#ifndef DEVICECONFIG_AVK31_LAN_H
#define DEVICECONFIG_AVK31_LAN_H

#include "DeviceConfig.h"

class cDeviceConfig_DB_lan: public cDeviceConfig
{
private:
    unsigned char UMUGain[81];

public:
    cDeviceConfig_DB_lan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_DB_lan(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_AVK31_LAN_H */
