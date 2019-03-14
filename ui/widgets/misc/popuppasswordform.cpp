#include "popuppasswordform.h"
#include "ui_popuppasswordform.h"

PopupPasswordForm::PopupPasswordForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PopupPasswordForm)
    , _currentTruePassword("")
{
    parentWidget = parent;
    ui->setupUi(this);
    QPalette bgPal = ui->backgroundWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->backgroundWidget->setPalette(bgPal);
    QPalette windowPal = ui->passwordWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->passwordWidget->setPalette(windowPal);
    ui->passwordWidget->setAttribute(Qt::WA_NoSystemBackground, false);
    hide();
    _changePasswordMode = false;
    _mode = false;
}

PopupPasswordForm::~PopupPasswordForm()
{
    delete ui;
}

void PopupPasswordForm::popup()
{
    QSize parentSize = parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    raise();
    show();
}

void PopupPasswordForm::on_oneButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "1");
}

void PopupPasswordForm::on_twoButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "2");
}

void PopupPasswordForm::on_threeButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "3");
}

void PopupPasswordForm::on_fourButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "4");
}

void PopupPasswordForm::on_fiveButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "5");
}

void PopupPasswordForm::on_sixButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "6");
}

void PopupPasswordForm::on_sevenButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "7");
}

void PopupPasswordForm::on_eightButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "8");
}

void PopupPasswordForm::on_nineButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "9");
}

void PopupPasswordForm::on_clearButton_released()
{
    ui->passwordInputLineEdit->setText("");
}

void PopupPasswordForm::on_zeroButton_released()
{
    ui->passwordInputLineEdit->setText(ui->passwordInputLineEdit->text() + "0");
}

void PopupPasswordForm::on_enterButton_released()
{
    if (_changePasswordMode) {
        ui->passwordInputLineEdit->setStyleSheet("color: green;");
        _newPassword = ui->passwordInputLineEdit->text();
        ui->passwordInputLineEdit->clear();
        emit passwordEntered();
        hide();
        return;
    }

    if (!_mode) {
        if (_currentTruePassword == ui->passwordInputLineEdit->text()) {
            ui->passwordInputLineEdit->setStyleSheet("color: green;");
            ui->passwordInputLineEdit->clear();
            emit passwordEntered();
            hide();
        }
        else {
            ui->passwordInputLineEdit->setStyleSheet("color: red;");
        }
    }
    else {
        if (_currentTruePassword == ui->passwordInputLineEdit->text()) {
            ui->passwordInputLineEdit->setStyleSheet("color: green;");
            _changePasswordMode = true;
        }
        else {
            ui->passwordInputLineEdit->setStyleSheet("color: red;");
        }
        if (_changePasswordMode) {
            ui->passwordInputLineEdit->setStyleSheet("color: black;");
            ui->topPasswordLabel->setText(tr("Enter new password:"));
            ui->passwordInputLineEdit->clear();
        }
    }
}

void PopupPasswordForm::on_exitButton_released()
{
    ui->passwordInputLineEdit->clear();
    emit exitPressed();
    hide();
}

QString PopupPasswordForm::newPassword() const
{
    return _newPassword;
}

DevicePermissions PopupPasswordForm::currentPermission() const
{
    return _currentPermission;
}

void PopupPasswordForm::setPasswordMode(const QString& modeName, bool isNew)
{
    _changePasswordMode = false;
    ui->topPasswordLabel->setText(modeName);
    _mode = isNew;
}

void PopupPasswordForm::setCurrentTruePassword(DevicePermissions permission, const QString& currentTruePassword)
{
    ui->passwordInputLineEdit->setStyleSheet("color: black;");
    ui->passwordInputLineEdit->clear();
    _currentTruePassword = currentTruePassword;
    _currentPermission = permission;
}
