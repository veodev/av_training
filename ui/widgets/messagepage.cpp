#include "messagepage.h"
#include "ui_messagepage.h"

#include "debug.h"

MessagePage::MessagePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MessagePage)
    , _closure(0)
{
    ui->setupUi(this);
    ui->infoLabel->hide();
}

MessagePage::~MessagePage()
{
    delete ui;
}

void MessagePage::setInfoLabel(const QString& info)
{
    ui->infoLabel->setText(info);
    ui->infoLabel->show();
}

void MessagePage::clearInfoLabel()
{
    ui->infoLabel->setText("");
    ui->infoLabel->hide();
}

bool MessagePage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void MessagePage::on_okButton_released()
{
    clearInfoLabel();
    emit leaveTheWidget();
    if (_closure != 0) {
        _closure->execute();
        delete _closure;
        _closure = 0;
    }
}

void MessagePage::on_cancelButton_released()
{
    clearInfoLabel();
    emit leaveTheWidget();
}
