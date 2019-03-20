#include <QCoreApplication>
#include <QThread>

#include "core.h"
#include "debug.h"
#include "defcore.h"
#include "channelscontroller.h"
#include "checkcalibration.h"
#include "alarmtone.h"
#include "registration.h"
#include "roles.h"
#include "screen.h"
#include "gpio.h"
#include "settings.h"
#include "notifier.h"
#include "limitsforsens.h"
#include "defectmarker.h"
#include "temperature/temperaturesensormanager.h"
#include "accessories.h"
#include "defcore/ekasui/EK_ASUIdata.h"
#include "bluetooth/bluetoothmanager.h"
#include "passwordmanager.h"
#include "ftp/ftpuploader.h"
#include "appdatapath.h"
#include "remotecontrol/remotecontrol.h"
#include "videocontroller/videoplayercontroller.h"
#include "colorschememanager.h"
#include "colorscheme.h"
#include "filemanager.h"
#include "Device_definitions.h"
#include "battery.h"
#include "notifications/notificationmanager.h"
#include "screen.h"

#if defined(IMX_DEVICE)
#include "bluetooth/bluetoothmanagerqt.h"
#elif defined ANDROID
#include "bluetooth/bluetoothmanagerandroid.h"
#else
#include "bluetooth/fakebluetoothmanager.h"
#endif

const int MIN_TEMPERATURE_DIFFERENCE = 10;
const int INTERVAL_FOR_CHECK_CALIBRATION_TEMPERATURE = 60000;
const unsigned char COUNTS_OF_NOTES = 3;
const int CRITICAL_SOC_TEMPERATURE_DEGREES = 80;
const int INTERVAL_FOR_CHECK_SOC_TEMPERATURE_MS = 120000;
const int INTERVAL_FOR_CHECK_SOC_TEMPERATURE_AFTER_DECREASE_BRIGHTNESS_MS = 60000;
const int MINIMAL_BRIGHTNESS = 30;
const int MINIMAL_COUNT_LOGS = 100;

Core& Core::instance()
{
    static Core _instance;
    return _instance;
}

void Core::start()
{
    qDebug() << "Defcore thread start";
    Q_ASSERT(_defcoreThread);
    Q_ASSERT(_defcore);
    _defcoreThread->start();
}

ChannelsController* Core::channelsController()
{
    Q_ASSERT(_channelsController);
    return _channelsController;
}

AlarmTone* Core::alarmTone()
{
    Q_ASSERT(_alarmTone != nullptr);
    return _alarmTone;
}

Registration& Core::registration()
{
    Q_ASSERT(_registration);
    return (*_registration);
}

CheckCalibration& Core::checkcalibration()
{
    Q_ASSERT(_checkcalibration);
    return *_checkcalibration;
}

DefectMarker& Core::defectmarker()
{
    Q_ASSERT(_defectmarker);
    return *_defectmarker;
}

void Core::setCalibrationMode(const QString& newCalibrationName)
{
    Q_ASSERT(_defcore != nullptr);
    Q_ASSERT(_channelsController != nullptr);
    Q_ASSERT(_alarmTone != nullptr);

    emit doCreateNewCalibration(newCalibrationName);
    _channelsController->setCalibrationMode();
    _channelsController->onCalibrationChanged();
    _channelsController->setDataForAllItems(true, IsAlarmedRole);
    _channelsController->setDataForSlaveChannel();
    _alarmTone->setSilentMode(true);

    setCalibrationMode(cmSens);
}

void Core::setCalibrationMode(eCalibrationMode mode)
{
    qDebug() << QString("Info| Setted calibration mode");
    emit doSetCalibrationMode(mode);
}

void Core::changeCalibrationMode(eCalibrationMode mode)
{
    emit doChangeCalibrationMode(mode);
}

void Core::setRecalibrationMode()
{
    Q_ASSERT(_channelsController != nullptr);
    Q_ASSERT(_alarmTone != nullptr);

    _channelsController->setCalibrationMode();
    _channelsController->setDataForAllItems(true, IsAlarmedRole);
    _channelsController->setDataForSlaveChannel();
    _alarmTone->setSilentMode(true);

    qDebug() << QString("Info| Setted recalibration mode");
    setCalibrationMode(cmSens);
}

void Core::setSearchMode()
{
    Q_ASSERT(_channelsController != nullptr);
    Q_ASSERT(_alarmTone != nullptr);

    _channelsController->setDataForAllItems(false, IsAlarmedRole);
    _channelsController->setSearchMode();
    _alarmTone->setSilentMode(false);

    emit doSetSearchMode();
}

int Core::evaluationGateLevel()
{
    return Defcore::aScanThreshold();
}

int Core::dbToAmp(int db)
{
    static const int dbToAmp[16] = {8, 10, 12, 16, 20, 25, 32, 40, 50, 63, 80, 101, 127, 160, 201, 254};
    if (Q_UNLIKELY(db > 15)) {
        db = 15;
    }
    return dbToAmp[db];
}

void Core::setSchemeIndex(int index)
{
    emit doSetSchemeIndex(index);
}

int Core::currentSchemeIndex() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->schemeIndex();
}

int Core::schemesCount() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->schemeCount();
}

QString Core::deviceSerialNumber() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->deviceSerialNumber();
}

QString Core::deviceFirmwareVersion() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->deviceFirmwareVersion();
}

QString Core::devicePldVersion() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->devicePldVersion();
}

QStringList Core::deviceSerialNumbers() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->deviceSerialNumbers();
}

QStringList Core::deviceFirmwareVersions() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->deviceFirmwareVersions();
}

QStringList Core::devicePldVersions() const
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->devicePldVersions();
}

