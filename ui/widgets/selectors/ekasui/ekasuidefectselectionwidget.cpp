#include "ekasuidefectselectionwidget.h"
#include "ui_ekasuidefectselectionwidget.h"
#include <debug.h>

EKASUIDefectSelectionWidget::EKASUIDefectSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::EKASUIDefectSelectionWidget)
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

EKASUIDefectSelectionWidget::~EKASUIDefectSelectionWidget()
{
    delete ui;
}

void EKASUIDefectSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void EKASUIDefectSelectionWidget::setDefectsList(const QStringList& list)
{
    ui->ekasuiDefectListWidget->clear();
    ui->ekasuiDefectListWidget->addItems(list);
}

void EKASUIDefectSelectionWidget::on_okSelectionButton_released()
{
    QListWidgetItem* item = ui->ekasuiDefectListWidget->currentItem();
    QString defectName("");
    if (item != nullptr) {
        defectName = item->text();
        qDebug() << "EKASUI defect:" << defectName;
        emit defectSelected(defectName);
    }
}

void EKASUIDefectSelectionWidget::on_cancelButton_released()
{
    emit closeWidget();
}
