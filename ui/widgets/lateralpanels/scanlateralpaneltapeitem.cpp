#include <QAbstractProxyModel>
#include <QPainter>
#include <QPixmap>

#include "scanlateralpaneltapeitem.h"
#include "ui_scanlateralpaneltapeitem.h"
#include "styles.h"
#include "roles.h"
#include "core.h"
#include "channelscontroller.h"
#include "debug.h"

const static int PEN_WIDTH = 1;
const static int FONT_SIZE = 10;
const static QString FONT_FAMILY = QString("Sans");

ScanLateralPanelTapeItem::ScanLateralPanelTapeItem(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScanLateralPanelTapeItem)
    , _side(LeftPanel)
    , _isTopSensSeted(false)
    , _isBottomSensSeted(false)
    , _model(nullptr)
    , _isTopSensBlinking(false)
    , _isBottomSensBlinking(false)
    , _topNormalPix(nullptr)
    , _topSelectedPix(nullptr)
    , _topAlarmedPix(nullptr)
    , _topSelectedAlarmedPix(nullptr)
    , _topNormalHideBlinkPix(nullptr)
    , _topSelectedHideBlinkPix(nullptr)
    , _topAlarmedHideBlinkPix(nullptr)
    , _topSelectedAlarmedHideBlinkPix(nullptr)
    , _bottomNormalPix(nullptr)
    , _bottomSelectedPix(nullptr)
    , _bottomAlarmedPix(nullptr)
    , _bottomSelectedAlarmedPix(nullptr)
    , _bottomNormalHideBlinkPix(nullptr)
    , _bottomSelectedHideBlinkPix(nullptr)
    , _bottomAlarmedHideBlinkPix(nullptr)
    , _bottomSelectedAlarmedHideBlinkPix(nullptr)
{
    ui->setupUi(this);

#if defined ANDROID
    ui->sensTop->setMinimumWidth(30);
    ui->sensTop->setMaximumWidth(30);

    ui->sensBottom->setMinimumWidth(30);
    ui->sensBottom->setMaximumWidth(30);

    ui->noteTop->setMinimumWidth(25);
    ui->noteTop->setMaximumWidth(25);

    ui->noteBottom->setMinimumWidth(25);
    ui->noteBottom->setMaximumWidth(25);
#endif
#if defined TARGET_AVICONDB
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);

    ui->sensTop->setMinimumWidth(30);
    ui->sensTop->setMaximumWidth(30);
    ui->sensTop->setFont(font);
    ui->sensTop->setFont(font);

    ui->sensBottom->setMinimumWidth(30);
    ui->sensBottom->setMaximumWidth(30);
    ui->sensBottom->setFont(font);
    ui->sensBottom->setFont(font);

    ui->noteTop->setMinimumWidth(30);
    ui->noteTop->setMaximumWidth(30);
    ui->noteTop->setFont(font);
    ui->noteTop->setFont(font);

    ui->noteBottom->setMinimumWidth(30);
    ui->noteBottom->setMaximumWidth(30);
    ui->noteBottom->setFont(font);
    ui->noteBottom->setFont(font);
