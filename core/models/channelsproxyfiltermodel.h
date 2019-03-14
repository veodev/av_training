#ifndef CHANNELSPROXYFILTERMODEL_H
#define CHANNELSPROXYFILTERMODEL_H

#include <QAbstractProxyModel>
#include "modeltypes.h"

class ChannelsProxyFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit ChannelsProxyFilterModel(QObject* parent = 0);
    ChannelsProxyFilterModel(ChannelType channelType = UnknownChannel, QObject* parent = 0);

    void setSourceModel(QAbstractItemModel* sourceModel);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& proxyIndex, const QVariant& value, int role = Qt::EditRole);

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

private slots:
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void modelAboutToBeReset();

private:
    ChannelType _channelType;
    QList<QModelIndex> _mapToSource;
    QList<QModelIndex> _mapFromSource;
    QAbstractItemModel* _channelsModel;
};

#endif  // CHANNELSPROXYFILTERMODEL_H
