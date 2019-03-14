#include "batteryoptions.h"
#include "ui_batteryoptions.h"
#include "settings.h"
#include "debug.h"

BatteryOptions::BatteryOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BatteryOptions)
{
    ui->setupUi(this);
    ui->minimalLevelBatterySpinBox->setSuffix(" %");
    ui->minimalLevelBatterySpinBox->setValue(restoreMinimalBatteryPercent(), false);
    ASSERT(connect(ui->minimalLevelBatterySpinBox, &SpinBoxNumber::valueChanged, this, &BatteryOptions::minimalLevelBatteryChanged));
    ui->enableLogButton->setText(tr("Enable logging"));
    ui->plotWidget->setLoggingEnabled(false);
#if defined(TARGET_AVICON15)
    ui->label_39->setText(tr("The minimal level of UMU battery:"));
#endif
    ui->label->hide();
    ui->enableLogButton->hide();
    ui->plotWidget->hide();
}

BatteryOptions::~BatteryOptions()
{
    delete ui;
}

void BatteryOptions::on_enableLogButton_released()
{
    if (ui->plotWidget->loggingEnabled()) {
        ui->enableLogButton->setText(tr("Enable logging"));
        ui->plotWidget->setLoggingEnabled(false);
    }
    else {
        ui->enableLogButton->setText(tr("Disable logging"));
        ui->plotWidget->setLoggingEnabled(true);
    }
}