void Core::connectCoreSignals()
{
    ASSERT(connect(_defcoreThread, &QThread::started, _defcore, &Defcore::init));
    ASSERT(connect(_defcoreThread, &QThread::finished, _defcore, &QObject::deleteLater));
    ASSERT(connect(this, &Core::stopDefcore, _defcore, &Defcore::stop, Qt::BlockingQueuedConnection));

    ASSERT(connect(_defcore, &Defcore::doSchemeChanged, this, &Core::onSchemeChanged));
    ASSERT(connect(this, &Core::doSetSchemeIndex, _defcore, &Defcore::onSetSchemeIndex));
    ASSERT(connect(this, &Core::doSetCalibrationIndex, _defcore, &Defcore::onSetCalibrationIndex));
    ASSERT(connect(_defcore, &Defcore::doInited, this, &Core::doInited));

    ASSERT(connect(_defcore, &Defcore::doAScanMeas, this, &Core::doAScanMeas));
    ASSERT(connect(_defcore, &Defcore::doAScanData, this, &Core::doAScanData));
    ASSERT(connect(_defcore, &Defcore::doTVGData, this, &Core::doTVGData));
    ASSERT(connect(_defcore, &Defcore::doAlarmData, this, &Core::doAlarmData));
    ASSERT(connect(_defcore, &Defcore::doBScan2Data, this, &Core::doBScan2Data));
    ASSERT(connect(_defcore, &Defcore::doMScan2Data, this, &Core::doMScan2Data));
    ASSERT(connect(_defcore, &Defcore::doPathStepData, this, &Core::doPathStepData));
    ASSERT(connect(_defcore, &Defcore::doDeviceSpeed, this, &Core::doDeviceSpeed));

    ASSERT(connect(_defcore, &Defcore::doInited, this, &Core::startRegistration));

    ASSERT(connect(this, &Core::doSetCalibrationMode, _defcore, &Defcore::onSetCalibrationMode));
    ASSERT(connect(this, &Core::doChangeCalibrationMode, _defcore, &Defcore::onChangeCalibrationMode));
    ASSERT(connect(this, &Core::doSetSearchMode, _defcore, &Defcore::onSetSearchMode));

    ASSERT(connect(this, &Core::doCreateNewCalibration, _defcore, &Defcore::createNewCalibration));

    ASSERT(connect(_defcore, &Defcore::doConnectionStatusDisconnected, this, &Core::doConnectionStatusDisconnected));
    ASSERT(connect(_defcore, &Defcore::doConnectionStatusConnecting, this, &Core::doConnectionStatusConnecting));
    ASSERT(connect(_defcore, &Defcore::doConnectionStatusConnected, this, &Core::doConnectionStatusConnected));

    ASSERT(connect(_defcore, &Defcore::doUmuConnectionStatusChanged, this, &Core::doUmuConnectionStatusChanged));
    ASSERT(connect(_defcore, &Defcore::doUmuConnectionStatusChanged, this, &Core::onUmuConnectionStatusChanged));

    ASSERT(connect(_defcore, &Defcore::dataChanged, this, &Core::onDefcoreDataChanged));

    ASSERT(connect(this, &Core::restoreAC, _defcore, &Defcore::restoreACSettings));

    ASSERT(connect(this, &Core::doCalibrationToRailType, _defcore, &Defcore::onCalibrationToRailType));
    ASSERT(connect(this, &Core::doResetSens, _defcore, &Defcore::onResetSens));
    ASSERT(connect(this, &Core::doCalibrationType2, _defcore, &Defcore::onCalibrationType2));
    ASSERT(connect(this, &Core::doChangeLanguage, _defcore, &Defcore::onChangeLanguage));
    ASSERT(connect(this, &Core::doAutoPrismDelayCalibration, _defcore, &Defcore::onAutoCalibrationTimeDelay));
    ASSERT(connect(this, &Core::doSetControlMode, _defcore, &Defcore::onSetControlMode));

    ASSERT(connect(this, &Core::doPauseModeOn, _defcore, &Defcore::onPauseModeOn));
    ASSERT(connect(this, &Core::doPauseModeOff, _defcore, &Defcore::onPauseModeOff));

    ASSERT(connect(this, &Core::doSetUmuLineToCompleteControl, _defcore, &Defcore::onSetUmuLineToCompleteControl));
    ASSERT(connect(this, &Core::doSetUmuLineToScannerControl, _defcore, &Defcore::onSetUmuLineToScannerControl));

    ASSERT(connect(this, &Core::doTestSensLevels, _defcore, &Defcore::onTestSensLevels));

    ASSERT(connect(this, &Core::doSetRegistrationThreshold, _defcore, &Defcore::onSetRegistrationThreshold));

    ASSERT(connect(this, &Core::doDeviceEnableFiltration, _defcore, &Defcore::onDeviceEnableFiltration));

    ASSERT(connect(this, &Core::doSetDeviceFilterParams, _defcore, &Defcore::onSetDeviceFilterParams));

    ASSERT(connect(this, &Core::doDisableAll, _defcore, &Defcore::disableAll, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &Core::doEnableAll, _defcore, &Defcore::enableAll, Qt::BlockingQueuedConnection));

    ASSERT(connect(this, &Core::doResetPathEncoder, _defcore, &Defcore::resetPathEncoder, Qt::BlockingQueuedConnection));

    ASSERT(connect(this, &Core::SetAv17CoordX, _defcore, &Defcore::onSetAv17CoordX));
    ASSERT(connect(this, &Core::SetAv17CoordY, _defcore, &Defcore::onSetAv17CoordY));
    ASSERT(connect(this, &Core::doEnableSMChSensAutoCalibration, _defcore, &Defcore::onEnableSMChSensAutoCalibration));
    ASSERT(connect(this, &Core::doEnableAc, _defcore, &Defcore::onEnableAC));

    ASSERT(connect(this, &Core::doUsePathEncoderEmulator, _defcore, &Defcore::onUsePathEncoderEmulator));

#if defined TARGET_AVICON31
    ASSERT(connect(Core::channelsController(), &ChannelsController::changeCalibration, this, &Core::onChangeCalibration));

    ASSERT(connect(_temperatureSensorManager, &TemperatureSensorManager::internalTemperatureChanged, this, &Core::onInternalTemperatureChanged));
    ASSERT(connect(_temperatureSensorManager, &TemperatureSensorManager::externalTemperatureChanged, this, &Core::onExternalTemperatureChanged));
    ASSERT(connect(_temperatureSensorManager, &TemperatureSensorManager::internalTemperatureChanged, this, &Core::internalTemperatureChanged));
    ASSERT(connect(_temperatureSensorManager, &TemperatureSensorManager::externalTemperatureChanged, this, &Core::externalTemperatureChanged));
    ASSERT(connect(this, &Core::pollTempSensor, _temperatureSensorManager, &TemperatureSensorManager::sendSignals));
    ASSERT(connect(_temperatureSensorManager, &TemperatureSensorManager::sensorFailure, this, &Core::onTempSensorFailure));

    ASSERT(connect(_timerForCheckCalibrationTemperature, &QTimer::timeout, this, &Core::onTimerForCheckCalibrationTimeOut));
    ASSERT(connect(&Battery::instance(), &Battery::temperatureSoc, this, &Core::onTemperatureSocChanged));
    ASSERT(connect(_timerForCheckSocTemperature, &QTimer::timeout, this, &Core::onTimerForCheckSocTemperatureTimeout));
#endif
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    ASSERT(connect(_timerForAcNotification, &QTimer::timeout, this, &Core::onTimerForAcNotificationTimeOut));
#endif

#if defined ANDROID
    ASSERT(connect(this, &Core::doBatteryVoltageQury, _defcore, &Defcore::onUMUBatteryVoltageQuery));
    ASSERT(connect(this, &Core::doBatteryPerecentQuery, _defcore, &Defcore::onUMUBatteryChargePercentQuery));

    ASSERT(connect(_defcore, &Defcore::doBatteryVoltage, this, &Core::doBatteryVoltage));
    ASSERT(connect(_defcore, &Defcore::doBatteryChargePercent, this, &Core::doBatteryChargePercent));
#endif

    ASSERT(connect(this, &Core::doChangeColorScheme, _channelsController, &ChannelsController::onColorSchemeChanged));
    ASSERT(connect(this, &Core::doPlaySound, _alarmTone, &AlarmTone::playSound));
    ASSERT(connect(this, &Core::doUseAutoGainAdjustment, _defcore, &Defcore::onAutoGainAdjustmentEnabled));
    ASSERT(connect(this, &Core::doSetDynamicRailType, _defcore, &Defcore::onSetDynamicRailType));
}

