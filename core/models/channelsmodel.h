#ifndef CHANNELSMODEL_H
#define CHANNELSMODEL_H

#include <QAbstractListModel>

#include "roles.h"
#include "modeltypes.h"
#include "direction.h"

class ChannelsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChannelsModel(QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    bool insertRows(int position, int count, const QModelIndex& parent = QModelIndex());

    void reset();

private:
    friend class ChannelsTableModel;
    struct Item
    {
        Item()
            : isAlarmed(false)
            , isPlayAlarmTone(true)
            , isStartGateLocked(false)
            , isEndGateLocked(false)
            , isNotchEnable(false)
            , isCopySlaveChannel(false)
            , channelId(-1)
            , gain(0)
            , zone(0)
            , sens(0)
            , alarmTone(0)
            , angle(-1)
            , strokeDuration(0)
            , recommendedSens(0)
            , gateIndex(0)
            , gateCount(0)
            , startBscanGate(0)
            , endBscanGate(0)
            , defaultStartGate(0)
            , defaultEndGate(0)
            , defaultGain(0)
            , delayMultiply(1)
            , aScanLenMks(0)
            , aScanScale(1)
            , isSensOutRange(0)
            , calibrationTemperature(cTemperatureValueUnknown)
            , startNotch(0)
            , sizeNotch(0)
            , levelNotch(0)
            , tvg(0)
            , startGate(0)
            , endGate(0)
            , prismDelay(0.0)
            , mark(0)
            , frequency(wf2_5MHz)
            , channelType(UnknownChannel)
            , side(NoneDeviceSide)
            , method(imNotSet)
            , direction(UnknownDirection)
            , note("")
            , channelName("")
            , channelTitle("")
        {
        }
        bool isAlarmed;
        bool isPlayAlarmTone;
        bool isStartGateLocked;
        bool isEndGateLocked;
        bool isNotchEnable;
        bool isCopySlaveChannel;

        int channelId;
        int gain;
        int zone;  // enum eControlZone from Definitions.h
        int sens;
        int alarmTone;
        int angle;
        int strokeDuration;
        int recommendedSens;
        int gateIndex;
        int gateCount;
        int startBscanGate;
        int endBscanGate;
        int defaultStartGate;
        int defaultEndGate;
        int defaultGain;
        int delayMultiply;
        int aScanLenMks;
        int aScanScale;
        int isSensOutRange;
        int calibrationTemperature;
        int startNotch;
        int sizeNotch;
        int levelNotch;

        float tvg;
        float startGate;
        float endGate;
        float prismDelay;
        float mark;

        eWorkFrequency frequency;
        ChannelType channelType;
        DeviceSide side;
        eInspectionMethod method;
        Direction direction;

        QString note;
        QString channelName;
        QString channelTitle;
        unsigned char bscanColorIndex;
        unsigned short bscanColor;
        unsigned short bscanSpecialColor;
        unsigned short bscanIndividualColor;
    };
    std::vector<Item> _data;
};

#endif  // CHANNELSMODEL_H
