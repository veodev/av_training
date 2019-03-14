#include "operatorselectionwidget.h"
#include "ui_operatorselectionwidget.h"
#include "settings.h"
#include "accessories.h"
#include "virtualkeyboards.h"
#include "popupdialogwidget.h"

OperatorSelectionWidget::OperatorSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::OperatorSelectionWidget)
{
    this->hide();
    ui->setupUi(this);
    ui->operatorsListWidget->clear();
    ui->operatorsListWidget->addItems(restoreOperatorsToSettings());
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    QPalette bgPal = ui->bgWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->bgWidget->setPalette(bgPal);

    QPalette windowPal = ui->windowWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->windowWidget->setPalette(windowPal);
    ui->windowWidget->setAttribute(Qt::WA_NoSystemBackground, false);
}

OperatorSelectionWidget::~OperatorSelectionWidget()
{
    delete ui;
}

void OperatorSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

bool OperatorSelectionWidget::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

QListWidgetItem* OperatorSelectionWidget::getCurrentItem()
{
    return ui->operatorsListWidget->currentItem();
}

QList<QListWidgetItem*> OperatorSelectionWidget::getSelectedItems()
{
    return ui->operatorsListWidget->selectedItems();
}

int OperatorSelectionWidget::getOperatorsCount()
{
    return ui->operatorsListWidget->count();
}

QStringList OperatorSelectionWidget::getAllOperators()
{
    return convertToQStringList(*(ui->operatorsListWidget));
}

void OperatorSelectionWidget::addNewOperator()
{
    QString newOperator = VirtualKeyboards::instance()->value();
    if (newOperator.isEmpty() == false) {
        QList<QListWidgetItem*> items = ui->operatorsListWidget->findItems(newOperator, Qt::MatchExactly | Qt::MatchCaseSensitive);
        if (items.count() == 0) {
            ui->operatorsListWidget->addItem(newOperator);
        }
    }

    saveOperatorsToSettings(getAllOperators());
}

void OperatorSelectionWidget::editOperator()
{
    QString editedOperator = VirtualKeyboards::instance()->value();
    if (editedOperator.isEmpty() == false) {
        QListWidgetItem* item = getCurrentItem();
        if (item != 0) {
            item[0].setText(editedOperator);
        }
    }
    saveOperatorsToSettings(getAllOperators());
}

void OperatorSelectionWidget::deleteOperator()
{
    QListWidgetItem* item = getCurrentItem();
    if (item != 0) {
        QString operatorName = item->text();
        qDeleteAll(getSelectedItems());
        emit operatorDeleted(operatorName);
        saveOperatorsToSettings(getAllOperators());
    }
}

void OperatorSelectionWidget::on_addOperatorButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &OperatorSelectionWidget::addNewOperator);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue("");
    virtualKeyboards->show();
}

void OperatorSelectionWidget::on_deleteOperatorButton_released()
{
    PopupDialogWidget* popup = new PopupDialogWidget(this);
    popup->setMessage(tr("Delete operator?"));
    connect(popup, &PopupDialogWidget::dialogExited, this, &OperatorSelectionWidget::onDeleteDialogExited);
    popup->popup();
}

void OperatorSelectionWidget::on_editOperatorButton_released()
{
    QListWidgetItem* item = ui->operatorsListWidget->currentItem();
    if (item != 0) {
        VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
        virtualKeyboards->setCallback(this, &OperatorSelectionWidget::editOperator);
        virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
        virtualKeyboards->setValidator(VirtualKeyboards::None);
        virtualKeyboards->setValue(item->text());
        virtualKeyboards->show();
    }
}

void OperatorSelectionWidget::on_okOperatorButton_released()
{
    QListWidgetItem* item = ui->operatorsListWidget->currentItem();
    QString operatorName("");
    if (item != 0) {
        operatorName = item->text();
    }
    emit operatorSelected(operatorName);
}

void OperatorSelectionWidget::onDeleteDialogExited(bool status)
{
    if (status) {
        deleteOperator();
    }
}

void OperatorSelectionWidget::on_closeButton_released()
{
    hide();
}