void Core::disconnectCoreSignals()
{
    ASSERT(disconnect(_defcore, &Defcore::doAScanMeas, this, &Core::doAScanMeas));
    ASSERT(disconnect(_defcore, &Defcore::doAScanData, this, &Core::doAScanData));
    ASSERT(disconnect(_defcore, &Defcore::doTVGData, this, &Core::doTVGData));
    ASSERT(disconnect(_defcore, &Defcore::doAlarmData, this, &Core::doAlarmData));
    ASSERT(disconnect(_defcore, &Defcore::doBScan2Data, this, &Core::doBScan2Data));
    ASSERT(disconnect(_defcore, &Defcore::doMScan2Data, this, &Core::doMScan2Data));
    ASSERT(disconnect(_defcore, &Defcore::doPathStepData, this, &Core::doPathStepData));
    ASSERT(disconnect(_defcore, &Defcore::dataChanged, this, &Core::onDefcoreDataChanged));

    // ASSERT(disconnect(_defcore, &Defcore::doInited, this, &Core::startRegistration));

    ASSERT(disconnect(this, &Core::doSetCalibrationMode, _defcore, &Defcore::onSetCalibrationMode));
    ASSERT(disconnect(this, &Core::doChangeCalibrationMode, _defcore, &Defcore::onChangeCalibrationMode));
    ASSERT(disconnect(this, &Core::doSetSearchMode, _defcore, &Defcore::onSetSearchMode));

    ASSERT(disconnect(this, &Core::doCalibrationToRailType, _defcore, &Defcore::onCalibrationToRailType));
    ASSERT(disconnect(this, &Core::doResetSens, _defcore, &Defcore::onResetSens));
    ASSERT(disconnect(this, &Core::doCalibrationType2, _defcore, &Defcore::onCalibrationType2));
    ASSERT(disconnect(this, &Core::doChangeLanguage, _defcore, &Defcore::onChangeLanguage));
    ASSERT(disconnect(this, &Core::doAutoPrismDelayCalibration, _defcore, &Defcore::onAutoCalibrationTimeDelay));
    ASSERT(disconnect(this, &Core::doSetControlMode, _defcore, &Defcore::onSetControlMode));

    ASSERT(disconnect(this, &Core::doPauseModeOn, _defcore, &Defcore::onPauseModeOn));
    ASSERT(disconnect(this, &Core::doPauseModeOff, _defcore, &Defcore::onPauseModeOff));

    ASSERT(disconnect(this, &Core::doSetUmuLineToCompleteControl, _defcore, &Defcore::onSetUmuLineToCompleteControl));
    ASSERT(disconnect(this, &Core::doSetUmuLineToScannerControl, _defcore, &Defcore::onSetUmuLineToScannerControl));

    ASSERT(disconnect(this, &Core::doTestSensLevels, _defcore, &Defcore::onTestSensLevels));

    ASSERT(disconnect(this, &Core::doSetRegistrationThreshold, _defcore, &Defcore::onSetRegistrationThreshold));

    ASSERT(disconnect(this, &Core::doDeviceEnableFiltration, _defcore, &Defcore::onDeviceEnableFiltration));

    ASSERT(disconnect(this, &Core::doSetDeviceFilterParams, _defcore, &Defcore::onSetDeviceFilterParams));

    ASSERT(disconnect(this, &Core::doDisableAll, _defcore, &Defcore::disableAll));
    ASSERT(disconnect(this, &Core::doEnableAll, _defcore, &Defcore::enableAll));

    ASSERT(disconnect(this, &Core::doResetPathEncoder, _defcore, &Defcore::resetPathEncoder));

    ASSERT(disconnect(this, &Core::SetAv17CoordX, _defcore, &Defcore::onSetAv17CoordX));
    ASSERT(disconnect(this, &Core::SetAv17CoordY, _defcore, &Defcore::onSetAv17CoordY));
    ASSERT(disconnect(this, &Core::doEnableSMChSensAutoCalibration, _defcore, &Defcore::onEnableSMChSensAutoCalibration));
    ASSERT(disconnect(this, &Core::doEnableAc, _defcore, &Defcore::onEnableAC));
    ASSERT(disconnect(this, &Core::restoreAC, _defcore, &Defcore::restoreACSettings));

#if defined TARGET_AVICON31
    ASSERT(disconnect(Core::channelsController(), &ChannelsController::changeCalibration, this, &Core::onChangeCalibration));
    ASSERT(disconnect(_temperatureSensorManager, &TemperatureSensorManager::internalTemperatureChanged, this, &Core::onInternalTemperatureChanged));
    ASSERT(disconnect(_temperatureSensorManager, &TemperatureSensorManager::externalTemperatureChanged, this, &Core::onExternalTemperatureChanged));
    ASSERT(disconnect(_temperatureSensorManager, &TemperatureSensorManager::internalTemperatureChanged, this, &Core::internalTemperatureChanged));
    ASSERT(disconnect(_temperatureSensorManager, &TemperatureSensorManager::externalTemperatureChanged, this, &Core::externalTemperatureChanged));
    ASSERT(disconnect(this, &Core::pollTempSensor, _temperatureSensorManager, &TemperatureSensorManager::sendSignals));
    ASSERT(disconnect(_temperatureSensorManager, &TemperatureSensorManager::sensorFailure, this, &Core::onTempSensorFailure));
    ASSERT(disconnect(_timerForCheckCalibrationTemperature, &QTimer::timeout, this, &Core::onTimerForCheckCalibrationTimeOut));
    ASSERT(disconnect(&Battery::instance(), &Battery::temperatureSoc, this, &Core::onTemperatureSocChanged));
    ASSERT(disconnect(_timerForCheckSocTemperature, &QTimer::timeout, this, &Core::onTimerForCheckSocTemperatureTimeout));
#endif
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    ASSERT(disconnect(_timerForAcNotification, &QTimer::timeout, this, &Core::onTimerForAcNotificationTimeOut));
#endif
    ASSERT(disconnect(this, &Core::doChangeColorScheme, _channelsController, &ChannelsController::onColorSchemeChanged));
    ASSERT(disconnect(this, &Core::doPlaySound, _alarmTone, &AlarmTone::playSound));
    ASSERT(disconnect(this, &Core::doUseAutoGainAdjustment, _defcore, &Defcore::onAutoGainAdjustmentEnabled));
    ASSERT(disconnect(_defcore, &Defcore::doUmuConnectionStatusChanged, this, &Core::doUmuConnectionStatusChanged));
    ASSERT(disconnect(_defcore, &Defcore::doUmuConnectionStatusChanged, this, &Core::onUmuConnectionStatusChanged));
}

void Core::connectRemoteControlSignals()
{
    ASSERT(connect(this, &Core::doListen, _remoteControl, &RemoteControl::listen));
    ASSERT(connect(_remoteControl, &RemoteControl::doRcConnected, this, &Core::doRcConnected));
    ASSERT(connect(_remoteControl, &RemoteControl::doRcDisconnected, this, &Core::doRcDisconnected));
    ASSERT(connect(_remoteControl, &RemoteControl::doTrainingPcConnected, this, &Core::doTrainingPcConnected));
    ASSERT(connect(_remoteControl, &RemoteControl::doTrainingPcDisconnected, this, &Core::doTrainingPcDisconnected));
}

void Core::disConnectRemoteControlSignals()
{
    ASSERT(disconnect(this, &Core::doListen, _remoteControl, &RemoteControl::listen));
    ASSERT(disconnect(_remoteControl, &RemoteControl::doRcConnected, this, &Core::doRcConnected));
    ASSERT(disconnect(_remoteControl, &RemoteControl::doRcDisconnected, this, &Core::doRcDisconnected));
    ASSERT(disconnect(_remoteControl, &RemoteControl::doTrainingPcConnected, this, &Core::doTrainingPcConnected));
    ASSERT(disconnect(_remoteControl, &RemoteControl::doTrainingPcDisconnected, this, &Core::doTrainingPcDisconnected));
}

void Core::connectVideoControlSignals()
{
    ASSERT(connect(this, &Core::doVideoRegistrationStart, _videoPlayerController, &VideoPlayerController::onRegistrationOn));
    ASSERT(connect(this, &Core::doVideoRegistrationStop, _videoPlayerController, &VideoPlayerController::onRegistrationOff));
    ASSERT(connect(this, &Core::doVideoModeRealtime, _videoPlayerController, &VideoPlayerController::onRealtimeMode));
    ASSERT(connect(this, &Core::doVideoModeView, _videoPlayerController, &VideoPlayerController::onViewMode));
    ASSERT(connect(this, &Core::doVideoSetCoord, _videoPlayerController, &VideoPlayerController::onSetCoord));
}

void Core::disConnectVideoControlSignals()
{
    disconnect(this, &Core::doVideoRegistrationStart, _videoPlayerController, &VideoPlayerController::onRegistrationOn);
    disconnect(this, &Core::doVideoRegistrationStop, _videoPlayerController, &VideoPlayerController::onRegistrationOff);
    disconnect(this, &Core::doVideoModeRealtime, _videoPlayerController, &VideoPlayerController::onRealtimeMode);
    disconnect(this, &Core::doVideoModeView, _videoPlayerController, &VideoPlayerController::onViewMode);
    disconnect(this, &Core::doVideoSetCoord, _videoPlayerController, &VideoPlayerController::onSetCoord);
}

void Core::initEKASUI()
{
    Q_ASSERT(_ekasui == nullptr);
    _ekasui = new EK_ASUI(ekasuiPath());
    _ekasui->SetCarID(restoreEKASUICarId());
}

void Core::changeScreenHeaterState(bool state)
{
    GpioOutput(0, state);
    _screenHeaterCurrentState = state;
    emit heaterStateChanged(_screenHeaterCurrentState);
}

