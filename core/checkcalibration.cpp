#include <QCoreApplication>
#include <QDateTime>
#include <QDir>

#include "checkcalibration.h"
#include "core.h"
#include "defcore.h"
#include "debug.h"
#include "channelscontroller.h"
#include "wrongcalibrationmsg.h"
#include "roles.h"
#include "settings.h"
#include "modeltypes.h"
#include "Device_definitions.h"

#define SIGNAL_BREAK_MIN_INTERVAL 12  // in otschetah (no in millimeters), realy not use, init in settings.cpp

#define SIGNAL_42_MIN_DELAY 45
#define SIGNAL_42_MAX_DELAY 115

#define SIGNAL_ECHO_MIN_DELAY 20
#define SIGNAL_ECHO_MAX_DELAY 35

#define SIGNAL_58_MIN_DELAY 40  // 35
#define SIGNAL_58_MAX_DELAY 180

#define SIGNAL_70_MIN_DELAY 30
#define SIGNAL_70_MAX_DELAY 90

#define MIN_SIGNALS_IN_PACKET_COUNT_42 15    // 16
#define MIN_SIGNALS_IN_PACKET_COUNT_ECHO 15  // 12
//#define MIN_SIGNALS_IN_PACKET_COUNT_58     4
#define MIN_SIGNALS_IN_PACKET_COUNT_58_6dB 8
#define MIN_SIGNALS_IN_PACKET_COUNT_70 6

#define MIN_KD_42 (-10)
#define MIN_KD_ECHO (-8)  //-6  init in settings.cpp

#define MIN_GOOD_PAKETS_COUNT_42 3
#define MIN_GOOD_PAKETS_COUNT_ECHO 3
#define MIN_GOOD_PAKETS_COUNT_58 1
#define MIN_GOOD_PAKETS_COUNT_70 1
#define MIN_GOOD_ZTM_SIGNAL_BREAKS 3

#define MIN_AMPLITUE_58 0
#define MIN_AMPLITUDE (-6)

// ----------------------------------------------------------------------------------------------------------------
CheckCalibration::CheckCalibration(QObject* parent)
    : QObject(parent)
    , _defcore(nullptr)
    , _channelsModel(nullptr)
    , _minSignalBreaksInterval(SIGNAL_BREAK_MIN_INTERVAL)
    , _minKdEchoPrm(MIN_KD_ECHO)
    , _isEnabledCheckCalibration(restoreStateCheckCalibrationAlgorithm())

