#include <QAbstractListModel>
#include <QTimer>

#include "channelscontroller.h"
#include "channelsproxyfiltermodel.h"
#include "channelsmodel.h"
#include "tapemodel.h"
#include "defcore.h"
#include "debug.h"
#include "core.h"
#include "audiogenerator.h"
#include "DeviceCalibration.h"
#include "DeviceConfig.h"
#include "Device_definitions.h"

#define ALARTM_EXPIRED_TIME_MSec 150
#define ALARTM_EXPIRED_TIMER_INTERVAL_MSec 60

static QStringList letterByControlZone(const eControlZone zone, eControlZone railZone[2])
{
    switch (zone) {
    case czHeadWork:
    case czHeadLeftWork:
    case czHeadAndWork:
        railZone[0] = czHeadWork;
        return QStringList() << QObject::tr("g");
    case czHeadUnWork:
    case czHeadLeftUnWork:
    case czHeadAndUnWork:
        railZone[0] = czHeadUnWork;
        return QStringList() << QObject::tr("f");
    case czWeb:
        railZone[0] = czWeb;
        return QStringList() << QObject::tr("wb");
    case czBase:
        railZone[0] = czBase;
        return QStringList() << QObject::tr("bs");
    case czWebAndBase:
        railZone[0] = czWeb;
        railZone[1] = czBase;
        return QStringList() << QObject::tr("bs") << QObject::tr("wb");
    default:
        railZone[0] = zone;
        railZone[1] = zone;
        return QStringList();
    }
    return QStringList();
}

ChannelsController::ChannelsController(QObject* parent)
    : QObject(parent)
    , _defcore(nullptr)
    , _channelsTable(nullptr)
    , _deviceConfig(nullptr)
    , _deviceConfigHeadScan(nullptr)
    , _deviceCalibration(nullptr)
    , _channelsModel(nullptr)
    , _headScanChannelsModel(nullptr)
    , _tapeModel(nullptr)
    , _handChannelsModel(nullptr)
    , _scanChannelsModel(nullptr)
    , _alarmExpiredTimer(this)
    , _msmChannelStartGateLeft(0)
    , _msmChannelEndGateLeft(0)
    , _msmChannelStartGateRight(0)
    , _msmChannelEndGateRight(0)
    , _msmChannelStartGate(0)
    , _msmChannelEndGate(0)
    , _isCalibrationMode(false)
    , _deviceType(static_cast<DeviceType>(restoreDeviceType()))
{
    _alarmExpiredTimer.setInterval(ALARTM_EXPIRED_TIMER_INTERVAL_MSec);
    ASSERT(connect(&_alarmExpiredTimer, &QTimer::timeout, this, &ChannelsController::alarmExpiredTimeTimeout));
    _alarmExpiredTimer.setSingleShot(false);
}

ChannelsController::~ChannelsController()
{
    qDebug() << "Deleting channels controller...";
    ASSERT(disconnect(&_alarmExpiredTimer, &QTimer::timeout, this, &ChannelsController::alarmExpiredTimeTimeout));
    ASSERT(disconnect(this, &ChannelsController::doSetChannel, _defcore, &Defcore::onSetChannel));
    ASSERT(disconnect(this, &ChannelsController::doSetSide, _defcore, &Defcore::onSetSide));
    ASSERT(disconnect(this, &ChannelsController::doSetTvg, _defcore, &Defcore::onSetTvg));
    ASSERT(disconnect(this, &ChannelsController::doSetPrismDelay, _defcore, &Defcore::onSetPrismDelay));
    ASSERT(disconnect(this, &ChannelsController::doSetSens, _defcore, &Defcore::onSetSens));
    ASSERT(disconnect(this, &ChannelsController::doSetStartGate, _defcore, &Defcore::onSetStartGate));
    ASSERT(disconnect(this, &ChannelsController::doSetEndGate, _defcore, &Defcore::onSetEndGate));
    ASSERT(disconnect(this, &ChannelsController::doSetGateIndex, _defcore, &Defcore::onSetGateIndex));
    ASSERT(disconnect(this, &ChannelsController::doSetMark, _defcore, &Defcore::onSetMark));
    ASSERT(disconnect(this, &ChannelsController::doDeviceUpdate, _defcore, &Defcore::onDeviceUpdate));
    ASSERT(disconnect(this, &ChannelsController::doSetStartNotch, _defcore, &Defcore::onSetStartNotch));
    ASSERT(disconnect(this, &ChannelsController::doSetSizeNotch, _defcore, &Defcore::onSetSizeNotch));
    ASSERT(disconnect(this, &ChannelsController::doSetLevelNotch, _defcore, &Defcore::onSetLevelNotch));

    ASSERT(disconnect(_defcore, &Defcore::doCalibrationChanged, this, &ChannelsController::onCalibrationChanged));
    ASSERT(disconnect(_defcore, &Defcore::dataChanged, this, &ChannelsController::onDefcoreDataChanged));
    ASSERT(disconnect(_defcore, &Defcore::dataChanged, this, &ChannelsController::onDefcoreHeadScanChannelsDataChanged));

    _alarmExpiredTimer.stop();

    qDebug() << "Channels controller deleted!";
}

void ChannelsController::setDefcore(Defcore* defcore)
{
    Q_ASSERT(defcore != nullptr);

    _defcore = defcore;

    ASSERT(connect(this, &ChannelsController::doSetChannel, defcore, &Defcore::onSetChannel));
    ASSERT(connect(this, &ChannelsController::doSetSide, defcore, &Defcore::onSetSide));
    ASSERT(connect(this, &ChannelsController::doSetTvg, defcore, &Defcore::onSetTvg));
    ASSERT(connect(this, &ChannelsController::doSetPrismDelay, defcore, &Defcore::onSetPrismDelay));
    ASSERT(connect(this, &ChannelsController::doSetSens, defcore, &Defcore::onSetSens));
    ASSERT(connect(this, &ChannelsController::doSetStartGate, defcore, &Defcore::onSetStartGate));
    ASSERT(connect(this, &ChannelsController::doSetEndGate, defcore, &Defcore::onSetEndGate));
    ASSERT(connect(this, &ChannelsController::doSetGateIndex, defcore, &Defcore::onSetGateIndex));
    ASSERT(connect(this, &ChannelsController::doSetMark, defcore, &Defcore::onSetMark));
    ASSERT(connect(this, &ChannelsController::doDeviceUpdate, defcore, &Defcore::onDeviceUpdate));
    ASSERT(connect(this, &ChannelsController::doSetStartNotch, defcore, &Defcore::onSetStartNotch));
    ASSERT(connect(this, &ChannelsController::doSetSizeNotch, defcore, &Defcore::onSetSizeNotch));
    ASSERT(connect(this, &ChannelsController::doSetLevelNotch, defcore, &Defcore::onSetLevelNotch));

    ASSERT(connect(defcore, &Defcore::doCalibrationChanged, this, &ChannelsController::onCalibrationChanged));

    qRegisterMetaType<eDeviceSide>("eDeviceSide");
    qRegisterMetaType<CID>("CID");
    qRegisterMetaType<eValueID>("eValueID");
    ASSERT(connect(defcore, &Defcore::dataChanged, this, &ChannelsController::onDefcoreDataChanged));
    ASSERT(connect(defcore, &Defcore::dataChanged, this, &ChannelsController::onDefcoreHeadScanChannelsDataChanged));
}

