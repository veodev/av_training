#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QStringList>

class BluetoothManager : public QObject
{
    Q_OBJECT
protected:
    bool _status;
    QStringList _scanList;
    QStringList _pairedList;
signals:
    void bluetoothStatusChanged(bool);
    void scanListChanged(QStringList);
    void pairingListChanged(QStringList);
    void connectionStatus(bool);
    void setDeviceAsAudio();
    void scanStarted();
    void scanFinished();

public slots:
    virtual void checkStatus() = 0;

public:
    BluetoothManager()
        : _status(false)
    {
    }
    virtual void init() = 0;
    virtual void enableBluetooth(bool enable) = 0;
    virtual bool getBluetoothStatus() const = 0;

    virtual void startScan() = 0;
    virtual QStringList getScanList() = 0;
    virtual QStringList getPairedList() = 0;
    virtual bool pair(const QString& macaddr) = 0;
    virtual void unpair(const QString& macaddr) = 0;
    virtual void addAudioDevice(const QString& macaddr) = 0;
    virtual void resetDevice() = 0;
    virtual bool checkConnectionBtDeviceStatus() = 0;
};

#endif  // BLUETOOTHMANAGER_H
