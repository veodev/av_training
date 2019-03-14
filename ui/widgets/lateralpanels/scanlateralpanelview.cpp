#include <QVBoxLayout>

#include "scanlateralpanelview.h"

#include "scanlateralpanelitem.h"
#include "scanlateralbuttonsview.h"
#include "styles.h"
#include "roles.h"
#include "debug.h"
#include "modeltypes.h"

const int BLINK_INTERVAL_MS = 1500;

ScanLateralPanelView::ScanLateralPanelView(QWidget* parent)
    : QWidget(parent)
    , _type(LeftPanel)
    , _model(nullptr)
    , _dependedPanel(nullptr)
    , _dependedButtonsView(nullptr)
    , _curentSelectionIndex(-1)
    , _blinkingTimer(this)
    , _isGlow(true)
{
    _layout = new QVBoxLayout(this);
    _layout->setSpacing(1);
    _layout->setContentsMargins(1, 0, 1, 0);
    _layout->setAlignment(Qt::AlignTop);

    QPalette palette;
    QBrush brush(QColor(255, 255, 255, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    QBrush brush1(QColor(149, 147, 145, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
    setPalette(palette);
    setAutoFillBackground(true);

    connect(&_blinkingTimer, &QTimer::timeout, this, &ScanLateralPanelView::onBlinkingTimerTimeout);
    _blinkingTimer.start(BLINK_INTERVAL_MS);
}

ScanLateralPanelView::~ScanLateralPanelView()
{
    _blinkingTimer.stop();
}

void ScanLateralPanelView::setType(LateralPanelType type)
{
    _type = type;

#if defined(IMX_DEVICE)
    if (type == RightPanel) {
        setMinimumWidth(22);
        setMaximumWidth(22);
    }
    else {
#endif

#if defined ANDROID
        setMinimumWidth(100);
        setMaximumWidth(100);
#else
    setMinimumWidth(68);
    setMaximumWidth(68);
#endif
#if defined TARGET_AVICONDBHS || defined TARGET_AVICONDB
        setMinimumWidth(100);
        setMaximumWidth(100);
#endif

#if defined(IMX_DEVICE)
    }
#endif

    for (ScanLateralPanelItem* item : _items) {
        item->setSide(_type);
    }

    if (_dependedButtonsView != nullptr) {
        _dependedButtonsView->setType(_type);
    }
}

bool ScanLateralPanelView::select(int index)
{
    if (index >= 0 && index < _items.size()) {
        if (_items[index]->select(index) == false) {
            return false;
        }
        if (_dependedPanel != nullptr) {
            _dependedPanel->clearSelection();
        }
        if (_curentSelectionIndex != -1 && _curentSelectionIndex != index) {
            _items[_curentSelectionIndex]->unselect();
        }
        _curentSelectionIndex = index;
    }

    return true;
}

void ScanLateralPanelView::unselect(int index)
{
    if (index >= 0 && index < _items.size()) {
        _items[index]->unselect();
    }
}

void ScanLateralPanelView::clearSelection()
{
    for (int i = 0; i < _items.count(); ++i) {
        unselect(i);
    }
}

void ScanLateralPanelView::alarm(int index)
{
    if (index >= 0 && index < _items.size()) {
        _items[index]->alarm();
    }
}

void ScanLateralPanelView::unalarm(int index)
{
    if (index >= 0 && index < _items.size()) {
        _items[index]->unalarm();
    }
}

void ScanLateralPanelView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (_model != nullptr) {
        _model->disconnect(this);
    }

    if (_model != nullptr) {
        detachModel();
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::destroyed, this, &ScanLateralPanelView::detachModel));

    if (_model->rowCount() != 0) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, 0);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        DeviceSide prevSide = NoneDeviceSide;
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            const QModelIndex& parentModel = _model->index(i, 0);
            const QModelIndex& index = _model->index(0, 0, parentModel);
            if (index.isValid() == true) {
                DeviceSide side = static_cast<DeviceSide>(_model->data(index, SideRole).toInt());
                if ((i > 0) && (prevSide != side) && prevSide != NoneDeviceSide) {
                    QWidget* widget = new QWidget(this);
                    widget->setSizePolicy(sizePolicy);
                    widget->setMinimumHeight(1);
                    widget->setMaximumHeight(1);
                    _separators.append(widget);
                    _layout->addWidget(widget);
                }
                prevSide = side;
                ScanLateralPanelItem* scanLateralPanelTapeItem = new ScanLateralPanelItem(i, this);
                scanLateralPanelTapeItem->setSide(_type);
                scanLateralPanelTapeItem->setModel(const_cast<QAbstractItemModel*>(index.model()));
                scanLateralPanelTapeItem->setSizePolicy(sizePolicy);
                _items.append(scanLateralPanelTapeItem);
                ASSERT(connect(scanLateralPanelTapeItem, &ScanLateralPanelItem::clicked, this, &ScanLateralPanelView::itemClicked));
                _layout->addWidget(scanLateralPanelTapeItem);
            }
            else {
                QWidget* widget = new QWidget(this);
                widget->setSizePolicy(sizePolicy);
                _separators.append(widget);
                _layout->addWidget(widget);
            }
        }

        dataChanged(topLeft, bottomRight, QVector<int>() << AngleRole << AngleNoteRole);
    }

    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &ScanLateralPanelView::dataChanged, Qt::UniqueConnection));
    ASSERT(connect(_model, &QAbstractItemModel::modelAboutToBeReset, this, &ScanLateralPanelView::modelAboutToBeReset, Qt::UniqueConnection));

    if (_dependedButtonsView != nullptr) {
        _dependedButtonsView->setModel(model);
    }
}

