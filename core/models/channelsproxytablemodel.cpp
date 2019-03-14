#include "channelsproxytablemodel.h"
#include "roles.h"
#include "debug.h"

#define UNSETTED (-1)
#define FAKED (-2)

ChannelsProxyTableModel::ChannelsProxyTableModel(QObject* parent)
    : QAbstractProxyModel(parent)
    , _channelsModel(nullptr)
{
}

QModelIndex ChannelsProxyTableModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return createIndex(row, column, nullptr);
}

QModelIndex ChannelsProxyTableModel::parent(const QModelIndex& child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

void ChannelsProxyTableModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    if (_channelsModel != nullptr) {
        disconnect(_channelsModel);
    }
    _channelsModel = sourceModel;
    ASSERT(connect(_channelsModel, &QAbstractItemModel::dataChanged, this, &ChannelsProxyTableModel::dataChanged));
}

QAbstractItemModel* ChannelsProxyTableModel::sourceModel() const
{
    return _channelsModel;
}

int ChannelsProxyTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _data.size();
}

int ChannelsProxyTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return columnsCount;
}

QVariant ChannelsProxyTableModel::data(const QModelIndex& proxyIndex, int role) const
{
    if (role == FakedChannelRole) {
        if (proxyIndex.isValid() == false || proxyIndex.column() < 0 || proxyIndex.column() >= columnCount() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
            return false;
        }
        Item* item = getItem(proxyIndex);
        if (*item == FAKED) {
            return true;
        }

        return false;
    }
    return _channelsModel->data(mapToSource(proxyIndex), role);
}

bool ChannelsProxyTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() == false || index.column() < 0 || index.column() >= columnCount() || index.row() < 0 || index.row() >= rowCount()) {
        return false;
    }

    if (role == FakedChannelRole) {
        Item* item = getItem(index);
        *item = FAKED;
        return true;
    }

    return _channelsModel->setData(getItemIndexAndAddIfNotExist(index), value, role);
}

bool ChannelsProxyTableModel::insertRows(int position, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), position, position + count - 1);
    for (int i = position; i < position + count; ++i) {
        Row rowItem = new Item[columnsCount];
        rowItem[0] = UNSETTED;
        rowItem[1] = UNSETTED;
        if (i >= _data.size()) {
            _data.push_back(rowItem);
        }
        else {
            _data.insert(_data.begin() + i, rowItem);
        }
    }
    endInsertRows();

    return true;
}

QModelIndex ChannelsProxyTableModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    Q_UNUSED(sourceIndex);

    int i = 0;
    for (const Row row : _data) {
        for (int j = 0; j < columnsCount; ++j) {
            if (row[j] == sourceIndex.row()) {
                return index(i, j);
            }
        }
        ++i;
    }

    return QModelIndex();
}

QModelIndex ChannelsProxyTableModel::mapToSource(const QModelIndex& proxyIndex) const
{
    int row = proxyIndex.row();
    int col = proxyIndex.column();
    if (proxyIndex.isValid() == false || row >= _data.size() || row < 0 || col >= columnsCount || col < 0) {
        return QModelIndex();
    }

    return _channelsModel->index(_data[row][col], 0);
}

void ChannelsProxyTableModel::reset()
{
    beginResetModel();
    for (Row rowItem : _data) {
        delete[] rowItem;
    }
    _data.clear();
    endResetModel();
}

void ChannelsProxyTableModel::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
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

ChannelsProxyTableModel::Item* ChannelsProxyTableModel::getItem(const QModelIndex& index) const
{
    return &(_data[index.row()][index.column()]);
}

QModelIndex ChannelsProxyTableModel::getItemIndex(ChannelsProxyTableModel::Item* item)
{
    Q_ASSERT(item);
    return _channelsModel->index(*item, 0);
}

QModelIndex ChannelsProxyTableModel::getItemIndexAndAddIfNotExist(const QModelIndex& index)
{
    Item* item = getItem(index);
    if (*item == UNSETTED) {
        if (_channelsModel->insertRows(_channelsModel->rowCount(), 1)) {
            QModelIndex index = _channelsModel->index(_channelsModel->rowCount() - 1, 0);
            *item = index.row();
            return index;
        }
    }
    else {
        return getItemIndex(item);
    }

    return QModelIndex();
}
