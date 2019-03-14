#include "ekasuitracknumberselectionwidget.h"
#include "ui_ekasuitracknumberselectionwidget.h"

EKASUITrackNumberSelectionWidget::EKASUITrackNumberSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::EKASUITrackNumberSelectionWidget)
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

EKASUITrackNumberSelectionWidget::~EKASUITrackNumberSelectionWidget()
{
    delete ui;
}

void EKASUITrackNumberSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void EKASUITrackNumberSelectionWidget::setTrackNumbersList(const QStringList& list)
{
    ui->ekasuiTrackNumberListWidget->clear();
    ui->ekasuiTrackNumberListWidget->addItems(list);
}

void EKASUITrackNumberSelectionWidget::on_okSelectionButton_released()
{
    QListWidgetItem* item = ui->ekasuiTrackNumberListWidget->currentItem();
    QString trackNumber("");
    if (item != 0) {
        trackNumber = item->text();
        emit trackNumberSelected(trackNumber);
    }
}

void EKASUITrackNumberSelectionWidget::on_cancelButton_released()
{
    emit closeWidget();
}
