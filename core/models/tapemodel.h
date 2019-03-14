#ifndef TAPEMODEL_H
#define TAPEMODEL_H

#include <QAbstractListModel>
#include <QAbstractTableModel>

#include "channelsproxytablemodel.h"
#include "modeltypes.h"

class TapeModel : public QAbstractListModel
{
    friend class DefcoreThread;
    Q_OBJECT

public:
    explicit TapeModel(QObject* parent = 0);

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int position, int count, const QModelIndex& parent = QModelIndex());

    void reset();

    void setChannelsModel(QAbstractItemModel* channelsModel);

private:
    struct Item;
    Item* getItemFromIndex(const QModelIndex& index) const;

private:
    struct Item
    {
        Item()
            : angle(-1)
            , side(NoneDeviceSide)
            , visible(false)
            , tapeConformity(-1)
            , isTapeVisibleInBoltJointMode(false)
            , forwardColor(0x0000)
            , backwardColor(0xFFFF)
        {
        }
        ChannelsProxyTableModel channel;
        int angle;
        QString angleNote;
        DeviceSide side;
        bool visible;
        int tapeConformity;
        bool isTapeVisibleInBoltJointMode;
        unsigned short forwardColor;
        unsigned short backwardColor;
    };

    QList<Item*> _data;
    QAbstractItemModel* _channelsModel;
};

#endif  // TAPEMODEL_H
