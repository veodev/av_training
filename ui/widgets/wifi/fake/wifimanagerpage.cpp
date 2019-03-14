#include "wifimanagerpage.h"
#include "ui_wifimanagerpage.h"

WifiManagerPage::WifiManagerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WifiManagerPage)
{
    ui->setupUi(this);
}

WifiManagerPage::~WifiManagerPage()
{
    delete ui;
}

bool WifiManagerPage::isFaked()
{
    return true;
}