Core::Core(QObject* parent)
    : QObject(parent)
    , _defcoreThread(new QThread(this))
    , _defcore(new Defcore())
    , _rcThread(nullptr)
    , _videoControllerThread(nullptr)
    , _channelsController(new ChannelsController(this))
    , _alarmTone(new AlarmTone(this))
    , _registration(new Registration(this))
    , _limitsForSens(new LimitsForSens(this))
    , _lastTemperature(cTemperatureValueUnknown)
    , _lastSocTemperature(cTemperatureValueUnknown)
    , _screenHeaterTargetTemperature(-273)
    , _checkcalibration(new CheckCalibration(this))
    , _defectmarker(new DefectMarker(this))
    , _temperatureSensorManager(new TemperatureSensorManager(restoreExternalTempSensorType(), restoreInternalTempSensorType()))
    , _colorSchemeManager(new ColorSchemeManager())
    , _temperatureSensorManagerThread(new QThread(this))
    , _ftpUploader(new FTPUploader())
    , _trackMarks(nullptr)
    , _ekasui(nullptr)
    , _btManager(nullptr)
    , _countsOfNotes(COUNTS_OF_NOTES)
    , _currentDirection(1)
    , _isUmuConnected(false)
    , _acNotificationEnabled(restoreACVoiceNotificationEnabled())
    , _screenHeaterStatus(false)
    , _screenHeaterCurrentState(false)
    , _isScreenBrightnessDecrease(false)
    , _isCduOverheated(false)
    , _timerForAcNotification(new QTimer(this))
    , _passwordManager(new PasswordManager())
    , _fileManager(nullptr)
    , _fileManagerThread(new QThread(this))
    , _remoteControl(nullptr)
    , _videoPlayerController(nullptr)
    , _deviceType(static_cast<DeviceType>(restoreDeviceType()))
{
    qRegisterMetaType<CID>("CID");
    qRegisterMetaType<DeviceSide>("DeviceSide");
    qRegisterMetaType<eCalibrationMode>("eCalibrationMode");
    qRegisterMetaType<eDeviceControlMode>("eDeviceControlMode");
    qRegisterMetaType<tBScan2FilterParamId>("tBScan2FilterParamId");
    qRegisterMetaType<tTrolleySide>("tTrolleySide");
    qRegisterMetaType<Direction>("Direction");
    qRegisterMetaType<ViewCoordinate>("ViewCoordinate");
    qRegisterMetaType<QGeoPositionInfo>("QGeoPositionInfo");

    _acNotifyStates[0] = false;
    _acNotifyStates[1] = false;

    _defcoreThread->setObjectName("_defcoreThread");
    ASSERT(connect(_defcoreThread, &QThread::finished, _defcore, &QObject::deleteLater));
    _checkcalibration->setDefcore(this, _defcore);
    _defectmarker->setDefcore(this, _defcore);
    _registration->setDefcore(_defcore);

    _defcore->setObjectName("_defcore");
    _defcore->moveToThread(_defcoreThread);

    _timerForCheckCalibrationTemperature = new QTimer(this);
    _timerForCheckCalibrationTemperature->setInterval(INTERVAL_FOR_CHECK_CALIBRATION_TEMPERATURE);

    _timerForCheckSocTemperature = new QTimer(this);

    connectCoreSignals();

    _rcThread = new QThread(this);
    _rcThread->setObjectName("remoteControlThread");
    _remoteControl = new RemoteControl();
    ASSERT(connect(_rcThread, &QThread::started, _remoteControl, &RemoteControl::start));
    ASSERT(connect(_rcThread, &QThread::finished, this, &Core::onRemoteControlFinished));

    connectRemoteControlSignals();
    _remoteControl->moveToThread(_rcThread);
    _rcThread->start();


    if (_deviceType == Avicon31Estonia) {
        _videoControllerThread = new QThread(this);
        _videoControllerThread->setObjectName("videoControllerThread");

        _videoPlayerController = new VideoPlayerController();
        ASSERT(connect(this, &Core::startVideoController, _videoPlayerController, &VideoPlayerController::onInit, Qt::BlockingQueuedConnection));
        ASSERT(connect(this, &Core::stopVideoController, _videoPlayerController, &VideoPlayerController::onStop, Qt::BlockingQueuedConnection));
        connectVideoControlSignals();

        _videoPlayerController->moveToThread(_videoControllerThread);
        _videoControllerThread->start();
        emit startVideoController();
    }

    getTrackMarks();

    Q_ASSERT(_channelsController != nullptr);
    _channelsController->setDefcore(_defcore);
    _channelsController->setChannelsTable(_defcore->channelsTable());
    _channelsController->setDeviceConfig(_defcore->deviceConfig());
#if defined TARGET_AVICON31
    _channelsController->setDeviceConfigHeadScan(_defcore->deviceConfigHeadScan());
#endif
    _channelsController->setDeviceCalibration(_defcore->deviceCalibration());
    _channelsController->setSearchMode();


    Q_ASSERT(_alarmTone);
    _alarmTone->setObjectName("_alarmTone");

    _ftpUploader->setParameters(restoreFtpServer(), restoreFtpPath(), restoreFtpLogin(), restoreFtpPassword());
#if IMX_DEVICE
    _btManager = new BluetoothManagerQt();
    _btManager->init();
#elif ANDROID
    _btManager = new BluetoothManagerAndroid();
    _btManager->init();
#else
    _btManager = new FakeBluetoothManager();
#endif

    _fileManagerThread->setObjectName("fileManagerThread");
    _fileManager = new FileManager();
    connect(_fileManagerThread, &QThread::started, _fileManager, &FileManager::started);
    _fileManager->moveToThread(_fileManagerThread);
    _fileManagerThread->start();
}

Core::~Core()
{
    qDebug() << "Deleting core...";


    delete _temperatureSensorManager;
    delete _ftpUploader;

#if defined TARGET_AVICON31
    delete _btManager;
#endif

    delete _ekasui;
    delete _passwordManager;


    qDebug() << "Core deleted!";
}

void Core::finit()
{
    Q_ASSERT(_defcore);
    Q_ASSERT(_defcoreThread);

    disconnectCoreSignals();
    disConnectRemoteControlSignals();
    if (_rcThread != nullptr) {
        _rcThread->quit();
        _rcThread->wait(8000);
        if (_rcThread->isRunning()) {
            _rcThread->terminate();
            _rcThread->wait(8000);
        }
    }
    if (_deviceType == Avicon31Estonia) {
        disConnectVideoControlSignals();


        if (_videoControllerThread != nullptr) {
            emit stopVideoController();
            _videoControllerThread->quit();
            _videoControllerThread->wait(8000);
            if (_videoControllerThread->isRunning()) {
                _videoControllerThread->terminate();
                _videoControllerThread->wait(8000);
            }
        }
    }
#if defined TARGET_AVICON31
    _timerForCheckCalibrationTemperature->stop();
    delete _timerForCheckCalibrationTemperature;
#endif

    emit doStopTempSensorManager();
    _temperatureSensorManagerThread->quit();
    _temperatureSensorManagerThread->wait(8000);

    if (_registration != nullptr) {
        delete _registration;
        _registration = nullptr;
    }

    delete _checkcalibration;
    delete _defectmarker;
    delete _channelsController;

    emit stopDefcore();

    if (_defcore->isRunning()) {
        if (!_defcore->wait(8000)) {
            qDebug() << "Unable to stop _defcore! Terminating...";
            _defcore->terminate();
            _defcore->wait(8000);
        }
    }

    _defcoreThread->quit();

    if (_defcoreThread->isRunning()) {
        if (!_defcoreThread->wait(8000)) {
            qDebug() << "Unable to stop _defcoreThread! Terminating...";
            _defcoreThread->terminate();
            _defcoreThread->wait(8000);
        }
    }

    delete _alarmTone;

    Q_ASSERT(_fileManagerThread);
    Q_ASSERT(_fileManager);
    _fileManager->cancel();
    while (_fileManager->isCopying()) {
        QThread::msleep(100);
    }
    disconnect(_fileManagerThread, &QThread::started, _fileManager, &FileManager::started);

    _fileManagerThread->quit();
    _fileManagerThread->wait(8000);
    if (_fileManagerThread->isRunning()) {
        _fileManagerThread->terminate();
        _fileManagerThread->wait(8000);
    }
    if (_fileManagerThread->isRunning() == false) {
        qDebug() << "File manager stopped!";
    }
    else {
        qDebug() << "Failed to stop filemanager!";
    }

    qDebug() << "Finit finished!";
}

