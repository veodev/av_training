#ifndef CDEVICECONFIGAVK31CAN_H
#define CDEVICECONFIGAVK31CAN_H

#include "DeviceConfig.h"

class cDeviceConfig_DB_can : public cDeviceConfig
{
public:
    cDeviceConfig_DB_can(cChannelsTable *channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_DB_can();
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif // CDEVICECONFIGAVK31CAN_H
