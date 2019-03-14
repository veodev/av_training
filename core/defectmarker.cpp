
#include "core.h"
#include "defcore.h"
#include "defectmarker.h"
#include "debug.h"
#include "settings.h"
#include "modeltypes.h"
#include "registration.h"
#include "ChannelsIds.h"

#define MIN_DEFECT_LENGTH 10  // in millimeters
#define MIN_SIGNALS_0dB_COUNT 5
#define MIN_SPACING_LEVEL 0

DefectMarker::DefectMarker(QObject* parent)
    : QObject(parent)
    , _defcore(nullptr)
    , _registration(nullptr)
    , _boltJointActive(false)
    , _minPacketSize(static_cast<int>(restoreImportantAreaMinPacketSize() / encoderCorrection()))
    , _minSignals0dB(restoreImportantAreaMinSignals0dB())
    , _swichShunterActive(false)
    , _minZTMSpacing(10)
{
}

void DefectMarker::setDefcore(Core* core, Defcore* defcore)
{
    Q_ASSERT(defcore != nullptr);

    _defcore = defcore;

    _registration = &(core->registration());

    if (restoreImportantAreaIndicateEnable()) {
        ASSERT(connect(core, &Core::doStartBoltJoint, this, &DefectMarker::onStartBoltJoint, Qt::UniqueConnection));
        ASSERT(connect(core, &Core::doStopBoltJoint, this, &DefectMarker::onStopBoltJoint, Qt::UniqueConnection));
        ASSERT(connect(defcore, &Defcore::doDeviceSignalsPacket, this, &DefectMarker::onDeviceSignalsPacket));
        ASSERT(connect(defcore, &Defcore::doDeviceSignalSpacing, this, &DefectMarker::onDeviceSignalSpacing));
    }

    ASSERT(connect(defcore, &Defcore::doDefect53_1, this, &DefectMarker::onDefect53_1));
}


DefectMarker::~DefectMarker()
{
    qDebug() << "Deleting DefectMarker...";
}


void DefectMarker::onStartBoltJoint()
{
    _boltJointActive = true;
}


void DefectMarker::onStopBoltJoint()
{
    _boltJointActive = false;
}

void DefectMarker::onDeviceSignalsPacket(QSharedPointer<tDEV_Packet> signalPacket)
{
    if (_boltJointActive || _swichShunterActive) {
        return;
    }

    tDEV_Packet* packet = signalPacket.data();

    int packetWidth = static_cast<int>(packet->Length);

    if (packetWidth >= _minPacketSize && static_cast<int>(packet->Count_0dB) >= _minSignals0dB) {
        int packetCentr = static_cast<int>(packet->EdDisplayCrd - static_cast<long>((packetWidth / 2)));

        if (packet->Channel == N0EMS) {
            unsigned char centrDelay = static_cast<unsigned char>((static_cast<int>(packet->StDelay) + static_cast<int>(packet->EdDelay)) / 2);
            _registration->addDefectMarker(packet->Side, packet->Channel, 0, packetCentr, packetWidth, centrDelay - 8, centrDelay + 8);
        }
        else
            _registration->addDefectMarker(packet->Side, packet->Channel, 0, packetCentr, packetWidth, packet->StDelay, packet->EdDelay);
    }
}


void DefectMarker::onDeviceSignalSpacing(QSharedPointer<tDEV_SignalSpacing> signalSpacing)
{
    Q_ASSERT(_registration);
    if (_boltJointActive || _swichShunterActive) {
        return;
    }
    tDEV_SignalSpacing* spacing = signalSpacing.data();

    if (spacing->SpacingLevel < MIN_SPACING_LEVEL) {
        return;
    }

    unsigned char centrDelay = static_cast<unsigned char>((static_cast<int>(spacing->StDelay) + static_cast<int>(spacing->EdDelay)) / 2);

    if ((spacing->Length) >= _minZTMSpacing) {
        int spacingCentr = spacing->EdDisplayCrd - static_cast<int>(spacing->Length / 2);
        _registration->addDefectMarker(spacing->Side, spacing->Channel, 1, spacingCentr, spacing->Length, centrDelay - 12, centrDelay + 12);
    }
}

void DefectMarker::enableImportantAreaIndication(bool isEnabled)
{
    Core& core = Core::instance();

    disconnect(&core, &Core::doStartBoltJoint, this, &DefectMarker::onStartBoltJoint);
    disconnect(&core, &Core::doStopBoltJoint, this, &DefectMarker::onStopBoltJoint);
    disconnect(_defcore, &Defcore::doDeviceSignalsPacket, this, &DefectMarker::onDeviceSignalsPacket);
    disconnect(_defcore, &Defcore::doDeviceSignalSpacing, this, &DefectMarker::onDeviceSignalSpacing);

    if (isEnabled) {
        ASSERT(connect(&core, &Core::doStartBoltJoint, this, &DefectMarker::onStartBoltJoint, Qt::UniqueConnection));
        ASSERT(connect(&core, &Core::doStopBoltJoint, this, &DefectMarker::onStopBoltJoint, Qt::UniqueConnection));
        ASSERT(connect(_defcore, &Defcore::doDeviceSignalsPacket, this, &DefectMarker::onDeviceSignalsPacket));
        ASSERT(connect(_defcore, &Defcore::doDeviceSignalSpacing, this, &DefectMarker::onDeviceSignalSpacing));
    }
}

void DefectMarker::setMinPacketSize(int value)
{
    if (value >= 0) {
        _minPacketSize = static_cast<int>(value / encoderCorrection());
    };
}

void DefectMarker::setMinSignals0dB(int value)
{
    if (value >= 0) {
        _minSignals0dB = value;
    }
}

void DefectMarker::setSwitchShunterState(bool state)
{
    _swichShunterActive = state;
}

void DefectMarker::setMinZTMSpacing(int value)
{
    if (value >= 0) {
        _minZTMSpacing = static_cast<int>(value / encoderCorrection());
    }
}

void DefectMarker::onDefect53_1(QSharedPointer<tDEV_Defect53_1> defect53_1)
{
    tDEV_Defect53_1* defect = defect53_1.data();

    int defectWidth = static_cast<int>(defect->EdDisplayCrd - defect->StDisplayCrd);
    int defectCentr = static_cast<int>(defect->EdDisplayCrd - static_cast<long>((defectWidth / 2)));

    _registration->addDefectMarker(defect->Side, defect->Channel, 0, defectCentr, defectWidth, defect->StDelay, defect->EdDelay);
}