void Core::onSchemeChanged(int index)
{
    // stopRegistration();
    Q_ASSERT(_channelsController);
    _channelsController->recreateModels();
    emit doModelsReady();

    updateChannelsTemperatureMap();
    Q_ASSERT(_alarmTone);
    _alarmTone->setModel(_channelsController->channelsModel());

    emit doSchemeChanged(index);
    // startRegistration();
}

void Core::audioPlayerStateChanged(QMediaPlayer::State newState)
{
    Q_ASSERT(_alarmTone);
    if (newState != QMediaPlayer::PlayingState) {
        _alarmTone->resume();
    }
}

void Core::onInternalTemperatureChanged(qreal value)
{
    if (_screenHeaterStatus && (_screenHeaterTargetTemperature > value)) {
        if (!_screenHeaterCurrentState) {
            changeScreenHeaterState(true);
        }
    }
    else {
        if (_screenHeaterCurrentState) {
            changeScreenHeaterState(false);
        }
    }
}

void Core::onExternalTemperatureChanged(qreal value)
{
    _lastTemperature = value;
}

void Core::onTimerForCheckCalibrationTimeOut()
{
    if (restoreExternalTemperatureEnabled()) {
        if (_countsOfNotes == 0) {
            _registration->addRecalibrationLabel();
            _timerForCheckCalibrationTemperature->stop();
            return;
        }

        QString message;
        int count = 0;
        for (ItemOfMap item : _channelsTemperatureMap) {
            if ((item.temperature != cTemperatureValueUnknown) && (abs(item.temperature - _lastTemperature) >= MIN_TEMPERATURE_DIFFERENCE)) {
                ++count;
                if (message.isEmpty()) {
                    message.append(QString(tr("Current temperature: ")) + QString::number(_lastTemperature) + QString(" ℃\n") + QString(tr("Recalibrate:\n")));
                }
                if (count <= 3) {
                    message.append(QString(" ") + QString(item.channelNote) + QString("(") + QString::number(item.temperature) + QString(" ℃)\n"));
                }
                continue;
            }
        }
        if (!message.isEmpty()) {
            if (count > 3) {
                message.append(QString(tr(" and another ")) + QString::number(count - 3) + QString(tr(" channels!")));
            }
            Notifier::instance().addNote(Qt::red, QString(message));
            qDebug() << message;
            --_countsOfNotes;
        }
    }
}

void Core::onTimerForCheckSocTemperatureTimeout()
{
    if (_lastSocTemperature >= CRITICAL_SOC_TEMPERATURE_DEGREES) {
        if (_isScreenBrightnessDecrease == false) {
            int currentBrightness = restoreBrightness();
            if (currentBrightness > MINIMAL_BRIGHTNESS) {
                emit doChangeBrightnessOverheat(MINIMAL_BRIGHTNESS);
                Notifier::instance().addNote(Qt::red, tr("Forced decrease in brightness!"));
            }
            _isScreenBrightnessDecrease = true;
            _timerForCheckSocTemperature->start(INTERVAL_FOR_CHECK_SOC_TEMPERATURE_AFTER_DECREASE_BRIGHTNESS_MS);
            _isCduOverheated = true;
        }
        else {
            _timerForCheckSocTemperature->stop();
            emit doPowerOffOverheat();
        }
    }
}

void Core::onTimerForAcNotificationTimeOut()
{
    voiceNotifyAC();
    _timerForAcNotification->stop();
}

void Core::onChangeCalibration()
{
    updateChannelsTemperatureMap();
}

void Core::onUmuConnectionStatusChanged(bool isConnected)
{
    _isUmuConnected = isConnected;
}

void Core::onDefcoreDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value)
{
    Q_UNUSED(groupIndex);
    Q_UNUSED(side);
    Q_UNUSED(channel);
    Q_UNUSED(gateIndex);
    switch (id) {
    case vidChangeSensBySpeed:
        _registration->addChangingSensBySpeed(value);
        break;
    default:
        break;
    }
}

void Core::onTempSensorFailure(int id)
{
    qDebug() << "Temperature sensor failure:" << id;
    switch (id) {
    case 0:
        Notifier::instance().addNote(Qt::red, QString(tr("Error - internal temperature sensor failed!")));
        break;
    case 1:
        Notifier::instance().addNote(Qt::red, QString(tr("Error - external temperature sensor failed!")));
        break;
    }
}

void Core::onRemoteControlFinished()
{
    qDebug() << "Remote control is stopped!";
}

void Core::onTemperatureSocChanged(double value)
{
    _lastSocTemperature = value;
    if (value >= CRITICAL_SOC_TEMPERATURE_DEGREES) {
        if (_timerForCheckSocTemperature->isActive() == false) {
            _timerForCheckSocTemperature->start(INTERVAL_FOR_CHECK_SOC_TEMPERATURE_MS);
            Notifier::instance().addNote(Qt::red, tr("Attention! Overheating CDU!"));
            qDebug() << _timerForCheckSocTemperature->interval() << _timerForCheckSocTemperature->isActive();
        }
    }
    else {
        _timerForCheckSocTemperature->stop();
        _isScreenBrightnessDecrease = false;
        _isCduOverheated = false;
    }
}

FileManager* Core::getFileManager() const
{
    return _fileManager;
}

void Core::setDynamicRailType(bool isEnabled)
{
    emit doSetDynamicRailType(isEnabled);
}

bool Core::isCduOverheated()
{
    return _isCduOverheated;
}

int Core::getScreenHeaterTargetTemperature() const
{
    return _screenHeaterTargetTemperature;
}

void Core::setScreenHeaterTargetTemperature(int screenHeaterTargetTemperature)
{
    _screenHeaterTargetTemperature = screenHeaterTargetTemperature;
}

bool Core::getScreenHeaterStatus() const
{
    return _screenHeaterStatus;
}

void Core::setScreenHeaterStatus(bool screenHeaterStatus)
{
    _screenHeaterStatus = screenHeaterStatus;
}

unsigned short Core::getColorForChannel(eBScanColorDescr key)
{
    return _colorSchemeManager->getCurrentScheme().getColorForKey(key);
}

unsigned short Core::getIndividualColorForChannel(CID key)
{
    return _colorSchemeManager->getCurrentScheme().getIndividualColorForKey(key);
}

unsigned short Core::getCurrentForwardColor()
{
    return _colorSchemeManager->getCurrentScheme().getForwardColor();
}

unsigned short Core::getCurrentBackwardColor()
{
    return _colorSchemeManager->getCurrentScheme().getBackwardColor();
}

std::vector<ColorScheme> Core::getColorSchemes()
{
    return _colorSchemeManager->getSchemes();
}

void Core::switchCurrentColorScheme(int index)
{
    _colorSchemeManager->switchCurrentScheme(index);
    emit doChangeColorScheme();
}

PasswordManager* Core::getPasswordManager() const
{
    Q_ASSERT(_passwordManager);
    return _passwordManager;
}

int Core::getExternalTemperature()
{
    return _lastTemperature;
}

void Core::resetTemperatureCache()
{
    emit pollTempSensor();
}

void Core::updateRemoteControlMeter()
{
    emit doSendMeter(static_cast<TMRussian*>(_trackMarks)->getM());
}

std::set<DevicePermissions> Core::getCurrentPermissions() const
{
    return _currentPermissions;
}

void Core::setPermission(DevicePermissions permission, bool isActive)
{
    auto it = _currentPermissions.find(permission);
    if (isActive) {
        if (it == _currentPermissions.end()) {
            _currentPermissions.insert(permission);
            emit permissionsChanged();
        }
    }
    else {
        if (it != _currentPermissions.end()) {
            _currentPermissions.erase(it);
            emit permissionsChanged();
        }
    }
}