void ChannelsController::setChannelsTable(cChannelsTable* channelsTable)
{
    Q_ASSERT(channelsTable != nullptr);
    _channelsTable = channelsTable;
}

void ChannelsController::setDeviceConfig(cDeviceConfig* deviceConfig)
{
    Q_ASSERT(deviceConfig != nullptr);
    _deviceConfig = deviceConfig;
}

void ChannelsController::setDeviceConfigHeadScan(cDeviceConfig* deviceConfigHeadScan)
{
    Q_ASSERT(deviceConfigHeadScan != nullptr);
    _deviceConfigHeadScan = deviceConfigHeadScan;
}

void ChannelsController::setDeviceCalibration(cDeviceCalibration* deviceCalibration)
{
    Q_ASSERT(deviceCalibration != nullptr);
    _deviceCalibration = deviceCalibration;
}

QAbstractItemModel* ChannelsController::channelsModel() const
{
    return _channelsModel;
}

QAbstractItemModel* ChannelsController::headScanChannelsModel() const
{
    return _headScanChannelsModel;
}

QAbstractItemModel* ChannelsController::tapeModel() const
{
    return _tapeModel;
}

QAbstractItemModel* ChannelsController::scanChannelsModel() const
{
    return _scanChannelsModel;
}

QAbstractItemModel* ChannelsController::handChannelsModel() const
{
    return _handChannelsModel;
}

void ChannelsController::setDataForAllItems(const QVariant& value, Roles role)
{
    Q_ASSERT(_channelsModel);
    for (int i = 0; i < _channelsModel->rowCount(); ++i) {
        const QModelIndex& channelIndex = _channelsModel->index(i, 0);
        if (_channelsModel->data(channelIndex, AngleRole).toInt() == 42 && _channelsModel->data(channelIndex, GateIndexRole).toInt() == 1) {
            continue;
        }
        _channelsModel->setData(channelIndex, value, role);
    }
}

void ChannelsController::setDataForSlaveChannel()
{
    for (int i = 0; i < _channelsModel->rowCount(); ++i) {
        const QModelIndex& channelIndex = _channelsModel->index(i, 0);
        if (_channelsModel->data(channelIndex, IsCopySlaveChannelRole).toBool()) {
            _channelsModel->setData(channelIndex, false, IsAlarmedRole);
            continue;
        }
    }
}

