#include "avicon31bluetooth.h"
#include <QProcess>
#include <QApplication>
#include <QThread>
#include <QFile>
#include "debug.h"

bool Avicon31Bluetooth::checkDBus()
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write("ps aux | grep dbus-daemon | grep -v grep");
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
        qDebug() << "Check Dbus timeout!";
        return false;
    }
    const QByteArray& output = process.readAllStandardOutput();
    int rc = process.exitCode();
    process.close();
    if (rc != 0) {
        qDebug() << "Check Dbus rc:" << rc;
        return false;
    }
    return !output.isEmpty();
}

bool Avicon31Bluetooth::checkBluetoothd()
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write("ps aux | grep bluetoothd | grep -v grep");
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
        qDebug() << "Check Bluetoothd timeout!";
        return false;
    }
    const QByteArray& output = process.readAllStandardOutput();
    int rc = process.exitCode();
    process.close();
    if (rc != 0) {
        qDebug() << "Check Bluetoothd rc:" << rc;
        return false;
    }
    return !output.isEmpty();
}

bool Avicon31Bluetooth::checkHCIDevice()
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write("hciconfig");
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
        qDebug() << "Check HCI Device timeout!";
        return false;
    }
    const QByteArray& output = process.readAllStandardOutput();
    int rc = process.exitCode();
    process.close();
    if (rc != 0) {
        qDebug() << "Check HCI Device rc:" << rc;
        return false;
    }
    return !output.isEmpty();
}

Avicon31Bluetooth::Avicon31Bluetooth()
    : _dbusStatus(false)
    , _bluetoothdStatus(false)
    , _hciDeviceStatus(false)
{
    qDebug() << "Avicon31Buetooth created!";
}

void Avicon31Bluetooth::init()
{
    _status = false;
}

void Avicon31Bluetooth::checkStatus()
{
    bool dbusStatus = checkDBus();
    if (_dbusStatus != dbusStatus) {
        _dbusStatus = dbusStatus;
        emit dbusStatusChanged(_dbusStatus);
    }

    bool bluetoothdStatus = checkBluetoothd();
    if (_bluetoothdStatus != bluetoothdStatus) {
        _bluetoothdStatus = bluetoothdStatus;
        emit bluetoothdStatusChanged(_bluetoothdStatus);
    }

    bool hciDeviceStatus = checkHCIDevice();
    if (_hciDeviceStatus != hciDeviceStatus) {
        _hciDeviceStatus = hciDeviceStatus;
        emit hciDeviceStatusChanged(_hciDeviceStatus);
    }
}

void Avicon31Bluetooth::enableBluetooth(bool enable)
{
    _status = enable;
    emit bluetoothStatusChanged(enable);
}

bool Avicon31Bluetooth::getBluetoothStatus() const
{
    return _status;
}

void Avicon31Bluetooth::startScan()
{
    _scanList.clear();
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write("hcitool scan");
    process.closeWriteChannel();
    if (!process.waitForFinished(60000)) {
        qDebug() << "Scan timeout!";
        process.kill();
        return;
    }

    const QByteArray& output = process.readAllStandardOutput();
    int scanResult = process.exitCode();
    qDebug() << "OUTPUT: " << output << " exit code: " << scanResult;
    qDebug() << "ERROR: " << process.readAllStandardError();
    process.close();

    if (scanResult != 0) {
        qDebug() << "Scan failed! ec:" << scanResult;
        return;
    }
    if (output.isEmpty()) {
        qDebug() << "EMPTY SCAN";
        _scanList = QStringList();
        emit scanListChanged(_scanList);
        return;
    }
    QString outputStr(output);
    const QStringList& tempList = outputStr.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    QStringList retList;
    for (const QString& str : tempList) {
        QStringList macString = str.split(QRegExp("[\t]"), QString::SkipEmptyParts);
        if (macString.size() == 2) {
            const QString& nameMac = macString.last() + " " + macString.first();
            retList.push_back(nameMac);
        }
        else {
            qDebug() << "Bad mac addr:" << macString;
        }
    }
    _scanList = retList;
    emit scanListChanged(_scanList);
}

QStringList Avicon31Bluetooth::getScanList()
{
    return _scanList;
}

QStringList Avicon31Bluetooth::getPairedList()
{
    return _pairedList;
}

QString Avicon31Bluetooth::getDevice()
{
    const QString& getDeviceCmd = "dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez / org.bluez.Manager.DefaultAdapter";
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(getDeviceCmd.toStdString().c_str());
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
        qDebug() << "getDevice timeout!";
        process.kill();
    }
    const QString& output = process.readAllStandardOutput();
    process.close();

    const QStringList& lines = output.split(QRegExp("[\r\n]"));
    for (const QString& line : lines) {
        if (line.contains("object path")) {
            int from = line.indexOf("\"", 0) + 1;
            int to = line.indexOf("\"", from);
            return line.mid(from, to - from);
        }
    }
    qDebug() << "Bluetooth device not found!";
    return "";
}

bool Avicon31Bluetooth::pair(const QString& macaddr)
{
    qDebug() << "Pair:" << macaddr;
    if (macaddr.size() == 17) {
        QString macDev = macaddr;
        macDev.replace(":", "_");
        qDebug() << "Dev" << macDev;
        const QString& objectPath = getDevice();
        if (objectPath.size() > 0) {
            qDebug() << "Device path:" << objectPath;
            if (runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.RemoveDevice objpath:" + objectPath + "/dev_" + macDev) != 0) {
                qDebug() << "Unable remove device: " << macDev;
            }
            waitForBluetooth();
            int rc = runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.CreatePairedDevice string:" + macaddr + " objpath:/org/bluez/agent string:DisplayYesNo");
            if (rc != 0) {
                waitForBluetooth();
                runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.RemoveDevice objpath:" + objectPath + "/dev_" + macDev);
                waitForBluetooth();
                runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.CreateDevice string:" + macaddr);
                waitForBluetooth();
                rc = runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + "/dev_" + macDev + " org.bluez.Device.DiscoverServices string:");
                if (rc != 0) {
                    qDebug() << "Failed to discover services!";
                    return false;
                }

                _pairedList.push_back(macaddr);
                return true;
            }

            _pairedList.push_back(macaddr);
            return true;
        }

        qDebug() << "Wrong device:" << objectPath;
        return false;
    }

    qDebug() << "Wrong MAC:" << macaddr;
    return false;
}

