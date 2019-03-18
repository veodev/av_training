#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QCoreApplication>
#include <qmath.h>
#include <QElapsedTimer>

#include "debug.h"
#include "defcore.h"
#include "settings.h"
#include "appdatapath.h"

#include "CriticalSection_Lin.h"
#include "ThreadClassList_Lin.h"
#include "EventManager_Qt.h"
#include "Device.h"
#include "platforms.h"
#include "androidJNI.h"

#include "Device_definitions.h"

#ifdef TARGET_AVICON31
#include "DeviceConfig_Avk31.h"
#include "DeviceConfig_HeadScan.h"
#elif TARGET_AVICONDB
#include "DeviceConfig_DB_lan.h"
#elif TARGET_AVICONDBHS
#include "DeviceConfig_DB_can.h"
#elif TARGET_AVICON15
#include "DeviceConfig_Av15_usbcan.h"
#elif TARGET_AVICON31E
#include "DeviceConfig_Avk31E.h"
#else
#message "unknown config"
#endif

Defcore* Defcore::_defcore = nullptr;

Defcore::Defcore(QObject* parent)
    : QThread(parent)
    , _stopThread(false)
    , _interrupInit(false)
    , _isDeviceStarted(false)
    , _defcoreFinit(false)
    , _defcoreTimerFinished(false)
    , _threadList(nullptr)
    , _channelsTable(nullptr)
    , _deviceConfig(nullptr)
    , _deviceConfigHeadScan(nullptr)
    , _deviceCalibration(nullptr)
    , _deviceEventManager(nullptr)
    , _umuEventManager(nullptr)
    , _criticalSection(nullptr)
    , _criticalSectionDevice(nullptr)
    , _calibrationCriticalSection(nullptr)
    , _device(nullptr)
    , _auxilaryTimer(new QTimer(this))
    , _checkUmuConnectionTimer(new QTimer(this))
    , _schemeNumber(0)
    , _lastMode(dmPaused)
{
    qRegisterMetaType<QSharedPointer<tDEV_AScanMeasure>>("QSharedPointer<tDEV_AScanMeasure>");
    qRegisterMetaType<QSharedPointer<tDEV_AScanHead>>("QSharedPointer<tDEV_AScanHead>");
    qRegisterMetaType<QSharedPointer<tUMU_AScanData>>("QSharedPointer<tUMU_AScanData>");
    qRegisterMetaType<QSharedPointer<tDEV_AlarmHead>>("QSharedPointer<tDEV_AlarmHead>");
    qRegisterMetaType<QSharedPointer<tUMU_AlarmItem>>("QSharedPointer<tUMU_AlarmItem>");
    qRegisterMetaType<QSharedPointer<tDEV_BScan2Head>>("QSharedPointer<tDEV_BScan2Head>");
    qRegisterMetaType<QSharedPointer<tUMU_BScanData>>("QSharedPointer<tUMU_BScanData>");
    qRegisterMetaType<QSharedPointer<tDEV_PathStepData>>("QSharedPointer<tDEV_PathStepData>");
    qRegisterMetaType<tCalibrationToRailTypeResult*>("tCalibrationToRailTypeResult*");
    qRegisterMetaType<QSharedPointer<tDEV_Packet>>("QSharedPointer<tDEV_Packet>");
    qRegisterMetaType<QSharedPointer<tDEV_SignalSpacing>>("QSharedPointer<tDEV_SignalSpacing>");
    qRegisterMetaType<QSharedPointer<tDEV_Defect53_1>>("QSharedPointer<tDEV_Defect53_1>");
    qRegisterMetaType<QSharedPointer<int>>("QSharedPointer<int>");
    qRegisterMetaType<QSharedPointer<unsigned int>>("QSharedPointer<unsigned int>");


    _channelsTable = new cChannelsTable;
    Q_ASSERT(_channelsTable != nullptr);

#ifdef TARGET_AVICON31
    _deviceConfig = new cDeviceConfig_Avk31(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfig != nullptr);
    _deviceConfigHeadScan = new cDeviceConfig_HeadScan(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfigHeadScan != nullptr);
#elif TARGET_AVICONDB
    _deviceConfig = new cDeviceConfig_DB_lan(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfig != nullptr);
#elif TARGET_AVICONDBHS
    _deviceConfig = new cDeviceConfig_DB_can(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfig != nullptr);
#elif TARGET_AVICON15
    _deviceConfig = new cDeviceConfig_Av15_usbcan(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfig != nullptr);
#elif TARGET_AVICON31E
    _deviceConfig = new cDeviceConfig_Avk31E(_channelsTable, Defcore::aScanThreshold(), Defcore::bScanThreshold());
    Q_ASSERT(_deviceConfig != nullptr);
#else
#warning "unknown target"
#endif

    _calibrationCriticalSection = new cCriticalSection_Lin();
    QString calibrationFilename = appDataPath() + "calibration.dat";
    _deviceCalibration = new cDeviceCalibration(calibrationFilename.toLocal8Bit().data(), _channelsTable, _calibrationCriticalSection, false);
    Q_ASSERT(_deviceCalibration != nullptr);

    Q_ASSERT(_defcore == nullptr);
    _defcore = this;
}

Defcore::~Defcore()
{
    qDebug() << "Defcore destroyed!";
}

void Defcore::restoreACSettings()
{
    int acGain = restoreACGain();
    int acLevel = restoreACLevel();
    int acFreq = restoreACFreq();

    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    _device->SetACControlATT(acGain);
    _device->SetAcousticContactThreshold_(acLevel, acFreq);
    _criticalSection->Release();

    qDebug() << QString("Acoustic contact| gain: ") + QString::number(acGain);
    qDebug() << QString("Acoustic contact| level: ") + QString::number(acLevel);
    qDebug() << QString("Acoustic contact| freq: ") + QString::number(acFreq);
}

