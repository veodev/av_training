#ifndef DEVICECONFIG_HEADSCAN_H
#define DEVICECONFIG_HEADSCAN_H

#include "DeviceConfig.h"

typedef struct {
  int ScanLength;        // Зона сканирования по горизонтали (полная), мм
  int VerticalStartPos;  // Начальное положение сканера по вертикали, мм
} tHeadScanParams;

typedef tHeadScanParams* pHeadScanParams;

class cDeviceConfig_HeadScan: public cDeviceConfig
{

 private:
    tHeadScanParams Params_;
    unsigned char UMUGain[81];

public:
    cDeviceConfig_HeadScan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold);
    ~cDeviceConfig_HeadScan(void);
    unsigned char dBGain_to_UMUGain(char dBGain);
};

#endif /* DEVICECONFIG_HEADSCAN_H */
