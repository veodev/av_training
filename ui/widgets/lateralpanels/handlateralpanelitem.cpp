#include <QMouseEvent>

#include "handlateralpanelitem.h"
#include "ui_handlateralpanelitem.h"

#include "styles.h"
#include "debug.h"

HandLateralPanelItem::HandLateralPanelItem(QWidget* parent, int index)
    : QWidget(parent)
    , ui(new Ui::HandLateralPanelItem)
    , _index(index)
{
    ui->setupUi(this);

    ASSERT(connect(&(_states), &LateralPanelFsm::doNormalState, this, &HandLateralPanelItem::setNormalState, Qt::UniqueConnection));
    ASSERT(connect(&(_states), &LateralPanelFsm::doAlarmState, this, &HandLateralPanelItem::setAlarmState, Qt::UniqueConnection));
    ASSERT(connect(&(_states), &LateralPanelFsm::doSelectedState, this, &HandLateralPanelItem::setSelectedState, Qt::UniqueConnection));
    ASSERT(connect(&(_states), &LateralPanelFsm::doAlarmAndSelectedState, this, &HandLateralPanelItem::setAlarmAndSelectedState, Qt::UniqueConnection));

    setNormalState();

#if defined TARGET_AVICONDBHS || defined TARGET_AVICONDB
    setMinimumWidth(100);
    setMaximumWidth(100);
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);
    ui->angle->setFont(font);
#endif
}

HandLateralPanelItem::~HandLateralPanelItem()
{
    delete ui;
}

void HandLateralPanelItem::setAngle(int value)
{
    ui->angle->setText(QString("%1°").arg(value));
}

void HandLateralPanelItem::reset()
{
    ui->angle->setText("");
    unselect();
}

void HandLateralPanelItem::select()
{
    _states.select();
}

void HandLateralPanelItem::unselect()
{
    _states.unselect();
}

void HandLateralPanelItem::alarm()
{
    _states.alarm();
}

void HandLateralPanelItem::unalarm()
{
    _states.unalarm();
}

void HandLateralPanelItem::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->x() < this->width()) && (event->y() < this->height())) {
        emit clicked(_index);
    }
    QWidget::mouseReleaseEvent(event);
}

void HandLateralPanelItem::setNormalState()
{
    ui->angle->setPalette(Styles::cellType1Palette());
}

void HandLateralPanelItem::setAlarmState()
{
    ui->angle->setPalette(Styles::cellAlarmedPalette());
}

void HandLateralPanelItem::setSelectedState()
{
    ui->angle->setPalette(Styles::cellSelectedPalette());
}

void HandLateralPanelItem::setAlarmAndSelectedState()
{
    ui->angle->setPalette(Styles::cellAlarmedAndSelectedPalette());
}
