#ifndef BLUETOOTHMANAGERPAGE_H
#define BLUETOOTHMANAGERPAGE_H

#include <QWidget>
#include <QTimer>

class BluetoothManager;

namespace Ui
{
class BluetoothManagerPage;
}

class BluetoothManagerPage : public QWidget
{
    Q_OBJECT

    BluetoothManager* _manager;

public:
    explicit BluetoothManagerPage(QWidget* parent = 0);
    ~BluetoothManagerPage();

signals:
    void connectionStatus(bool);
    void setDeviceAsAudio();

private slots:
    void on_scanButton_released();
    void on_pairButton_released();
    void onScanListChanged(QStringList list);
    void onPairingListChanged(QStringList list);
    void on_resetBluetoothButton_released();
    void on_useAudioButton_released();
    void on_useDataButton_released();
    void on_unpairButton_released();

    void onScanStarted();
    void onScanFinished();
    void onTimerTimeout();

private:
    Ui::BluetoothManagerPage* ui;
    QTimer _scanningTimer;
    bool _isConnected;
};

#endif  // BLUETOOTHMANAGERPAGE_H