void Defcore::init()
{
    Q_ASSERT(thread() == QThread::currentThread());

    _threadList = new cThreadClassList_Lin;
    Q_ASSERT(_threadList != nullptr);

    _deviceEventManager = new cEventManager_Qt();
    Q_ASSERT(_deviceEventManager != nullptr);

    _umuEventManager = new cEventManager_Qt();
    Q_ASSERT(_umuEventManager != nullptr);

    _criticalSection = new cCriticalSection_Lin();
    Q_ASSERT(_criticalSection != nullptr);

    _criticalSectionDevice = new cCriticalSection_Lin();
    Q_ASSERT(_criticalSectionDevice != nullptr);

    _device = new cDevice(_threadList, _deviceConfig, _deviceCalibration, _channelsTable, _deviceEventManager, _umuEventManager, _criticalSectionDevice, _criticalSection);

    Q_ASSERT(_device != nullptr);

    _criticalSection->Enter();
    _device->SetCallBack(Defcore::callBack);
    _device->SetMode(dmSearch);
    _device->SetSide(dsLeft);
    _device->EnableFiltration(restoreFiltrationEnable());
    _device->EnableFiltration(false);
    _device->setFilterParams(fprmMinPacketSize, static_cast<unsigned int>(restoreFilterMinPacketSize() / encoderCorrection()));
    _device->setFilterParams(fprmMaxCoordSpacing, static_cast<unsigned int>(restoreFilterMaxCoordSpacing()));
    _device->setFilterParams(fprmMaxDelayDelta, static_cast<unsigned int>(restoreFilterMaxDelayDelta()));
    _device->setFilterParams(fprmMaxSameDelayConsecutive, static_cast<unsigned int>(restoreFilterMaxSameDelayConsecutive()));
    _device->setFilterParams(fprmMinPacketSizeForEvent, qCeil(static_cast<float>(restoreImportantAreaMinPacketSize()) / encoderCorrection()));
    _device->setFilterParams(frmmMinSignals0dBInNormModeForEvent, static_cast<unsigned int>(restoreImportantAreaMinSignals0dB()));
    _device->setFilterParams(frmmMinSpacing0dBInNormModeForEvent, qCeil(static_cast<float>(restoreImportantAreaMinZTMSpacing()) / encoderCorrection()));
    _device->setFilterParams(fprmMinSpacingMinus6dBInBoltTestModeForEvent, qCeil(static_cast<float>(restoreEvalCalibrMinZTMSpacingSize()) / encoderCorrection()));
    _criticalSection->Release();

    int schemeNumber = restoreSchemeNumber();
    if (schemeNumber < 0 || schemeNumber >= _deviceConfig->GetChannelGroupCount()) {
        schemeNumber = 0;
    }
    TRACE_VALUE(schemeNumber);

    onSetSchemeIndex(schemeNumber);

    connectToDevice();
    onEnableAC(restoreStateAcousticContactControl());
    restoreACSettings();
    _criticalSection->Enter();
    _device->SetFixBadSensState(restoreAutoSensResetEnabled());
    _device->SetTimeToFixBadSens(restoreAutoSensResetDelay());
    _criticalSection->Release();
}

void Defcore::connectToDevice()
{
    emit doConnectionStatusConnecting();
    qDebug() << QString("Connection| Connecting ...");
    Q_ASSERT(_device);

    if (_device->connectToDevice() >= 0) {
        emit doConnectionStatusConnected();
        qDebug() << QString("Connection| Connected");
        _device->StartWork();
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
        _device->SetSide(dsLeft);
        _device->SetChannel(F70E);
        _device->Update(true);
#elif defined(TARGET_AVICONDBHS)
        _device->SetSide(dsNone);
        _device->SetChannel(H70);
        _device->Update(true);
#elif defined(TARGET_AVICONDB)
        _device->SetSide(dsLeft);
        _device->SetChannel(F65E_WP);
        _device->Update(true);
#elif defined(TARGET_AVICON15)
        _device->SetSide(dsNone);
        _device->SetChannel(F70E);
        _device->Update(true);
#endif
        _isDeviceStarted = true;

        start();
        emit doInited();

        _auxilaryTimer->setInterval(10 * 1000);
        _auxilaryTimer->setSingleShot(false);
#if !defined TARGET_AVICONDBHS
        ASSERT(connect(_auxilaryTimer, &QTimer::timeout, this, &Defcore::umuTimerTimeout));
#endif
        _auxilaryTimer->start();

        _checkUmuConnectionTimer->setInterval(3000);
        _checkUmuConnectionTimer->setSingleShot(false);
        ASSERT(connect(_checkUmuConnectionTimer, &QTimer::timeout, this, &Defcore::umuConnectionTimerTimeout));
        _checkUmuConnectionTimer->start();
        _defcoreTimerFinished = true;
    }
    else {
        if (!_interrupInit) {
            _auxilaryTimer->singleShot(1000, this, &Defcore::connectToDevice);
        }
        else {
            _defcoreTimerFinished = true;
        }
    }
}

int Defcore::schemeIndex() const
{
    Q_ASSERT(_deviceConfig != nullptr);
    return _deviceConfig->GetCurrentChannelGroup();
}

int Defcore::schemeCount() const
{
    Q_ASSERT(_deviceConfig != nullptr);
    return _deviceConfig->GetChannelGroupCount();
}

double Defcore::maxSpeed() const
{
    Q_ASSERT(_deviceConfig != nullptr);
    return _deviceConfig->getMaxControlSpeed();
}

void Defcore::onTestSensLevels()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);

    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->MakeTestSensCallBackForAllChannel();
    }
    _criticalSection->Release();
}

QString Defcore::getCurrentCalibrationName()
{
    _calibrationMutex.lock();
    QString calibrationName = (QString::fromLocal8Bit(_deviceCalibration->GetName()));
    _calibrationMutex.unlock();
    return calibrationName;
}

QStringList Defcore::calibrationNames()
{
    _calibrationMutex.lock();
    QStringList calibrationNames = _calibrationNames;
    _calibrationMutex.unlock();
    return calibrationNames;
}

