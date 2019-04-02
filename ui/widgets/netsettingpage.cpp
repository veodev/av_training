#include "netsettingpage.h"
#include "ui_netsettingpage.h"
#include "settings.h"
#include "core.h"

NetSettingPage::NetSettingPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NetSettingPage)
{
    ui->setupUi(this);
    ui->trainingPcIpAddressLineEdit->setText(restoreTrainingPcTcpServerIpAddress());
    ui->trainingPcPortLineEdit->setText(QString::number(restoreTrainingPcTcpServerPort()));
    ui->cduPortLineEdit->setText(QString::number(restoreRcTcpServerPort()));
}

NetSettingPage::~NetSettingPage()
{
    delete ui;
}

void NetSettingPage::on_backButton_released()
{
    saveTrainingPcTcpServerIpAddress(ui->trainingPcIpAddressLineEdit->text());
    saveTrainingPcTcpServerPort(static_cast<quint16>(ui->trainingPcPortLineEdit->text().toInt()));
    saveRcTcpServerPort(static_cast<quint16>(ui->cduPortLineEdit->text().toInt()));
    hide();
    Core::instance().netSettingsChanged();
}
