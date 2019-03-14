#include "optionslistpage.h"
#include "ui_optionslistpage.h"

#include <QScrollBar>

OptionsListPage::OptionsListPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsListPage)
{
    ui->setupUi(this);
}

OptionsListPage::~OptionsListPage()
{
    delete ui;
}

void OptionsListPage::addWidget(QWidget* widget)
{
    Q_ASSERT(widget);
    if (widget != nullptr) {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(false);
        widget->setSizePolicy(sizePolicy);
        ui->itemsLayout->addWidget(widget);
        _items.append(widget);
    }
}

void OptionsListPage::setItemEnabled(QString objectName, bool isEnabled)
{
    for (auto item: _items) {
        if (item->objectName() == objectName) {
            item->setEnabled(isEnabled);
            break;
        }
    }
}

void OptionsListPage::setItemVisible(QString objectName, bool isVisible)
{
    for (auto item: _items) {
        if (item->objectName() == objectName) {
            item->setVisible(isVisible);
            break;
        }
    }
}

void OptionsListPage::setItemText(QString objectName, QString text)
{
    for (auto item: _items) {
        if (item->objectName() == objectName) {
            static_cast<QPushButton*>(item)->setText(text);
            break;
        }
    }
}