void Defcore::createNewCalibration(const QString& name)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    bool rc = createCalibration(_deviceConfig->GetCurrentChannelGroupIndex(), name);
    if (rc) {
        _calibrationMutex.lock();
        _filteredCalibrations.append(_deviceCalibration->Count() - 1);
        _calibrationNames.append(name);
        _calibrationMutex.unlock();
        qDebug() << "Create new calibration: " << name;
    }
    else {
        qDebug() << "Failed to create new calibration: " << name;
    }

    saveLastCalibrationForScheme(_schemeNumber, _filteredCalibrations.count() - 1);
}

void Defcore::onUsePathEncoderEmulator(bool value)
{
    _criticalSection->Enter();
    _device->setUsePathEncoderSimulationinSearchMode(value);
    _criticalSection->Release();
}

void Defcore::onAutoGainAdjustmentEnabled(bool state)
{
    _criticalSection->Enter();
    _device->ManageSensBySpeed(state, encoderCorrection());
    _criticalSection->Release();
}

void Defcore::deleteCalibration(int index)
{
    Q_ASSERT(_deviceCalibration != nullptr);
    Q_ASSERT(_filteredCalibrations.count() > 0);
    Q_ASSERT(index >= 0 && index < _filteredCalibrations.count());

    _calibrationMutex.lock();
    int realIndex = _filteredCalibrations.at(index);
    Q_ASSERT(realIndex >= 0 && realIndex < _deviceCalibration->Count());
    _calibrationMutex.unlock();
    deleteScreenShootsOfCalibration(realIndex);
    _deviceCalibration->Delete(realIndex, false);
    remapCalibrations();
}

void Defcore::setScreenShootKey(eDeviceSide side, CID channelId, int gateIndex, unsigned int key)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    _deviceCalibration->SetSnapShotFileID(side, channelId, gateIndex, key);
}

unsigned int Defcore::getScreenShootKey(eDeviceSide side, CID channelId, int gateIndex)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    sSnapshotFileID snapFileId;
    _deviceCalibration->GetSnapShotFileID(&snapFileId, side, channelId, gateIndex);
    return snapFileId;
}

void Defcore::deleteScreenShootsOfCalibration(int index)
{
    sSnapshotFileIDList* listOfScreenShoots = _deviceCalibration->CreateSnapShotFileIDList(index);
    Q_ASSERT(listOfScreenShoots != nullptr);
    QDir dir(registrationServiceScreenShoots());
    for (const sSnapshotFileID& fileId : *listOfScreenShoots) {
        dir.remove(QString::number(fileId) + QString(".png"));
    }
    _deviceCalibration->DeleteSnapShotFileIDList(listOfScreenShoots);
}

void Defcore::setCalibrationTemperature(eDeviceSide side, CID channelId, int value)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    _deviceCalibration->SetCalibrationTemperature(side, channelId, value);
}

int Defcore::getCalibrationTemperature(eDeviceSide side, CID channelId)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    return _deviceCalibration->GetCalibrationTemperature(side, channelId);
}

int Defcore::calibrationIndex()
{
    Q_ASSERT(_deviceCalibration != nullptr);

    int index = _deviceCalibration->GetCurrent();
    Q_ASSERT(index >= 0);

    _calibrationMutex.lock();
    int count = _filteredCalibrations.count();
    int i;
    for (i = 0; i < count; ++i) {
        if (_filteredCalibrations[i] == index) {
            break;
        }
    }
    _calibrationMutex.unlock();
    if (i == count) {
        i = -1;
    }

    return i;
}

int Defcore::realCalibrationIndex(int index)
{
    return _filteredCalibrations.at(index);
}

void Defcore::setCalibrationName(const QString& name, int index)
{
    Q_ASSERT(_deviceCalibration != nullptr);

    _calibrationMutex.lock();
    QByteArray byteArray = name.toLocal8Bit();
    _deviceCalibration->SetNameByIndex(byteArray.data(), _filteredCalibrations.at(index));
    _calibrationNames[index] = name;
    _calibrationMutex.unlock();
}

bool Defcore::calibrateChannel(int& sens)
{
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    bool rc = _device->ChannelSensCalibration();
    if (rc) {
        sens = _device->GetSens();
    }
    _criticalSection->Release();

    return rc;
}

int Defcore::GetCurrentBScanSessionID()
{
    _criticalSection->Enter();
    int session = _device->GetCurrentBScanSessionID();
    _criticalSection->Release();
    return session;
}

int Defcore::aScanThreshold()
{
    return 32;
}

int Defcore::bScanThreshold()
{
    return 8;
}

eDeviceSide Defcore::currentSide()
{
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    eDeviceSide side = _device->GetSide();
    _criticalSection->Release();
    return side;
}

CID Defcore::currentChannelId()
{
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    CID channel = _device->GetChannel();
    _criticalSection->Release();
    return channel;
}

int Defcore::currentGateIndex()
{
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    int index = (_device->GetGateIndex() - 1);
    _criticalSection->Release();
    return index;
}

QString Defcore::deviceSerialNumber() const
{
    if (_device == nullptr || !_isDeviceStarted) {
        return tr("---");
    }
    _criticalSection->Enter();
    QString str = QString::number(_device->getSerialNumberForUMU(0));
#if defined TARGET_AVICON31E
    str += "  |  " + QString::number(_device->getSerialNumberForUMU(1));
#endif
    _criticalSection->Release();
    return str;
}

QString Defcore::deviceFirmwareVersion() const
{
    QString res(tr("---"));

    if (_device == nullptr || !_isDeviceStarted) {
        return res;
    }
    _criticalSection->Enter();
    res = QString(_device->getFirmwareVersionForUMU(0).c_str());
    res += '.' + QString::number(_device->getBuildNumberForUMU(0));
#if defined TARGET_AVICON31E
    res += "  |  " + QString(_device->getFirmwareVersionForUMU(1).c_str());
    res += '.' + QString::number(_device->getBuildNumberForUMU(1));
#endif
    _criticalSection->Release();
    return res;
}

