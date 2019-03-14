#ifndef BLUETOOTHMANAGERQT_H
#define BLUETOOTHMANAGERQT_H

#include "bluetoothmanager.h"
#include <QtBluetooth/QtBluetooth>

class BluetoothManagerQt : public BluetoothManager
{
    Q_OBJECT

    QBluetoothDeviceDiscoveryAgent* _discoveryAgent;
    QBluetoothLocalDevice* _localDevice;
    std::vector<QBluetoothDeviceInfo> _foundDevices;

private:
    bool pairWithDevice(const QBluetoothDeviceInfo& info);
    bool unpairDevice(const QBluetoothDeviceInfo& info);
    bool checkDeviceCapabilities(const QBluetoothDeviceInfo& info);
    void changeMacInConfig(const QString& mac);
private slots:
    void addDevice(const QBluetoothDeviceInfo& info);
    void onScanFinished();
    void pairingDone(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing);
    void onDeviceConnected(const QBluetoothAddress& address);
    void onDeviceDisconnected(const QBluetoothAddress& address);
    void onError(QBluetoothLocalDevice::Error error);
    void onPairingDisplayConfirmation(const QBluetoothAddress& address, QString pin);

public:
    BluetoothManagerQt();

    // BluetoothManager interface
public slots:
    void checkStatus();

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

#endif  // BLUETOOTHMANAGERQT_H
