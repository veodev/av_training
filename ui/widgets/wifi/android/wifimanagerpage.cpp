#include "wifimanagerpage.h"
#include "ui_wifimanagerpage.h"
#include <QtAndroidExtras>
#include "accessories.h"

const int CHECK_WIFI_INTERVAL_MS = 1000;

WifiManagerPage::WifiManagerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WifiManagerPage)
    , _isWifiOn(false)
    , _defaultGateway(QString())
{
    ui->setupUi(this);
    _checkWifiTimer.setInterval(CHECK_WIFI_INTERVAL_MS);
    connect(&_checkWifiTimer, &QTimer::timeout, this, &WifiManagerPage::onCheckWifiTimerTimeout);
}

WifiManagerPage::~WifiManagerPage()
{
    delete ui;
}

QString& WifiManagerPage::getDefaultGetaway()
{
    return _defaultGateway;
}

void WifiManagerPage::setVisible(bool visible)
{
    visible ? _checkWifiTimer.start() : _checkWifiTimer.stop();
    QWidget::setVisible(visible);
}

void WifiManagerPage::on_wifiOnButton_released()
{
    bool res = QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon31/WifiAccess", "enableWifi", "(Landroid/content/Context;)Z", QtAndroid::androidContext().object());
    Q_UNUSED(res);
}

void WifiManagerPage::on_wifiOffButton_released()
{
    ui->networksListWidget->clear();
    bool res = QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon31/WifiAccess", "disableWifi", "(Landroid/content/Context;)Z", QtAndroid::androidContext().object());
    Q_UNUSED(res);
    _isWifiOn = false;
    emit disconnectToWifi();
}

void WifiManagerPage::on_connectButton_released()
{
    QStringList selectedItems = convertToQStringList(ui->networksListWidget->selectedItems());
    if (selectedItems.isEmpty() == false) {
        QString ssid = selectedItems.first();
        QAndroidJniObject ssidJni = QAndroidJniObject::fromString(ssid);
        QAndroidJniObject::callStaticMethod<void>("com/radioavionica/avicon31/WifiAccess", "connectToSsid", "(Landroid/content/Context;Ljava/lang/String;)V", QtAndroid::androidContext().object(), ssidJni.object<jstring>());
        emit connectToWifi();
    }
}

void WifiManagerPage::on_scanButton_released()
{
    ui->networksListWidget->clear();
    QAndroidJniObject resString = QAndroidJniObject::callStaticObjectMethod("com/radioavionica/avicon31/WifiAccess", "scanWifi", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidContext().object());
    QString qtString = resString.toString();
    QStringList splitQtString = qtString.split("///");
    splitQtString.removeLast();
    ui->networksListWidget->addItems(splitQtString);
}

void WifiManagerPage::onCheckWifiTimerTimeout()
{
    bool res = QAndroidJniObject::callStaticMethod<jboolean>("com/radioavionica/avicon31/WifiAccess", "isWifiEnabled", "(Landroid/content/Context;)Z", QtAndroid::androidContext().object());
    if (res) {
        ui->wifiStatusLabel->setStyleSheet("color: green; font: bold;");
        ui->wifiStatusLabel->setText(tr("ON"));
        _isWifiOn = true;
    }
    else {
        ui->wifiStatusLabel->setStyleSheet("color: red; font: bold;");
        ui->wifiStatusLabel->setText(tr("OFF"));
        _isWifiOn = false;
    }

    QAndroidJniObject ipAddressObject = QAndroidJniObject::callStaticObjectMethod("com/radioavionica/avicon31/WifiAccess", "getIpAddress", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidContext().object());
    QAndroidJniObject ssidObject = QAndroidJniObject::callStaticObjectMethod("com/radioavionica/avicon31/WifiAccess", "getSsid", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidContext().object());
    QAndroidJniObject defaultGatewayObject = QAndroidJniObject::callStaticObjectMethod("com/radioavionica/avicon31/WifiAccess", "getDefaultGateway", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidContext().object());
    QString ipAddress = ipAddressObject.toString();
    QString ssid = ssidObject.toString();
    _defaultGateway = defaultGatewayObject.toString();

    ui->connectionInfoPlainTextEdit->clear();
    ui->connectionInfoPlainTextEdit->appendPlainText(QString(tr("SSID: ")) + ssid);
    ui->connectionInfoPlainTextEdit->appendPlainText(QString(tr("IP: ")) + ipAddress);
    ui->connectionInfoPlainTextEdit->appendPlainText(QString(tr("GATEWAY: ")) + _defaultGateway);
}