void ChannelsController::setSens(int value)
{
    emit doSetSens(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setTimeDelay(qreal value)
{
    emit doSetPrismDelay(qRound(value * 10.));
    emit doDeviceUpdate(false);
}

void ChannelsController::setStartGate(int value)
{
    emit doSetStartGate(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setEndGate(int value)
{
    emit doSetEndGate(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setTvg(int value)
{
    emit doSetTvg(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setStartNotch(int value)
{
    emit doSetStartNotch(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setSizeNotch(int value)
{
    emit doSetSizeNotch(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setLevelNotch(int value)
{
    emit doSetLevelNotch(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setMark(int value)
{
    emit doSetMark(value);
    emit doDeviceUpdate(false);
}

void ChannelsController::setChannel(const QModelIndex& index)
{
    Q_ASSERT(index.isValid());

    QVariant value = _channelsModel->data(index, ChannelTypeRole);
    Q_ASSERT(value.isValid());
    ChannelType channelType = static_cast<ChannelType>(value.toInt());

    value = _channelsModel->data(index, ChannelIdRole);
    Q_ASSERT(value.isValid());
    int channelId = value.toInt();
    Q_ASSERT(channelId >= 0);

    QString sideName;
    int gateIndex = -1;
    if (channelType == ScanChannel) {
        value = _channelsModel->data(index, SideRole);
        Q_ASSERT(value.isValid());
        DeviceSide deviceSide = static_cast<DeviceSide>(value.toInt());
        switch (deviceSide) {
        case NoneDeviceSide:
            sideName = "none";
            break;
        case LeftDeviceSide:
            sideName = "left";
            break;
        case RightDeviceSide:
            sideName = "right";
            break;
        }

        value = _channelsModel->data(index, GateIndexRole);
        Q_ASSERT(value.isValid());
        gateIndex = value.toInt();
        Q_ASSERT(gateIndex >= 0);
        Q_ASSERT(gateIndex <= 1);

        emit doSetSide(deviceSide);
        emit doSetGateIndex(gateIndex);
    }

    emit doSetChannel(channelId);
    emit doDeviceUpdate(false);

    _currentModelIndex = index;
    emit doChannelSetted(index);

    if (channelType == ScanChannel) {
        qDebug() << QString("Info| Setted scan channel id:0x") << QString::number(channelId, 16) << QString(" angle: ") << QString::number(_channelsModel->data(index, AngleRole).toInt()) << QString("° side: ") << sideName << QString(" gate index: ") << QString::number(gateIndex);
    }
    else if (channelType == HandChannel) {
        qDebug() << QString("Info| Setted hand channel id:0x") << QString::number(channelId, 16) << QString(" angle: ") << QString::number(_channelsModel->data(index, AngleRole).toInt()) << QString("°");
    }
}

void ChannelsController::setHeadChannel(const QModelIndex& index)
{
    Q_ASSERT(index.isValid());
    QVariant value = _headScanChannelsModel->data(index, ChannelTypeRole);
    Q_ASSERT(value.isValid());
    ChannelType channelType = static_cast<ChannelType>(value.toInt());
    value = _headScanChannelsModel->data(index, ChannelIdRole);
    Q_ASSERT(value.isValid());
    CID channelId = value.toInt();
    Q_ASSERT(channelId >= 0);

    QString sideName;
    int gateIndex = -1;
    if (channelType == ScanChannel) {
        value = _headScanChannelsModel->data(index, SideRole);
        Q_ASSERT(value.isValid());
        DeviceSide deviceSide = static_cast<DeviceSide>(value.toInt());
        switch (deviceSide) {
        case NoneDeviceSide:
            sideName = "none";
            break;
        case LeftDeviceSide:
            sideName = "left";
            break;
        case RightDeviceSide:
            sideName = "right";
            break;
        }

        value = _headScanChannelsModel->data(index, GateIndexRole);
        Q_ASSERT(value.isValid());
        gateIndex = value.toInt();
        Q_ASSERT(gateIndex >= 0);
        Q_ASSERT(gateIndex <= 1);

        emit doSetSide(deviceSide);
        emit doSetGateIndex(gateIndex);
    }

    emit doSetChannel(channelId);
    emit doDeviceUpdate(false);
    emit doHeadChannelSetted(index);

    qDebug() << QString("Info| Setted head scan channel id:0x") << QString::number(channelId, 16) << QString(" angle: ") << QString::number(_headScanChannelsModel->data(index, AngleRole).toInt()) << QString("° side: ") << sideName << QString(" gate index: ") << QString::number(gateIndex);
}

void ChannelsController::selectTape(int tapeIndex, int position, int side)
{
    emit doTapeSelected(tapeIndex, position, side);
}

const QModelIndex& ChannelsController::currentModelIndex() const
{
    return _currentModelIndex;
}

const QModelIndex ChannelsController::modelIndexByCidSideAndGateIndex(int cid, int side, int gateIndex) const
{
    return _channelsMap.value(getKeyByCidSideAndGateIndex(cid, side, gateIndex), QModelIndex());
}

const QModelIndex ChannelsController::headScanModelIndexByCidSideAndGateIndex(int cid, int side, int gateIndex) const
{
    return _headScanChannelsMap.value(getKeyByCidSideAndGateIndex(cid, side, gateIndex), QModelIndex());
}

void ChannelsController::recreateModels()
{
    Q_ASSERT(_channelsTable != nullptr);
    Q_ASSERT(_deviceConfig != nullptr);
    Q_ASSERT(_deviceCalibration != nullptr);
#if defined TARGET_AVICON31
    recreateHeadScanModel();
#endif

    if (_channelsModel == nullptr) {
        _channelsModel = new ChannelsModel(this);
    }
    else {
        _channelsModel->reset();
    }

    int handChannelsCount = _deviceConfig->GetHandChannelsCount();
    _channelsModel->insertRows(0, handChannelsCount);
    for (int i = 0; i < handChannelsCount; ++i) {
        sHandChannelDescription handChanDesc;
        if (!_deviceConfig->getHChannelbyIdx(i, &handChanDesc)) {
            return;
        }

        CID cid = handChanDesc.Id;

        sChannelDescription chanDesc;
        if (!_channelsTable->ItemByCID(cid, &chanDesc)) {
            continue;
        }

        Q_ASSERT(chanDesc.cdBScanDelayMultiply != 0);
        const QModelIndex& index = _channelsModel->index(i, 0);
        _channelsModel->setData(index, HandChannel, ChannelTypeRole);
        _channelsModel->setData(index, chanDesc.cdEnterAngle, AngleRole);
        _channelsModel->setData(index, chanDesc.id, ChannelIdRole);
        _channelsModel->setData(index, _deviceCalibration->GetSens(dsNone, cid, 0), SensRole);
        _channelsModel->setData(index, _deviceCalibration->GetTVG(dsNone, cid), TvgRole);
        _channelsModel->setData(index, _deviceCalibration->GetStGate(dsNone, cid, 0), StartGateRole);
        _channelsModel->setData(index, _deviceCalibration->GetEdGate(dsNone, cid, 0), EndGateRole);
        _channelsModel->setData(index, _deviceCalibration->GetPrismDelay(dsNone, cid), TimeDelayRole);
        _channelsModel->setData(index, _deviceCalibration->GetMark(dsNone, cid), MarkRole);
        _channelsModel->setData(index, chanDesc.cdBScanGate.gStart, StartBscangateRole);
        _channelsModel->setData(index, chanDesc.cdBScanGate.gEnd, EndBscangateRole);
        _channelsModel->setData(index, chanDesc.RecommendedSens[0], RecommendedSensRole);
        _channelsModel->setData(index, chanDesc.cdMethod[0], MethodRole);
        _channelsModel->setData(index, chanDesc.DefaultGain, DefaultGainRole);
        _channelsModel->setData(index, _deviceCalibration->GetGain(dsNone, cid, 0), GainRole);
        _channelsModel->setData(index, chanDesc.cdBScanDelayMultiply, DelayMultiplyRole);
        _channelsModel->setData(index, chanDesc.cdStrokeDuration, StrokeDurationRole);
        _channelsModel->setData(index, chanDesc.cdAScanScale, AScanScaleRole);
        _channelsModel->setData(index, chanDesc.LockStGate[0], IsStartGateLocked);
        _channelsModel->setData(index, chanDesc.LockEdGate[0], IsEndGateLocked);
        _channelsModel->setData(index, 0, SensLevelsRole);
        _channelsModel->setData(index, chanDesc.WorkFrequency, FrequencyChannelRole);

        int tone = 0;
        switch (chanDesc.cdEnterAngle) {
        case 0:
            tone = AudioGenerator::left500Hz | AudioGenerator::right500Hz;
            break;
        default:
            tone = AudioGenerator::left2000Hz | AudioGenerator::right2000Hz;
            break;
        }
        ASSERT(_channelsModel->setData(index, tone, AlarmToneRole));
    }

    if (_tapeModel == nullptr) {
        _tapeModel = new TapeModel(this);
        _tapeModel->setChannelsModel(_channelsModel);
    }
    else {
        _tapeModel->reset();
    }

    int schemeNumber = _deviceConfig->GetCurrentChannelGroup();

    int scanChannelsCount = _deviceConfig->GetAllScanChannelsCount();
    if (_deviceConfig->getControlledRail() == crBoth) {
        scanChannelsCount *= 2;
    }

    for (int i = 0; i < scanChannelsCount; ++i) {
        sScanChannelDescription scanChanDesc;
        if (!_deviceConfig->getSChannelbyIdx(i, &scanChanDesc)) {
            continue;
        }

        if (schemeNumber != scanChanDesc.StrokeGroupIdx) {
            continue;
        }

        CID cid = scanChanDesc.Id;

        sChannelDescription chanDesc;
        if (!_channelsTable->ItemByCID(cid, &chanDesc)) {
            continue;
        }

        int columnIndex = -1;
        if (chanDesc.cdDir == cdZoomIn || chanDesc.cdDir == cdNone) {
            columnIndex = 0;
        }
        else if (chanDesc.cdDir == cdZoomOut) {
            columnIndex = 1;
        }

        int rowIndex = -1;
        if ((chanDesc.cdEnterAngle == 58) && (chanDesc.cdMethod[0] == imMirror)) {
            if (chanDesc.cdZone == czHeadWork || chanDesc.cdZone == czHeadLeftWork || chanDesc.cdZone == czHeadAndWork || chanDesc.cdZone == czHeadBoth) {
                rowIndex = 0;
            }
            else if (chanDesc.cdZone == czHeadUnWork || chanDesc.cdZone == czHeadLeftUnWork || chanDesc.cdZone == czHeadAndUnWork) {
                rowIndex = 1;
            }
        }

        if (scanChanDesc.BScanTape >= _tapeModel->rowCount()) {
            _tapeModel->insertRows(_tapeModel->rowCount(), scanChanDesc.BScanTape - _tapeModel->rowCount() + 1);
        }


        eControlZone railZone[2] = {czAll, czAll};
        const QStringList& letters = letterByControlZone(chanDesc.cdZone, railZone);
        const QModelIndex& tapeIndex = _tapeModel->index(scanChanDesc.BScanTape, 0);
        _tapeModel->setData(tapeIndex, chanDesc.cdEnterAngle, AngleRole);

        QString angleNote = "";

        if ((chanDesc.cdEnterAngle == 58) && (chanDesc.cdMethod[0] == imMirror)) {
            angleNote = tr("mir");
        }
        _tapeModel->setData(tapeIndex, angleNote, AngleNoteRole);

        switch (chanDesc.cdGateCount) {
        case 1: {
            QModelIndex channelIndex = _tapeModel->index(0, columnIndex, tapeIndex);
            if (chanDesc.cdMethod[0] == imMirror) {
                channelIndex = _tapeModel->index(rowIndex, columnIndex, tapeIndex);
            }

            const QVariant& value = _tapeModel->data(channelIndex, SensRole);
            if (channelIndex.isValid() && value.isValid()) {
                const QModelIndex& channelIndexBottom = _tapeModel->index(1, columnIndex, tapeIndex);
                if (!channelIndexBottom.isValid()) {
                    _tapeModel->insertRows(1, 1, tapeIndex);
                }
                channelIndex = _tapeModel->index(1, columnIndex, tapeIndex);
            }
            else {
                if (!channelIndex.isValid()) {
                    if (chanDesc.cdMethod[0] == imMirror) {
                        _tapeModel->insertRows(rowIndex, 1, tapeIndex);
                        channelIndex = _tapeModel->index(rowIndex, columnIndex, tapeIndex);
                        if (!channelIndex.isValid()) {
                            _tapeModel->insertRows(rowIndex, 1, tapeIndex);
                            channelIndex = _tapeModel->index(rowIndex, columnIndex, tapeIndex);
                        }
                    }
                    else {
                        _tapeModel->insertRows(0, 1, tapeIndex);
                        channelIndex = _tapeModel->index(0, columnIndex, tapeIndex);
                    }
                }
            }

            if (channelIndex.isValid()) {
                if (!letters.isEmpty()) {
                    _tapeModel->setData(channelIndex, letters.at(0), NoteRole);
                }
                setDataForTapeModel(channelIndex, 0, 0, cid, chanDesc, scanChanDesc, railZone, schemeNumber);
                if (chanDesc.cdDir == cdNone) {
                    _tapeModel->setData(_tapeModel->index(channelIndex.row(), 1, tapeIndex), true, FakedChannelRole);
                }
                else if (chanDesc.cdZone == czHeadBoth && chanDesc.cdMethod[0] == imMirror) {
                    _tapeModel->setData(_tapeModel->index(channelIndex.row(), 0, tapeIndex), true, FakedChannelRole);
                }
            }
        } break;
        case 2: {
            QModelIndex channelIndex = _tapeModel->index(1, 1, tapeIndex);
            if (!channelIndex.isValid()) {
                _tapeModel->insertRows(0, 2, tapeIndex);
            }
            if (chanDesc.cdEnterAngle != 0) {
                QModelIndex channelIndex = _tapeModel->index(1, columnIndex, tapeIndex);
                setDataForTapeModel(channelIndex, 0, 0, cid, chanDesc, scanChanDesc, railZone, schemeNumber);

                channelIndex = _tapeModel->index(0, columnIndex, tapeIndex);
                setDataForTapeModel(channelIndex, 1, 1, cid, chanDesc, scanChanDesc, railZone, schemeNumber);
            }
            else {
                QModelIndex channelIndex = _tapeModel->index(0, 0, tapeIndex);
                setDataForTapeModel(channelIndex, 0, 0, cid, chanDesc, scanChanDesc, railZone, schemeNumber);

                if (scanChanDesc.DeviceSide == dsLeft) {
                    _msmChannelStartGateLeft = _deviceCalibration->GetStGate(scanChanDesc.DeviceSide, cid, 1);
                    _msmChannelEndGateLeft = _deviceCalibration->GetEdGate(scanChanDesc.DeviceSide, cid, 1);
                }
                else if (scanChanDesc.DeviceSide == dsRight) {
                    _msmChannelStartGateRight = _deviceCalibration->GetStGate(scanChanDesc.DeviceSide, cid, 1);
                    _msmChannelEndGateRight = _deviceCalibration->GetEdGate(scanChanDesc.DeviceSide, cid, 1);
                }
                else if (scanChanDesc.DeviceSide == dsNone) {
                    _msmChannelStartGate = _deviceCalibration->GetStGate(scanChanDesc.DeviceSide, cid, 1);
                    _msmChannelEndGate = _deviceCalibration->GetEdGate(scanChanDesc.DeviceSide, cid, 1);
                }

                _tapeModel->setData(_tapeModel->index(1, 0, tapeIndex), true, FakedChannelRole);
                _tapeModel->setData(_tapeModel->index(0, 1, tapeIndex), true, FakedChannelRole);

                channelIndex = _tapeModel->index(1, 1, tapeIndex);
                setDataForTapeModel(channelIndex, 1, 1, cid, chanDesc, scanChanDesc, railZone, schemeNumber);
            }
            if (letters.count() == 2) {
                QModelIndex channelIndex = _tapeModel->index(0, columnIndex, tapeIndex);
                _tapeModel->setData(channelIndex, letters.at(0), NoteRole);
                channelIndex = _tapeModel->index(1, columnIndex, tapeIndex);
                _tapeModel->setData(channelIndex, letters.at(1), NoteRole);
            }
        } break;
        default:
            break;
        }
    }  // for (int i = 0; i < scanChannelsCount; ++i)

    for (int i = 0; i < _tapeModel->rowCount(); ++i) {
        const sBScanTape& bScanTape = _deviceConfig->getTapeByIndex(i);
        const QModelIndex& index = _tapeModel->index(i, 0);
        _tapeModel->setData(index, bScanTape.tapeConformity, TapeConformityRole);
        _tapeModel->setData(index, bScanTape.isVisibleInBoltJointMode, IsTapeVisibleInBoltJointModeRole);
        _tapeModel->setData(index, Core::instance().getCurrentForwardColor(), BScanForwardColorRole);
        _tapeModel->setData(index, Core::instance().getCurrentBackwardColor(), BScanBackwardColorRole);
    }

    if (_handChannelsModel == nullptr) {
        _handChannelsModel = new ChannelsProxyFilterModel(HandChannel, this);
    }
    Q_ASSERT(_handChannelsModel != nullptr);
    _handChannelsModel->setSourceModel(_channelsModel);

    if (_scanChannelsModel == nullptr) {
        _scanChannelsModel = new ChannelsProxyFilterModel(ScanChannel, this);
    }
    Q_ASSERT(_scanChannelsModel != nullptr);
    _scanChannelsModel->setSourceModel(_channelsModel);

    updateChannelsMap();
}

void ChannelsController::recreateHeadScanModel()
{
    Q_ASSERT(_deviceConfigHeadScan != nullptr);

    if (_headScanChannelsModel == nullptr) {
        _headScanChannelsModel = new ChannelsModel(this);
    }
    else {
        _headScanChannelsModel->reset();
    }

    int headScanChannelsCount = _deviceConfigHeadScan->GetAllScanChannelsCount();
    _headScanChannelsModel->insertRows(0, headScanChannelsCount);

    for (int i = 0; i < headScanChannelsCount; ++i) {
        sScanChannelDescription scanChanDesc;
        if (!_deviceConfigHeadScan->getSChannelbyIdx(i, &scanChanDesc)) {
            return;
        }

        CID cid = scanChanDesc.Id;

        sChannelDescription chanDesc;
        if (!_channelsTable->ItemByCID(cid, &chanDesc)) {
            continue;
        }

        const QModelIndex& index = _headScanChannelsModel->index(i, 0);
        _headScanChannelsModel->setData(index, ScanChannel, ChannelTypeRole);
        _headScanChannelsModel->setData(index, chanDesc.cdEnterAngle, AngleRole);
        _headScanChannelsModel->setData(index, chanDesc.id, ChannelIdRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetSens(dsNone, cid, 0), SensRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetTVG(dsNone, cid), TvgRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetStGate(dsNone, cid, 0), StartGateRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetEdGate(dsNone, cid, 0), EndGateRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetPrismDelay(dsNone, cid), TimeDelayRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetMark(dsNone, cid), MarkRole);
        _headScanChannelsModel->setData(index, chanDesc.cdBScanGate.gStart, StartBscangateRole);
        _headScanChannelsModel->setData(index, chanDesc.cdBScanGate.gEnd, EndBscangateRole);
        _headScanChannelsModel->setData(index, chanDesc.RecommendedSens[0], RecommendedSensRole);
        _headScanChannelsModel->setData(index, chanDesc.cdMethod[0], MethodRole);
        _headScanChannelsModel->setData(index, chanDesc.DefaultGain, DefaultGainRole);
        _headScanChannelsModel->setData(index, _deviceCalibration->GetGain(dsNone, cid, 0), GainRole);
        _headScanChannelsModel->setData(index, chanDesc.cdBScanDelayMultiply, DelayMultiplyRole);
        _headScanChannelsModel->setData(index, chanDesc.cdStrokeDuration, StrokeDurationRole);
        _headScanChannelsModel->setData(index, chanDesc.cdAScanScale, AScanScaleRole);
        _headScanChannelsModel->setData(index, chanDesc.LockStGate[0], IsStartGateLocked);
        _headScanChannelsModel->setData(index, chanDesc.LockEdGate[0], IsEndGateLocked);
        _headScanChannelsModel->setData(index, 0, SensLevelsRole);
        _headScanChannelsModel->setData(index, chanDesc.WorkFrequency, FrequencyChannelRole);
    }
    updateHeadScanChannelsMap();
}

void ChannelsController::getGatesForMsmChannel(int& startGate, int& endGate)
{
    startGate = _msmChannelStartGate;
    endGate = _msmChannelEndGate;
}

void ChannelsController::getGatesForMsmChannel(int& startGateLeft, int& endGateLeft, int& startGateRight, int& endGateRight)
{
    startGateLeft = _msmChannelStartGateLeft;
    endGateLeft = _msmChannelEndGateLeft;
    startGateRight = _msmChannelStartGateRight;
    endGateRight = _msmChannelEndGateRight;
}

void ChannelsController::setStartGatesForMsmChannel(eDeviceSide& side, int& value)
{
    switch (side) {
    case dsLeft:
        _msmChannelStartGateLeft = value;
        break;
    case dsRight:
        _msmChannelStartGateRight = value;
        break;
    case dsNone:
        _msmChannelStartGate = value;
        break;
    }
}

void ChannelsController::setEndGatesForMsmChannel(eDeviceSide& side, int& value)
{
    switch (side) {
    case dsLeft:
        _msmChannelEndGateLeft = value;
        break;
    case dsRight:
        _msmChannelEndGateRight = value;
        break;
    case dsNone:
        _msmChannelEndGate = value;
        break;
    }
}

void ChannelsController::setPlayAlarm(const QModelIndex& index, const eGateAlarmMode& value)
{
    if (value == amOff) {
        _channelsModel->setData(index, false, IsPlayAlarmToneRole);
    }
    else {
        _channelsModel->setData(index, true, IsPlayAlarmToneRole);
    }
}

void ChannelsController::setCalibrationMode()
{
    _isCalibrationMode = true;
    _alarmExpiredTimer.stop();
    _alarmExpiredMap.clear();
    disconnect(_defcore, &Defcore::doAlarmData, this, &ChannelsController::onAlarmData);
}

void ChannelsController::setSearchMode()
{
    _isCalibrationMode = false;
    disconnect(_defcore, &Defcore::doAlarmData, this, &ChannelsController::onAlarmData);
    ASSERT(connect(_defcore, &Defcore::doAlarmData, this, &ChannelsController::onAlarmData, Qt::UniqueConnection));
}

eControlledRail ChannelsController::contoledRail() const
{
    Q_ASSERT(_deviceConfig != nullptr);
    return _deviceConfig->getControlledRail();
}

void ChannelsController::onCalibrationChanged()
{
    if (_channelsModel == nullptr) {
        return;
    }

    for (int i = 0; i < _channelsModel->rowCount(); ++i) {
        const QModelIndex& channelIndex = _channelsModel->index(i, 0);
        Q_ASSERT(channelIndex.isValid() == true);
        QVariant value = _channelsModel->data(channelIndex, ChannelIdRole);
        Q_ASSERT(value.isValid() == true);
        int cid = value.toInt();
        value = _channelsModel->data(channelIndex, SideRole);
        Q_ASSERT(value.isValid() == true);
        eDeviceSide side = static_cast<eDeviceSide>(value.toInt());
        value = _channelsModel->data(channelIndex, GateIndexRole);
        Q_ASSERT(value.isValid() == true);
        int gateIndex = value.toInt();
        _channelsModel->setData(channelIndex, _deviceCalibration->GetSens(side, cid, gateIndex), SensRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetGain(side, cid, gateIndex), GainRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetTVG(side, cid), TvgRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetStGate(side, cid, gateIndex), StartGateRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetEdGate(side, cid, gateIndex), EndGateRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetPrismDelay(side, cid), TimeDelayRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetMark(side, cid), MarkRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetCalibrationTemperature(side, cid), CalibrationTemperatureRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetNotchStart(side, cid, gateIndex), StartNotchRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetNotchDur(side, cid, gateIndex), SizeNotchRole);
        _channelsModel->setData(channelIndex, _deviceCalibration->GetNotchLevel(side, cid, gateIndex), LevelNotchRole);
    }
    emit changeCalibration();
}

void ChannelsController::onAlarmData(QSharedPointer<tDEV_AlarmHead> head)
{
    if (_isCalibrationMode) {
        return;
    }

    for (const tDEV_AlarmHeadItem& item : head.data()->Items) {
        for (unsigned char i = 1; i <= 2; ++i) {
            if (item.State[i]) {
                const QModelIndex& index = modelIndexByCidSideAndGateIndex(item.Channel, item.Side, i - 1);
                if (index.isValid()) {
                    auto* model = const_cast<QAbstractItemModel*>(index.model());
                    Q_ASSERT(model != nullptr);
                    model->setData(index, true, IsAlarmedRole);
                    _alarmExpiredMap[index] = ALARTM_EXPIRED_TIME_MSec;
                    if (!_alarmExpiredTimer.isActive()) {
                        _alarmExpiredTimer.start();
                    }
                }
            }
        }
    }
}

void ChannelsController::alarmExpiredTimeTimeout()
{
    QMap<QModelIndex, int>::iterator iter = _alarmExpiredMap.begin();
    while (iter != _alarmExpiredMap.end()) {
        *iter = iter.value() - ALARTM_EXPIRED_TIMER_INTERVAL_MSec;
        if (iter.value() <= 0) {
            Q_ASSERT(iter.key().isValid() == true);
            const QModelIndex& index = iter.key();
            auto* model = const_cast<QAbstractItemModel*>(index.model());
            Q_ASSERT(model != nullptr);
            model->setData(index, false, IsAlarmedRole);
            iter = _alarmExpiredMap.erase(iter);
        }
        else {
            ++iter;
        }
    }
    if (_alarmExpiredMap.count() == 0) {
        _alarmExpiredTimer.stop();
    }
}

void ChannelsController::onDefcoreDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value)
{
    Q_UNUSED(groupIndex);

    if (gateIndex > 0) {
        --gateIndex;
    }

    const QModelIndex& modelIndex = modelIndexByCidSideAndGateIndex(channel, side, gateIndex);
    if (!modelIndex.isValid()) {
        return;
    }

    switch (id) {
    case vidTVG:
        _channelsModel->setData(modelIndex, value, TvgRole);
        break;
    case vidPrismDelay:
        _channelsModel->setData(modelIndex, value, TimeDelayRole);
        break;
    case vidSens:
        _channelsModel->setData(modelIndex, value, SensRole);
        break;
    case vidGain:
        _channelsModel->setData(modelIndex, value, GainRole);
        break;
    case vidStGate:
        _channelsModel->setData(modelIndex, value, StartGateRole);
        if (channel == 1 && gateIndex == 1) {
            setStartGatesForMsmChannel(side, value);
        }
        break;
    case vidEdGate:
        _channelsModel->setData(modelIndex, value, EndGateRole);
        if (channel == 1 && gateIndex == 1) {
            setEndGatesForMsmChannel(side, value);
        }
        break;
    case vidMark:
        _channelsModel->setData(modelIndex, value, MarkRole);
        break;
    case vidAlarm:
        setPlayAlarm(modelIndex, static_cast<eGateAlarmMode>(value));
        break;
    case vidTestSens:
        if (_deviceType == Avicon31Default || _deviceType == Avicon15) {
            _channelsModel->setData(modelIndex, value, SensLevelsRole);
        }
        break;
#if defined TARGET_AVICONDB
    case vidStNotch:
        _channelsModel->setData(modelIndex, value, StartNotchRole);
        break;
    case vidNotchLen:
        _channelsModel->setData(modelIndex, value, SizeNotchRole);
        break;
    case vidNotchLevel:
        _channelsModel->setData(modelIndex, value, LevelNotchRole);
        break;
#endif
    default:
        break;
    }
}

void ChannelsController::onDefcoreHeadScanChannelsDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value)
{
    Q_UNUSED(groupIndex);

    if (gateIndex > 0) {
        --gateIndex;
    }

    const QModelIndex& modelIndex = headScanModelIndexByCidSideAndGateIndex(channel, side, gateIndex);
    if (!modelIndex.isValid()) {
        return;
    }

    switch (id) {
    case vidTVG:
        _headScanChannelsModel->setData(modelIndex, value, TvgRole);
        break;
    case vidPrismDelay:
        _headScanChannelsModel->setData(modelIndex, value, TimeDelayRole);
        break;
    case vidSens:
        _headScanChannelsModel->setData(modelIndex, value, SensRole);
        break;
    case vidGain:
        _headScanChannelsModel->setData(modelIndex, value, GainRole);
        break;
    case vidStGate:
        _headScanChannelsModel->setData(modelIndex, value, StartGateRole);
        break;
    case vidEdGate:
        _headScanChannelsModel->setData(modelIndex, value, EndGateRole);
        break;
    case vidMark:
        _headScanChannelsModel->setData(modelIndex, value, MarkRole);
        break;
    default:
        break;
    }
}

void ChannelsController::onColorSchemeChanged()
{
    qDebug() << "Switching color scheme";
    Core& core = Core::instance();
    for (int i = 0; i < _tapeModel->rowCount(); ++i) {
        const QModelIndex& index = _tapeModel->index(i, 0);
        _tapeModel->setData(index, core.getCurrentForwardColor(), BScanForwardColorRole);
        _tapeModel->setData(index, core.getCurrentBackwardColor(), BScanBackwardColorRole);
    }

    for (int i = 0; i < _channelsModel->rowCount(); ++i) {
        const QModelIndex& index = _channelsModel->index(i);
        setColorForChannel(index);
    }
}

void ChannelsController::onViewModeCalibrationChanged()
{

    if (_scanChannelsModel == nullptr) {
        return;
    }
    Core& core = Core::instance();

    for (int i = 0; i < _scanChannelsModel->rowCount(); ++i) {
        const QModelIndex& channelIndex = _scanChannelsModel->index(i, 0);
        Q_ASSERT(channelIndex.isValid() == true);
        QVariant value = _scanChannelsModel->data(channelIndex, ChannelIdRole);
        Q_ASSERT(value.isValid() == true);
        int cid = value.toInt();
        value = _scanChannelsModel->data(channelIndex, GateIndexRole);
        Q_ASSERT(value.isValid() == true);
        int gateIndex = value.toInt();
        value = _scanChannelsModel->data(channelIndex, SideRole);
        Q_ASSERT(value.isValid() == true);
        _scanChannelsModel->setData(channelIndex, core.getKuSens(gateIndex, cid), SensRole);
        _scanChannelsModel->setData(channelIndex, core.getstStrSens(gateIndex, cid), StartGateRole);
        _scanChannelsModel->setData(channelIndex, core.getendStrSens(gateIndex, cid), EndGateRole);
        }
    emit changeCalibration();
}

void ChannelsController::updateChannelsMap()
{
    _channelsMap.clear();

    for (int i = 0; i < _channelsModel->rowCount(); ++i) {
        const QModelIndex& index = _channelsModel->index(i);

        int cid = _channelsModel->data(index, ChannelIdRole).toInt();
        int side = _channelsModel->data(index, SideRole).toInt();
        int gateIndex = _channelsModel->data(index, GateIndexRole).toInt();
        int key = getKeyByCidSideAndGateIndex(cid, side, gateIndex);
        Q_ASSERT(_channelsMap.contains(key) == false);
        _channelsMap[key] = index;
    }
}

void ChannelsController::updateHeadScanChannelsMap()
{
    _headScanChannelsMap.clear();

    for (int i = 0; i < _headScanChannelsModel->rowCount(); ++i) {
        const QModelIndex& index = _headScanChannelsModel->index(i);

        int cid = _headScanChannelsModel->data(index, ChannelIdRole).toInt();
        int side = _headScanChannelsModel->data(index, SideRole).toInt();
        int gateIndex = _headScanChannelsModel->data(index, GateIndexRole).toInt();
        int key = getKeyByCidSideAndGateIndex(cid, side, gateIndex);
        Q_ASSERT(_headScanChannelsMap.contains(key) == false);
        _headScanChannelsMap[key] = index;
    }
}

int ChannelsController::getTone(int angle, eDeviceSide side, eInspectionMethod inspectionMethod)
{
    int tone = 0;

    if (side == dsLeft) {
        switch (angle) {
        case 0:
            if (static_cast<eInspectionMethod>(inspectionMethod) == imMirrorShadow) {
                tone = AudioGenerator::left500Hz;
            }
            else if (static_cast<eInspectionMethod>(inspectionMethod) == imEcho) {
                tone = AudioGenerator::left500HzBlink;
            }
            break;
        case 42:
        case 45:
            tone = AudioGenerator::left1000Hz;
            break;
        case 58:
            tone = AudioGenerator::left2000Hz;
            break;
        case 65:
        case 70:
            tone = AudioGenerator::left2000HzBlink;
            break;
        default:
            break;
        }
    }
    else if (side == dsRight) {
        switch (angle) {
        case 0:
            if (static_cast<eInspectionMethod>(inspectionMethod) == imMirrorShadow) {
                tone = AudioGenerator::right500Hz;
            }
            else if (static_cast<eInspectionMethod>(inspectionMethod) == imEcho) {
                tone = AudioGenerator::right500HzBlink;
            }
            break;
        case 42:
        case 45:
            tone = AudioGenerator::right1000Hz;
            break;
        case 58:
            tone = AudioGenerator::right2000Hz;
            break;
        case 65:
        case 70:
            tone = AudioGenerator::right2000HzBlink;
            break;
        default:
            break;
        }
    }
    else if (side == dsNone) {
        switch (angle) {
        case 0:
            if (static_cast<eInspectionMethod>(inspectionMethod) == imMirrorShadow) {
                tone = AudioGenerator::left500Hz | AudioGenerator::right500Hz;
            }
            else if (static_cast<eInspectionMethod>(inspectionMethod) == imEcho) {
                tone = AudioGenerator::left500HzBlink | AudioGenerator::right500HzBlink;
            }
            break;
        case 42:
        case 45:
            tone = AudioGenerator::left1000Hz | AudioGenerator::right1000Hz;
            break;
        case 55:
        case 58:
        case 65:
            tone = AudioGenerator::left2000Hz | AudioGenerator::right2000Hz;
            break;
        case 70:
            tone = AudioGenerator::left2000HzBlink | AudioGenerator::right2000HzBlink;
            break;
        default:
            break;
        }
    }

    return tone;
}

void ChannelsController::setDataForTapeModel(const QModelIndex& channelIndex, int gateIndex, int i, int cid, const sChannelDescription& chanDesc, const sScanChannelDescription& scanChanDesc, eControlZone railZone[2], int schemeNumber)
{
    Q_ASSERT(chanDesc.cdBScanDelayMultiply != 0);
    _tapeModel->setData(channelIndex, chanDesc.id, ChannelIdRole);
    _tapeModel->setData(channelIndex, ScanChannel, ChannelTypeRole);
    _tapeModel->setData(channelIndex, chanDesc.Name.c_str(), ChannelNameRole);
    _tapeModel->setData(channelIndex, chanDesc.Title.c_str(), ChannelTitleRole);
    _tapeModel->setData(channelIndex, scanChanDesc.DeviceSide, SideRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetSens(scanChanDesc.DeviceSide, cid, gateIndex), SensRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetGain(scanChanDesc.DeviceSide, cid, gateIndex), GainRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetTVG(scanChanDesc.DeviceSide, cid), TvgRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetStGate(scanChanDesc.DeviceSide, cid, gateIndex), StartGateRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetEdGate(scanChanDesc.DeviceSide, cid, gateIndex), EndGateRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetPrismDelay(scanChanDesc.DeviceSide, cid), TimeDelayRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetMark(scanChanDesc.DeviceSide, cid), MarkRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetCalibrationTemperature(scanChanDesc.DeviceSide, cid), CalibrationTemperatureRole);
    _tapeModel->setData(channelIndex, chanDesc.cdEnterAngle, AngleRole);
    _tapeModel->setData(channelIndex, chanDesc.cdMethod[i], MethodRole);
    _tapeModel->setData(channelIndex, railZone[i], ZoneRole);
    _tapeModel->setData(channelIndex, chanDesc.RecommendedSens[gateIndex], RecommendedSensRole);
    _tapeModel->setData(channelIndex, gateIndex, GateIndexRole);
    _tapeModel->setData(channelIndex, directionByEChannelDir(chanDesc.cdDir), DirectionRole);
    _tapeModel->setData(channelIndex, chanDesc.cdGateCount, GateCountRole);
    _tapeModel->setData(channelIndex, chanDesc.cdBScanGate.gStart, StartBscangateRole);
    _tapeModel->setData(channelIndex, chanDesc.cdBScanGate.gEnd, EndBscangateRole);
    _tapeModel->setData(channelIndex, chanDesc.RecommendedAlarmGate[gateIndex].gStart, DefaultStartGateRole);
    _tapeModel->setData(channelIndex, chanDesc.RecommendedAlarmGate[gateIndex].gEnd, DefaultEndGateRole);
    _tapeModel->setData(channelIndex, chanDesc.DefaultGain, DefaultGainRole);
    _tapeModel->setData(channelIndex, getTone(chanDesc.cdEnterAngle, scanChanDesc.DeviceSide, chanDesc.cdMethod[i]), AlarmToneRole);
    _tapeModel->setData(channelIndex, chanDesc.cdBScanDelayMultiply, DelayMultiplyRole);
    _tapeModel->setData(channelIndex, chanDesc.cdStrokeDuration, StrokeDurationRole);
    _tapeModel->setData(channelIndex, chanDesc.cdAScanScale, AScanScaleRole);
    _tapeModel->setData(channelIndex, chanDesc.LockStGate[gateIndex], IsStartGateLocked);
    _tapeModel->setData(channelIndex, chanDesc.LockEdGate[gateIndex], IsEndGateLocked);
    _tapeModel->setData(channelIndex, 0, SensLevelsRole);
    _tapeModel->setData(channelIndex, chanDesc.cdUseNotch, UseNotchRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetNotchStart(scanChanDesc.DeviceSide, cid, gateIndex), StartNotchRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetNotchDur(scanChanDesc.DeviceSide, cid, gateIndex), SizeNotchRole);
    _tapeModel->setData(channelIndex, _deviceCalibration->GetNotchLevel(scanChanDesc.DeviceSide, cid, gateIndex), LevelNotchRole);
    _tapeModel->setData(channelIndex, _deviceConfig->IsCopySlaveChannel(scanChanDesc.DeviceSide, cid), IsCopySlaveChannelRole);
    _tapeModel->setData(channelIndex, scanChanDesc.ColorDescr, BScanColorIndexRole);
    setColorForChannel(channelIndex);


    const QModelIndex& tapeIndex = _tapeModel->index(scanChanDesc.BScanTape, 0);
    _tapeModel->setData(tapeIndex, scanChanDesc.DeviceSide, SideRole);
}

void ChannelsController::setColorForChannel(const QModelIndex& channelIndex)
{
    eBScanColorDescr colorIndex = static_cast<eBScanColorDescr>(_tapeModel->data(channelIndex, BScanColorIndexRole).toUInt());
    CID cid = static_cast<CID>(_tapeModel->data(channelIndex, ChannelIdRole).toUInt());
    _tapeModel->setData(channelIndex, Core::instance().getIndividualColorForChannel(cid), BScanIndividualColorRole);
    if (colorIndex != cdTMDirect1) {
        _tapeModel->setData(channelIndex, Core::instance().getColorForChannel(colorIndex), BScanNormalColorRole);
        _tapeModel->setData(channelIndex, 0x0000, BScanSpecialColorRole);
    }
    else {
        _tapeModel->setData(channelIndex, Core::instance().getColorForChannel(cdTMDirect1), BScanNormalColorRole);
        _tapeModel->setData(channelIndex, Core::instance().getColorForChannel(cdTMDirect2), BScanSpecialColorRole);
    }
}
