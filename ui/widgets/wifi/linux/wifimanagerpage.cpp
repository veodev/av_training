#include "wifimanagerpage.h"
#include "ui_wifimanagerpage.h"

WifiManagerPage::WifiManagerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WifiManagerPage)
{
    ui->setupUi(this);
    connect(ui->wifiPage, &WifiPage::connectToWifi, this, &WifiManagerPage::connectToWifi);
    connect(ui->wifiPage, &WifiPage::disconnectToWifi, this, &WifiManagerPage::disconnectToWifi);
    connect(ui->wifiPage, &WifiPage::qualityPercent, this, &WifiManagerPage::qualityPercent);
}

WifiManagerPage::~WifiManagerPage()
{
    delete ui;
}

bool WifiManagerPage::isFaked()
{
    return false;
}

QString WifiManagerPage::getDefaultGetaway()
{
    return QString();
}