QString Defcore::devicePldVersion() const
{
    QString res(tr("---"));

    if (_device == nullptr || !_isDeviceStarted) {
        return res;
    }
    _criticalSection->Enter();
    res = QString(_device->getFPGAVersionForUMU(0).c_str());
#if defined TARGET_AVICON31E
    res += "  |  " + QString(_device->getFPGAVersionForUMU(1).c_str());
#endif
    _criticalSection->Release();

    return res;
}

QStringList Defcore::deviceSerialNumbers() const
{
    QStringList res;
    if (_device == nullptr || !_isDeviceStarted) {
        res.append(tr("---"));
    }
    else {
        _criticalSection->Enter();
        uint umuCount = _deviceConfig->GetUMUCount();
        for (uint i = 0; i < umuCount; ++i) {
            res.append(QString::number(_device->getSerialNumberForUMU(i)));
        }
        _criticalSection->Release();
    }
    return res;
}

QStringList Defcore::deviceFirmwareVersions() const
{
    QStringList res;
    if (_device == nullptr || !_isDeviceStarted) {
        res.append(tr("---"));
    }
    else {
        _criticalSection->Enter();
        uint umuCount = _deviceConfig->GetUMUCount();
        for (uint i = 0; i < umuCount; ++i) {
            res.append(QString(_device->getFirmwareVersionForUMU(i).c_str()) + '.' + QString::number(_device->getBuildNumberForUMU(i)));
        }
        _criticalSection->Release();
    }
    return res;
}

QStringList Defcore::devicePldVersions() const
{
    QStringList res;
    if (_device == nullptr || !_isDeviceStarted) {
        res.append(tr("---"));
    }
    else {
        _criticalSection->Enter();
        uint umuCount = _deviceConfig->GetUMUCount();
        for (uint i = 0; i < umuCount; ++i) {
            res.append(QString(_device->getFPGAVersionForUMU(i).c_str()));
        }
        _criticalSection->Release();
    }
    return res;
}

void Defcore::resetPathEncoder()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    _device->ResetPathEncoder();
    _criticalSection->Release();
}

void Defcore::disableAll()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    _device->DisableAll();
    _criticalSection->Release();
}

void Defcore::enableAll()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    _device->EnableAll();
    _criticalSection->Release();
}

void Defcore::callBack(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value)
{
    Q_ASSERT(_defcore != nullptr);
    _defcore->emitDataChanged(groupIndex, side, channel, gateIndex, id, value);
}

void Defcore::emitDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value)
{
    emit dataChanged(groupIndex, side, channel, gateIndex, id, value);
}

cChannelsTable* Defcore::getChannelsTablePtr()
{
    return _channelsTable;
}

eDeviceMode Defcore::getDeviceMode()
{
    _criticalSection->Enter();
    eDeviceMode mode = _device->GetMode();
    _criticalSection->Release();
    return mode;
}

void Defcore::onSetSchemeIndex(int index)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_deviceConfig != nullptr);
    Q_ASSERT(_deviceConfig->GetChannelGroupCount() > 0);
    Q_ASSERT(index >= 0 && index < _deviceConfig->GetChannelGroupCount());
    Q_ASSERT(_deviceConfig->GetChannelGroup(index) >= 0);

    _schemeNumber = index;
    saveSchemeNumber(index);
    _deviceConfig->SetChannelGroup(_deviceConfig->GetChannelGroup(index));

    createCalibrationIfDoesnotExist(index);
    remapCalibrations();
    int restoredIndex = restoreLastCalibrationForScheme(index);
    onSetCalibrationIndex(restoredIndex);
    emit doSchemeChanged(index);
}

void Defcore::onSetCalibrationIndex(int index)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_deviceCalibration != nullptr);
    Q_ASSERT(_filteredCalibrations.count() > 0);

    if ((index >= 0 && index < _filteredCalibrations.count()) == false) {
        index = 0;
    }
    saveLastCalibrationForScheme(_schemeNumber, index);
    _calibrationMutex.lock();
    int realIndex = _filteredCalibrations.at(index);
    Q_ASSERT(realIndex >= 0 && realIndex <= _deviceCalibration->Count());
    _calibrationMutex.unlock();

    if (_deviceCalibration->GetCurrent() == realIndex) {
        return;
    }

    _deviceCalibration->SetCurrent(realIndex);

    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    _device->Update(true);
    _criticalSection->Release();

    emit doCalibrationChanged();
}

void Defcore::onSetChannel(CID id)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetChannel(id);
    }
    _criticalSection->Release();
}

void Defcore::onSetSide(DeviceSide side)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetSide(static_cast<eDeviceSide>(side));
    }
    _criticalSection->Release();
}