BluetoothManager* Core::getBtManager() const
{
    Q_ASSERT(_btManager);
    return _btManager;
}

EK_ASUI* Core::getEkasui()
{
    if (_ekasui == nullptr) {
        qDebug() << "Initializing EKASUI...";
        initEKASUI();
        qDebug() << "EKASUI ready!";
    }
    return _ekasui;
}

void Core::voiceNotifyAC()
{
    playSound(ACNotify);
}

TrackMarks* Core::getTrackMarks()
{
    if (_trackMarks == nullptr) {
        switch (getSystemCoordType()) {
        case csMetricRF:
            _trackMarks = new TMRussian();
            break;
        case csMetric1km:
            _trackMarks = new TMMetric1KM();
            break;
        case csImperial:
            _trackMarks = nullptr;
            break;
        case csMetric0_1km:
            _trackMarks = nullptr;
            break;
        case csMetric1kmTurkish:
            _trackMarks = nullptr;
            break;
        }
        Q_ASSERT(_trackMarks);

        if (_trackMarks != nullptr) {
            _trackMarks->reset();
            _trackMarks->setDirection(ForwardDirection);
        }
    }
    return _trackMarks;
}

void Core::setTrackMarks(TrackMarks* trackMarks)
{
    Q_ASSERT(trackMarks);
    _trackMarks = trackMarks;
}

void Core::handleNotifications()
{
    if (!_notifications.empty()) {
        if (_trackMarks->checkNotification(_notifications.back(), restoreNotificationDistance())) {
            _registration->addOperatorRemindLabel(_notifications.back().getRail(), 0, _notifications.back().getDef(), 0, 0);
            Notifier::instance().addNote(
                Qt::green,
                tr("Notification") + "\n(" + QString::number(_notifications.back().getKm()) + "," + QString::number(_notifications.back().getPk()) + "," + QString::number(_notifications.back().getMetre()) + ")\n[" + _notifications.back().getDef() + "] " + _notifications.back().getNote() + "");
            _notifications.pop_back();
        }
    }
}

void Core::setNotifications(const std::vector<Notification>& notifications)
{
    _notifications = notifications;
    sortNotifications();
}

void Core::sortNotifications()
{
    if (_currentDirection == 1) {
        std::sort(_notifications.begin(), _notifications.end(), [](const Notification& a, const Notification& b) -> bool {
            if (a.getKm() == b.getKm()) {
                if (a.getPk() == b.getPk()) {
                    if (a.getMetre() == b.getMetre()) {
                        return false;
                    }

                    return a.getMetre() > b.getMetre();
                }

                return a.getPk() > b.getPk();
            }

            return a.getKm() > b.getKm();
        });
    }
    else if (_currentDirection == -1) {
        std::sort(_notifications.begin(), _notifications.end(), [](const Notification& a, const Notification& b) -> bool {
            if (a.getKm() == b.getKm()) {
                if (a.getPk() == b.getPk()) {
                    if (a.getMetre() == b.getMetre()) {
                        return true;
                    }

                    return a.getMetre() < b.getMetre();
                }

                return a.getPk() < b.getPk();
            }

            return a.getKm() < b.getKm();
        });
    }
}

void Core::setCurrentDirection(signed char currentDirection)
{
    _currentDirection = currentDirection;
    sortNotifications();
    Core::instance().getTrackMarks()->setDirection(static_cast<Direction>(currentDirection));
    emit doUpdateRemoteState();
}

void Core::stopAudioOutput()
{
    QDEBUG << "Stopping alarm tone...";
    Q_ASSERT(_alarmTone);
    _alarmTone->stop();
}

void Core::resumeAudioOutput()
{
    QDEBUG << "Resuming alarm tone...";
    Q_ASSERT(_alarmTone);
    _alarmTone->resume();
}

void Core::setWireOutput()
{
    stopAudioOutput();
    QThread::msleep(100);
    qputenv("ALSA_DEVICE", "");
    qDebug() << "ALSA_DEVICE = " << qgetenv("ALSA_DEVICE");
    QThread::msleep(100);
    resumeAudioOutput();
}

void Core::setBtWireOutput()
{
    stopAudioOutput();
    QThread::msleep(100);
    qputenv("ALSA_DEVICE", "both_softvol");
    qDebug() << "ALSA_DEVICE = " << qgetenv("ALSA_DEVICE");
    QThread::msleep(100);
    resumeAudioOutput();
}

FTPUploader* Core::getFtpUploader() const
{
    Q_ASSERT(_ftpUploader);
    return _ftpUploader;
}

eDeviceMode Core::getDeviceMode()
{
    Q_ASSERT(_defcore);
    return _defcore->getDeviceMode();
}

tSensValidRangesList Core::getSensValidRanges()
{
    Q_ASSERT(_defcore);
    return _defcore->getSensValidRanges();
}

eCoordSys Core::getSystemCoordType()
{
    Q_ASSERT(_defcore);
    return _defcore->getSystemCoordType();
}

void Core::setleftSideTrolleyPosition(tTrolleySide trolleySide)
{
    emit doSetleftSideTrolleyPosition(trolleySide);
}

double Core::maxSpeed() const
{
    Q_ASSERT(_defcore);
    return _defcore->maxSpeed();
}

void Core::restoreACValues()
{
    emit restoreAC();
}

void Core::checkExtraLogs()
{
    QDir dir(logsPath());
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    if (fileList.count() > MINIMAL_COUNT_LOGS) {
        for (int i = 0; i < fileList.count() - MINIMAL_COUNT_LOGS; ++i) {
            dir.remove(fileList[i].absoluteFilePath());
        }
    }
}

void Core::startRegistration()
{
    setSearchMode();
    Q_ASSERT(_registration);
    _registration->start();
}

void Core::stopRegistration()
{
    _registration->stop(true);
}

void Core::startTimerForCheckCalibrationTemperature()
{
    Q_ASSERT(_timerForCheckCalibrationTemperature);
    _countsOfNotes = COUNTS_OF_NOTES;
    _timerForCheckCalibrationTemperature->start();
}

void Core::stopTimerForCheckCalibrationTemperature()
{
    Q_ASSERT(_timerForCheckCalibrationTemperature);
    _timerForCheckCalibrationTemperature->stop();
}

void Core::powerSaveMode(bool on)
{
    screenPower(!on);
    //    umuPower(!on);
}

void Core::umuPower(bool value)
{
    GpioOutput gpio(3, !value);
}

void Core::startBoltJoint(int additive)
{
    emit doSetControlMode(cmTestBoltJoint, additive);
    emit doStartBoltJoint();
}

void Core::stopBoltJoint()
{
    emit doSetControlMode(cmNormal, 0);
    emit doStopBoltJoint();
}

void Core::pauseModeOn()
{
    emit doPauseModeOn();
}

void Core::pauseModeOff()
{
    emit doPauseModeOff();
}

void Core::calibrationToRailType()
{
    emit doCalibrationToRailType();
}

QStringList Core::calibrationNamesList()
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->calibrationNames();
}

void Core::setCalibrationIndex(int index)
{
    emit doSetCalibrationIndex(index);
}

void Core::deleteCalibration(int index)
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->deleteCalibration(index);
}

void Core::setCalibrationName(const QString& name, int index)
{
    Q_ASSERT(_defcore != nullptr);
    _defcore->setCalibrationName(name, index);
}

int Core::currentCalibrationIndex()
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->calibrationIndex();
}

int Core::currentRealCalibrationIndex(int index)
{
    return _defcore->realCalibrationIndex(index);
}

void Core::setScreenShootKey(eDeviceSide side, CID channelId, int gateIndex, unsigned int key)
{
    Q_ASSERT(_defcore != nullptr);
    _defcore->setScreenShootKey(side, channelId, gateIndex, key);
}

unsigned int Core::getScreenShootKey(eDeviceSide side, CID channelId, int gateIndex)
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->getScreenShootKey(side, channelId, gateIndex);
}

