#include "fakebluetoothmanager.h"
#include "debug.h"

FakeBluetoothManager::FakeBluetoothManager() {}

void FakeBluetoothManager::init() {}

void FakeBluetoothManager::checkStatus() {}

void FakeBluetoothManager::enableBluetooth(bool enable)
{
    _status = enable;
    emit bluetoothStatusChanged(enable);
}

bool FakeBluetoothManager::getBluetoothStatus() const
{
    return _status;
}

void FakeBluetoothManager::startScan() {}

QStringList FakeBluetoothManager::getScanList()
{
    return QStringList() << QString("fake");
}

QStringList FakeBluetoothManager::getPairedList()
{
    return QStringList() << QString("fake");
}

bool FakeBluetoothManager::pair(const QString& macaddr)
{
    qDebug() << "Fake pairing with:" << macaddr;
    return true;
}

void FakeBluetoothManager::unpair(const QString& macaddr)
{
    qDebug() << "Fake unpairing with:" << macaddr;
}

void FakeBluetoothManager::addAudioDevice(const QString& macaddr)
{
    qDebug() << "Fake add audio device:" << macaddr;
}

void FakeBluetoothManager::resetDevice() {}

bool FakeBluetoothManager::checkConnectionBtDeviceStatus()
{
    return true;
}
