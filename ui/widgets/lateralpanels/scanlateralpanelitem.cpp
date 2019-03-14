#include <QMouseEvent>

#include "scanlateralpanelitem.h"
#include "ui_scanlateralpanelitem.h"

#include "styles.h"
#include "debug.h"
#include "roles.h"

ScanLateralPanelItem::ScanLateralPanelItem(int index, QWidget* parent)
    : QWidget(parent)
    , _firstSelectedTop(true)
    , _index(index)
    , ui(new Ui::ScanLateralPanelItem)
{
    ui->setupUi(this);

    ASSERT(connect(&_state, &LateralPanelFsm::doNormalState, this, &ScanLateralPanelItem::setNormalState, Qt::UniqueConnection));
    ASSERT(connect(&_state, &LateralPanelFsm::doAlarmState, this, &ScanLateralPanelItem::setAlarmState, Qt::UniqueConnection));
    ASSERT(connect(&_state, &LateralPanelFsm::doSelectedState, this, &ScanLateralPanelItem::setSelectedState, Qt::UniqueConnection));
    ASSERT(connect(&_state, &LateralPanelFsm::doAlarmAndSelectedState, this, &ScanLateralPanelItem::setAlarmAndSelectedState, Qt::UniqueConnection));

    reset();
#if defined TARGET_AVICONDB
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);
    ui->angle->setFont(font);
    ui->angleNote->setFont(font);
#endif
}

ScanLateralPanelItem::~ScanLateralPanelItem()
{
    delete ui;
}

void ScanLateralPanelItem::reset()
{
    ui->angle->setText("");
    ui->angle->setVisible(false);
    ui->angleNote->setText("");
    ui->angleNote->setVisible(false);

    ui->tapeView->reset();
    _state.reset();
}

void ScanLateralPanelItem::hideAngle()
{
    ui->angle->setText("");
}

void ScanLateralPanelItem::setAngle(unsigned int value)
{
    ui->angle->setText(QString("%1°").arg(value));
    ui->angle->setVisible(true);
}

void ScanLateralPanelItem::setAngleNote(const QString& value)
{
    ui->angleNote->setText(value);
    ui->angleNote->setVisible(true);
}

void ScanLateralPanelItem::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (model->rowCount() > 1) {
        const QModelIndex& index = model->index(0, 0);
        int angle = model->data(index, AngleRole).toInt();
        if (angle == 42 || angle == 45) {
            _firstSelectedTop = false;
        }
    }
    ui->tapeView->setModel(model);
    ui->tapeView->setScanLateralViewPointer(this->parent());
}

void ScanLateralPanelItem::setSide(LateralPanelType side)
{
    if (side == RightPanel) {
        ui->horizontalLayout->removeItem(ui->verticalLayoutAngle);
        ui->horizontalLayout->removeItem(ui->verticalLayoutTape);
        ui->horizontalLayout->addItem(ui->verticalLayoutTape);
        ui->horizontalLayout->addItem(ui->verticalLayoutAngle);
    }
    ui->tapeView->setSide(side);
}

bool ScanLateralPanelItem::select(int tapeIndex)
{
    ScanLateralPanelTapeItem* item = ui->tapeView;
    Q_ASSERT(item);

    ScanLateralPanelTapeItem::ItemPosition index = ScanLateralPanelTapeItem::bottomItem;
    if (item->isTopSensSetted() && item->isBottomSensSetted()) {
        _firstSelectedTop ? index = ScanLateralPanelTapeItem::topItem : index = ScanLateralPanelTapeItem::bottomItem;
    }
    else if (item->isTopSensSetted()) {
        index = ScanLateralPanelTapeItem::topItem;
    }
    else if (item->isBottomSensSetted()) {
        index = ScanLateralPanelTapeItem::bottomItem;
    }
    else {
        return false;
    }

    const QModelIndexList& list = item->selectedRows();
    if (list.count()) {
        if (list.at(0).row() == 0 && item->isBottomSensSetted()) {
            index = ScanLateralPanelTapeItem::bottomItem;
        }
        if (list.at(0).row() == 1 && item->isTopSensSetted() && item->isBottomSensSetted()) {
            index = ScanLateralPanelTapeItem::topItem;
        }
        item->clearSelection();
    }
    item->select(tapeIndex, index);

    _state.select();

    return true;
}

void ScanLateralPanelItem::unselect()
{
    _state.unselect();
}

void ScanLateralPanelItem::alarm()
{
    _state.alarm();
}

void ScanLateralPanelItem::unalarm()
{
    _state.unalarm();
}

void ScanLateralPanelItem::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->x() < this->width()) && (event->y() < this->height())) {
        emit clicked(_index);
    }
    QWidget::mouseReleaseEvent(event);
}

void ScanLateralPanelItem::setNormalState()
{
    ui->angle->setPalette(Styles::cellType1Palette());
    ui->angleNote->setPalette(Styles::cellType1Palette());

    ui->tapeView->clearSelection();
}

void ScanLateralPanelItem::setAlarmState()
{
    ui->angle->setPalette(Styles::cellAlarmedPalette());
    ui->angleNote->setPalette(Styles::cellAlarmedPalette());
}

void ScanLateralPanelItem::setSelectedState()
{
    ui->angle->setPalette(Styles::cellSelectedPalette());
    ui->angleNote->setPalette(Styles::cellSelectedPalette());
}

void ScanLateralPanelItem::setAlarmAndSelectedState()
{
    ui->angle->setPalette(Styles::cellSelectedPalette());
    ui->angleNote->setPalette(Styles::cellSelectedPalette());
}