void Core::setCalibrationTemperature(eDeviceSide side, CID channelId)
{
    Q_ASSERT(_defcore != nullptr);

    QModelIndex currentModelIndex = _channelsController->currentModelIndex();
    if (currentModelIndex.isValid()) {
        auto* model = const_cast<QAbstractItemModel*>(currentModelIndex.model());
        Q_ASSERT(model != nullptr);
        model->setData(currentModelIndex, _lastTemperature, CalibrationTemperatureRole);
        _defcore->setCalibrationTemperature(side, channelId, _lastTemperature);
        updateChannelsTemperatureMap();
    }
}

int Core::getCalibrationTemperature(eDeviceSide side, CID channelId)
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->getCalibrationTemperature(side, channelId);
}

bool Core::calibrateChannel()
{
    bool rc = false;
    Q_ASSERT(_defcore != nullptr);

    QModelIndex currentModelIndex = _channelsController->currentModelIndex();

    if (currentModelIndex.isValid()) {
        auto* model = const_cast<QAbstractItemModel*>(currentModelIndex.model());
        Q_ASSERT(model != nullptr);
        qDebug() << QString("Info| Calibration| Calibrate channel id:0x") + QString::number(model->data(currentModelIndex, ChannelIdRole).toInt(), 16) + " angle:" + QString::number(model->data(currentModelIndex, AngleRole).toInt()) + QString("°");
        int sens = 0;
        rc = _defcore->calibrateChannel(sens);
        if (rc) {
            model->setData(currentModelIndex, sens, SensRole);
        }
        else {
            qDebug() << QString("Calibration| Calibration of channel failed.");
        }
    }
    else {
        qDebug() << QString("Calibration| Model index is not valid");
    }

    return rc;
}

void Core::setBScanDisplayThresholdChanged(int value)
{
    emit doBScanDisplayThresholdChanged(value);
}

void Core::resetSens()
{
    emit doResetSens();
}

void Core::calibrationType1()
{
    emit doCalibrationType1();
}

void Core::onLanguageChanged()
{
    emit doChangeLanguage();
}

void Core::onTrackMarksSelected()
{
    int km = static_cast<TMRussian*>(_trackMarks)->getKm();
    int pk = static_cast<TMRussian*>(_trackMarks)->getPk();
    int m = static_cast<TMRussian*>(_trackMarks)->getM();
    emit doCurrentTrackMarks(km, pk, m);
}

void Core::onCurrentTrackMark(int km, int pk)
{
    static_cast<TMRussian*>(_trackMarks)->setKm(km);
    static_cast<TMRussian*>(_trackMarks)->setPk(pk);
    static_cast<TMRussian*>(_trackMarks)->updatePost();
    static_cast<TMRussian*>(_trackMarks)->resetMeter();
    Q_ASSERT(_registration);
    _registration->setMrfPost(static_cast<TMRussian*>(_trackMarks));
    emit doTmSelectedFromRemoteControl();
}

void Core::onACNotificationEnabled(bool value)
{
    _acNotificationEnabled = value;
}

void Core::onSatellitesInUse(int countSatellites)
{
    _registration->setSatelliteInUseCount(countSatellites);
    emit doSatellitesInUse(countSatellites);
}

void Core::onSatellitesInfo(const QGeoPositionInfo& info)
{
    _registration->setGeoPositionInfo(info);
    emit doSatellitesInfo(info);
}

void Core::onRemoteControlConnected()
{
    _registration->setAntennaStatus(GeoPosition::AntennaStatusConnected);
    emit doRemoteControlConnected();
}

void Core::onRemoteControlDisconnected()
{
    _registration->setAntennaStatus(GeoPosition::AntennaStatusDisconnected);
    emit doRemoteControlDisconnected();
}

void Core::onRemoteControlPingFailed()
{
    Notifier::instance().addNote(Qt::red, QString(tr("Loss of communication with the remote control!")));
}

void Core::onStartSwitchLabel()
{
    _registration->addStartSwitch();
    _defectmarker->setSwitchShunterState(true);
}

void Core::onEndSwitchLabel()
{
    _registration->addEndSwitch();
    _defectmarker->setSwitchShunterState(false);
}

void Core::onTextLabel(QString& textLabel)
{
    _registration->addTextLabel(textLabel);
}

void Core::calibrationType2()
{
    emit doCalibrationType2();
}

void Core::autoPrismDelayCalibration()
{
    emit doAutoPrismDelayCalibration();
}

void Core::saveGainForControlSectionOfTrack(int scheme)
{
    QMap<int, int> mapOfGain;
    QAbstractItemModel* model = Core::instance().channelsController()->scanChannelsModel();
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        mapOfGain.insert(createKeyFromSchemeAndSideAndChannelAndGateIndex(scheme, model->data(index, SideRole).toInt(), model->data(index, ChannelIdRole).toInt(), model->data(index, GateIndexRole).toInt()), model->data(index, GainRole).toInt());
    }
    saveGainForControlSection(mapOfGain);
}

LimitsForSens& Core::limitsForSens()
{
    Q_ASSERT(_limitsForSens);
    return (*_limitsForSens);
}

QString Core::getCurrentCalibrationName()
{
    Q_ASSERT(_defcore != nullptr);
    return _defcore->getCurrentCalibrationName();
}

void Core::setUmuLineToCompleteControl()
{
    Q_ASSERT(_alarmTone);
    _alarmTone->setSilentMode(false);
    emit doSetUmuLineToCompleteControl();
}

void Core::setUmuLineToScannerControl()
{
    Q_ASSERT(_channelsController);
    Q_ASSERT(_alarmTone);
    _channelsController->setDataForAllItems(false, IsAlarmedRole);
    _alarmTone->setSilentMode(true);

    emit doSetUmuLineToScannerControl();
}

void Core::testSensLevels()
{
    emit doTestSensLevels();
}

void Core::playSound(SystemSounds sound)
{
    Q_ASSERT(_alarmTone);
    qDebug() << "Playing sound #" << static_cast<int>(sound);
    emit doPlaySound(sound);
}

bool Core::isUmuConnected()
{
    return _isUmuConnected;
}

void Core::updateChannelsTemperatureMap()
{
    _channelsTemperatureMap.clear();
    QAbstractItemModel* model = _channelsController->scanChannelsModel();
    int rowCount = model->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex channelIndex = model->index(i, 0);
        if (channelIndex.isValid()) {
            int channelId = model->data(channelIndex, ChannelIdRole).toInt();
            DeviceSide side = static_cast<DeviceSide>(model->data(channelIndex, SideRole).toInt());
            int calibrationTemperature = model->data(channelIndex, CalibrationTemperatureRole).toInt();
            int angle = model->data(channelIndex, AngleRole).toInt();
            int key = createKeyFromSideAndChannel(side, channelId);
            Direction direction = static_cast<Direction>(model->data(channelIndex, DirectionRole).toInt());
            eControlZone currentZone = static_cast<eControlZone>(model->data(channelIndex, ZoneRole).toInt());
            if (!_channelsTemperatureMap.contains(key)) {
                ItemOfMap element;
                QString channelNote("");
                switch (side) {
                case LeftDeviceSide:
                    channelNote += tr("left ");
                    break;
                case RightDeviceSide:
                    channelNote += tr("right ");
                    break;
                default:
                    break;
                }

                channelNote += QString::number(angle) + QString(0x00B0) + QString(" ");
                switch (currentZone) {
                case czWeb:
                    channelNote += QObject::tr("W-");
                    break;
                case czBase:
                    channelNote += QObject::tr("B-");
                    break;
                case czHeadAndWork:
                case czHeadLeftWork:
                case czHeadWork:
                    channelNote += QObject::tr("Gauge-");
                    break;
                case czHeadAndUnWork:
                case czHeadLeftUnWork:
                case czHeadUnWork:
                    channelNote += QObject::tr("Field-");
                    break;
                default:
                    break;
                }

                switch (direction) {
                case ForwardDirection:
                    channelNote += QObject::tr("F");
                    break;
                case BackwardDirection:
                    channelNote += QObject::tr("B");
                    break;
                case UnknownDirection:
                    break;
                }
                element.channelNote = channelNote;
                element.temperature = calibrationTemperature;
                _channelsTemperatureMap.insert(key, element);
            }
        }
    }
}

