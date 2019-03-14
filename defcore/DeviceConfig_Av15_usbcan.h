#ifndef DEVICECONFIG_AV15_USBCAN_H
#define DEVICECONFIG_AV15_USBCAN_H

#include "DeviceConfig.h"

class cDeviceConfig_Av15_usbcan: public cDeviceConfig
{
public:
    cDeviceConfig_Av15_usbcan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
	~cDeviceConfig_Av15_usbcan(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_AV15_USBAN_H */
