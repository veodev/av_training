#include "deviceoptions.h"
#include "ui_deviceoptions.h"
#include "settings.h"
#include "coredefinitions.h"

DeviceOptions::DeviceOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DeviceOptions)
{
    ui->setupUi(this);
    ui->deviceTypeSpinBox->addItem("Avicon 31 Default", Avicon31Default);
    ui->deviceTypeSpinBox->addItem("Avicon 31 KZ", Avicon31KZ);
    ui->deviceTypeSpinBox->addItem("Avicon DB", AviconDB);
    ui->deviceTypeSpinBox->addItem("Avicon DBHS", AviconDBHS);
    ui->deviceTypeSpinBox->addItem("Avicon 15", Avicon15);
    ui->deviceTypeSpinBox->addItem("Avicon 31 Estonia", Avicon31Estonia);
    ui->deviceTypeSpinBox->setIndex(restoreDeviceType());
}

DeviceOptions::~DeviceOptions()
{
    delete ui;
}

void DeviceOptions::on_saveButton_released()
{
    saveDeviceType(ui->deviceTypeSpinBox->index());
    switch (ui->deviceTypeSpinBox->userData().toInt()) {
    case Avicon31Default:
        saveLanguage(QString("Russian"));
        saveACVoiceNotificationEnabled(false);
        saveHeadScannerUseState(false);
        saveHeadScannerPageState(false);
        saveRegistrationType(DefaultRegistration);
        saveAutoSensResetEnabled(false);
        saveFiltrationEnable(true);
        saveStateCheckCalibrationAlgorithm(true);
        saveImportantAreaIndicateEnable(false);
        saveEkasuiPageState(true);
        saveTypeViewCoordinateForBScan(KmPkM);
        break;
    case Avicon31KZ:
        saveLanguage(QString("Russian"));
        saveACVoiceNotificationEnabled(false);
        saveHeadScannerUseState(false);
        saveHeadScannerPageState(false);
        saveRegistrationType(DefaultRegistration);
        saveAutoSensResetEnabled(true);
        saveFiltrationEnable(true);
        saveStateCheckCalibrationAlgorithm(true);
        saveImportantAreaIndicateEnable(false);
        saveEkasuiPageState(false);
        saveTypeViewCoordinateForBScan(KmPkM);
        break;
    case AviconDB:
        saveLanguage(QString("English"));
        saveACVoiceNotificationEnabled(false);
        saveHeadScannerUseState(false);
        saveHeadScannerPageState(false);
        saveRegistrationType(DefaultRegistration);
        saveAutoSensResetEnabled(true);
        saveFiltrationEnable(true);
        saveStateCheckCalibrationAlgorithm(false);
        saveImportantAreaIndicateEnable(false);
        saveEkasuiPageState(false);
        saveInternalTempSensorType(0);
        saveExternalTempSensorType(0);
        saveExternalTemperatureEnabled(false);
        break;
    case AviconDBHS:
        saveLanguage(QString("English"));
        break;
    case Avicon15:
        saveLanguage(QString("Russian"));
        saveACVoiceNotificationEnabled(false);
        saveStateAcousticContactControl(false);
        saveHeadScannerUseState(false);
        saveHeadScannerPageState(false);
        saveRegistrationType(DefaultRegistration);
        saveAutoSensResetEnabled(false);
        saveFiltrationEnable(false);
        saveStateCheckCalibrationAlgorithm(false);
        saveImportantAreaIndicateEnable(false);
        saveEkasuiPageState(true);
        saveTypeViewCoordinateForBScan(KmPkM);
        break;
    case Avicon31Estonia:
        saveLanguage(QString("Russian"));
        saveACVoiceNotificationEnabled(false);
        saveHeadScannerUseState(false);   // turn off head scanner
        saveHeadScannerPageState(false);  // turn off head scanner page
        saveRegistrationType(DefaultRegistration);
        saveAutoSensResetEnabled(true);
        saveFiltrationEnable(true);
        saveStateCheckCalibrationAlgorithm(false);
        saveImportantAreaIndicateEnable(false);
        saveEkasuiPageState(false);
        saveInternalTempSensorType(0);
        saveExternalTempSensorType(0);
        saveExternalTemperatureEnabled(false);
        break;
    default:
        break;
    }
}
