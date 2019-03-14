#include "ekasuistationselectionwidget.h"
#include "ui_ekasuistationselectionwidget.h"

EKASUIStationSelectionWidget::EKASUIStationSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::EKASUIStationSelectionWidget)
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

EKASUIStationSelectionWidget::~EKASUIStationSelectionWidget()
{
    delete ui;
}

void EKASUIStationSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void EKASUIStationSelectionWidget::setStationList(const QStringList& list)
{
    ui->ekasuiStationListWidget->clear();
    ui->ekasuiStationListWidget->addItems(list);
}

void EKASUIStationSelectionWidget::on_okSelectionButton_released()
{
    QListWidgetItem* item = ui->ekasuiStationListWidget->currentItem();
    QString trackNumber("");
    if (item != 0) {
        trackNumber = item->text();
        emit stationSelected(trackNumber);
    }
}

void EKASUIStationSelectionWidget::on_cancelButton_released()
{
    emit closeWidget();
}