#endif

    const QPixmap& mute = QPixmap(":/sound_mute_11x8_red.png");

    ui->sensTop->setLineWidth(2);
    ui->sensTop->setFrameShape(QFrame::NoFrame);
    ui->sensTop->setFrameShadow(QFrame::Plain);
    ui->sensTopImage->setGeometry(1, 1, 11, 8);
    ui->sensTopImage->setFrameShape(QFrame::NoFrame);
    ui->sensTopImage->setPixmap(mute);
    ui->sensTopImage->hide();

    ui->sensBottom->setLineWidth(2);
    ui->sensBottom->setFrameShape(QFrame::NoFrame);
    ui->sensBottom->setFrameShadow(QFrame::Plain);
    ui->sensBottomImage->setGeometry(1, 1, 11, 8);
    ui->sensBottomImage->setFrameShape(QFrame::NoFrame);
    ui->sensBottomImage->setPixmap(mute);
    ui->sensBottomImage->hide();

    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doNormalState, this, &ScanLateralPanelTapeItem::setTopNormalState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doAlarmState, this, &ScanLateralPanelTapeItem::setTopAlarmState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doSelectedState, this, &ScanLateralPanelTapeItem::setTopSelectedState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doAlarmAndSelectedState, this, &ScanLateralPanelTapeItem::setTopAlarmAndSelectedState, Qt::DirectConnection));

    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doNormalState, this, &ScanLateralPanelTapeItem::setBottomNormalState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmState, this, &ScanLateralPanelTapeItem::setBottomAlarmState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doSelectedState, this, &ScanLateralPanelTapeItem::setBottomSelectedState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmAndSelectedState, this, &ScanLateralPanelTapeItem::setBottomAlarmAndSelectedState, Qt::DirectConnection));

    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doNormalHideBlinkState, this, &ScanLateralPanelTapeItem::setTopNormalHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doAlarmHideBlinkState, this, &ScanLateralPanelTapeItem::setTopAlarmHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setTopSelectedHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[topItem]), &LateralPanelFsm::doAlarmAndSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setTopAlarmAndSelectedHideBlinkState, Qt::DirectConnection));

    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doNormalHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomNormalHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomAlarmHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomSelectedHideBlinkState, Qt::DirectConnection));
    ASSERT(connect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmAndSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomAlarmAndSelectedHideBlinkState, Qt::DirectConnection));
    reset();
}

ScanLateralPanelTapeItem::~ScanLateralPanelTapeItem()
{
    disconnect(&(_states[topItem]), &LateralPanelFsm::doNormalState, this, &ScanLateralPanelTapeItem::setTopNormalState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doAlarmState, this, &ScanLateralPanelTapeItem::setTopAlarmState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doSelectedState, this, &ScanLateralPanelTapeItem::setTopSelectedState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doAlarmAndSelectedState, this, &ScanLateralPanelTapeItem::setTopAlarmAndSelectedState);

    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doNormalState, this, &ScanLateralPanelTapeItem::setBottomNormalState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmState, this, &ScanLateralPanelTapeItem::setBottomAlarmState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doSelectedState, this, &ScanLateralPanelTapeItem::setBottomSelectedState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmAndSelectedState, this, &ScanLateralPanelTapeItem::setBottomAlarmAndSelectedState);

    disconnect(&(_states[topItem]), &LateralPanelFsm::doNormalHideBlinkState, this, &ScanLateralPanelTapeItem::setTopNormalHideBlinkState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doAlarmHideBlinkState, this, &ScanLateralPanelTapeItem::setTopAlarmHideBlinkState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setTopSelectedHideBlinkState);
    disconnect(&(_states[topItem]), &LateralPanelFsm::doAlarmAndSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setTopAlarmAndSelectedHideBlinkState);

    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doNormalHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomNormalHideBlinkState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomAlarmHideBlinkState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomSelectedHideBlinkState);
    disconnect(&(_states[bottomItem]), &LateralPanelFsm::doAlarmAndSelectedHideBlinkState, this, &ScanLateralPanelTapeItem::setBottomAlarmAndSelectedHideBlinkState);

    if (_topNormalPix != nullptr) {
        delete _topNormalPix;
    }
    if (_topSelectedPix != nullptr) {
        delete _topSelectedPix;
    }
    if (_topAlarmedPix != nullptr) {
        delete _topAlarmedPix;
    }
    if (_topSelectedAlarmedPix != nullptr) {
        delete _topSelectedAlarmedPix;
    }
    if (_topNormalHideBlinkPix != nullptr) {
        delete _topNormalHideBlinkPix;
    }
    if (_topSelectedHideBlinkPix != nullptr) {
        delete _topSelectedHideBlinkPix;
    }
    if (_topAlarmedHideBlinkPix != nullptr) {
        delete _topAlarmedHideBlinkPix;
    }
    if (_topSelectedAlarmedHideBlinkPix != nullptr) {
        delete _topSelectedAlarmedHideBlinkPix;
    }

    if (_bottomNormalPix != nullptr) {
        delete _bottomNormalPix;
    }
    if (_bottomSelectedPix != nullptr) {
        delete _bottomSelectedPix;
    }
    if (_bottomAlarmedPix != nullptr) {
        delete _bottomAlarmedPix;
    }
    if (_bottomSelectedAlarmedPix != nullptr) {
        delete _bottomSelectedAlarmedPix;
    }
    if (_bottomNormalHideBlinkPix != nullptr) {
        delete _bottomNormalHideBlinkPix;
    }
    if (_bottomSelectedHideBlinkPix != nullptr) {
        delete _bottomSelectedHideBlinkPix;
    }
    if (_bottomAlarmedHideBlinkPix != nullptr) {
        delete _bottomAlarmedHideBlinkPix;
    }
    if (_bottomSelectedAlarmedHideBlinkPix != nullptr) {
        delete _bottomSelectedAlarmedHideBlinkPix;
    }

    delete ui;
}