void ScanLateralPanelView::setDependedPanel(ScanLateralPanelView* panel)
{
    _dependedPanel = panel;
}

void ScanLateralPanelView::setDependedButtonsView(ScanLateralButtonsView* dependedButtonsView)
{
    if (_dependedButtonsView == dependedButtonsView) {
        return;
    }
    _dependedButtonsView = dependedButtonsView;

    ASSERT(connect(_dependedButtonsView, &ScanLateralButtonsView::clicked, this, &ScanLateralPanelView::clicked));

    _dependedButtonsView->setType(_type);
    _dependedButtonsView->setVisible(this->isVisible());
}

void ScanLateralPanelView::blockButtons(bool isBlock)
{
    for (ScanLateralPanelItem* item : _items) {
        item->setDisabled(isBlock);
    }
}

void ScanLateralPanelView::setVisible(bool visible)
{
    if (_dependedButtonsView != nullptr) {
        _dependedButtonsView->setVisible(visible);
    }
    QWidget::setVisible(visible);
}

void ScanLateralPanelView::detachModel()
{
    _model = nullptr;
    for (ScanLateralPanelItem* item : _items) {
        item->setParent(nullptr);
        delete item;
    }
    _items.clear();
    for (QWidget* item : _separators) {
        item->setParent(nullptr);
        delete item;
    }
    _separators.clear();
}

void ScanLateralPanelView::onBlinkingTimerTimeout()
{
    _isGlow = !_isGlow;
    emit blinkingTimerTimeOut(_isGlow);
}

void ScanLateralPanelView::setSeparatorVisible(bool value)
{
    for (QWidget* sparator : _separators) {
        sparator->setVisible(value);
    }
}

void ScanLateralPanelView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        if (i >= _items.size()) {
            break;
        }
        QVariant value;
        const QModelIndex& index = _model->index(i, 0);
        for (int role : roles) {
            switch (role) {
            case AngleRole:
#if defined(IMX_DEVICE)
                if (_type != RightPanel) {
#endif
                    value = _model->data(index, AngleRole);
                    _items[i]->setAngle(value.toInt());
#if defined TARGET_AVICONDB
                    if (_type == RightPanel && value.toInt() == 55) {  // TODO: Needs create a hard ui config file for each device.
                        _items.at(i)->hideAngle();
                    }
#endif
#if defined(IMX_DEVICE)
                }
#endif
                break;
            case AngleNoteRole:
#if defined(IMX_DEVICE)
                if (_type != RightPanel) {
#endif
                    value = _model->data(index, AngleNoteRole);
                    if (value.toString().length() > 0) {
                        _items[i]->setAngleNote(value.toString());
                    }
#if defined(IMX_DEVICE)
                }
#endif
                break;
            case VisibleRole:
                value = _model->data(index, VisibleRole);
                Q_ASSERT(value.isValid());
                _items[i]->setVisible(value.toBool());
                break;
            case ShowSeparatorsRole:
                setSeparatorVisible(true);
                break;
            case HideSeparatorsRole:
                setSeparatorVisible(false);
                break;
            default:
                break;
            }
        }
    }
}

void ScanLateralPanelView::modelAboutToBeReset()
{
    for (ScanLateralPanelItem* item : _items) {
        item->reset();
    }
}

void ScanLateralPanelView::itemClicked(int index)
{
    emit clicked(index, _type);
}
