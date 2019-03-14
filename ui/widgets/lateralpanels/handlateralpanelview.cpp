#include <QAbstractProxyModel>

#include "handlateralpanelview.h"
#include "ui_handlateralpanelview.h"

#include "handlateralbuttonsview.h"
#include "roles.h"
#include "core.h"
#include "channelscontroller.h"
#include "debug.h"

HandLateralPanelView::HandLateralPanelView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HandLateralPanelView)
    , _model(0)
    , _dependedButtonsView(Q_NULLPTR)
    , _curentSelectionIndex(-1)
{
    ui->setupUi(this);

    HandLateralPanelItem* item;

    int index = 0;
    for (unsigned char i = 0; i < 5; ++i) {
        item = new HandLateralPanelItem(this, index++);
        _items.append(item);
        ui->verticalLayout0->layout()->addWidget(item);
        ASSERT(connect(item, &HandLateralPanelItem::clicked, this, &HandLateralPanelView::clicked));
    }
    for (unsigned char i = 0; i < 5; ++i) {
        item = new HandLateralPanelItem(this, index++);
        _items.append(item);
        ui->verticalLayout1->layout()->addWidget(item);
        ASSERT(connect(item, &HandLateralPanelItem::clicked, this, &HandLateralPanelView::clicked));
    }
}

HandLateralPanelView::~HandLateralPanelView()
{
    delete ui;
}

bool HandLateralPanelView::select(int index)
{
    bool rc = false;
    if (index >= 0 && index < _items.size()) {
        QModelIndex modelIndex = _model->index(index, 0);
        QModelIndex channelsModelIndex = reinterpret_cast<QAbstractProxyModel*>(_model)->mapToSource(modelIndex);
        if (channelsModelIndex.isValid()) {
            _items.at(index)->select();
            if (_curentSelectionIndex != -1 && _curentSelectionIndex != index) {
                _items.at(_curentSelectionIndex)->unselect();
            }
            _curentSelectionIndex = index;
            Core::instance().channelsController()->setChannel(channelsModelIndex);
            rc = true;
        }
    }

    return rc;
}

bool HandLateralPanelView::selectHeadScan(int index)
{
    bool rc = false;
    if (index >= 0 && index < _items.size()) {
        QModelIndex modelIndex = _model->index(index, 0);
        if (modelIndex.isValid()) {
            _items.at(index)->select();
            if (_curentSelectionIndex != -1 && _curentSelectionIndex != index) {
                _items.at(_curentSelectionIndex)->unselect();
            }
            _curentSelectionIndex = index;
            Core::instance().channelsController()->setHeadChannel(modelIndex);
            rc = true;
        }
    }

    return rc;
}

void HandLateralPanelView::unselect(int index)
{
    if (index >= 0 && index < _items.size()) {
        _items.at(index)->unselect();
    }
}

void HandLateralPanelView::clearSelection()
{
    for (int i = 0; i < _items.count(); ++i) {
        unselect(i);
    }
    _curentSelectionIndex = -1;
}

void HandLateralPanelView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (_model != 0) {
        _model->disconnect(this);
    }
    _model = model;

    if (_model != NULL && _model->rowCount()) {
        QModelIndex topLeft = _model->index(0, 0);
        QModelIndex bottomRight = _model->index(_model->rowCount() - 1, 0);
        dataChanged(topLeft, bottomRight, QVector<int>() << AngleRole);

        connect(_model, &QAbstractItemModel::dataChanged, this, &HandLateralPanelView::dataChanged, Qt::UniqueConnection);
        connect(_model, &QAbstractItemModel::modelAboutToBeReset, this, &HandLateralPanelView::modelAboutToBeReset, Qt::UniqueConnection);
    }

    if (_dependedButtonsView != Q_NULLPTR) {
        _dependedButtonsView->setModel(model);
    }
}

void HandLateralPanelView::setDependedButtonsView(HandLateralButtonsView* dependedButtonsView)
{
    if (_dependedButtonsView == dependedButtonsView) {
        return;
    }
    _dependedButtonsView = dependedButtonsView;

    ASSERT(connect(_dependedButtonsView, &HandLateralButtonsView::clicked, this, &HandLateralPanelView::clicked));

    _dependedButtonsView->setVisible(this->isVisible());
}

void HandLateralPanelView::blockPanel(bool isBlock)
{
    for (HandLateralPanelItem* item : _items) {
        item->setDisabled(isBlock);
    }
}

void HandLateralPanelView::setVisible(bool visible)
{
    if (_dependedButtonsView != Q_NULLPTR) {
        _dependedButtonsView->setVisible(visible);
    }
    QWidget::setVisible(visible);
}

void HandLateralPanelView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        Q_ASSERT(i < _items.count());
        QModelIndex index = _model->index(i, 0);
        for (int role : roles) {
            if (role == AngleRole) {
                QVariant value = _model->data(index, AngleRole);
                _items.at(i)->setAngle(value.toInt());
            }
            else if (role == IsAlarmedRole) {
                QVariant value = _model->data(index, IsAlarmedRole);
                if (value.toBool() == true) {
                    _items.at(i)->alarm();
                }
                else {
                    _items.at(i)->unalarm();
                }
            }
        }
    }
}

void HandLateralPanelView::modelAboutToBeReset()
{
    for (HandLateralPanelItem* item : _items) {
        item->reset();
    }
}

void HandLateralPanelView::reset()
{
    for (HandLateralPanelItem* item : _items) {
        item->deleteLater();
    }
    _items.clear();
}
