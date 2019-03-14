#include "indicatorbluetooth.h"
#include "ui_indicatorbluetooth.h"

IndicatorBluetooth::IndicatorBluetooth(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::IndicatorBluetooth)
{
    ui->setupUi(this);
}

IndicatorBluetooth::~IndicatorBluetooth()
{
    delete ui;
}

void IndicatorBluetooth::changeBluetoothStatus(bool isOn)
{
    if (isOn) {
        ui->label->setPixmap(QPixmap(":/bluetooth_on.png"));
    }
    else {
        ui->label->setPixmap(QPixmap(":/bluetooth_off.png"));
    }
}