{
    _boltJointActive = false;
    for (unsigned short i = 0; i < RAILS; ++i) {
        _signalGoodBreaksCnt[i] = 0;
        for (unsigned short j = 0; j < CHNLS; ++j) {
            _sensitivity[i][j] = 0;
            _goodPacketCount[i][j] = 0;
            _kd[i][j] = 0;
            _checkCalibrationResult[i][j] = false;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------
CheckCalibration::~CheckCalibration()
{
    qDebug() << "Deleting CheckCalibration...";
    Q_ASSERT(_defcore != nullptr);
    Core& core = Core::instance();

    disconnect(&core, &Core::doStartBoltJoint, this, &CheckCalibration::onStartBoltJoint);
    disconnect(&core, &Core::doStopBoltJoint, this, &CheckCalibration::onStopBoltJoint);
    disconnect(&core, &Core::doModelsReady, this, &CheckCalibration::updateChannelsMap);
    disconnect(_defcore, &Defcore::doDeviceSignalsPacket, this, &CheckCalibration::onDeviceSignalsPacket);

    qDebug() << "CheckCalibration deleted!";
}

// --------------------------------------------------------------------------------------------------------------
void CheckCalibration::setDefcore(Core* core, Defcore* defcore)
{
    Q_ASSERT(defcore != nullptr);
    _defcore = defcore;

    ASSERT(connect(core, &Core::doStartBoltJoint, this, &CheckCalibration::onStartBoltJoint, Qt::UniqueConnection));
    ASSERT(connect(core, &Core::doStopBoltJoint, this, &CheckCalibration::onStopBoltJoint, Qt::UniqueConnection));
    ASSERT(connect(core, &Core::doModelsReady, this, &CheckCalibration::updateChannelsMap));
    ASSERT(connect(defcore, &Defcore::doDeviceSignalsPacket, this, &CheckCalibration::onDeviceSignalsPacket));

    _minSignalBreaksInterval = static_cast<unsigned int>(restoreEvalCalibrMinZTMSpacingSize() / encoderCorrection());
    _minKdEchoPrm = restoreMinKdEchoPrm();
}

// ---------------------------------------------------------------------------------------------------------------
bool CheckCalibration::getCheckCalibrationResult(unsigned char rail, CID channel)
{
    return _checkCalibrationResult[rail][channel];
}

// ---------------------------------------------------------------------------------------------------------------
tBadChannelList CheckCalibration::getBadChannelList(eDeviceSide side)
{
    if (side == dsLeft) {
        return _leftSideBadChannelList;
    }

    return _rightSideBadChannelList;
}

// ---------------------------------------------------------------------------------------------------------------
void CheckCalibration::cidToNameString(CID channel, QString* name)
{
    Q_ASSERT(name != nullptr);
    if (_channelsMap.contains(channel)) {
        *name = _channelsMap[channel];
    }
}

// --------------------------------------------------------------------------------------------------------------
void CheckCalibration::onDeviceSignalsPacket(QSharedPointer<tDEV_Packet> signalPacket)
{
    tDEV_Packet* packet = signalPacket.data();
    Q_ASSERT(packet != nullptr);

    if (_boltJointActive) {
        bool OutOfRange = false;
        bool ProcessEnable = true;
        switch (packet->Channel) {
        case F42E:  // 6
            if (packet->EdDelay < SIGNAL_42_MIN_DELAY || packet->StDelay > SIGNAL_42_MAX_DELAY) {
                OutOfRange = true;
            }
            break;
        case B42E:  // 7
            if (packet->EdDelay < SIGNAL_42_MIN_DELAY || packet->StDelay > SIGNAL_42_MAX_DELAY) {
                OutOfRange = true;
            }
            break;
        case N0EMS:
            packet->StDelay = static_cast<unsigned char>(packet->StDelay / 3);
            packet->EdDelay = static_cast<unsigned char>(packet->EdDelay / 3);
            if (packet->EdDelay < SIGNAL_ECHO_MIN_DELAY || packet->StDelay > SIGNAL_ECHO_MAX_DELAY) {
                OutOfRange = true;
            }
            break;
        case F58ELW:  // 2
        case F58ELU:  // 2
        case B58ELW:  // 3
        case B58ELU:  // 3
        case F58MLW:
        case F58MLU:
        case B58MLW:
        case B58MLU:
            if (packet->EdDelay < SIGNAL_58_MIN_DELAY || packet->StDelay > SIGNAL_58_MAX_DELAY) {
                OutOfRange = true;
            }
            break;
        case F70E:
        case B70E:
            if (packet->EdDelay < SIGNAL_70_MIN_DELAY || packet->StDelay > SIGNAL_70_MAX_DELAY) {
                OutOfRange = true;
            }
            break;
        default:
            ProcessEnable = false;
        }

        if (!OutOfRange && ProcessEnable) {
            checkPacketQuality(packet);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------
void CheckCalibration::onStartBoltJoint()
{
    if (_isEnabledCheckCalibration) {
        _boltJointActive = true;
        readParams();
        packetProcessInit();
        signalBreaksProcessInit();
    }
}

// --------------------------------------------------------------------------------------------------------------
void CheckCalibration::onStopBoltJoint()
{
    _boltJointActive = false;
    if (_isEnabledCheckCalibration) {
        checkResult();
    }
}

void CheckCalibration::updateChannelsMap()
{
    QAbstractItemModel* scanChannelsModel = Core::instance().channelsController()->scanChannelsModel();
    for (int i = 0; i < scanChannelsModel->rowCount(); ++i) {
        const QModelIndex& index = scanChannelsModel->index(i, 0);
        Q_ASSERT(index.isValid());
        CID channelId = scanChannelsModel->data(index, ChannelIdRole).toInt();
        const QString& channelName = scanChannelsModel->data(index, ChannelTitleRole).toString();
        _channelsMap.insert(channelId, channelName);
    }
}

// ---------------------------------------------------------------------------------------------------------------
void CheckCalibration::readParams()
{
    ChannelsController* channelsController = Core::instance().channelsController();
    Q_ASSERT(channelsController != nullptr);
    QAbstractItemModel* scanChannelsModel = channelsController->scanChannelsModel();
    for (int i = 0; i < scanChannelsModel->rowCount(); ++i) {
        const QModelIndex& index = scanChannelsModel->index(i, 0);
        QVariant value;
        value = scanChannelsModel->data(index, ChannelIdRole);
        Q_ASSERT(value.isValid());
        CID channelId = value.toInt();
        value = scanChannelsModel->data(index, SideRole);
        Q_ASSERT(value.isValid());
        eDeviceSide side = static_cast<eDeviceSide>(value.toInt());
        value = scanChannelsModel->data(index, GateIndexRole);
        Q_ASSERT(value.isValid());
        int gateIndex = value.toInt();
        value = scanChannelsModel->data(index, SensRole);
        Q_ASSERT(value.isValid());
        int sens = value.toInt();
        value = scanChannelsModel->data(index, StartGateRole);
        Q_ASSERT(value.isValid());

        if (gateIndex == 0 && side != dsNone) {
            switch (channelId) {
            case N0EMS:
            case F42E:
            case B42E:
            case F58ELW:
            case F58ELU:
            case B58ELW:
            case B58ELU:
            case F58MLW:
            case F58MLU:
            case B58MLW:
            case B58MLU:
            case F70E:
            case B70E:
                _sensitivity[side - 1][channelId] = sens;
                break;
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------
void CheckCalibration::checkResult()
{
    unsigned char rail, channel;
    bool wrongLeftSide = false;
    bool wrongRightSide = false;
    bool wrongLeftSide58 = false;
    bool wrongRightSide58 = false;
    tBadChannelListItem badItem;

    bool noSignalLeftrail = true;
    bool noSignalRightrail = true;
    bool no58SignalLeftrail = true;
    bool no58SignalRightrail = true;
    bool no70SignalLeftrail = true;
    bool no70SignalRightrail = true;

    Core& core = Core::instance();
    Registration& registration = core.registration();

    _leftSideBadChannelList.clear();
    _rightSideBadChannelList.clear();

    badItem.GateIndex = 0;

    for (rail = 0; rail < RAILS; ++rail) {
        channel = F42E;  // 6
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_42) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? noSignalLeftrail = false : noSignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide = true : wrongRightSide = true;
        }

        channel = B42E;  // 7
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_42) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? noSignalLeftrail = false : noSignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide = true : wrongRightSide = true;
        }

        channel = N0EMS;  // 1
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_ECHO) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? noSignalLeftrail = false : noSignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide = true : wrongRightSide = true;
        }

        channel = F58ELW;  // 2
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
        }

        channel = F58ELU;  // 2
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
        }

        channel = B58ELW;  // 3
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
        }

        channel = B58ELU;  // 3
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
            badItem.ChId = channel;
            rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
            rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
        }

        if (restoreSchemeNumber() == 1) {
            // Sheme 2 --> 58 mirror channels added
            // Analize them with the same parameters

            channel = F58MLW;
            if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
                _checkCalibrationResult[rail][channel] = true;
                rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
            }
            else {
                _checkCalibrationResult[rail][channel] = false;
                badItem.ChId = channel;
                rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
                rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
            }

            channel = F58MLU;
            if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
                _checkCalibrationResult[rail][channel] = true;
                rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
            }
            else {
                _checkCalibrationResult[rail][channel] = false;
                badItem.ChId = channel;
                rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
                rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
            }

            channel = B58MLW;
            if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
                _checkCalibrationResult[rail][channel] = true;
                rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
            }
            else {
                _checkCalibrationResult[rail][channel] = false;
                badItem.ChId = channel;
                rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
                rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
            }

            channel = B58MLU;
            if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_58) {
                _checkCalibrationResult[rail][channel] = true;
                rail == 0 ? no58SignalLeftrail = false : no58SignalRightrail = false;
            }
            else {
                _checkCalibrationResult[rail][channel] = false;
                badItem.ChId = channel;
                rail == 0 ? _leftSideBadChannelList.push_back(badItem) : _rightSideBadChannelList.push_back(badItem);
                rail == 0 ? wrongLeftSide58 = true : wrongRightSide58 = true;
            }
        }

        channel = F70E;  //
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_70) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no70SignalLeftrail = false : no70SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
        }

        channel = B70E;  //
        if (_goodPacketCount[rail][channel] >= MIN_GOOD_PAKETS_COUNT_70) {
            _checkCalibrationResult[rail][channel] = true;
            rail == 0 ? no70SignalLeftrail = false : no70SignalRightrail = false;
        }
        else {
            _checkCalibrationResult[rail][channel] = false;
        }
    }

    tBadChannelList nullBadChannelList;

    if (noSignalLeftrail && no58SignalLeftrail) {
        registration.addQualityCalibrationRec(dsLeft, nullBadChannelList, qrtNoJoint);
    }
    else {
        if (no70SignalLeftrail) {
            emit doLeftSideNo70Signal();
        }
        else {
            if (wrongLeftSide || wrongLeftSide58) {
                emit doLeftSideWrongCalibration();
            }
            else {
                registration.addQualityCalibrationRec(dsLeft, nullBadChannelList, qrtValidByBoltedJoint);
            }
        }
    }

    if (noSignalRightrail && no58SignalRightrail) {
        registration.addQualityCalibrationRec(dsRight, nullBadChannelList, qrtNoJoint);
    }
    else {
        if (no70SignalRightrail) {
            emit doRightSideNo70Signal();
        }
        else {
            if (wrongRightSide || wrongRightSide58) {
                emit doRightSideWrongCalibration();
            }
            else {
                registration.addQualityCalibrationRec(dsRight, nullBadChannelList, qrtValidByBoltedJoint);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------
void CheckCalibration::checkPacketQuality(tDEV_Packet* packet)
{
    unsigned char rail = 0;
    unsigned char channel = static_cast<unsigned char>(packet->Channel);

    if (packet->Side != dsNone) {
        rail = packet->Side - 1;
    }

    _kd[rail][channel] = static_cast<int>(packet->MaxAmpl) - _sensitivity[rail][channel];

    switch (channel) {
    case F42E:
    case B42E:
        if (packet->Count_6dB >= MIN_SIGNALS_IN_PACKET_COUNT_42 && _kd[rail][channel] >= MIN_KD_42) {
            ++_goodPacketCount[rail][channel];
        }
        break;
    case N0EMS:
        if (packet->Count_6dB >= MIN_SIGNALS_IN_PACKET_COUNT_ECHO && _kd[rail][channel] >= _minKdEchoPrm) {
            ++_goodPacketCount[rail][channel];
        }
        break;
    case F58ELW:
    case F58ELU:
    case B58ELW:
    case B58ELU:
    case F58MLW:
    case F58MLU:
    case B58MLW:
    case B58MLU:
        if (packet->Count_6dB >= MIN_SIGNALS_IN_PACKET_COUNT_58_6dB) {
            ++_goodPacketCount[rail][channel];
        }
    case F70E:
    case B70E:
        if (packet->Count_6dB >= MIN_SIGNALS_IN_PACKET_COUNT_70) {
            ++_goodPacketCount[rail][channel];
        }

        break;
    }
}

// -------------------------------------------------------------------------------------------------------------------------
void CheckCalibration::signalBreaksProcessInit()
{
    for (unsigned char rail = 0; rail < RAILS; ++rail) {
        _signalGoodBreaksCnt[rail] = 0;
    }
}

// -------------------------------------------------------------------------------------------------------------------------
void CheckCalibration::packetProcessInit()
{
    for (unsigned char rail = 0; rail < RAILS; ++rail) {
        for (unsigned char channel = 0; channel < CHNLS; ++channel) {
            _goodPacketCount[rail][channel] = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------------
void CheckCalibration::setEvalCalibrMinZTMSpacingSize(int value)
{
    _minSignalBreaksInterval = static_cast<unsigned int>(value / encoderCorrection());
}

// ---------------------------------------------------------------------------------------------------------------------------
void CheckCalibration::setMinKdEchoPrm(int value)
{
    _minKdEchoPrm = value;
}
