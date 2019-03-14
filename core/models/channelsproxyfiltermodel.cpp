#include "channelsproxyfiltermodel.h"
#include "debug.h"
#include "roles.h"
#include "modeltypes.h"

ChannelsProxyFilterModel::ChannelsProxyFilterModel(QObject* parent)
    : QAbstractProxyModel(parent)
    , _channelType(UnknownChannel)
    , _channelsModel(0)
{
}

ChannelsProxyFilterModel::ChannelsProxyFilterModel(ChannelType channelType, QObject* parent)
    : QAbstractProxyModel(parent)
    , _channelType(channelType)
    , _channelsModel(0)
{
}

void ChannelsProxyFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    modelAboutToBeReset();

    if (_channelsModel != 0) {
        disconnect(_channelsModel);
    }
    _channelsModel = sourceModel;
    ASSERT(connect(_channelsModel, &QAbstractItemModel::dataChanged, this, &ChannelsProxyFilterModel::dataChanged));
    ASSERT(connect(_channelsModel, &QAbstractItemModel::modelAboutToBeReset, this, &ChannelsProxyFilterModel::modelAboutToBeReset));

    for (int i = 0; i < sourceModel->rowCount(); ++i) {
        QModelIndex index = _channelsModel->index(i, 0);
        QVariant value = _channelsModel->data(index, ChannelTypeRole);
        Q_ASSERT(value.isValid() != false);
        ChannelType channelType = static_cast<ChannelType>(value.toInt());
        if (channelType == _channelType) {
            _mapToSource.append(index);
            _mapFromSource.append(createIndex(_mapToSource.count() - 1, 0, (quintptr) 0));
        }
        else {
            _mapFromSource.append(QModelIndex());
        }
    }
}

QModelIndex ChannelsProxyFilterModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    if (row < 0 || row >= rowCount() || column != 0) {
        return QModelIndex();
    }

    return createIndex(row, column, (quintptr) 0);
}

QModelIndex ChannelsProxyFilterModel::parent(const QModelIndex& child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

int ChannelsProxyFilterModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _mapToSource.count();
}

int ChannelsProxyFilterModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return 0;
}

QVariant ChannelsProxyFilterModel::data(const QModelIndex& proxyIndex, int role) const
{
    return _channelsModel->data(mapToSource(proxyIndex), role);
}

bool ChannelsProxyFilterModel::setData(const QModelIndex& proxyIndex, const QVariant& value, int role)
{
    return _channelsModel->setData(mapToSource(proxyIndex), value, role);
}

QModelIndex ChannelsProxyFilterModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (sourceIndex.isValid() == false || sourceIndex.row() >= _mapFromSource.count()) {
        return QModelIndex();
    }

    return _mapFromSource[sourceIndex.row()];
}

QModelIndex ChannelsProxyFilterModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (proxyIndex.isValid() == false || proxyIndex.row() >= _mapToSource.count()) {
        return QModelIndex();
    }

    return _mapToSource[proxyIndex.row()];
}

void ChannelsProxyFilterModel::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        for (int column = topLeft.column(); column <= bottomRight.column(); ++column) {
            QModelIndex index = _channelsModel->index(row, column);
            index = mapFromSource(index);
            if (index.isValid()) {
                emit QAbstractProxyModel::dataChanged(index, index, roles);
            }
        }
    }
}

void ChannelsProxyFilterModel::modelAboutToBeReset()
{
    beginResetModel();
    _mapToSource.clear();
    _mapFromSource.clear();
    endResetModel();
}
