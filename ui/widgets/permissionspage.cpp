#include "permissionspage.h"
#include "ui_permissionspage.h"
#include "core.h"
#include "debug.h"
#include "passwordmanager.h"

PermissionsPage::PermissionsPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PermissionsPage)
{
    ui->setupUi(this);
    _popupPasswordForm = new PopupPasswordForm(this);
    ASSERT(connect(_popupPasswordForm, &PopupPasswordForm::passwordEntered, this, &PermissionsPage::onPopupPasswordEntered));


    // TODO: remove?
    ui->managerPermissionsTitle->hide();
    ui->managerPermissionButton->hide();
    ui->managerPermissionLabel->hide();

    updatePermissions();
}

PermissionsPage::~PermissionsPage()
{
    delete ui;
}

void PermissionsPage::setDeviceType(DeviceType type)
{
    switch (type) {
    case Avicon15:
        ui->ekasuiPermissionButton->hide();
        ui->ekasuiPermissionLabel->hide();
        ui->ekasuiPermissionsTitle->hide();
        ui->labPermissionButton->hide();
        ui->labPermissionLabel->hide();
        ui->labPermissionsTitle->hide();
        break;
    default:
        break;
    }
}

void PermissionsPage::updatePermissions()
{
    Core& core = Core::instance();

    const auto& permissions = core.getCurrentPermissions();
    configureLabel(permissions.find(factoryPermissions) != permissions.end(), ui->factoryPermissionLabel, ui->factoryPermissionButton);
    configureLabel(permissions.find(labPermissions) != permissions.end(), ui->labPermissionLabel, ui->labPermissionButton);
    configureLabel(permissions.find(managerPermissions) != permissions.end(), ui->managerPermissionLabel, ui->managerPermissionButton);
    configureLabel(permissions.find(ekasuiPermissions) != permissions.end(), ui->ekasuiPermissionLabel, ui->ekasuiPermissionButton);
}

void PermissionsPage::configureLabel(bool active, QLabel* label, QPushButton* button)
{
    if (active) {
        label->setText(tr("Active"));
        label->setStyleSheet("color: green;");
        button->setText(tr("Disable"));
    }
    else {
        label->setText(tr("Inactive"));
        label->setStyleSheet("color: red;");
        button->setText(tr("Enable"));
    }
}

void PermissionsPage::spawnPasswordPopup(DevicePermissions permission)
{
    Core& core = Core::instance();
    const auto& permissions = core.getCurrentPermissions();
    if (permissions.find(permission) != permissions.end()) {
        core.setPermission(permission, false);
    }
    else {
        _popupPasswordForm->setCurrentTruePassword(permission, core.getPasswordManager()->getPasswordForPermission(permission));
        _popupPasswordForm->popup();
    }

    updatePermissions();
}

void PermissionsPage::on_configurePasswordsButton_released()
{
    emit configurePasswordsPressed();
}

void PermissionsPage::on_factoryPermissionButton_released()
{
    spawnPasswordPopup(factoryPermissions);
}

void PermissionsPage::on_labPermissionButton_released()
{
    spawnPasswordPopup(labPermissions);
}

void PermissionsPage::on_managerPermissionButton_released()
{
    spawnPasswordPopup(managerPermissions);
}

void PermissionsPage::on_ekasuiPermissionButton_released()
{
    spawnPasswordPopup(ekasuiPermissions);
}

void PermissionsPage::onPopupPasswordEntered()
{
    Core& core = Core::instance();
    core.setPermission(_popupPasswordForm->currentPermission(), true);
    updatePermissions();
}
