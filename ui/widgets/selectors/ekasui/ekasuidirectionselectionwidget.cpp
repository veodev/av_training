#include "ekasuidirectionselectionwidget.h"
#include "ui_ekasuidirectionselectionwidget.h"

EKASUIDirectionSelectionWidget::EKASUIDirectionSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::EKASUIDirectionSelectionWidget)
{
    this->hide();
    ui->setupUi(this);
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

EKASUIDirectionSelectionWidget::~EKASUIDirectionSelectionWidget()
{
    delete ui;
}

void EKASUIDirectionSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void EKASUIDirectionSelectionWidget::setDirectionsList(const QStringList& list)
{
    ui->ekasuiDirectionListWidget->clear();
    ui->ekasuiDirectionListWidget->addItems(list);
}

void EKASUIDirectionSelectionWidget::on_okSelectionButton_released()
{
    QListWidgetItem* item = ui->ekasuiDirectionListWidget->currentItem();
    QString directionName("");
    if (item != 0) {
        directionName = item->text();
        emit directionSelected(directionName);
    }
}

void EKASUIDirectionSelectionWidget::on_cancelButton_released()
{
    emit closeWidget();
}
