#include "genericselectionwidget.h"
#include "ui_genericselectionwidget.h"

GenericSelectionWidget::GenericSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::GenericSelectionWidget)
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

GenericSelectionWidget::~GenericSelectionWidget()
{
    delete ui;
}

void GenericSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void GenericSelectionWidget::setValues(const QString& title, const std::vector<std::pair<QString, int>>& valuesWithId)
{
    ui->titleLabel->setText(title);
    ui->listWidget->clear();
    _valuesWithId = valuesWithId;
    for (const auto& pair : _valuesWithId) {
         ui->listWidget->addItem(pair.first);
    }
}

void GenericSelectionWidget::on_okButton_released()
{
    if (ui->listWidget->selectedItems().size() > 0) {
        QString item = ui->listWidget->selectedItems().back()->text();
        int id = _valuesWithId.at(ui->listWidget->currentIndex().row()).second;
        emit selected(item, id);
        hide();
    }
}

void GenericSelectionWidget::on_closeButton_released()
{
    hide();
}
