#include "passwordmanagerpage.h"
#include "ui_passwordmanagerpage.h"
#include "debug.h"
#include "core.h"
#include "passwordmanager.h"
#include "popuppasswordform.h"

PasswordManagerPage::PasswordManagerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PasswordManagerPage)
{
    ui->setupUi(this);
    _popupPasswordForm = new PopupPasswordForm(this);
    ASSERT(connect(_popupPasswordForm, &PopupPasswordForm::passwordEntered, this, &PasswordManagerPage::onNewPassword));
    updatePasswords();

    // TODO: make password reset
    ui->resetEKASUIPassword->hide();
    ui->resetFactoryPassword->hide();
    ui->resetLabPassword->hide();
    ui->resetManagerPassword->hide();

    ui->changeFactoryPassword->hide();
    ui->factoryPassword->hide();
    ui->factoryPasswordInfoLabel->hide();

    ui->changeManagerPassword->hide();
    ui->managerPassword->hide();
    ui->managerTitleLabel->hide();
}

PasswordManagerPage::~PasswordManagerPage()
{
    delete ui;
}

void PasswordManagerPage::updatePasswords()
{
    Core& core = Core::instance();
    PasswordManager* manager = core.getPasswordManager();
    fillStars(manager, factoryPermissions, ui->factoryPassword);
    fillStars(manager, labPermissions, ui->labPassword);
    fillStars(manager, managerPermissions, ui->managerPassword);
    fillStars(manager, ekasuiPermissions, ui->ekasuiPassword);
}

void PasswordManagerPage::fillStars(PasswordManager* manager, DevicePermissions permission, QLabel* label)
{
    int passSize = manager->getPasswordForPermission(permission).size();
    QString stars;
    for (int i = 0; i < passSize; ++i) {
        stars.append('*');
    }
    label->setText(stars);
}

void PasswordManagerPage::makePopup(PasswordManager* manager, DevicePermissions permission, const QString& label)
{
    _popupPasswordForm->setCurrentTruePassword(permission, manager->getPasswordForPermission(permission));
    _popupPasswordForm->setPasswordMode(label, true);
    _popupPasswordForm->popup();
}

void PasswordManagerPage::on_changeFactoryPassword_released()
{
    makePopup(Core::instance().getPasswordManager(), factoryPermissions, tr("Enter old factory password:"));
}

void PasswordManagerPage::on_resetFactoryPassword_released()
{
}

void PasswordManagerPage::on_changeLabPassword_released()
{
    makePopup(Core::instance().getPasswordManager(), labPermissions, tr("Enter old lab password:"));
}

void PasswordManagerPage::on_resetLabPassword_released()
{
}

void PasswordManagerPage::on_changeManagerPassword_released()
{
    makePopup(Core::instance().getPasswordManager(), managerPermissions, tr("Enter old manager password:"));
}

void PasswordManagerPage::on_resetManagerPassword_released()
{
}

void PasswordManagerPage::on_changeEKASUIPassword_released()
{
    makePopup(Core::instance().getPasswordManager(), ekasuiPermissions, tr("Enter old ekasui password:"));
}

void PasswordManagerPage::on_resetEKASUIPassword_released()
{
}

void PasswordManagerPage::onNewPassword()
{
    Core& core = Core::instance();
    core.getPasswordManager()->setPasswordForPermission(_popupPasswordForm->currentPermission(), _popupPasswordForm->newPassword());
    updatePasswords();
}
