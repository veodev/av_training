#include "switchoperatorwidget.h"
#include "ui_switchoperatorwidget.h"
#include "core.h"
#include "registration.h"

SwitchOperatorWidget::SwitchOperatorWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SwitchOperatorWidget)
{
    ui->setupUi(this);
}

SwitchOperatorWidget::~SwitchOperatorWidget()
{
    delete ui;
}

void SwitchOperatorWidget::setOperatorsList(QStringList operators)
{
    ui->operatorsListWidget->clear();
    ui->operatorsListWidget->addItems(operators);
}

void SwitchOperatorWidget::setActiveOperator(QString name)
{
    ui->currentOperatorLabel->setText(name);
    _currentOperator = name;
}

void SwitchOperatorWidget::on_okOperatorButton_released()
{
    QListWidgetItem* item = ui->operatorsListWidget->currentItem();
    QString operatorName("");
    if (item != 0) {
        operatorName = item->text();
        if(operatorName != _currentOperator){
            ui->currentOperatorLabel->setText(operatorName);
            Core::instance().registration().changeOperator(operatorName);
            _currentOperator = operatorName;
        }
    }
}

void SwitchOperatorWidget::on_cancelPushButton_released()
{
    emit closeWidget();
}
