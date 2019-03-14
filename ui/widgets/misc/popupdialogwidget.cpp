#include "popupdialogwidget.h"
#include "ui_popupdialogwidget.h"

QString PopupDialogWidget::getMessage() const
{
    return message;
}

void PopupDialogWidget::setMessage(const QString& value)
{
    message = value;
}

PopupDialogWidget::PopupDialogWidget(QWidget* parent)
    : QWidget(parent)
    , message(tr("Are you sure?"))
    , ui(new Ui::PopupDialogWidget)
{
    parentWidget = parent;
    ui->setupUi(this);
    // setAttribute( Qt::WA_NoSystemBackground, true );
    // setAttribute( Qt::WA_OpaquePaintEvent, false );
    QPalette bgPal = ui->backgroundWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->backgroundWidget->setPalette(bgPal);
    QPalette windowPal = ui->windowWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->windowWidget->setPalette(windowPal);
    ui->windowWidget->setAttribute(Qt::WA_NoSystemBackground, false);
}

PopupDialogWidget::~PopupDialogWidget()
{
    delete ui;
}

void PopupDialogWidget::popup()
{
    ui->warningLabel->setText(message);
    QSize parentSize = parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    raise();
    show();
}

void PopupDialogWidget::on_yesButton_released()
{
    emit dialogExited(true);
    hide();
    deleteLater();
}

void PopupDialogWidget::on_cancelButton_released()
{
    emit dialogExited(false);
    hide();
    deleteLater();
}
