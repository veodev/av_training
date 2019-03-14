#include "bluetoothmanagerandroid.h"
#include <QtAndroidExtras>
#include "accessories.h"

const int CHECK_A2DP_INTERVAL_MS = 3000;

void BluetoothManagerAndroid::onCheckA2dpConnection()
{
    bool res = QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon15/BluetoothClass", "isBluetoothConnected", "()Z");
    if (res != _isA2dpConnect) {
        _isA2dpConnect = res;
        emit connectionStatus(_isA2dpConnect);
    }
}

BluetoothManagerAndroid::BluetoothManagerAndroid()
    : _isA2dpConnect(false)
{
    enableBluetooth(false);
    delay(1000);
    enableBluetooth(true);
    delay(2000);
    setPairedList();
    QtAndroid::runOnAndroidThread([] {
        QAndroidJniObject jniObject("com/radioavionica/avicon15/BluetoothClass");
        jniObject.callMethod<void>("registerBroadcastReceiver", "(Landroid/content/Context;)V", QtAndroid::androidContext().object());
    });
    _checkConnectionTimer.setInterval(CHECK_A2DP_INTERVAL_MS);
    connect(&_checkConnectionTimer, &QTimer::timeout, this, &BluetoothManagerAndroid::onCheckA2dpConnection);
    QTimer::singleShot(5000, this, &BluetoothManagerAndroid::startCheckConnectionTimer);
}

void BluetoothManagerAndroid::addAudioDevice(const QString& macaddr)
{
    qDebug() << "ADD AUDIO DEVICE!!!" << macaddr;
}

void BluetoothManagerAndroid::enableBluetooth(bool enable)
{
    if (enable) {
        QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon15/BluetoothClass", "enableBluetooth", "()Z");
    }
    else {
        QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon15/BluetoothClass", "disableBluetooth", "()Z");
    }
}

void BluetoothManagerAndroid::unpair(const QString& macaddr)
{
    if (_pairedList.contains(macaddr)) {
        QAndroidJniObject macAddrJni = QAndroidJniObject::fromString(macaddr);
        QAndroidJniObject::callStaticMethod<void>("com/radioavionica/avicon15/BluetoothClass", "unpairDevice", "(Ljava/lang/String;)V", macAddrJni.object<jstring>());
    }
    _pairedList.removeOne(macaddr);
    emit pairingListChanged(_pairedList);
}

void BluetoothManagerAndroid::setPairedList()
{
    QAndroidJniObject resString = QAndroidJniObject::callStaticObjectMethod("com/radioavionica/avicon15/BluetoothClass", "getPairedDevices", "()Ljava/lang/String;");
    QString pairedDevices = resString.toString();
    QStringList splitPairedDevices = pairedDevices.split("|");
    splitPairedDevices.removeLast();
    _pairedList = splitPairedDevices;
}

void BluetoothManagerAndroid::startCheckConnectionTimer()
{
    _checkConnectionTimer.start();
}
