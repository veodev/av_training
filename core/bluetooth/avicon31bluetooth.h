#ifndef AVICON31BLUETOOTH_H
#define AVICON31BLUETOOTH_H

#include <QProcess>
#include "bluetoothmanager.h"

class Avicon31Bluetooth : public BluetoothManager
{
    Q_OBJECT
private:
    bool _dbusStatus;
    bool _bluetoothdStatus;
    bool _hciDeviceStatus;

private:
    bool checkDBus();
    bool checkBluetoothd();
    bool checkHCIDevice();
    int runProcess(const QString& prog);
    void waitForBluetooth();
    void removeAllDevices();
    QStringList getDBusDevices();
    void removeDBusDevice(const QString& device);

    QString getDevice();

public:
    Avicon31Bluetooth();

signals:
    void dbusStatusChanged(bool);
    void bluetoothdStatusChanged(bool);
    void hciDeviceStatusChanged(bool);
    void connectionStatus(bool);
    void setDeviceAsAudio();
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

#endif  // AVICON31BLUETOOTH_H