void ScanLateralPanelTapeItem::reset()
{
    ui->noteTop->setText("");
    ui->noteTop->setVisible(false);
    ui->noteBottom->setText("");
    ui->noteBottom->setVisible(false);

    ui->sensTop->setVisible(false);
    ui->sensBottom->setVisible(false);

    _isTopSensSeted = false;
    _isBottomSensSeted = false;

    _states[0].reset();
    _states[1].reset();
}

void ScanLateralPanelTapeItem::setTopNote(const QString& value)
{
    ui->noteTop->setText(value);
    ui->noteTop->setVisible(true);
}

void ScanLateralPanelTapeItem::setBottomNote(const QString& value)
{
    ui->noteBottom->setText(value);
    ui->noteBottom->setVisible(true);
}

void ScanLateralPanelTapeItem::setSens(int value)
{
    setTopSens(value);
}

void ScanLateralPanelTapeItem::setTopSens(int value)
{
    _isTopSensSeted = true;
    setTopSensVisible(true);
    _prevTopSens = QString::number(value);
    int width = ui->sensTop->width();
    int height = ui->sensTop->height();
    generateTopPixmaps(width, height);
    updateViewItem();
}

void ScanLateralPanelTapeItem::setTopSensVisible(bool visible)
{
    ui->sensTop->setVisible(visible);
}

bool ScanLateralPanelTapeItem::isTopSensSetted() const
{
    return _isTopSensSeted;
}

void ScanLateralPanelTapeItem::setBottomSens(int value)
{
    _isBottomSensSeted = true;
    setBottomSensVisible(true);
    _prevBottomSens = QString::number(value);
    int width = ui->sensBottom->width();
    int height = ui->sensBottom->height();
    generateBottomPixmaps(width, height);
    updateViewItem();
}

void ScanLateralPanelTapeItem::setBottomSensVisible(bool visible)
{
    ui->sensBottom->setVisible(visible);
}

bool ScanLateralPanelTapeItem::isBottomSensSetted() const
{
    return _isBottomSensSeted;
}

void ScanLateralPanelTapeItem::clearSelection()
{
    unselect(topItem);
    unselect(bottomItem);
}

QModelIndexList ScanLateralPanelTapeItem::selectedRows() const
{
    QModelIndexList list;
    if (this->isTopSensSetted()
        && (_states[topItem].state() == LateralPanelFsm::selectedState || _states[topItem].state() == LateralPanelFsm::alarmAndSelectedState || _states[topItem].state() == LateralPanelFsm::selectedHideBlinkState || _states[topItem].state() == LateralPanelFsm::alarmAndSelectedHideBlinkState)) {
        list << _model->index(0, 0);
    }
    if (this->isBottomSensSetted()
        && (_states[bottomItem].state() == LateralPanelFsm::selectedState || _states[bottomItem].state() == LateralPanelFsm::alarmAndSelectedState || _states[bottomItem].state() == LateralPanelFsm::selectedHideBlinkState
            || _states[bottomItem].state() == LateralPanelFsm::alarmAndSelectedHideBlinkState)) {
        list << _model->index(1, 0);
    }
    return list;
}

