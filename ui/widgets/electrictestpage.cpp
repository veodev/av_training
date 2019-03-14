#include "electrictestpage.h"
#include "ui_electrictestpage.h"

#include <QDebug>

ElectricTestPage::ElectricTestPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ElectricTestPage)
{
    ui->setupUi(this);
    _textEdit = ui->plainTextEdit;
    ui->stopElTestButton->setEnabled(false);
}

ElectricTestPage::~ElectricTestPage()
{
    delete ui;
}

void ElectricTestPage::startElectricTestAfterCrash()
{
    on_startElTestButton_released();
}

void ElectricTestPage::stopElectricTest()
{
    on_stopElTestButton_released();
}

bool ElectricTestPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void ElectricTestPage::setCountCrashes(int count)
{
    ui->countCrashesLineEdit->setText(QString::number(count));
    count == 0 ? ui->crashesLabel->setPixmap(QPixmap(":/good.png")) : ui->crashesLabel->setPixmap(QPixmap(":/not_good.png"));
}

void ElectricTestPage::setCountCduReboot(int count)
{
    ui->countRebootLineEdit->setText(QString::number(count));
    count == 0 ? ui->cduRebootLabel->setPixmap(QPixmap(":/good.png")) : ui->cduRebootLabel->setPixmap(QPixmap(":/not_good.png"));
}

void ElectricTestPage::setCountUmuDisconnections(int count)
{
    ui->countUmuDisconnetionsLineEdit->setText(QString::number(count));
    count == 0 ? ui->umuDisconnectLabel->setPixmap(QPixmap(":/good.png")) : ui->umuDisconnectLabel->setPixmap(QPixmap(":/not_good.png"));
}

QPlainTextEdit* ElectricTestPage::getViewObject()
{
    return _textEdit;
}

void ElectricTestPage::on_startElTestButton_released()
{
    ui->infoLabel->setText(tr("Electric test is started."));
    ui->startElTestButton->setEnabled(false);
    ui->stopElTestButton->setEnabled(true);
    emit doStartElTest();
}

void ElectricTestPage::on_stopElTestButton_released()
{
    ui->infoLabel->setText(tr("Electric test is stopped."));
    ui->startElTestButton->setEnabled(true);
    ui->stopElTestButton->setEnabled(false);
    emit doStopElTest();
}

void ElectricTestPage::on_closePageButton_released()
{
    this->hide();
}
