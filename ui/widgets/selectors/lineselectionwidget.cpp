#include "lineselectionwidget.h"
#include "ui_lineselectionwidget.h"
#include "core.h"
#include "settings.h"
#include "accessories.h"
#include "virtualkeyboards.h"
#include "misc/popupdialogwidget.h"
#include "notifier.h"

LineSelectionWidget::LineSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::LineSelectionWidget)
{
    this->hide();
    ui->setupUi(this);
    ui->linesListWidget->clear();
    ui->linesListWidget->addItems(restoreLinesToSettings());

    QPalette bgPal = ui->bgWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->bgWidget->setPalette(bgPal);

    QPalette windowPal = ui->windowWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->windowWidget->setPalette(windowPal);
    ui->windowWidget->setAttribute(Qt::WA_NoSystemBackground, false);
    if (restoreDeviceType() == Avicon31Default) {
        insertControlSectionInLineListWidget();
        translateFirstItemLineListWidget();
    }
}

LineSelectionWidget::~LineSelectionWidget()
{
    delete ui;
}

void LineSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    raise();
    show();
}

void LineSelectionWidget::on_okLineButton_released()
{
    QListWidgetItem* item = ui->linesListWidget->currentItem();
    QString lineName("");
    if (item != 0) {
        lineName = item->text();
    }
    if (restoreDeviceType() == Avicon31Default) {
        emit lineSelected(lineName, ui->linesListWidget->currentRow() == 0);
    }
    else {
        emit lineSelected(lineName, false);
    }
}

void LineSelectionWidget::on_addLineButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &LineSelectionWidget::addNewLine);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue("");
    virtualKeyboards->show();
}

void LineSelectionWidget::on_editLineButton_released()
{
    if (ui->linesListWidget->currentRow() == 0 && restoreDeviceType() == Avicon31Default) {
        Notifier::instance().addNote(Qt::red, tr("Can not edit!"));
        return;
    }

    QListWidgetItem* item = ui->linesListWidget->currentItem();
    if (item != 0) {
        VirtualKeyboards::instance()->setCallback(this, &LineSelectionWidget::editLine);
        VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
        virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
        virtualKeyboards->setValidator(VirtualKeyboards::None);
        virtualKeyboards->setValue(item->text());
        virtualKeyboards->show();
    }
}

void LineSelectionWidget::on_deleteLineButton_released()
{
    if (ui->linesListWidget->currentRow() == 0 && restoreDeviceType() == Avicon31Default) {
        Notifier::instance().addNote(Qt::red, tr("Can not delete!"));
        return;
    }
    QListWidgetItem* item = ui->linesListWidget->currentItem();
    if (item != 0) {
        PopupDialogWidget* popup = new PopupDialogWidget(this);
        popup->setMessage(tr("Delete line?"));
        connect(popup, &PopupDialogWidget::dialogExited, this, &LineSelectionWidget::onDeleteDialogExited);
        popup->popup();
    }
}

void LineSelectionWidget::insertControlSectionInLineListWidget()
{
    if (ui->linesListWidget->count() == 0) {
        ui->linesListWidget->addItem(tr("Control section of track"));
        saveLinesToSettings(convertToQStringList(*(ui->linesListWidget)));
    }
}

void LineSelectionWidget::translateFirstItemLineListWidget()
{
#if defined(TARGET_AVICONDB) || defined(TARGET_AVICONDBHS) || defined(TARGET_AVICON15)
    return;
#endif
    QListWidgetItem* item = ui->linesListWidget->takeItem(0);
    item->setText(tr("Control section of track"));
    ui->linesListWidget->insertItem(0, item);
}

void LineSelectionWidget::addNewLine()
{
    QString newLine = VirtualKeyboards::instance()->value();
    if (newLine.isEmpty() == false) {
        ui->linesListWidget->addItem(newLine);
    }
    saveLinesToSettings(convertToQStringList(*(ui->linesListWidget)));
}

void LineSelectionWidget::editLine()
{
    QString editedLine = VirtualKeyboards::instance()->value();
    if (editedLine.isEmpty() == false) {
        QListWidgetItem* item = ui->linesListWidget->currentItem();
        if (item != 0) {
            item[0].setText(editedLine);
        }
    }
    saveLinesToSettings(convertToQStringList(*(ui->linesListWidget)));
}

void LineSelectionWidget::deleteLine()
{
    QListWidgetItem* item = ui->linesListWidget->currentItem();
    if (item != 0) {
        emit lineDeleted(item->text());
        qDeleteAll(ui->linesListWidget->selectedItems());
        saveLinesToSettings(convertToQStringList(*(ui->linesListWidget)));
    }
}

bool LineSelectionWidget::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (restoreDeviceType() == Avicon31Default) {
            insertControlSectionInLineListWidget();
            translateFirstItemLineListWidget();
        }
    }
    return QWidget::event(e);
}

void LineSelectionWidget::onDeleteDialogExited(bool status)
{
    if (status) {
        deleteLine();
    }
}

void LineSelectionWidget::on_closeButton_released()
{
    hide();
}