void ScanLateralPanelTapeItem::select(int tapeIndex, ItemPosition position)
{
    _states[position].select();
    const QModelIndex& index = _model->index(position, _side);
    const QModelIndex& channelsModelIndex = reinterpret_cast<QAbstractProxyModel*>(_model)->mapToSource(index);

    if (channelsModelIndex.isValid()) {
        ChannelsController* channelsController = Core::instance().channelsController();
        Q_ASSERT(channelsController);
        channelsController->setChannel(channelsModelIndex);
        channelsController->selectTape(tapeIndex, position, _side);
    }
}

void ScanLateralPanelTapeItem::unselect(ItemPosition index)
{
    _states[index].unselect();
}

void ScanLateralPanelTapeItem::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (_model != nullptr) {
        _model->disconnect(this);
    }
    _model = model;

    if (_model != nullptr && _model->rowCount() > 0) {
        const QModelIndex& topLeft = _model->index(0, _side);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, _side);
        dataChanged(topLeft, bottomRight, QVector<int>() << NoteRole << SensRole << FakedChannelRole);
        connect(_model, &QAbstractItemModel::dataChanged, this, &ScanLateralPanelTapeItem::dataChanged, Qt::UniqueConnection);
        connect(_model, &QAbstractItemModel::destroyed, this, &ScanLateralPanelTapeItem::modelDestroyed, Qt::UniqueConnection);
    }
}

void ScanLateralPanelTapeItem::setSide(LateralPanelType side)
{
    _side = side;

    if (side == RightPanel) {
        ui->horizontalLayout->removeItem(ui->verticalLayoutNote);
        ui->horizontalLayout->removeItem(ui->verticalLayoutSens);
        ui->horizontalLayout->addItem(ui->verticalLayoutSens);
        ui->horizontalLayout->addItem(ui->verticalLayoutNote);
    }
}

void ScanLateralPanelTapeItem::setScanLateralViewPointer(QObject* obj)
{
    ScanLateralPanelView* pointer = reinterpret_cast<ScanLateralPanelView*>(obj);
    ASSERT(connect(pointer, &ScanLateralPanelView::blinkingTimerTimeOut, this, &ScanLateralPanelTapeItem::onBlink));
}

void ScanLateralPanelTapeItem::stopBlinkTopSens()
{
    _states[topItem].showBlink();
}

void ScanLateralPanelTapeItem::stopBlinkBottomSens()
{
    _states[bottomItem].showBlink();
}

void ScanLateralPanelTapeItem::onBlinkingTopSens(bool isGlow)
{
    if (_isTopSensBlinking == false) {
        return;
    }

    (isGlow == false) ? _states[topItem].hideBlink() : _states[topItem].showBlink();
}

void ScanLateralPanelTapeItem::onBlinkingBottomSens(bool isGlow)
{
    if (_isBottomSensBlinking == false) {
        return;
    }

    (isGlow == false) ? _states[bottomItem].hideBlink() : _states[bottomItem].showBlink();
}

void ScanLateralPanelTapeItem::onBlink(bool isGlow)
{
    onBlinkingTopSens(isGlow);
    onBlinkingBottomSens(isGlow);
}

void ScanLateralPanelTapeItem::alarm(ItemPosition index)
{
    _states[index].alarm();
}

void ScanLateralPanelTapeItem::unalarm(ItemPosition index)
{
    _states[index].unalarm();
}

void ScanLateralPanelTapeItem::sensInRange(ScanLateralPanelTapeItem::ItemPosition index)
{
    switch (index) {
    case topItem:
        _isTopSensBlinking = false;
        break;
    case bottomItem:
        _isBottomSensBlinking = false;
        break;
    }
    _states[index].showBlink();
}

void ScanLateralPanelTapeItem::sensOutOfRange(ScanLateralPanelTapeItem::ItemPosition index)
{
    switch (index) {
    case topItem:
        _isTopSensBlinking = true;
        break;
    case bottomItem:
        _isBottomSensBlinking = true;
        break;
    }
    _states[index].hideBlink();
}

