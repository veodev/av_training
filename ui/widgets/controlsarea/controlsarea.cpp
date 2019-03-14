#include <QScrollBar>
#include <QKeyEvent>

#include "controlsarea.h"
#include "ui_controlsarea.h"
#include "styles.h"
#include "debug.h"
#include "accessories.h"

ControlsArea::ControlsArea(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ControlsArea)
{
    ui->setupUi(this);
    addSpacer();

    QScrollBar* scrollBar = ui->scrollArea->verticalScrollBar();
    Q_ASSERT(scrollBar);
    ASSERT(connect(scrollBar, &QScrollBar::valueChanged, this, &ControlsArea::scrollBarValueChanged));
    scrollBar->setValue(0);
    ASSERT(connect(scrollBar, &QScrollBar::rangeChanged, this, &ControlsArea::scrollBarRangeChanged));
    scrollBarRangeChanged(scrollBar->minimum(), scrollBar->maximum());

    ASSERT(connect(ui->controls, &ControlsWidget::widthChanged, this, &ControlsArea::controlsWidgedWidthChanged));

    ui->scrollUpButton->setStyleSheet(Styles::areaButton());
    ui->scrollUpButton->setFocusPolicy(Qt::NoFocus);
    ui->scrollDownButton->setStyleSheet(Styles::areaButton());
    ui->scrollDownButton->setFocusPolicy(Qt::NoFocus);
}

ControlsArea::~ControlsArea()
{
    delete ui;
}

void ControlsArea::addWidget(QWidget* widget)
{
    Q_ASSERT(widget);
    if (widget != nullptr) {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(false);
        widget->setSizePolicy(sizePolicy);
        if (ui->controlsLayout->count()) {
            ui->controlsLayout->insertWidget(ui->controlsLayout->count() - 1, widget);
        }
        else {
            ui->controlsLayout->insertWidget(0, widget);
        }
        ui->controls->adjustSize();
    }
}

void ControlsArea::reset()
{
    QLayoutItem* child = 0;
    while ((child = ui->controlsLayout->takeAt(0)) != 0) {
        QWidget* widget = child->widget();
        if (widget != nullptr) {
            widget->setParent(0);
        }
        delete child;
    };
    addSpacer();
}

void ControlsArea::setVisible(bool visible)
{
    if (visible == true) {
        if (ui->controls->height() <= this->height()) {
            ui->scrollUpButton->setVisible(false);
            ui->scrollDownButton->setVisible(false);
        }
        else {
            scrollBarValueChanged(ui->scrollArea->verticalScrollBar()->value());
        }
    }

    QWidget::setVisible(visible);
}

void ControlsArea::addSpacer()
{
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->controlsLayout->addItem(verticalSpacer);
}

void ControlsArea::scrollBarValueChanged(int value)
{
    QScrollBar* scrollBar = ui->scrollArea->verticalScrollBar();
    Q_ASSERT(scrollBar);
    if (value <= scrollBar->minimum()) {
        ui->scrollUpButton->setVisible(false);
    }
    else {
        ui->scrollUpButton->setVisible(true);
    }
    if (value >= scrollBar->maximum()) {
        ui->scrollDownButton->setVisible(false);
    }
    else {
        ui->scrollDownButton->setVisible(true);
    }
}

void ControlsArea::scrollBarRangeChanged(int min, int max)
{
    if (ui->controls->height() <= this->height() || (min == 0 && max == 0)) {
        ui->scrollUpButton->setVisible(false);
        ui->scrollDownButton->setVisible(false);
    }
    else {
        scrollBarValueChanged(ui->scrollArea->verticalScrollBar()->value());
    }
}

void ControlsArea::controlsWidgedWidthChanged(int width)
{
    setMinimumWidth(width);
    ui->scrollArea->setMinimumWidth(width);
}

void ControlsArea::on_scrollUpButton_released()
{
    QScrollBar* scrollBar = ui->scrollArea->verticalScrollBar();
    Q_ASSERT(scrollBar);
    scrollBar->setValue(scrollBar->value() - scrollBar->pageStep());
}

void ControlsArea::on_scrollDownButton_released()
{
    QScrollBar* scrollBar = ui->scrollArea->verticalScrollBar();
    Q_ASSERT(scrollBar);
    scrollBar->setValue(scrollBar->value() + scrollBar->pageStep());
}