void Defcore::onSetTvg(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetTVG(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetPrismDelay(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetPrismDelay(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetSens(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetSens(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetStartGate(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetStGate(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetEndGate(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetEdGate(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetGateIndex(int gateIndex)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetGateIndex(gateIndex + 1);
    }
    _criticalSection->Release();
}

void Defcore::onSetMark(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted && value > 0) {
        _device->SetMarkWidth(10);  // TODO: remove hardcode
        _device->StartUseMark();
        _device->SetMark(value);
    }
    else {
        _device->StopUseMark();
    }
    _criticalSection->Release();
}

void Defcore::onSetCalibrationMode(eCalibrationMode mode)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    Q_ASSERT(_deviceCalibration != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetMode(dmCalibration);
        _device->SetCalibrationMode(mode);
        _device->InitTuningGateList();
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
        _device->SetSide(dsLeft);
        _device->SetChannel(F70E);
        _device->Update(true);
#elif defined(TARGET_AVICONDBHS)
        _device->SetSide(dsNone);
        _device->SetChannel(H70);
        _device->Update(true);
#elif defined(TARGET_AVICONDB)
        _device->SetSide(dsLeft);
        _device->SetChannel(F65E_WP);
        _device->Update(true);
#elif defined(TARGET_AVICON15)
        _device->SetSide(dsNone);
        _device->SetChannel(F70E);
        _device->Update(true);
#else
        _device->Update(false);
#endif
        emit doCalibrationChanged();
    }
    _criticalSection->Release();

    qDebug() << QString("Info| Setted calibration mode.");
}

void Defcore::onChangeCalibrationMode(eCalibrationMode mode)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    Q_ASSERT(_deviceCalibration != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetMode(dmCalibration);
        _device->SetCalibrationMode(mode);
        _device->Update(false);
        emit doCalibrationChanged();
    }
    _criticalSection->Release();
}

void Defcore::onSetSearchMode()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetMode(dmSearch);
#if !defined TARGET_AVICONDBHS
        sScanChannelDescription scanChanDesc;
        memset(&scanChanDesc, 0, sizeof(sScanChannelDescription));
        getFirstScanChannelDescriptionInScheme(scanChanDesc, schemeIndex());
        _device->SetSide(scanChanDesc.DeviceSide);  // TODO: DeviceSide may be uninitialized
#else
        sHandChannelDescription handChanDesc;
        memset(&handChanDesc, 0, sizeof(sHandChannelDescription));
        getFirstHandChannelDescription(handChanDesc);
        _device->SetSide(dsNone);
#endif

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E  // FDV
        scanChanDesc.Id = N0EMS;                         // FDV Сброс id так как при вкл. схеме 2 getFirstScanChannelDescriptionInScheme дает CID из схемы 4 и device переключается на нее !
#endif                                                   // FDV

#if !defined TARGET_AVICONDBHS
        _device->SetChannel(scanChanDesc.Id);
#else
        _device->SetChannel(handChanDesc.Id);
#endif
        _device->Update(false);
        qDebug() << QString("Info| Setted search mode.");
        emit doCalibrationChanged();
    }
    _criticalSection->Release();
}

void Defcore::onDeviceUpdate(bool resetStrokes)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->Update(resetStrokes);
    }
    _criticalSection->Release();
}

void Defcore::onSetStartNotch(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetStNotch(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetSizeNotch(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetNotchLen(value);
    }
    _criticalSection->Release();
}

void Defcore::onSetLevelNotch(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetNotchLevel(value);
    }
    _criticalSection->Release();
}

void Defcore::onCalibrationToRailType()
{
    _criticalSection->Enter();
    pCalibrationToRailTypeResult result = _device->CalibrationToRailType();
    _criticalSection->Release();
    for (std::vector<sCalibrationToRailTypeResItem>::const_iterator it = result->begin(); it != result->end(); ++it) {
        const sCalibrationToRailTypeResItem& cur = it.operator*();
        QDEBUG << Q_FUNC_INFO << cur.Side << cur.Result << cur.id << cur.RailHeight;
    }

    emit doRailTypeResult(result);
}

void Defcore::onResetSens()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->ChannelSensCalibration_ResetSens();
    }
    _criticalSection->Release();
}

void Defcore::onCalibrationType1(int& sens)
{
    calibrateChannel(sens);
}

void Defcore::onCalibrationType2()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->ChannelSensCalibration_Type2();
    }
    _criticalSection->Release();
}

void Defcore::onAutoCalibrationTimeDelay()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->PrismDelayAutoCalibration();
    }
    _criticalSection->Release();
}

void Defcore::onSetControlMode(eDeviceControlMode mode, int additive)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetControlMode(mode, &additive);
    }
    _criticalSection->Release();
}

void Defcore::onChangeLanguage()
{
    onSetSchemeIndex(restoreSchemeNumber());
}

void Defcore::onPauseModeOn()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _lastMode = _device->GetMode();
        _device->SetMode(dmPaused);
        _device->Update(false);
    }
    _criticalSection->Release();
}

void Defcore::onPauseModeOff()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetMode(_lastMode);
        _device->Update(false);
    }
    _criticalSection->Release();
}

void Defcore::onEnableAC(bool isEnabled)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    if (_isDeviceStarted) {
        _device->SetAcousticContact(2, isEnabled);
    }
    _criticalSection->Release();
}

void Defcore::stop()
{
    qDebug() << "Stop defcore";
    finit();
}

void Defcore::onSetRegistrationThreshold(int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    switch (value) {
    case -6:
        value = 16;
        break;
    case -12:
        value = 8;
        break;
    }

    _criticalSection->Enter();
    _device->SetBScanGateLevel(value);
    _criticalSection->Release();
}

void Defcore::onSetUmuLineToCompleteControl()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    //    Q_ASSERT(_isDeviceStarted);
    if (_isDeviceStarted) {
        _device->DisableAll();
        _device->ChangeDeviceConfig(_deviceConfig);
        _device->SetLeftSideSwitching(toCompleteControl);
        _device->EnableAll();
    }
    _criticalSection->Release();
}

void Defcore::onDeviceEnableFiltration(bool isEnable)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    Q_ASSERT(_isDeviceStarted);
    if (_isDeviceStarted) {
        _device->EnableFiltration(isEnable);
    }
    _criticalSection->Release();
}

void Defcore::onSetUmuLineToScannerControl()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    Q_ASSERT(_isDeviceStarted);
    if (_isDeviceStarted) {
        _device->ChangeDeviceConfig(_deviceConfigHeadScan);
        _device->SetLeftSideSwitching(toScaner);
        _device->SetPathEncoderData(static_cast<char>(deviceConfigHeadScan()->getMainPathEncoderIndex()), false, 0, true);
    }
    _criticalSection->Release();
}

