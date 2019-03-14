#include "bluetoothmanagerpage.h"
#include "ui_bluetoothmanagerpage.h"

#include "core.h"
#include "debug.h"
#include "bluetooth/bluetoothmanager.h"

BluetoothManagerPage::BluetoothManagerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BluetoothManagerPage)
    , _scanningTimer(this)
    , _isConnected(false)
{
    ui->setupUi(this);
    _manager = Core::instance().getBtManager();
    if (_manager != nullptr) {
        ASSERT(connect(_manager, &BluetoothManager::scanListChanged, this, &BluetoothManagerPage::onScanListChanged));
        ASSERT(connect(_manager, &BluetoothManager::pairingListChanged, this, &BluetoothManagerPage::onPairingListChanged));
        ASSERT(connect(_manager, &BluetoothManager::setDeviceAsAudio, this, &BluetoothManagerPage::setDeviceAsAudio));
        ASSERT(connect(_manager, &BluetoothManager::connectionStatus, this, &BluetoothManagerPage::connectionStatus));
        ASSERT(connect(_manager, &BluetoothManager::scanStarted, this, &BluetoothManagerPage::onScanStarted));
        ASSERT(connect(_manager, &BluetoothManager::scanFinished, this, &BluetoothManagerPage::onScanFinished));
        ui->pairedDevicesList->addItems(_manager->getPairedList());
    }
    ui->useDataButton->hide();
#ifdef ANDROID
    ui->useAudioButton->hide();
#endif
    ui->scanningProgressBar->hide();
    _scanningTimer.setInterval(10);
    ASSERT(connect(&_scanningTimer, &QTimer::timeout, this, &BluetoothManagerPage::onTimerTimeout));
}

BluetoothManagerPage::~BluetoothManagerPage()
{
    delete ui;
}

void BluetoothManagerPage::on_scanButton_released()
{
    if (_manager != nullptr) {
        ui->pairButton->setEnabled(false);
        ui->scanButton->setEnabled(false);
        ui->resetBluetoothButton->setEnabled(false);
        ui->scanDevicesList->clear();
        _manager->startScan();
        ui->pairButton->setEnabled(true);
        ui->resetBluetoothButton->setEnabled(true);
    }
}

void BluetoothManagerPage::on_pairButton_released()
{
    if (_manager != nullptr) {
        ui->pairButton->setEnabled(false);
        ui->scanButton->setEnabled(false);
        ui->resetBluetoothButton->setEnabled(false);
        const QList<QListWidgetItem*>& items = ui->scanDevicesList->selectedItems();
        if (!items.empty()) {
            QListWidgetItem* item = items.first();
            Q_ASSERT(item != nullptr);
            const QString& str = item->text();
            const QString& mac = str.right(17);
            ui->pairedDevicesList->clear();
            _manager->pair(mac);
        }
        ui->pairButton->setEnabled(true);
        ui->scanButton->setEnabled(true);
        ui->resetBluetoothButton->setEnabled(true);
        _isConnected = true;
    }
}

void BluetoothManagerPage::onScanListChanged(QStringList list)
{
    if (_manager != nullptr) {
        ui->scanDevicesList->clear();
        ui->scanDevicesList->addItems(list);
    }
}

void BluetoothManagerPage::onPairingListChanged(QStringList list)
{
    if (_manager != nullptr) {
        ui->pairedDevicesList->clear();
        ui->pairedDevicesList->addItems(list);
    }
}

void BluetoothManagerPage::on_resetBluetoothButton_released()
{
    if (_manager != nullptr) {
        ui->pairButton->setEnabled(false);
        ui->scanButton->setEnabled(false);
        ui->resetBluetoothButton->setEnabled(false);
        _manager->resetDevice();
        ui->pairButton->setEnabled(true);
        ui->scanButton->setEnabled(true);
        ui->resetBluetoothButton->setEnabled(true);
        ui->scanDevicesList->clear();
        ui->pairedDevicesList->clear();
        emit connectionStatus(false);
    }
}

void BluetoothManagerPage::on_useAudioButton_released()
{
    const QList<QListWidgetItem*>& items = ui->pairedDevicesList->selectedItems();
    if (!items.empty()) {
        const QString& mac = items.at(0)->text();
        _manager->addAudioDevice(mac);
    }
}

void BluetoothManagerPage::on_useDataButton_released() {}

void BluetoothManagerPage::on_unpairButton_released()
{
    const QList<QListWidgetItem*>& items = ui->pairedDevicesList->selectedItems();
    if (!items.empty()) {
        const QString& mac = items.at(0)->text();
        _manager->unpair(mac);
        emit connectionStatus(false);
    }
}

void BluetoothManagerPage::onScanStarted()
{
    ui->scanButton->setDisabled(true);
    ui->pairedDevicesList->setDisabled(true);
    ui->scanDevicesList->setDisabled(true);
    ui->scanningProgressBar->setValue(0);
    ui->scanningProgressBar->show();
    _scanningTimer.start();
}

void BluetoothManagerPage::onScanFinished()
{
    ui->scanButton->setDisabled(false);
    ui->pairedDevicesList->setDisabled(false);
    ui->scanDevicesList->setDisabled(false);
    ui->scanningProgressBar->hide();
    _scanningTimer.stop();
}

void BluetoothManagerPage::onTimerTimeout()
{
    ui->scanningProgressBar->setRange(0, 0);
}
