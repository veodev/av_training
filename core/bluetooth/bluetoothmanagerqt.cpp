#include "bluetoothmanagerqt.h"
#include "debug.h"
#include <unistd.h>

bool BluetoothManagerQt::pairWithDevice(const QBluetoothDeviceInfo& info)
{
    qDebug() << "Pairing with device:" << info.address().toString();
    _localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    QThread::msleep(200);
    _localDevice->requestPairing(info.address(), QBluetoothLocalDevice::AuthorizedPaired);
    QThread::msleep(200);
    return true;
}

bool BluetoothManagerQt::unpairDevice(const QBluetoothDeviceInfo& info)
{
    qDebug() << "Unpairing device:" << info.address().toString();
    _localDevice->deviceDisconnected(info.address());
    _localDevice->requestPairing(info.address(), QBluetoothLocalDevice::Unpaired);
    return true;
}

bool BluetoothManagerQt::checkDeviceCapabilities(const QBluetoothDeviceInfo& info)
{
    return info.serviceClasses().testFlag(QBluetoothDeviceInfo::AudioService);
}

void BluetoothManagerQt::changeMacInConfig(const QString& mac)
{
    sync();
    const QString& command = QString("sed -i '/device/d; /type bluetooth/ a\\device ") + mac + QString("' /etc/asound.conf");
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(command.toStdString().c_str());
    process.closeWriteChannel();
    if (!process.waitForFinished(20000)) {
        qDebug() << "AddAudioDevice timeout!";
        process.kill();
        return;
    }
    process.close();
    sync();
}

void BluetoothManagerQt::addDevice(const QBluetoothDeviceInfo& info)
{
    qDebug() << "Add device:" << info.address();
    _scanList.push_back(info.name() + " " + info.address().toString());
    _foundDevices.push_back(info);
    emit scanListChanged(_scanList);
}

void BluetoothManagerQt::onScanFinished()
{
    emit scanFinished();
    qDebug() << "Scan finished!";
}

void BluetoothManagerQt::pairingDone(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing)
{
    _pairedList.clear();
    _pairedList.push_back(address.toString());
    QThread::msleep(200);
    _localDevice->pairingConfirmation(true);

    emit pairingListChanged(_pairedList);

    qDebug() << "Pairing done with:" << address.toString() << "Result:" << pairing;
}

void BluetoothManagerQt::onDeviceConnected(const QBluetoothAddress& address)
{
    qDebug() << "Device connected:" << address;
    emit connectionStatus(true);
}

void BluetoothManagerQt::onDeviceDisconnected(const QBluetoothAddress& address)
{
    qDebug() << "Device disconnected:" << address;
    emit connectionStatus(false);
}

void BluetoothManagerQt::onError(QBluetoothLocalDevice::Error error)
{
    qDebug() << "Local device error:" << error;
}

void BluetoothManagerQt::onPairingDisplayConfirmation(const QBluetoothAddress& address, QString pin)
{
    qDebug() << "Displaying pairing confirmation: " << address.toString() << "pin:" << pin;
    _localDevice->pairingConfirmation(true);
}

BluetoothManagerQt::BluetoothManagerQt()
    : _localDevice(nullptr)
    , _discoveryAgent(nullptr)
{
}

void BluetoothManagerQt::checkStatus() {}

void BluetoothManagerQt::init()
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    _localDevice = new QBluetoothLocalDevice();
    QBluetoothAddress adapterAddress = _localDevice->address();
    qDebug() << "Local device address: " << adapterAddress.toString();
    _localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);

    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    _discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

    ASSERT(connect(_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothManagerQt::addDevice));
    ASSERT(connect(_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothManagerQt::onScanFinished));
    ASSERT(connect(_localDevice, &QBluetoothLocalDevice::pairingFinished, this, &BluetoothManagerQt::pairingDone));
    ASSERT(connect(_localDevice, &QBluetoothLocalDevice::deviceConnected, this, &BluetoothManagerQt::onDeviceConnected));
    ASSERT(connect(_localDevice, &QBluetoothLocalDevice::deviceDisconnected, this, &BluetoothManagerQt::onDeviceDisconnected));
    ASSERT(connect(_localDevice, &QBluetoothLocalDevice::error, this, &BluetoothManagerQt::onError));
    ASSERT(connect(_localDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, this, &BluetoothManagerQt::onPairingDisplayConfirmation));
}

void BluetoothManagerQt::enableBluetooth(bool enable) {}

bool BluetoothManagerQt::getBluetoothStatus() const
{
    return false;
}

void BluetoothManagerQt::startScan()
{
    qDebug() << "Starting scan...";
    _foundDevices.clear();
    _scanList.clear();
    emit scanStarted();
    Q_ASSERT(_discoveryAgent != nullptr);
    _localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    _discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
}

QStringList BluetoothManagerQt::getScanList()
{
    return _scanList;
}

QStringList BluetoothManagerQt::getPairedList()
{
    return _pairedList;
}

bool BluetoothManagerQt::pair(const QString& macaddr)
{
    for (const auto& device : _foundDevices) {
        if (device.isValid() && device.address().toString() == macaddr) {
            return pairWithDevice(device);
        }
    }

    return false;
}

void BluetoothManagerQt::unpair(const QString& macaddr)
{
    for (const auto& device : _foundDevices) {
        if (device.isValid() && device.address().toString() == macaddr) {
            unpairDevice(device);
            return;
        }
    }
}

void BluetoothManagerQt::addAudioDevice(const QString& macaddr)
{
    for (const auto& device : _foundDevices) {
        if (device.isValid() && device.address().toString() == macaddr) {
            if (!checkDeviceCapabilities(device)) {
                return;
            }
            qDebug() << "Device supports audio:" << macaddr << "[" << device.name() << "]";
            break;
        }
    }
    qDebug() << "Adding audio device:" << macaddr;
    QThread::msleep(500);
    changeMacInConfig(macaddr);
    QThread::msleep(500);
    emit setDeviceAsAudio();
}

void BluetoothManagerQt::resetDevice()
{
    changeMacInConfig("00:00:00:00:00:00");
    QThread::msleep(500);
    const auto& list = _localDevice->connectedDevices();
    for (const auto& device : list) {
        qDebug() << "Unpairing device:" << device.toString();
        _localDevice->requestPairing(device, QBluetoothLocalDevice::Unpaired);
    }
    _scanList.clear();
    _foundDevices.clear();
}

bool BluetoothManagerQt::checkConnectionBtDeviceStatus()
{
    return true;
}