void Core::deviceEnableFiltration(bool isEnable)
{
    emit doDeviceEnableFiltration(isEnable);
}

void Core::deviceSetFilterParams(tBScan2FilterParamId prmId, int value)
{
    emit doSetDeviceFilterParams(prmId, value);
}

void Core::disableAll(bool status)
{
    if (status) {
        emit doDisableAll();
    }
    else {
        emit doEnableAll();
    }
}

void Core::resetPathEncoder()
{
    emit doResetPathEncoder();
}

void Core::setRegistrationThreshold(int value)
{
    Q_ASSERT(_registration);
    _registration->addMinBscanThresholdRec(value);
    emit doSetRegistrationThreshold(value);
}

void Core::enableImportantAreaIndication(bool isEnable)
{
    Q_ASSERT(_defectmarker);
    _defectmarker->enableImportantAreaIndication(isEnable);
}

void Core::setImportantAreaMinPacketSize(int value)
{
    Q_ASSERT(_defectmarker);
    _defectmarker->setMinPacketSize(value);
    deviceSetFilterParams(fprmMinPacketSizeForEvent, value);
}

void Core::setImportantAreaMinSignals0dB(int value)
{
    Q_ASSERT(_defectmarker);
    _defectmarker->setMinSignals0dB(value);
    deviceSetFilterParams(frmmMinSignals0dBInNormModeForEvent, value);
}

void Core::setImportantAreaMinSpacingSize(int value)
{
    Q_ASSERT(_defectmarker);
    _defectmarker->setMinZTMSpacing(value);
    deviceSetFilterParams(frmmMinSpacing0dBInNormModeForEvent, value);
}

void Core::setEvalCalibrMinZTMSpacingSize(int value)
{
    Q_ASSERT(_checkcalibration);
    _checkcalibration->setEvalCalibrMinZTMSpacingSize(value);
    deviceSetFilterParams(fprmMinSpacingMinus6dBInBoltTestModeForEvent, restoreEvalCalibrMinZTMSpacingSize());
}

void Core::setMinKdEchoPrm(int value)
{
    Q_ASSERT(_checkcalibration);
    _checkcalibration->setMinKdEchoPrm(value);
}

void Core::SetFixBadSensState(bool State)
{
    Q_ASSERT(_defcore);
    _defcore->SetFixBadSensState(State);
}

void Core::SetTimeToFixBadSens(int TimeToFixBadSens)
{
    Q_ASSERT(_defcore);
    _defcore->SetTimeToFixBadSens(TimeToFixBadSens);
}

void Core::updateRemoteState(bool isRegOn, ViewCoordinate typeView)
{
    TrackMarks* trackMarks = Core::instance().getTrackMarks();
    Direction direction = trackMarks->getDirection();
    int km = static_cast<TMRussian*>(trackMarks)->getKm();
    int pk = static_cast<TMRussian*>(trackMarks)->getPk();
    int m = static_cast<TMRussian*>(trackMarks)->getM();
    emit doUpdateRemoteStateParams(isRegOn, typeView, direction, km, pk, m);
}

void Core::updateRemoteMarks()
{
    emit doUpdateRemoteMarks();
}

void Core::listenRemoteControl()
{
    emit doListen();
}

void Core::setACNotifyState(int type, bool state)
{
    if (_acNotificationEnabled) {
        if (!state) {
            if (_acNotifyStates[0] && _acNotifyStates[1]) {
                _timerForAcNotification->start(1000);
            }
        }
        else {
            _timerForAcNotification->stop();
        }
        _acNotifyStates[type] = state;
    }
}

QString Core::getOSVersion()
{
    QFile versionFile("/etc/os-version");
    QString version;
    if (versionFile.exists()) {
        if (versionFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&versionFile);
            version = in.readLine();
        }
        else {
            version = tr("Unknown version");
        }
    }
    else {
        version = "1.0";
    }
    versionFile.close();
    return version;
}

void Core::videoRegistrationStart(QString name)
{
    emit doVideoRegistrationStart(name);
}

void Core::videoRegistrationStop()
{
    emit doVideoRegistrationStop();
}

void Core::videoModeView()
{
    emit doVideoModeView();
}

void Core::videoModeRealtime()
{
    emit doVideoModeRealtime();
}

void Core::videoSetCoord(unsigned int coord)
{
    emit doVideoSetCoord(coord);
}

void Core::usePathEncoderEmulator(bool value)
{
    emit doUsePathEncoderEmulator(value);
}

void Core::initTemperatureManager()
{
    _temperatureSensorManagerThread->setObjectName("temperatureThread");
    _temperatureSensorManager->moveToThread(_temperatureSensorManagerThread);
    ASSERT(connect(this, &Core::doStartTempSensorManager, _temperatureSensorManager, &TemperatureSensorManager::init, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &Core::doStopTempSensorManager, _temperatureSensorManager, &TemperatureSensorManager::stop, Qt::BlockingQueuedConnection));
    _temperatureSensorManagerThread->start();
    emit doStartTempSensorManager();
}

int Core::openFile(QString& dir, QString& fileName)
{
    _filename = fileName;
    dir += fileName;
    resetCurrentTrackMark();
    disconnect(_defcore, &Defcore::doBScan2Data, this, &Core::doBScan2Data);
    registration().stop(false);
    registration().start();
    int res = registration().openFile(dir);
    if (res == 0) {
        emit doSetBScanMode(true);
        return 0;
    }
    else {
        closeFile();
        return res;
    }
}

QString Core::getFileName()
{
    return _filename;
}

void Core::closeFile()
{
    resetCurrentTrackMark();
    resetCurrentTapeModel();
    connect(_defcore, &Defcore::doBScan2Data, this, &Core::doBScan2Data);
    _filename = "";
    registration().closeFile();
    registration().stop(false);
    emit doSetBScanMode(false);
    registration().start();
}

void Core::setCurrentTrackMark(int disCoord, int sysCoord)
{
    int km = 0;
    int pk = 0;
    int m = 0;
    int direct = 1;
    registration().distanceCalculate(km, pk, m, disCoord, sysCoord, direct);
    if (direct > 1 || direct < -1) {
        direct = 0;
    }
    static_cast<TMRussian*>(_trackMarks)->setKm(km);
    static_cast<TMRussian*>(_trackMarks)->setPk(pk);
    static_cast<TMRussian*>(_trackMarks)->setM(m);
    static_cast<TMRussian*>(_trackMarks)->setDirection(static_cast<Direction>(direct));
    emit doUpdateViewedMark();
}

void Core::resetCurrentTrackMark()
{
    static_cast<TMRussian*>(_trackMarks)->setKm(0);
    static_cast<TMRussian*>(_trackMarks)->setPk(0);
    static_cast<TMRussian*>(_trackMarks)->setM(0);
    emit doUpdateViewedMark();
}

void Core::setCurrentTapeModel(int disCoord)
{
    registration().delegateSensToModel(disCoord, _kuSens, _stStrSens, _endStrSens);
    _channelsController->onViewModeCalibrationChanged();
}

void Core::resetCurrentTapeModel()
{
    _channelsController->onCalibrationChanged();
}

int Core::getKuSens(int side, int cid)
{
    int channel = registration().CIDToChannel(side, cid);
    return _kuSens.at(side).at(channel);
}

int Core::getstStrSens(int side, int cid)
{
    int channel = registration().CIDToChannel(side, cid);
    return _stStrSens.at(side).at(channel);
}

int Core::getendStrSens(int side, int cid)
{
    int channel = registration().CIDToChannel(side, cid);
    return _endStrSens.at(side).at(channel);
}

void Core::useAutoGainAdjustment(bool value)
{
    emit doUseAutoGainAdjustment(value);
}

void Core::enableAC(bool isEnabled)
{
    emit doEnableAc(isEnabled);
}

#ifdef ANDROID
void Core::getUMUBatteryVoltage(void)
{
    emit doBatteryVoltageQury();
}

void Core::getUMUBatteryPercent(void)
{
    emit doBatteryPerecentQuery();
}
#endif
