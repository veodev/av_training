#include "loadingpage.h"
#include "ui_loadingpage.h"
#include <QDebug>

LoadingPage::LoadingPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LoadingPage)
    , _netSettingsPage(new NetSettingPage())
{
    ui->setupUi(this);
    QObject* qmlPage = reinterpret_cast<QObject*>(ui->quickWidget->rootObject());
    connect(qmlPage, SIGNAL(indicatorPressedAndHold()), this, SLOT(onIndicatorPressedAndHold()));
}

LoadingPage::~LoadingPage()
{
    delete ui;
}

void LoadingPage::onIndicatorPressedAndHold()
{
    _netSettingsPage->setParent(this);
    _netSettingsPage->setGeometry(this->geometry());
    _netSettingsPage->show();
}
