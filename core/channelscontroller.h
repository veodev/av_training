#ifndef CHANNELSCONTROLLER_H
#define CHANNELSCONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include <QTimer>

#include "roles.h"
#include "modeltypes.h"
#include "Definitions.h"
#include "coredefinitions.h"

class Defcore;
class cChannelsTable;
class cDeviceConfig;
class cDeviceCalibration;
class QAbstractItemModel;
class QTimer;
class TapeModel;
class ChannelsModel;
class ChannelsProxyFilterModel;
class HandChannelsModel;
class tDEV_AlarmHead;

class ChannelsController : public QObject
{
    Q_OBJECT

public:
    explicit ChannelsController(QObject* parent = 0);
    ~ChannelsController();

    void setDefcore(Defcore* defcore);
    void setChannelsTable(cChannelsTable* channelsTable);
    void setDeviceConfig(cDeviceConfig* deviceConfig);
    void setDeviceConfigHeadScan(cDeviceConfig* deviceConfigHeadScan);
    void setDeviceCalibration(cDeviceCalibration* deviceCalibration);

    QAbstractItemModel* channelsModel() const;
    QAbstractItemModel* headScanChannelsModel() const;
    QAbstractItemModel* tapeModel() const;
    QAbstractItemModel* scanChannelsModel() const;
    QAbstractItemModel* handChannelsModel() const;

    void setDataForAllItems(const QVariant& value, Roles role);
    void setDataForSlaveChannel();

    void setSens(int value);
    void setTimeDelay(qreal value);
    void setStartGate(int value);
    void setEndGate(int value);
    void setTvg(int value);
    void setStartNotch(int value);
    void setSizeNotch(int value);
    void setLevelNotch(int value);
    void setMark(int value);

    void setChannel(const QModelIndex& index);
    void setHeadChannel(const QModelIndex& index);
    void selectTape(int tapeIndex, int position, int side);

    const QModelIndex& currentModelIndex() const;
    const QModelIndex modelIndexByCidSideAndGateIndex(int cid, int side, int gateIndex) const;
    const QModelIndex headScanModelIndexByCidSideAndGateIndex(int cid, int side, int gateIndex) const;

    void recreateModels();
    void recreateHeadScanModel();

    void getGatesForMsmChannel(int& startGate, int& endGate);
    void getGatesForMsmChannel(int& startGateLeft, int& endGateLeft, int& startGateRight, int& endGateRight);
    void setStartGatesForMsmChannel(eDeviceSide& side, int& value);
    void setEndGatesForMsmChannel(eDeviceSide& side, int& value);
    void setPlayAlarm(const QModelIndex& index, const eGateAlarmMode& value);

    void setCalibrationMode();
    void setSearchMode();
    eControlledRail contoledRail() const;
    void onViewModeCalibrationChanged();

signals:
    void doChannelSetted(const QModelIndex& index);
    void doHeadChannelSetted(const QModelIndex& index);
    void doTapeSelected(int tapeIndex, int position, int side);

    void doSetChannel(CID id);
    void doSetSide(DeviceSide side);
    void doSetTvg(int value);
    void doSetStartNotch(int value);
    void doSetSizeNotch(int value);
    void doSetLevelNotch(int value);
    void doSetPrismDelay(int value);
    void doSetSens(int value);
    void doSetStartGate(int value);
    void doSetEndGate(int value);
    void doSetGateIndex(int gateIndex);
    void doSetMark(int value);
    void doDeviceUpdate(bool resetStrokes);

    void changeCalibration();

public slots:
    void onCalibrationChanged();
    void onColorSchemeChanged();

private slots:
    void onAlarmData(QSharedPointer<tDEV_AlarmHead> head);
    void alarmExpiredTimeTimeout();

    void onDefcoreDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);
    void onDefcoreHeadScanChannelsDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);


private:
    void updateChannelsMap();
    void updateHeadScanChannelsMap();
    int getTone(int angle, eDeviceSide side, eInspectionMethod inspectionMethod);
    void setDataForTapeModel(const QModelIndex& channelIndex, int gateIndex, int i, int cid, const sChannelDescription& chanDesc, const sScanChannelDescription& scanChanDesc, eControlZone railZone[], int schemeNumber);
    void setColorForChannel(const QModelIndex& channelIndex);

private:
    Defcore* _defcore;
    cChannelsTable* _channelsTable;
    cDeviceConfig* _deviceConfig;
    cDeviceConfig* _deviceConfigHeadScan;
    cDeviceCalibration* _deviceCalibration;

    ChannelsModel* _channelsModel;
    ChannelsModel* _headScanChannelsModel;
    TapeModel* _tapeModel;
    ChannelsProxyFilterModel* _handChannelsModel;
    ChannelsProxyFilterModel* _scanChannelsModel;

    QModelIndex _currentModelIndex;

    QMap<int, QModelIndex> _channelsMap;
    QMap<int, QModelIndex> _headScanChannelsMap;

    QMap<QModelIndex, int> _alarmExpiredMap;
    QTimer _alarmExpiredTimer;

    // edge case with msm channel
    int _msmChannelStartGateLeft;
    int _msmChannelEndGateLeft;
    int _msmChannelStartGateRight;
    int _msmChannelEndGateRight;
    int _msmChannelStartGate;
    int _msmChannelEndGate;

    bool _isCalibrationMode;
    DeviceType _deviceType;
};

#endif  // CHANNELSCONTROLLER_H
