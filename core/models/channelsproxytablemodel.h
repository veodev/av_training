#ifndef CHANNELSPROXYTABLEMODEL_H
#define CHANNELSPROXYTABLEMODEL_H

#include <QAbstractProxyModel>

class ChannelsProxyTableModel : public QAbstractProxyModel
{
    friend class DefcoreThread;
    Q_OBJECT

public:
    explicit ChannelsProxyTableModel(QObject* parent = 0);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;

    void setSourceModel(QAbstractItemModel* sourceModel);
    QAbstractItemModel* sourceModel() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int position, int count, const QModelIndex& parent = QModelIndex());

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

    void reset();

private slots:
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
    static const int columnsCount = 2;
    typedef int Item;
    typedef Item* Row;

    Item* getItem(const QModelIndex& index) const;
    QModelIndex getItemIndex(Item* item);
    QModelIndex getItemIndexAndAddIfNotExist(const QModelIndex& index);

private:
    std::vector<Row> _data;

    QAbstractItemModel* _channelsModel;
};

#endif  // CHANNELSPROXYTABLEMODEL_H