void Avicon31Bluetooth::unpair(const QString& macaddr)
{
    qDebug() << "Unpairing:" << macaddr;
    if (_pairedList.contains(macaddr)) {
        _pairedList.removeAll(macaddr);
        const QString& objectPath = getDevice();
        QString macDev = macaddr;
        macDev.replace(":", "_");
        if (runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.RemoveDevice objpath:" + objectPath + "/dev_" + macDev) != 0) {
            qDebug() << "Failed to unpair:" << macaddr;
            return;
        }
    }
}

void Avicon31Bluetooth::addAudioDevice(const QString& macaddr)
{
    qDebug() << "Adding audio device:" << macaddr;
    const QString& command = QString("sed -i '/device/d; /type bluetooth/ a\\device ") + macaddr + QString("' /etc/asound.conf");
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(command.toStdString().c_str());
    process.closeWriteChannel();
    if (!process.waitForFinished(60000)) {
        qDebug() << "AddAudioDevice timeout!";
        process.kill();
        return;
    }
    process.close();
    emit setDeviceAsAudio();
}

int Avicon31Bluetooth::runProcess(const QString& prog)
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(prog.toStdString().c_str());
    process.closeWriteChannel();
    process.waitForStarted();
    if (!process.waitForFinished(60000)) {
        qDebug() << "Process timeout:" << prog;
        process.kill();
        return -99;
    }
    int rc = process.exitCode();
    qDebug() << "Command: " << prog << " RC: " << rc;
    qDebug() << "Read STDOUT [" << process.readAllStandardOutput() << "]";
    qDebug() << "Read STDERR [" << process.readAllStandardError() << "]";
    process.close();
    return rc;
}

void Avicon31Bluetooth::waitForBluetooth()
{
    for (int i = 0; i < 200; ++i) {
        QThread::msleep(10);
    }
}

void Avicon31Bluetooth::removeAllDevices()
{
    qDebug() << "Removing all bluetooth devices...";
    const QStringList& devices = getDBusDevices();
    qDebug() << "Found" << devices.size() << "devices";
    for (const QString& device : devices) {
        removeDBusDevice(device);
    }
    _pairedList.clear();
}

QStringList Avicon31Bluetooth::getDBusDevices()
{
    const QString& objectPath = getDevice();
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(QString("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.ListDevices").toStdString().c_str());
    process.closeWriteChannel();
    if (!process.waitForFinished()) {
        qDebug() << "getDBusDevices timeout!";
        process.kill();
        return QStringList();
    }
    const QByteArray& output = process.readAllStandardOutput();
    process.close();
    const QString& outString(output);
    const QStringList& strings = outString.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    QStringList devices;
    for (const QString& str : strings) {
        if (str.contains("object path")) {
            const QStringList& newStrings = str.split('\"', QString::SkipEmptyParts);
            for (const QString& str : newStrings) {
                if (str.contains("bluez")) {
                    devices.push_back(str);
                    break;
                }
            }
        }
    }
    return devices;
}

void Avicon31Bluetooth::removeDBusDevice(const QString& device)
{
    qDebug() << "Removing device:" << device;
    QString objectPath = getDevice();
    if (runProcess("dbus-send --system --print-reply --reply-timeout=25000 --type=method_call --dest=org.bluez  " + objectPath + " org.bluez.Adapter.RemoveDevice objpath:" + device) != 0) {
        qDebug() << "Failed to remove device: " << device;
        return;
    }
}

void Avicon31Bluetooth::resetDevice()
{
    _scanList.clear();
    emit scanListChanged(_scanList);
    removeAllDevices();
}

bool Avicon31Bluetooth::checkConnectionBtDeviceStatus()
{
    const QStringList& devices = getDBusDevices();
    if (devices.empty()) {
        _pairedList.clear();
        _scanList = QStringList();
        emit scanListChanged(_scanList);
        qDebug() << "No paired devices!";
        return false;
    }
    for (const QString& device : devices) {
        QProcess process;
        process.start("sh");
        process.waitForStarted();
        // QString command = "dbus-send --system --print-reply --type=method_call --reply-timeout=8000 --dest=org.bluez " + device + " org.bluez.Device.GetProperties";
        const QString& command = "dbus-send --system --print-reply --type=method_call --reply-timeout=6000 --dest=org.bluez " + device + " org.bluez.Device.DiscoverServices string:";
        process.write(command.toStdString().c_str());
        process.closeWriteChannel();
        process.waitForStarted();
        if (!process.waitForFinished(7000)) {
            qDebug() << "Bluetooth device lost!";
            process.kill();
            _pairedList.clear();
            _scanList.clear();
            emit scanListChanged(_scanList);
            removeDBusDevice(device);
            qDebug() << "No paired devices!";
            return false;
        }
        int rc = process.exitCode();
        process.close();
        if (rc != 0) {
            _pairedList.clear();
            _scanList.clear();
            emit scanListChanged(_scanList);
            removeDBusDevice(device);
            qDebug() << "Bluetooth device lost! rc:" << rc;
            return false;
        }
    }
    return true;
}