void ScanLateralPanelTapeItem::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);  // TODO: Use new sizes?
    int width = event->size().width();
    int height = event->size().height();
    generateTopPixmaps(width, height);
    generateBottomPixmaps(width, height);
    updateViewItem();
}

void ScanLateralPanelTapeItem::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (topLeft.column() != _side) {
        return;
    }

    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        const QModelIndex& index = _model->index(i, _side);
        for (int role : roles) {
            updateCell(index, role);
        }
    }
}

void ScanLateralPanelTapeItem::setTopNormalState()
{
    ui->noteTop->setPalette(Styles::cellType1Palette());
    if (_topNormalPix != nullptr) {
        ui->sensTop->setPixmap(*_topNormalPix);
    }
}

void ScanLateralPanelTapeItem::setBottomNormalState()
{
    ui->noteBottom->setPalette(Styles::cellType1Palette());
    if (_bottomNormalPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomNormalPix);
    }
}

void ScanLateralPanelTapeItem::setTopAlarmState()
{
    ui->noteTop->setPalette(Styles::cellType1Palette());
    if (_topAlarmedPix != nullptr) {
        ui->sensTop->setPixmap(*_topAlarmedPix);
    }
}

void ScanLateralPanelTapeItem::setBottomAlarmState()
{
    ui->noteBottom->setPalette(Styles::cellType1Palette());
    if (_bottomAlarmedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomAlarmedPix);
    }
}

void ScanLateralPanelTapeItem::setTopSelectedState()
{
    ui->noteTop->setPalette(Styles::cellSelectedPalette());
    if (_topSelectedPix != nullptr) {
        ui->sensTop->setPixmap(*_topSelectedPix);
    }
}

void ScanLateralPanelTapeItem::setBottomSelectedState()
{
    ui->noteBottom->setPalette(Styles::cellSelectedPalette());
    if (_bottomSelectedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomSelectedPix);
    }
}

void ScanLateralPanelTapeItem::setTopAlarmAndSelectedState()
{
    ui->noteTop->setPalette(Styles::cellSelectedPalette());
    if (_topSelectedAlarmedPix != nullptr) {
        ui->sensTop->setPixmap(*_topSelectedAlarmedPix);
    }
}

void ScanLateralPanelTapeItem::setBottomAlarmAndSelectedState()
{
    ui->noteBottom->setPalette(Styles::cellSelectedPalette());
    if (_bottomSelectedAlarmedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomSelectedAlarmedPix);
    }
}

