#ifndef BLUETOOTHMANAGERANDROID_H
#define BLUETOOTHMANAGERANDROID_H

#include "bluetoothmanagerqt.h"
#include <QtBluetooth/QtBluetooth>
#include <QDebug>
#include <QTimer>

class BluetoothManagerAndroid : public BluetoothManagerQt
{
    Q_OBJECT
    QTimer _checkConnectionTimer;
    bool _isA2dpConnect;

private slots:
    void onCheckA2dpConnection();

public:
    BluetoothManagerAndroid();
    void addAudioDevice(const QString& macaddr) override;
    void enableBluetooth(bool enable) override;
    void unpair(const QString& macaddr) override;

private:
    void setPairedList();
    void startCheckConnectionTimer();
};

#endif  // BLUETOOTHMANAGERANDROID_H
