#include "systempasswordform.h"
#include "ui_systempasswordform.h"
#include "core.h"
#include "settings.h"

SystemPasswordForm::SystemPasswordForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SystemPasswordForm)
{
    ui->setupUi(this);
    _change = false;
    ui->infoLabel->setText("");
}

SystemPasswordForm::~SystemPasswordForm()
{
    delete ui;
}

void SystemPasswordForm::on_pushButton_released()
{
    ui->passwordEdit->insert("1");
}

void SystemPasswordForm::on_pushButton_2_released()
{
    ui->passwordEdit->insert("2");
}

void SystemPasswordForm::on_pushButton_3_released()
{
    ui->passwordEdit->insert("3");
}

void SystemPasswordForm::on_pushButton_4_released()
{
    ui->passwordEdit->insert("4");
}

void SystemPasswordForm::on_pushButton_5_released()
{
    ui->passwordEdit->insert("5");
}

void SystemPasswordForm::on_pushButton_6_released()
{
    ui->passwordEdit->insert("6");
}

void SystemPasswordForm::on_pushButton_7_released()
{
    ui->passwordEdit->insert("7");
}

void SystemPasswordForm::on_pushButton_8_released()
{
    ui->passwordEdit->insert("8");
}

void SystemPasswordForm::on_pushButton_9_released()
{
    ui->passwordEdit->insert("9");
}

void SystemPasswordForm::on_pushButton_zero_released()
{
    ui->passwordEdit->insert("0");
}

void SystemPasswordForm::on_clearButton_released()
{
    ui->passwordEdit->clear();
    ui->infoLabel->setText("");
    ui->infoLabel->setStyleSheet("");
}

void SystemPasswordForm::on_enterButton_released()
{
}

void SystemPasswordForm::changePassButton(bool change)
{
    ui->infoLabel->setText("");
    _change = change;
}