void ScanLateralPanelTapeItem::setTopNormalHideBlinkState()
{
    ui->noteTop->setPalette(Styles::cellType1Palette());
    if (_topNormalPix != nullptr) {
        ui->sensTop->setPixmap(*_topNormalHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setBottomNormalHideBlinkState()
{
    ui->noteBottom->setPalette(Styles::cellType1Palette());
    if (_bottomNormalPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomNormalHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setTopAlarmHideBlinkState()
{
    ui->noteTop->setPalette(Styles::cellType1Palette());
    if (_topAlarmedPix != nullptr) {
        ui->sensTop->setPixmap(*_topAlarmedHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setBottomAlarmHideBlinkState()
{
    ui->noteBottom->setPalette(Styles::cellType1Palette());
    if (_bottomAlarmedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomAlarmedHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setTopSelectedHideBlinkState()
{
    ui->noteTop->setPalette(Styles::cellSelectedPalette());
    if (_topSelectedPix != nullptr) {
        ui->sensTop->setPixmap(*_topSelectedHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setBottomSelectedHideBlinkState()
{
    ui->noteBottom->setPalette(Styles::cellSelectedPalette());
    if (_bottomSelectedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomSelectedHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setTopAlarmAndSelectedHideBlinkState()
{
    ui->noteTop->setPalette(Styles::cellSelectedPalette());
    if (_topSelectedAlarmedPix != nullptr) {
        ui->sensTop->setPixmap(*_topSelectedAlarmedHideBlinkPix);
    }
}

void ScanLateralPanelTapeItem::setBottomAlarmAndSelectedHideBlinkState()
{
    ui->noteBottom->setPalette(Styles::cellSelectedPalette());
    if (_bottomSelectedAlarmedPix != nullptr) {
        ui->sensBottom->setPixmap(*_bottomSelectedAlarmedHideBlinkPix);
    }
}


void ScanLateralPanelTapeItem::modelDestroyed()
{
    _model = nullptr;
}

void ScanLateralPanelTapeItem::updateCell(const QModelIndex& index, int role)
{
    QVariant value;
    if (role == NoteRole) {
#if defined(IMX_DEVICE)
        if (_side == LeftPanel) {
#endif
            value = _model->data(index, NoteRole);
            if (value.isValid() && value.toString().length() > 0) {
                switch (index.row()) {
                case 0:
                    setTopNote(value.toString());
                    break;
                case 1:
                    setBottomNote(value.toString());
                    break;
                }
            }
#if defined(IMX_DEVICE)
        }
#endif
    }
    else if (role == SensRole) {
        value = _model->data(index, SensRole);
        if (value.isValid()) {
            switch (index.row()) {
            case 0:
                setTopSens(value.toInt());
                break;
            case 1:
                setBottomSens(value.toInt());
                break;
            }
        }
    }
    else if (role == IsAlarmedRole) {
        value = _model->data(index, IsAlarmedRole);
        if (value.isValid()) {
            bool isAlarmed = value.toBool();
            switch (index.row()) {
            case 0:
                (isAlarmed) ? alarm(topItem) : unalarm(topItem);
                break;
            case 1:
                (isAlarmed) ? alarm(bottomItem) : unalarm(bottomItem);
                break;
            }
        }
    }
    else if (role == SensLevelsRole) {
        value = _model->data(index, SensLevelsRole);
        if (value.isValid()) {
            int isSensOutRange = value.toInt();
            switch (index.row()) {
            case 0:
                (isSensOutRange == 0) ? sensInRange(topItem) : sensOutOfRange(topItem);
                break;
            case 1:
                (isSensOutRange == 0) ? sensInRange(bottomItem) : sensOutOfRange(bottomItem);
                break;
            }
        }
    }
    else if (role == FakedChannelRole) {
        value = _model->data(index, FakedChannelRole);
        if (value.isValid()) {
            if (value.toBool() == true) {
                switch (index.row()) {
                case 0:
                    setTopSensVisible(true);
                    break;
                case 1:
                    setBottomSensVisible(true);
                    break;
                }
            }
        }
    }
    else if (role == IsPlayAlarmToneRole) {
        value = _model->data(index, IsPlayAlarmToneRole);
        if (value.isValid() == true) {
            bool isPlayAlarmTone = value.toBool();
            switch (index.row()) {
            case 0:
                (isPlayAlarmTone == true) ? ui->sensTopImage->hide() : ui->sensTopImage->show();
                break;
            case 1:
                (isPlayAlarmTone == true) ? ui->sensBottomImage->hide() : ui->sensBottomImage->show();
                break;
            }
        }
    }
}

void ScanLateralPanelTapeItem::generateTopPixmaps(int width, int height)
{
    if (_topNormalPix != nullptr) {
        delete _topNormalPix;
    }
    _topNormalPix = new QPixmap(width, height);
    drawPixmap(_topNormalPix, QColor(Qt::black), QColor(Qt::white), _prevTopSens);

    if (_topSelectedPix != nullptr) {
        delete _topSelectedPix;
    }
    _topSelectedPix = new QPixmap(width, height);
    drawPixmap(_topSelectedPix, QColor(Qt::white), QColor(Qt::black), _prevTopSens);

    if (_topAlarmedPix != nullptr) {
        delete _topAlarmedPix;
    }
    _topAlarmedPix = new QPixmap(width, height);
    drawPixmap(_topAlarmedPix, QColor(Qt::white), QColor(Qt::red), _prevTopSens);

    if (_topSelectedAlarmedPix != nullptr) {
        delete _topSelectedAlarmedPix;
    }
    _topSelectedAlarmedPix = new QPixmap(width, height);
    drawPixmap(_topSelectedAlarmedPix, QColor(Qt::red), QColor(Qt::black), _prevTopSens);

    if (_topNormalHideBlinkPix != nullptr) {
        delete _topNormalHideBlinkPix;
    }
    _topNormalHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_topNormalHideBlinkPix, QColor(), QColor(Qt::white), QString());

    if (_topSelectedHideBlinkPix != nullptr) {
        delete _topSelectedHideBlinkPix;
    }
    _topSelectedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_topSelectedHideBlinkPix, QColor(), QColor(Qt::black), QString());

    if (_topAlarmedHideBlinkPix != nullptr) {
        delete _topAlarmedHideBlinkPix;
    }
    _topAlarmedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_topAlarmedHideBlinkPix, QColor(), QColor(Qt::red), QString());

    if (_topSelectedAlarmedHideBlinkPix != nullptr) {
        delete _topSelectedAlarmedHideBlinkPix;
    }
    _topSelectedAlarmedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_topSelectedAlarmedHideBlinkPix, QColor(), QColor(Qt::black), QString());
}

void ScanLateralPanelTapeItem::generateBottomPixmaps(int width, int height)
{
    if (_bottomNormalPix != nullptr) {
        delete _bottomNormalPix;
    }
    _bottomNormalPix = new QPixmap(width, height);
    drawPixmap(_bottomNormalPix, QColor(Qt::black), QColor(Qt::white), _prevBottomSens);

    if (_bottomSelectedPix != nullptr) {
        delete _bottomSelectedPix;
    }
    _bottomSelectedPix = new QPixmap(width, height);
    drawPixmap(_bottomSelectedPix, QColor(Qt::white), QColor(Qt::black), _prevBottomSens);

    if (_bottomAlarmedPix != nullptr) {
        delete _bottomAlarmedPix;
    }
    _bottomAlarmedPix = new QPixmap(width, height);
    drawPixmap(_bottomAlarmedPix, QColor(Qt::white), QColor(Qt::red), _prevBottomSens);

    if (_bottomSelectedAlarmedPix != nullptr) {
        delete _bottomSelectedAlarmedPix;
    }
    _bottomSelectedAlarmedPix = new QPixmap(width, height);
    drawPixmap(_bottomSelectedAlarmedPix, QColor(Qt::red), QColor(Qt::black), _prevBottomSens);

    if (_bottomNormalHideBlinkPix != nullptr) {
        delete _bottomNormalHideBlinkPix;
    }
    _bottomNormalHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_bottomNormalHideBlinkPix, QColor(), QColor(Qt::white), QString());

    if (_bottomSelectedHideBlinkPix != nullptr) {
        delete _bottomSelectedHideBlinkPix;
    }
    _bottomSelectedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_bottomSelectedHideBlinkPix, QColor(), QColor(Qt::black), QString());

    if (_bottomAlarmedHideBlinkPix != nullptr) {
        delete _bottomAlarmedHideBlinkPix;
    }
    _bottomAlarmedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_bottomAlarmedHideBlinkPix, QColor(), QColor(Qt::red), QString());

    if (_bottomSelectedAlarmedHideBlinkPix != nullptr) {
        delete _bottomSelectedAlarmedHideBlinkPix;
    }
    _bottomSelectedAlarmedHideBlinkPix = new QPixmap(width, height);
    drawPixmap(_bottomSelectedAlarmedHideBlinkPix, QColor(), QColor(Qt::black), QString());
}

void ScanLateralPanelTapeItem::updateViewItem()
{
    _states[topItem].updateFsm();
    _states[bottomItem].updateFsm();
}

void ScanLateralPanelTapeItem::drawPixmap(QPixmap* pix, const QColor& penColor, const QColor& brushColor, const QString& text)
{
    Q_ASSERT(pix);
    QPainter painter(pix);
    painter.setFont(QFont(FONT_FAMILY, FONT_SIZE));
    QPen pen(QBrush(penColor), PEN_WIDTH);
    painter.setPen(pen);
    painter.fillRect(pix->rect(), QBrush(brushColor));
    if (!text.isEmpty()) {
        painter.drawText(pix->rect(), text, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
    }
}
