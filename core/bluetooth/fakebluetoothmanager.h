#ifndef FAKEBLUETOOTHMANAGER_H
#define FAKEBLUETOOTHMANAGER_H

#include "bluetoothmanager.h"

class FakeBluetoothManager : public BluetoothManager
{
    Q_OBJECT
public:
    FakeBluetoothManager();

public slots:
    void checkStatus();
    // BluetoothManager interface
public:
    void init();
    void enableBluetooth(bool enable);
    bool getBluetoothStatus() const;
    void startScan();
    QStringList getScanList();
    QStringList getPairedList();
    bool pair(const QString& macaddr);
    void unpair(const QString& macaddr);
    void addAudioDevice(const QString& macaddr);
    void resetDevice();
    bool checkConnectionBtDeviceStatus();
};

#endif  // FAKEBLUETOOTHMANAGER_H
