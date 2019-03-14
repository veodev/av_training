#include "indicatorwifi.h"
#include "ui_indicatorwifi.h"

IndicatorWifi::IndicatorWifi(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::IndicatorWifi)
    , _wifi25(":/wifi_25.png")
    , _wifi50(":/wifi_50.png")
    , _wifi75(":/wifi_75.png")
    , _wifi100(":/wifi_100.png")
    , _wifiOff(":/wifi_off.png")

{
    ui->setupUi(this);
}

IndicatorWifi::~IndicatorWifi()
{
    delete ui;
}

void IndicatorWifi::setIndicatorPercent(qreal percent)
{
    if (percent >= 0 && percent <= 25.0f) {
        ui->label->setPixmap(_wifi25);
    }
    else if (percent > 25.0f && percent <= 50.0f) {
        ui->label->setPixmap(_wifi50);
    }
    else if (percent > 50.0f && percent <= 75.0f) {
        ui->label->setPixmap(_wifi75);
    }
    else if (percent > 75.0f && percent <= 100.0f) {
        ui->label->setPixmap(_wifi100);
    }
    else if (percent == -1) {
        ui->label->setPixmap(_wifiOff);
    }
}
