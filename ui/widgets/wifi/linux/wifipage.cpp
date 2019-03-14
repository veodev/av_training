#include <QDebug>

#include "wifipage.h"
#include "ui_wifipage.h"

#include "wifimanager.h"
#include "virtualkeyboards.h"

const int QUALITY_TIMER_INTERVAL_MS = 5000;

WifiPage::WifiPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WifiPage)
    , _wirelessInterface(new WifiManager(this))
    , _qualityTimer(new QTimer(this))
{
    ui->setupUi(this);

    QStringList healer;
    healer << tr("interface") << tr("state") << tr("inet addr") << tr("essid");
    ui->interfacesList->setColumnCount(healer.count());
    ui->interfacesList->setHorizontalHeaderLabels(healer);
    ui->interfacesList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->interfacesList->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->ipAddressLineEdit->setValidator(new QRegExpValidator(QRegExp("\\b((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|$)){4}\\b"), ui->ipAddressLineEdit));

    healer.clear();
    healer << tr("ssid") << tr("signal") << tr("enc.") << tr("channel") << tr("address");
    ui->accessPoints->setColumnCount(healer.count());
    ui->accessPoints->setHorizontalHeaderLabels(healer);
    ui->accessPoints->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->accessPoints->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->accessPoints->setColumnWidth(0, 150);
    ui->accessPoints->setColumnWidth(1, 48);
    ui->accessPoints->setColumnWidth(2, 42);
    ui->accessPoints->setColumnWidth(3, 42);
    ui->accessPoints->setColumnWidth(4, 170);

    _qualityTimer->setInterval(QUALITY_TIMER_INTERVAL_MS);
    connect(_qualityTimer, &QTimer::timeout, this, &WifiPage::onQualityTimerTimeout);
}

WifiPage::~WifiPage()
{
    disconnect(_qualityTimer, &QTimer::timeout, this, &WifiPage::onQualityTimerTimeout);
    if (_qualityTimer->isActive()) {
        _qualityTimer->stop();
    }
    delete ui;
}

void WifiPage::setVisible(bool visible)
{
    if (visible == true) {
        on_interfacesUpdatePushButton_released();
    }

    QWidget::setVisible(visible);
}

void WifiPage::on_interfacesUpdatePushButton_released()
{
    _wirelessInterface->scanInterfaces();
    const QStringList& list = _wirelessInterface->availableInterfaces();
    int count = list.count();
    ui->interfacesList->clearContents();
    ui->interfacesList->setRowCount(count);
    for (unsigned short i = 0; i < count; ++i) {
        ui->interfacesList->setItem(i, 0, new QTableWidgetItem(list[i]));
        ui->interfacesList->setItem(i, 1, new QTableWidgetItem(_wirelessInterface->isInterfaceConnected(i) ? "connected" : "disconnected"));
        ui->interfacesList->setItem(i, 2, new QTableWidgetItem(_wirelessInterface->getInetAddrForInterface(i)));
        ui->interfacesList->setItem(i, 3, new QTableWidgetItem(_wirelessInterface->getEssidForInterface(i)));
    }
    ui->interfacesList->resizeColumnsToContents();
}

void WifiPage::on_scanPushButton_released()
{
    _wirelessInterface->scanAcessPoints();
    int count = _wirelessInterface->apCount();

    ui->accessPoints->clearContents();
    ui->accessPoints->setRowCount(count);
    for (unsigned short i = 0; i < count; ++i) {
        ui->accessPoints->setItem(i, 0, new QTableWidgetItem(_wirelessInterface->ssid(i)));
        ui->accessPoints->setItem(i, 1, new QTableWidgetItem(QString("%1%").arg(int(_wirelessInterface->quality(i) * 100))));
        ui->accessPoints->setItem(i, 2, new QTableWidgetItem(_wirelessInterface->encryption(i)));
        ui->accessPoints->setItem(i, 3, new QTableWidgetItem(QString::number(_wirelessInterface->channel(i))));
        ui->accessPoints->setItem(i, 4, new QTableWidgetItem(_wirelessInterface->address(i)));
    }
}

void WifiPage::on_setIpAddressPushButton_released()
{
    _wirelessInterface->setInetAddress(ui->ipAddressLineEdit->text());
    on_interfacesUpdatePushButton_released();
}

void WifiPage::on_connectPushButton_released()
{
    _wirelessInterface->connectToNetwork(ui->accessPoints->currentRow());
    _qualityTimer->start();
    emit connectToWifi();
}

void WifiPage::on_disconnectPushButton_released()
{
    _wirelessInterface->disconnectFromToNetwork();
    _qualityTimer->stop();
    emit disconnectToWifi();
}

void WifiPage::on_changeIpAddressPushButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::IpAddress);
    virtualKeyboards->setValue(ui->ipAddressLineEdit->text());
    virtualKeyboards->setCallback(this, &WifiPage::changeIpAddrees);
    virtualKeyboards->show();
}

void WifiPage::onQualityTimerTimeout()
{
    qreal percent = _wirelessInterface->getQuality();
    emit qualityPercent(percent);
}

void WifiPage::changeIpAddrees()
{
    ui->ipAddressLineEdit->setText(VirtualKeyboards::instance()->value());
}
