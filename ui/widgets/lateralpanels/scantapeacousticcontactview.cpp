#include <QVBoxLayout>
#include <QAbstractItemModel>

#include "scantapeacousticcontactview.h"
#include "scantapeacousticcontactitem.h"
#include "modeltypes.h"
#include "roles.h"
#include "core.h"
#include "debug.h"
#include "settings.h"
#include "accessories.h"

ScanTapeAcousticContactView::ScanTapeAcousticContactView(QWidget* parent)
    : QWidget(parent)
    , _model(Q_NULLPTR)
    , _layout(new QVBoxLayout(this))
    , _type(LeftPanel)
{
    _layout->setSpacing(1);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setAlignment(Qt::AlignTop);
}

ScanTapeAcousticContactView::~ScanTapeAcousticContactView()
{
    Q_ASSERT(_layout);
    delete _layout;
}

void ScanTapeAcousticContactView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);
    deleteItems();
    _model = model;

    if (_model->rowCount() != 0) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, 0);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        DeviceSide prevSide = NoneDeviceSide;
        for (int tapeNumber = topLeft.row(); tapeNumber <= bottomRight.row(); ++tapeNumber) {
            const QModelIndex& tapeIndex = _model->index(tapeNumber, 0);
            const QModelIndex& index = _model->index(0, 0, tapeIndex);
            if (index.isValid() == true) {
                for (int row = 0; row < 2; ++row) {
                    int column;
                    _type == LeftPanel ? column = 0 : column = 1;
                    const QModelIndex& childIndex = _model->index(row, column, tapeIndex);
                    int key = createKeyFromSideAndChannel(_model->data(childIndex, SideRole).toInt(), _model->data(childIndex, ChannelIdRole).toInt());
                    if (_channelsMap.contains(key) == false) {
                        Item item;
                        item.tapeNumber = tapeNumber;
                        item.segment = static_cast<AcousticContactSegment>(row);
                        _channelsMap.insert(key, item);
                    }
                }
                DeviceSide side = static_cast<DeviceSide>(_model->data(index, SideRole).toInt());
                if ((tapeNumber > 0) && (prevSide != side)) {
                    QWidget* widget = new QWidget(this);
                    widget->setSizePolicy(sizePolicy);
                    widget->setMinimumHeight(1);
                    widget->setMaximumHeight(1);
                    _layout->addWidget(widget);
                    _separators.push_back(widget);
                }
                prevSide = side;
                ScanTapeAcousticContactItem* scanTapeAcousticContactItem = new ScanTapeAcousticContactItem();
                scanTapeAcousticContactItem->setSizePolicy(sizePolicy);
                if (_model->data(index, GateCountRole).toInt() == 2 || index.model()->rowCount() < 2) {
                    scanTapeAcousticContactItem->hideBottomWidget();
                }
                else if (index.model()->rowCount() > 1) {
                    scanTapeAcousticContactItem->showBottomWidget();
                }
                _items.push_back(scanTapeAcousticContactItem);
                _layout->addWidget(scanTapeAcousticContactItem);
            }
            else {
                QWidget* widget = new QWidget(this);
                widget->setSizePolicy(sizePolicy);
                _layout->addWidget(widget);
                _separators.push_back(widget);
            }
        }
    }
    disconnect(_model, &QAbstractItemModel::dataChanged, this, &ScanTapeAcousticContactView::onDataChanged);
    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &ScanTapeAcousticContactView::onDataChanged));
}

void ScanTapeAcousticContactView::setType(LateralPanelType type)
{
    _type = type;
}

void ScanTapeAcousticContactView::clearView()
{
    for (ScanTapeAcousticContactItem* item : _items) {
        Q_ASSERT(item);
        item->setAcousticContactState(TopSegment, true);
        item->setAcousticContactState(BottomSegment, true);
    }
}

void ScanTapeAcousticContactView::deleteItems()
{
    for (ScanTapeAcousticContactItem* item : _items) {
        Q_ASSERT(item);
        item->setParent(Q_NULLPTR);
        delete item;
    }
    _items.clear();

    for (QWidget* item : _separators) {
        Q_ASSERT(item);
        item->setParent(Q_NULLPTR);
        delete item;
    }
    _separators.clear();
}

void ScanTapeAcousticContactView::onBScanData(QSharedPointer<tDEV_BScan2Head> head)
{    
    Q_ASSERT(head.data() != 0);

    for (const tDEV_ACHeadItem& item : head.data()->ACItems) {
        int key = createKeyFromSideAndChannel(item.Side, item.Channel);

        if (_channelsMap.contains(key)) {
            const Item& itemOfMap = _channelsMap[key];
            bool acValue = item.ACValue;
            if (_items[itemOfMap.tapeNumber]->prevState() != acValue) {
                _items[itemOfMap.tapeNumber]->updateState(acValue);
                bool state = checkNotification();
                Core::instance().setACNotifyState(_type, state);
            }
            _items[itemOfMap.tapeNumber]->setAcousticContactState(itemOfMap.segment, acValue);
        }
    }
}
void ScanTapeAcousticContactView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_ASSERT(_model);
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        if (i >= _items.size()) {
            break;
        }

        const QModelIndex& index = _model->index(i, 0);
        for (int role : roles) {
            switch (role) {
            case VisibleRole: {
                QVariant value = _model->data(index, VisibleRole);
                Q_ASSERT(value.isValid());
                _items[i]->setVisible(value.toBool());
            } break;
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

void ScanTapeAcousticContactView::setSeparatorVisible(bool value)
{
    for (QWidget* separator : _separators) {
        separator->setVisible(value);
    }
}

bool ScanTapeAcousticContactView::checkNotification()
{
    for (const auto& item : _items) {
        if (!item->prevState()) {
            return false;
        }
    }
    return true;
}
