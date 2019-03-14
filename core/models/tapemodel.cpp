#include <QDebug>

#include "tapemodel.h"
#include "roles.h"

TapeModel::TapeModel(QObject* parent)
    : QAbstractListModel(parent)
    , _channelsModel(0)
{
}

QModelIndex TapeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid()) {
        QAbstractItemModel* model = &(getItemFromIndex(parent)->channel);
        if ((model == 0) || (row < 0) || (column < 0) || (row >= model->rowCount()) || (column >= model->columnCount())) {
            return QModelIndex();
        }
        return model->index(row, column);
    }
    return QAbstractListModel::index(row, 0);
}

int TapeModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _data.count();
}

QVariant TapeModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) {
        return QVariant();
    }
    if (index.model() != this) {
        return index.model()->data(index, role);
    }

    if (index.row() < 0 || index.row() >= rowCount()) {
        return QVariant();
    }

    switch (role) {
    case AngleRole:
        return _data[index.row()]->angle;
    case AngleNoteRole:
        return _data[index.row()]->angleNote;
    case SideRole:
        return _data[index.row()]->side;
    case VisibleRole:
        return _data[index.row()]->visible;
    case TapeConformityRole:
        return _data[index.row()]->tapeConformity;
    case IsTapeVisibleInBoltJointModeRole:
        return _data[index.row()]->isTapeVisibleInBoltJointMode;
    case Qt::DisplayRole:
        return QString::number(_data[index.row()]->angle);
    case BScanForwardColorRole:
        return _data[index.row()]->forwardColor;
    case BScanBackwardColorRole:
        return _data[index.row()]->backwardColor;
    default:
        break;
    }

    return QVariant();
}

bool TapeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() == false) {
        return false;
    }
    if (index.model() != this) {
        QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
        if (model) {
            return model->setData(index, value, role);
        }
        return false;
    }

    if (index.row() < 0 || index.row() >= rowCount()) {
        return false;
    }

    switch (role) {
    case AngleRole:
        _data[index.row()]->angle = value.toInt();
        break;
    case AngleNoteRole:
        _data[index.row()]->angleNote = value.toString();
        break;
    case SideRole:
        _data[index.row()]->side = static_cast<DeviceSide>(value.toInt());
        break;
    case VisibleRole:
        _data[index.row()]->visible = value.toBool();
        break;
    case ShowSeparatorsRole:
    case HideSeparatorsRole:
        break;
    case TapeConformityRole:
        _data[index.row()]->tapeConformity = value.toInt();
        break;
    case IsTapeVisibleInBoltJointModeRole:
        _data[index.row()]->isTapeVisibleInBoltJointMode = value.toBool();
        break;
    case BScanForwardColorRole:
        _data[index.row()]->forwardColor = static_cast<unsigned short>(value.toUInt());
        break;
    case BScanBackwardColorRole:
        _data[index.row()]->backwardColor = static_cast<unsigned short>(value.toUInt());
        break;
    default:
        return false;
        break;
    }

    emit dataChanged(index, index, QVector<int>() << role);

    return true;
}

bool TapeModel::insertRows(int position, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        return _data[parent.row()]->channel.insertRows(position, count);
    }

    if (position < 0 || position > rowCount()) {
        return false;
    }

    beginInsertRows(QModelIndex(), position, position + count - 1);
    for (int i = position; i < position + count; ++i) {
        Item* item = new Item();
        item->channel.setSourceModel(_channelsModel);
        _data.insert(i, item);
    }
    endInsertRows();

    return true;
}

void TapeModel::reset()
{
    beginResetModel();
    while (_data.count() > 0) {
        _data.at(0)->channel.reset();
        delete _data.at(0);
        _data.removeAt(0);
    }
    endResetModel();
}

void TapeModel::setChannelsModel(QAbstractItemModel* channelsModel)
{
    _channelsModel = channelsModel;
}

TapeModel::Item* TapeModel::getItemFromIndex(const QModelIndex& index) const
{
    return _data[index.row()];
}
