#include "externaltemperatureoptions.h"
#include "ui_externaltemperatureoptions.h"
#include "settings.h"

ExternalTemperatureOptions::ExternalTemperatureOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ExternalTemperatureOptions)
{
    ui->setupUi(this);
    ui->tempCheckBox->setChecked(restoreExternalTemperatureEnabled());
}

ExternalTemperatureOptions::~ExternalTemperatureOptions()
{
    delete ui;
}

void ExternalTemperatureOptions::on_tempCheckBox_released()
{
    if (ui->tempCheckBox->isChecked()) {
        saveExternalTemperatureEnabled(true);
    }
    else {
        saveExternalTemperatureEnabled(false);
    }
}