void Defcore::onSetDeviceFilterParams(tBScan2FilterParamId prmId, int value)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    Q_ASSERT(_isDeviceStarted);
    switch (prmId) {
    case fprmMinPacketSize:
    case fprmMinPacketSizeForEvent:
    case frmmMinSpacing0dBInNormModeForEvent:
    case fprmMinSpacingMinus6dBInBoltTestModeForEvent:
        value = qCeil(static_cast<float>(value) / encoderCorrection());
        break;
    default:
        // TODO: What about other values?
        break;
    }
    _criticalSection->Enter();
    _device->setFilterParams(prmId, static_cast<unsigned int>(value));
    _criticalSection->Release();
}

void Defcore::onSetleftSideTrolleyPosition(tTrolleySide troleySide)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != nullptr);
    _criticalSection->Enter();
    Q_ASSERT(_isDeviceStarted);
    if (_isDeviceStarted) {
        _device->SetFaceData(troleySide);
    }
    _criticalSection->Release();
}

void Defcore::run()
{
    static const int sizesSize = static_cast<int>(sizeof(unsigned long) + sizeof(void*) + sizeof(void*));
    _stopThread = false;
    while (!_stopThread) {
        if (_deviceEventManager->WaitForEvent()) {
            while (_deviceEventManager->EventDataSize() >= sizesSize) {
                unsigned long dataID = 0;
                _deviceEventManager->ReadEventData(&dataID, SIZE_OF_TYPE(dataID), NULL);  // Чтения заголовока новых данных
                Q_ASSERT(dataID != 0);
                switch (dataID) {
                case edBScan2Data: {
                    PtDEV_BScan2Head bScan2Head = NULL;

                    if (_deviceEventManager->ReadEventData(&bScan2Head, SIZE_OF_TYPE(&bScan2Head), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's head of B-Scan ") << Q_FUNC_INFO;
                        break;
                    }
                    if (bScan2Head == NULL) {
                        qDebug() << QString("Defcore| Warning| head of B-Scan is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doBScan2Data(QSharedPointer<tDEV_BScan2Head>(bScan2Head));
                } break;
                case edAlarmData: {
                    PtDEV_AlarmHead alarmHead = NULL;

                    if (_deviceEventManager->ReadEventData(&alarmHead, SIZE_OF_TYPE(&alarmHead), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's head of alarm ") << Q_FUNC_INFO;
                        break;
                    }
                    if (alarmHead == NULL) {
                        qDebug() << QString("Defcore| Warning| head of alarm is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doAlarmData(QSharedPointer<tDEV_AlarmHead>(alarmHead));
                } break;
                case edAScanMeas: {
                    PtDEV_AScanMeasure aScanMeasure = NULL;
                    if (_deviceEventManager->ReadEventData(&aScanMeasure, SIZE_OF_TYPE(&aScanMeasure), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of scan measure") << Q_FUNC_INFO;
                        break;
                    }
                    if (aScanMeasure == NULL) {
                        qDebug() << QString("Defcore| Warning| scan measure is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doAScanMeas(QSharedPointer<tDEV_AScanMeasure>(aScanMeasure));
                } break;
                case edAScanData: {
                    PtDEV_AScanHead aScanHead = NULL;
                    PtUMU_AScanData aScanData = NULL;
                    if (_deviceEventManager->ReadEventData(&aScanHead, SIZE_OF_TYPE(&aScanHead), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's nead of A-Scan ") << Q_FUNC_INFO;
                        break;
                    }
                    if (_deviceEventManager->ReadEventData(&aScanData, SIZE_OF_TYPE(&aScanData), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of A-Scan ") << Q_FUNC_INFO;
                        break;
                    }
                    if (aScanHead == NULL) {
                        qDebug() << QString("Defcore| Warning| head of A-Scan is null ") << Q_FUNC_INFO;
                        break;
                    }
                    if (aScanData == NULL) {
                        qDebug() << QString("Defcore| Warning| data of A-Scan is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doAScanData(QSharedPointer<tDEV_AScanHead>(aScanHead), QSharedPointer<tUMU_AScanData>(aScanData));
                } break;
                case edTVGData: {
                    PtDEV_AScanHead aScanHead = NULL;
                    PtUMU_AScanData aScanData = NULL;

                    if (_deviceEventManager->ReadEventData(&aScanHead, SIZE_OF_TYPE(&aScanHead), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's head of TVG ") << Q_FUNC_INFO;
                        break;
                    }
                    if (_deviceEventManager->ReadEventData(&aScanData, SIZE_OF_TYPE(&aScanData), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of TVG ") << Q_FUNC_INFO;
                        break;
                    }
                    if (aScanHead == NULL) {
                        qDebug() << QString("Defcore| Warning| head of TVG is null ") << Q_FUNC_INFO;
                        break;
                    }
                    if (aScanData == NULL) {
                        qDebug() << QString("Defcore| Warning| data of TVG is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doTVGData(QSharedPointer<tDEV_AScanHead>(aScanHead), QSharedPointer<tUMU_AScanData>(aScanData));
                } break;
                case edMScan2Data: {
                    PtDEV_BScan2Head bScan2Head = NULL;

                    if (_deviceEventManager->ReadEventData(&bScan2Head, SIZE_OF_TYPE(&bScan2Head), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's head of M-Scan ") << Q_FUNC_INFO;
                        break;
                    }
                    if (bScan2Head == NULL) {
                        qDebug() << QString("Defcore| Warning| head of M-Scan is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doMScan2Data(QSharedPointer<tDEV_BScan2Head>(bScan2Head));
                } break;
                case edPathStepData: {
                    PtDEV_PathStepData pathStepData = NULL;

                    if (_deviceEventManager->ReadEventData(&pathStepData, SIZE_OF_TYPE(&pathStepData), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of Path ") << Q_FUNC_INFO;
                        break;
                    }

                    if (pathStepData == NULL) {
                        qDebug() << QString("Defcore| Warning| data of Path is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doPathStepData(QSharedPointer<tDEV_PathStepData>(pathStepData));
                } break;
                case edSignalPacket: {
                    tDEV_Packet* devicePacket = NULL;

                    if (_deviceEventManager->ReadEventData(&devicePacket, SIZE_OF_TYPE(&devicePacket), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of device packet") << Q_FUNC_INFO;
                        break;
                    }
                    if (devicePacket == NULL) {
                        qDebug() << QString("Defcore| Warning| device packet is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doDeviceSignalsPacket(QSharedPointer<tDEV_Packet>(devicePacket));

                } break;
                case edSignalSpacing: {
                    tDEV_SignalSpacing* deviceSignalSpacing = NULL;

                    if (_deviceEventManager->ReadEventData(&deviceSignalSpacing, SIZE_OF_TYPE(&deviceSignalSpacing), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of device signal spacing") << Q_FUNC_INFO;
                        break;
                    }
                    if (deviceSignalSpacing == NULL) {
                        qDebug() << QString("Defcore| Warning| device signal spacing is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doDeviceSignalSpacing(QSharedPointer<tDEV_SignalSpacing>(deviceSignalSpacing));

                } break;
                case edDefect53_1: {
                    tDEV_Defect53_1* defect53_1 = NULL;

                    if (_deviceEventManager->ReadEventData(&defect53_1, SIZE_OF_TYPE(&defect53_1), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of defect53_1") << Q_FUNC_INFO;
                        break;
                    }
                    if (defect53_1 == NULL) {
                        qDebug() << QString("Defcore| Warning| defect53_1 is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doDefect53_1(QSharedPointer<tDEV_Defect53_1>(defect53_1));

                } break;
                case edBottomSignalAmpl: {
                    int* bottomSignalAmplPtr = NULL;

                    if (_deviceEventManager->ReadEventData(&bottomSignalAmplPtr, SIZE_OF_TYPE(&bottomSignalAmplPtr), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of bottomSignalAmpl") << Q_FUNC_INFO;
                        break;
                    }
                    if (bottomSignalAmplPtr == NULL) {
                        qDebug() << QString("Defcore| Warning| bottomSignalAmplPtr is null ") << Q_FUNC_INFO;
                        break;
                    }

                    emit doBottomSignalAmpl(QSharedPointer<int>(bottomSignalAmplPtr));
                } break;
                case edDeviceSpeed: {
                    unsigned int deviceSpeedInStepsPerSecond;
                    if (_deviceEventManager->ReadEventData(&deviceSpeedInStepsPerSecond, sizeof(unsigned int), NULL) == false) {
                        qDebug() << QString("Defcore| Warning| no event's data of Device Speed ") << Q_FUNC_INFO;
                        break;
                    }
                    emit doDeviceSpeed(deviceSpeedInStepsPerSecond);
                } break;
                default:
                    assert(false);
                    break;
                }
            }
        }
    }
}

void Defcore::finit()
{
    Q_ASSERT(thread() == QThread::currentThread());
    qDebug() << "START Defcore::finit()";
    _interrupInit = true;
    while (!_defcoreTimerFinished) {
        QThread::msleep(1);
        qApp->processEvents();
    }

    if (!_stopThread) {
        qDebug() << "Setting stop thread...";
        _stopThread = true;
        if (this->isRunning()) {
            qDebug() << "Wait defcore thread...";
            this->wait(8000);
            if (this->isRunning()) {
                qDebug() << "Cannot stop defcore thread - terminating!";
                this->terminate();
            }
        }
    }
    QThread::msleep(10);
    if (_device != nullptr) {
        qDebug() << "Deleting device...";
        delete _device;
        _device = nullptr;
    }
    if (_criticalSection != nullptr) {
        qDebug() << "Deleting cs...";
        delete _criticalSection;
        _criticalSection = nullptr;
    }
    if (_criticalSectionDevice != nullptr) {
        qDebug() << "Deleting cs device...";
        delete _criticalSectionDevice;
        _criticalSectionDevice = nullptr;
    }
    if (_umuEventManager != nullptr) {
        qDebug() << "Deleting umuEventManager...";
        delete _umuEventManager;
        _umuEventManager = nullptr;
    }
    if (_deviceEventManager != nullptr) {
        qDebug() << "Deleting deviceEventManager...";
        delete _deviceEventManager;
        _deviceEventManager = nullptr;
    }
    if (_deviceCalibration != nullptr) {
        qDebug() << "===== Deleting device calibration =====";
        delete _deviceCalibration;
        _deviceCalibration = nullptr;
        qDebug() << "===== Device calibration deleted! =====";
    }
    if (_deviceConfig != nullptr) {
        qDebug() << "Deleting deviceConfig...";
        delete _deviceConfig;
        _deviceConfig = nullptr;
    }
    if (_deviceConfigHeadScan != nullptr) {
        qDebug() << "Deleting deviceConfigHeadScan...";
        delete _deviceConfigHeadScan;
        _deviceConfigHeadScan = nullptr;
    }
    if (_channelsTable != nullptr) {
        qDebug() << "Deleting channelsTable...";
        delete _channelsTable;
        _channelsTable = nullptr;
    }
    if (_threadList != nullptr) {
        qDebug() << "Deleting thread list...";
        delete _threadList;
        _threadList = nullptr;
    }
    if (_calibrationCriticalSection != nullptr) {
        qDebug() << "Deleting calibration cs...";
        delete _calibrationCriticalSection;
        _calibrationCriticalSection = nullptr;
    }
    qDebug() << "FINISH Defcore::finit()";
    _defcoreFinit = true;
}

void Defcore::umuTimerTimeout()
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (_device != nullptr) {
        qDebug() << QString("UMU| Info| UMU error message count:%1 UMU skip message count:%2").arg(_device->GetUMUErrorMessageCount(0)).arg(_device->GetUMUSkipMessageCount(0));
#ifdef TARGET_AVICON31E
        qDebug() << QString("UMU 2| Info| UMU error message count:%1 UMU skip message count:%2").arg(_device->GetUMUErrorMessageCount(1)).arg(_device->GetUMUSkipMessageCount(1));
#endif
    }
}

void Defcore::umuConnectionTimerTimeout()
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (_device != nullptr) {
        _criticalSection->Enter();
        bool res = _device->GetUMUOnLineStatus(0);
        _criticalSection->Release();
        emit doUmuConnectionStatusChanged(res);
    }
}

cChannelsTable* Defcore::channelsTable() const
{
    return _channelsTable;
}

cDeviceConfig* Defcore::deviceConfig() const
{
    return _deviceConfig;
}

cDeviceConfig* Defcore::deviceConfigHeadScan() const
{
    return _deviceConfigHeadScan;
}

cDeviceCalibration* Defcore::deviceCalibration() const
{
    return _deviceCalibration;
}

void Defcore::remapCalibrations()
{
    _calibrationMutex.lock();

    _filteredCalibrations.clear();
    _calibrationNames.clear();

    int count = _deviceCalibration->Count();
    int currentSchemeIndex = _deviceConfig->GetCurrentChannelGroupIndex();
    for (int i = 0; i < count; ++i) {
        if (currentSchemeIndex == _deviceCalibration->GetScheme(i)) {
            _filteredCalibrations.append(i);
            _calibrationNames.append(QString::fromUtf8(_deviceCalibration->GetName(i)));
        }
    }

    _calibrationMutex.unlock();
}

int Defcore::getCalibrationCountBySchemeIndex(int schemeIndex)
{
    _calibrationMutex.lock();
    int count = _deviceCalibration->Count();
    int rez = 0;
    for (int i = 0; i < count; ++i) {
        if (schemeIndex == _deviceCalibration->GetScheme(i)) {
            ++rez;
        }
    }
    _calibrationMutex.unlock();
    return rez;
}

void Defcore::createCalibrationIfDoesnotExist(int schemeIndex)
{
    if (getCalibrationCountBySchemeIndex(schemeIndex) == 0) {
        createCalibration(schemeIndex);
    }
}

bool Defcore::createCalibration(int schemeIndex, const QString& name)
{
    _calibrationMutex.lock();
    int calibrationIndex = _deviceCalibration->Count();
    bool rc = _deviceCalibration->CreateNew();
    if (rc) {
        QByteArray byteArray = name.toLocal8Bit();
        _deviceCalibration->SetName(byteArray.data());
        _deviceCalibration->SetScheme(static_cast<unsigned int>(schemeIndex));
        _deviceCalibration->SetCurrent(calibrationIndex);
    }
    _calibrationMutex.unlock();
    return rc;
}

bool Defcore::getFirstScanChannelDescriptionInScheme(sScanChannelDescription& scanChanDesc, int schemeNumber)
{
    unsigned int scanChannelsCount = _deviceConfig->GetAllScanChannelsCount();
    if (_deviceConfig->getControlledRail() == crBoth) {
        scanChannelsCount *= 2;
    }

    int strokeGroupIdx = _deviceConfig->GetChannelGroup(schemeNumber);
    unsigned int idx;
    for (idx = 0; idx < scanChannelsCount; ++idx) {
        if (!_deviceConfig->getSChannelbyIdx(static_cast<int>(idx), &scanChanDesc)) {
            continue;
        }

        if (strokeGroupIdx == scanChanDesc.StrokeGroupIdx) {
            break;
        }
    }
    return idx < scanChannelsCount;
}

bool Defcore::getFirstHandChannelDescription(sHandChannelDescription& handChanDesc)
{
    unsigned int handChannelsCount = _deviceConfig->GetHandChannelsCount();
    unsigned int idx;
    for (idx = 0; idx < handChannelsCount; ++idx) {
        if (!_deviceConfig->getHChannelbyIdx(static_cast<int>(idx), &handChanDesc)) {
            continue;
        }
    }
    return idx < handChannelsCount;
}

bool Defcore::getDefcoreFinit() const
{
    return _defcoreFinit;
}

eCoordSys Defcore::getSystemCoordType() const
{
    return _deviceConfig->getCoordSys();
}

tSensValidRangesList Defcore::getSensValidRanges() const
{
    return _deviceConfig->getSensValidRangesList();
}

void Defcore::onSetDynamicRailType(bool isEnabled)
{
    _criticalSection->Enter();
    _device->SetRailTypeTrackingMode(isEnabled);
    qDebug() << "Dynamic rail type: " << isEnabled;
    _criticalSection->Release();
}

void Defcore::onSetAv17CoordX(int coord)
{
    _criticalSection->Enter();
    _device->SetPathEncoderData(1, false, coord, false);
    _criticalSection->Release();
}

void Defcore::onSetAv17CoordY(int coord)
{
    _criticalSection->Enter();
    _device->SetPathEncoderData(2, false, coord, false);
    _criticalSection->Release();
}

void Defcore::onEnableSMChSensAutoCalibration(bool isEnable)
{
    _criticalSection->Enter();
    _device->EnableSMChSensAutoCalibration(isEnable);
    _criticalSection->Release();
}

void Defcore::SetFixBadSensState(bool State)
{
    _criticalSection->Enter();
    _device->SetFixBadSensState(State);
    _criticalSection->Release();
}

void Defcore::SetTimeToFixBadSens(int TimeToFixBadSens)
{
    _criticalSection->Enter();
    _device->SetTimeToFixBadSens(TimeToFixBadSens);
    _criticalSection->Release();
}

#ifdef ANDROID
void Defcore::onUMUBatteryVoltageQuery(void)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != 0);

    double voltage;
    _criticalSection->Enter();
    _device->GetUAkkByVolt(&voltage);
    _criticalSection->Release();
    emit doBatteryVoltage(voltage);
}

void Defcore::onUMUBatteryChargePercentQuery(void)
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(_device != 0);

    unsigned int percent;
    _criticalSection->Enter();
    _device->GetUAkkByPercent(&percent);
    _criticalSection->Release();
    emit doBatteryChargePercent((double) (percent));
}
#endif
