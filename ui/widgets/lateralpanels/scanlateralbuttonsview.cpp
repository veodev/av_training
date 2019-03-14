#include <QVBoxLayout>
#include <QAbstractItemModel>

#include "scanlateralbuttonsview.h"
#include "scanlateralbuttonitem.h"
#include "roles.h"
#include "debug.h"
#include "modeltypes.h"

ScanLateralButtonsView::ScanLateralButtonsView(QWidget* parent)
    : QWidget(parent)
    , _model(nullptr)
    , _layout(new QVBoxLayout(this))
{
    _layout->setSpacing(1);
    _layout->setAlignment(Qt::AlignTop);

    setType(LeftPanel);
}

ScanLateralButtonsView::~ScanLateralButtonsView()
{
    Q_ASSERT(_layout);
    delete _layout;
}

void ScanLateralButtonsView::setType(LateralPanelType type)
{
    _type = type;

    if (type == RightPanel) {
        _layout->setContentsMargins(3, 0, 0, 0);
    }
    else if (type == LeftPanel) {
        _layout->setContentsMargins(0, 0, 3, 0);
    }

    for (ScanLateralButtonItem* button : _items) {
        Q_ASSERT(button);
        button->setType(_type);
    }
}

void ScanLateralButtonsView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (_model != nullptr) {
        detachModel();
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::destroyed, this, &ScanLateralButtonsView::detachModel));

    if (_model->rowCount() != 0) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, 0);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        DeviceSide prevSide = NoneDeviceSide;
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            const QModelIndex& index = _model->index(i, 0);
            if (index.isValid() == true) {
                DeviceSide side = static_cast<DeviceSide>(_model->data(index, SideRole).toInt());
                if ((i > 0) && (prevSide != side)) {
                    QWidget* widget = new QWidget(this);
                    widget->setSizePolicy(sizePolicy);
                    widget->setMinimumHeight(3);
                    widget->setMaximumHeight(3);
                    _separators.append(widget);
                    _layout->addWidget(widget);
                }
                prevSide = side;

                ScanLateralButtonItem* button = new ScanLateralButtonItem(i, _type, this);
                button->setSizePolicy(sizePolicy);
                button->setFocusPolicy(Qt::NoFocus);
                ASSERT(connect(button, &ScanLateralButtonItem::clicked, this, &ScanLateralButtonsView::clicked));
                _items.append(button);
                _layout->addWidget(button);
            }
        }
    }

    disconnect(_model, &QAbstractItemModel::dataChanged, this, &ScanLateralButtonsView::dataChanged);
    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &ScanLateralButtonsView::dataChanged, Qt::UniqueConnection));
}

void ScanLateralButtonsView::blockButtons(bool isBlock)
{
    for (ScanLateralButtonItem* item : _items) {
        Q_ASSERT(item);
        item->setDisabled(isBlock);
    }
}

void ScanLateralButtonsView::detachModel()
{
    _model = Q_NULLPTR;
    for (ScanLateralButtonItem* item : _items) {
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

void ScanLateralButtonsView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
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
                _items.at(i)->setVisible(value.toBool());
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

void ScanLateralButtonsView::setSeparatorVisible(bool value)
{
    for (QWidget* separator : _separators) {
        Q_ASSERT(separator);
        separator->setVisible(value);
    }
}
