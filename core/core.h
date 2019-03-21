#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>
#include <QMediaPlayer>
#include <QPointer>
#include <QFileInfo>
#include <QMap>
#include <QTimer>

#include "modeltypes.h"
#include "coredefinitions.h"
#include "Definitions.h"
#include "enums.h"

#include <set>
#include <vector>
#include <array>

class Defcore;
class ChannelsController;
class AlarmTone;
class TemperatureSensorManager;
class Registration;
class EK_ASUI;
class BluetoothManager;
class PasswordManager;
class FTPUploader;
class CheckCalibration;
class LimitsForSens;
class DefectMarker;
class TrackMarks;
class GpioOutput;
class RemoteControl;
class ColorSchemeManager;
class QGeoPositionInfo;
class VideoPlayerController;
class ColorScheme;
class FileManager;
class Notification;
class tDEV_AScanMeasure;
class tDEV_AScanHead;
class tDEV_AlarmHead;
class tDEV_BScan2Head;
class tDEV_PathStepData;

inline int getKeyByCidSideAndGateIndex(int cid, int side, int gateIndex)
{
    return ((gateIndex & 0xff) << 16) | ((side & 0xff) << 8) | (cid & 0xff);
}

class Core : public QObject
{
    Q_OBJECT

    struct ItemOfMap
    {
        ItemOfMap()
            : temperature(cTemperatureValueUnknown)
            , channelNote("")
        {
        }
        int temperature;
        QString channelNote;
    };

public:
    static Core& instance();
    void start();

    ChannelsController* channelsController();
    AlarmTone* alarmTone();
    Registration& registration();
    CheckCalibration& checkcalibration();
    DefectMarker& defectmarker();

    void setCalibrationMode(const QString& newCalibrationName);
    void setCalibrationMode(eCalibrationMode mode);
    void changeCalibrationMode(eCalibrationMode mode);
    void setRecalibrationMode();
    void setSearchMode();

    static int evaluationGateLevel();
    static int dbToAmp(int db);

    void setSchemeIndex(int index);
    int currentSchemeIndex() const;
    int schemesCount() const;

    QString deviceSerialNumber() const;
    QString deviceFirmwareVersion() const;
    QString devicePldVersion() const;

    QStringList deviceSerialNumbers() const;
    QStringList deviceFirmwareVersions() const;
    QStringList devicePldVersions() const;

    static void powerSaveMode(bool on = true);
    static void umuPower(bool on = true);

    void startBoltJoint(int additive);
    void stopBoltJoint();

    void pauseModeOn();
    void pauseModeOff();

    void calibrationToRailType();

    QStringList calibrationNamesList();
    void setCalibrationIndex(int index = 0);
    void deleteCalibration(int index = 0);
    void setCalibrationName(const QString& name, int index);
    int currentCalibrationIndex();
    int currentRealCalibrationIndex(int index);
    void setScreenShootKey(eDeviceSide side, CID channelId, int gateIndex, unsigned int key);
    unsigned int getScreenShootKey(eDeviceSide side, CID channelId, int gateIndex);
    void setCalibrationTemperature(eDeviceSide side, CID channelId);
    int getCalibrationTemperature(eDeviceSide side, CID channelId);
    bool calibrateChannel();
    void setBScanDisplayThresholdChanged(int value);
    void resetSens();
    void calibrationType2();
    void autoPrismDelayCalibration();
    void saveGainForControlSectionOfTrack(int scheme);
    LimitsForSens& limitsForSens();
    QString getCurrentCalibrationName();


    void deviceEnableFiltration(bool isEnable);
    void deviceSetFilterParams(tBScan2FilterParamId prmId, int value);

    void disableAll(bool status);
    void resetPathEncoder();

    void setUmuLineToCompleteControl();
    void setUmuLineToScannerControl();
    void testSensLevels();
    void setRegistrationThreshold(int value);
    void enableImportantAreaIndication(bool isEnable);
    void setImportantAreaMinPacketSize(int value);
    void setImportantAreaMinSignals0dB(int value);
    void setImportantAreaMinSpacingSize(int value);

    void setEvalCalibrMinZTMSpacingSize(int value);
    void setMinKdEchoPrm(int value);
    void startTimerForCheckCalibrationTemperature();
    void stopTimerForCheckCalibrationTemperature();

    FTPUploader* getFtpUploader() const;

    eDeviceMode getDeviceMode();
    tSensValidRangesList getSensValidRanges();

    eCoordSys getSystemCoordType();

    void setleftSideTrolleyPosition(tTrolleySide trolleySide);
    double maxSpeed() const;

    void restoreACValues();
    void checkExtraLogs();

#ifdef ANDROID
    void getUMUBatteryVoltage(void);
    void getUMUBatteryPercent(void);
#endif

    void SetFixBadSensState(bool State);
    void SetTimeToFixBadSens(int TimeToFixBadSens);

    void updateRemoteState(bool isRegOn, ViewCoordinate typeView);
    void updateRemoteMarks();
    void listenRemoteControl();
    void setACNotifyState(int type, bool state);
    QString getOSVersion();

    void videoRegistrationStart(QString name);
    void videoRegistrationStop();
    void videoModeView();
    void videoModeRealtime();
    void videoSetCoord(unsigned int coord);

    void usePathEncoderEmulator(bool value);
    void initTemperatureManager();
    void useAutoGainAdjustment(bool value);

    int openFile(QString& dir, QString& fileName);
    QString getFileName();
    void closeFile();
    void setCurrentTrackMark(int disCoord, int sysCoord);
    void resetCurrentTrackMark();
    void setCurrentTapeModel(int disCoord);
    void resetCurrentTapeModel();
    int getKuSens(int side, int cid);
    int getstStrSens(int side, int cid);
    int getendStrSens(int side, int cid);

public slots:
    void calibrationType1();
    void onLanguageChanged();

    // Remote control slots
    void onTrackMarksSelected();
    void onCurrentTrackMark(int km, int pk);
    void onACNotificationEnabled(bool value);
    void onSatellitesInUse(int countSatellites);
    void onSatellitesInfo(const QGeoPositionInfo& info);
    void onRemoteControlConnected();
    void onRemoteControlDisconnected();
    void onRemoteControlPingFailed();
    void onStartSwitchLabel();
    void onEndSwitchLabel();
    void onTextLabel(QString& textLabel);

    void registrationOn(QString operatorName, QString railroadPathName, int pathNumber, TrainingEnums::Direction direction, TrackMarks* trackMarks);
    void registrationOff();
    void setCurrentTrackMarks(int km, int pk);
    void setRailroadSwitch(int number);
    void setDefect(QString defectCode, TrainingEnums::RailroadSide side);
    void boltJointOn();
    void boltJointOff();
    void setCduMode(TrainingEnums::CduMode mode);

signals:
    void doAScanMeas(QSharedPointer<tDEV_AScanMeasure>);
    void doAScanData(QSharedPointer<tDEV_AScanHead>, QSharedPointer<tUMU_AScanData>);
    void doTVGData(QSharedPointer<tDEV_AScanHead>, QSharedPointer<tUMU_AScanData>);
    void doAlarmData(QSharedPointer<tDEV_AlarmHead>);
    void doBScan2Data(QSharedPointer<tDEV_BScan2Head>);
    void doMScan2Data(QSharedPointer<tDEV_BScan2Head>);
    void doPathStepData(QSharedPointer<tDEV_PathStepData>);
    void doBottomSignalAmpl(QSharedPointer<int>);
    void doDeviceSpeed(unsigned int);

    void doUpdateViewedMark();
    void doSetBScanMode(bool value);
    void doBScanDisplayThresholdChanged(int value);

    void doSetCalibrationMode(eCalibrationMode mode);
    void doChangeCalibrationMode(eCalibrationMode mode);
    void doSetCalibrationIndex(int index);
    void doSetSearchMode();

    void doStartBoltJoint();
    void doStopBoltJoint();
    void doPauseModeOn();
    void doPauseModeOff();
    void doCalibrationToRailType();
    void doSetMrfPost(const TrackMarks& trackMarks);
    void doResetSens();
    void doCalibrationType1();
    void doCalibrationType2();
    void doAutoPrismDelayCalibration();
    void doSetControlMode(eDeviceControlMode mode, int additive);

    void doSetSchemeIndex(int index);
    void doSchemeChanged(int index);
    void doInited();

    void doConnectionStatusDisconnected();
    void doConnectionStatusConnecting();
    void doConnectionStatusConnected();

    void doChangeLanguage();
    void doTestSensLevels();

    void doSetUmuLineToCompleteControl();
    void doSetUmuLineToScannerControl();

    void doSetRegistrationThreshold(int value);

    void doSetDeviceFilterParams(tBScan2FilterParamId prmId, int value);
    void doDeviceEnableFiltration(bool isEnable);

    void doModelsReady();

    void SetAv17CoordX(int Coord);  // Set coord for HeadScanner Av17
    void SetAv17CoordY(int Coord);  // Set coord for HeadScanner Av17
    void SetConfigAv31();
    void SetConfigAv17();
    void doEnableSMChSensAutoCalibration(bool isEnable);

    void internalTemperatureChanged(qreal value);
    void externalTemperatureChanged(qreal value);

    void accessLevelChanged(AccessLevel level);


    void doSetleftSideTrolleyPosition(tTrolleySide trolleySide);

    void doDisableAll();
    void doEnableAll();
    void doResetPathEncoder();

    void doUmuConnectionStatusChanged(bool isConnected);
    void doEnableAc(bool isEnabled);
    void stopDefcore();
    void permissionsChanged();
    void restoreAC();
    void doCreateNewCalibration(QString newCalibrationName);
    void pollTempSensor();
    void doStartTempSensorManager();
    void doStopTempSensorManager();

    // Remote control signals
    void doRcConnected();
    void doRcDisconnected();
    void doTrainingPcConnected();
    void doTrainingPcDisconnected();
    void doBoltJointButtonPressed();
    void doBoltJointButtonReleased();
    void doTrackMarksButtonReleased();
    void doServiceMarksButtonReleased();

    void doRegistrationOn(QString operatorName, QString railroadPathName, int pathNumber, TrainingEnums::Direction direction, int km, int pk, int m);
    void doRegistrationOff();
    void doSetCurrentTrackMarks(int km, int pk);
    void doSetRailroadSwitch(int number);
    void doSetDefect(QString defectCode, TrainingEnums::RailroadSide side);
    void doBoltJointOn();
    void doBoltJointOff();
    void doSetCduMode(TrainingEnums::CduMode mode);


    void doRemoteControlConnected();
    void doRemoteControlDisconnected();
    void doSendMeter(int meter);
    void doStartRegistration();
    void doStopRegistration();
    void doUpdateRemoteStateParams(bool isRegOn, ViewCoordinate viewType, Direction direction, int km, int pk, int m);
    void doUpdateRemoteState();
    void doUpdateRemoteMarks();
    void doChangedSpeed(double speed);
    void doCurrentTrackMarks(int km, int pk, int m);
    void doTmSelectedFromRemoteControl();
    void doListen();
    void doSatellitesInUse(int countSatellites);
    void doSatellitesInfo(const QGeoPositionInfo& info);

    void doChangeColorScheme();
    void doPlaySound(SystemSounds sound);
    void heaterStateChanged(bool _isOn);

    void doVideoRegistrationStart(QString name);
    void doVideoRegistrationStop();
    void doVideoModeView();
    void doVideoModeRealtime();
    void doVideoSetCoord(unsigned int coord);

#ifdef ANDROID
    void doBatteryVoltageQury(void);
    void doBatteryPerecentQuery(void);

    void doBatteryVoltage(double);
    void doBatteryChargePercent(double);
#endif

    void doUsePathEncoderEmulator(bool value);
    void doUseAutoGainAdjustment(bool value);

    void startVideoController();
    void stopVideoController();

    void doSetDynamicRailType(bool isEnabled);
    void doPowerOffOverheat();
    void doChangeBrightnessOverheat(qreal value);

protected:
    explicit Core(QObject* parent = nullptr);
    explicit Core(const Core& other);
    Core& operator=(const Core& object);

public:
    virtual ~Core();

    void setCurrentDirection(signed char currentDirection);

    void stopAudioOutput();
    void resumeAudioOutput();
    void setWireOutput();
    void setBtWireOutput();

    TrackMarks* getTrackMarks();
    void setTrackMarks(TrackMarks* trackMarks);
    void handleNotifications();
    void setNotifications(const std::vector<Notification>& notifications);
    void sortNotifications();

    EK_ASUI* getEkasui();

    BluetoothManager* getBtManager() const;
    void voiceNotifyAC();
    void playSound(SystemSounds sound);
    bool isUmuConnected();
    void enableAC(bool isEnabled);
    QString passFromCDU();
    void finit();

    std::set<DevicePermissions> getCurrentPermissions() const;
    void setPermission(DevicePermissions permission, bool isActive);

    PasswordManager* getPasswordManager() const;
    int getExternalTemperature();
    void resetTemperatureCache();
    void updateRemoteControlMeter();

    unsigned short getColorForChannel(eBScanColorDescr key);
    unsigned short getIndividualColorForChannel(CID key);
    unsigned short getCurrentForwardColor();
    unsigned short getCurrentBackwardColor();
    std::vector<ColorScheme> getColorSchemes();
    void switchCurrentColorScheme(int index);

    bool getScreenHeaterStatus() const;
    void setScreenHeaterStatus(bool screenHeaterStatus);

    int getScreenHeaterTargetTemperature() const;
    void setScreenHeaterTargetTemperature(int screenHeaterTargetTemperature);
    FileManager* getFileManager() const;
    void setDynamicRailType(bool isEnabled);
    bool isCduOverheated();

private:
    void updateChannelsTemperatureMap();
    void connectCoreSignals();
    void disconnectCoreSignals();

    // Remote control methods
    void connectRemoteControlSignals();
    void disConnectRemoteControlSignals();

    void connectVideoControlSignals();
    void disConnectVideoControlSignals();
    void initEKASUI();
    void changeScreenHeaterState(bool state);

private slots:
    void onSchemeChanged(int index);
    void audioPlayerStateChanged(QMediaPlayer::State newState);
    void onInternalTemperatureChanged(qreal value);
    void onExternalTemperatureChanged(qreal value);
    void onTimerForCheckCalibrationTimeOut();
    void onTimerForCheckSocTemperatureTimeout();
    void onTimerForAcNotificationTimeOut();
    void startRegistration();
    void stopRegistration();
    void onChangeCalibration();
    void onUmuConnectionStatusChanged(bool isConnected);
    void onDefcoreDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);
    void onTempSensorFailure(int id);
    void onRemoteControlFinished();
    void onTemperatureSocChanged(double value);

private:
    QThread* _defcoreThread;
    Defcore* _defcore;
    QThread* _rcThread;
    QThread* _videoControllerThread;
    ChannelsController* _channelsController;
    AlarmTone* _alarmTone;
    Registration* _registration;
    LimitsForSens* _limitsForSens;
    int _lastTemperature;
    double _lastSocTemperature;
    int _screenHeaterTargetTemperature;
    CheckCalibration* _checkcalibration;
    DefectMarker* _defectmarker;
    TemperatureSensorManager* _temperatureSensorManager;
    ColorSchemeManager* _colorSchemeManager;
    QThread* _temperatureSensorManagerThread;
    FTPUploader* _ftpUploader;
    QMap<int, ItemOfMap> _channelsTemperatureMap;
    QTimer* _timerForCheckCalibrationTemperature;
    QTimer* _timerForCheckSocTemperature;
    TrackMarks* _trackMarks;
    EK_ASUI* _ekasui;
    BluetoothManager* _btManager;
    unsigned char _countsOfNotes;
    signed char _currentDirection;
    bool _isUmuConnected;
    bool _acNotificationEnabled;
    bool _screenHeaterStatus;
    bool _screenHeaterCurrentState;
    bool _isScreenBrightnessDecrease;
    bool _isCduOverheated;
    std::array<bool, 2> _acNotifyStates;
    QTimer* _timerForAcNotification;
    std::set<DevicePermissions> _currentPermissions;
    std::vector<Notification> _notifications;
    PasswordManager* _passwordManager;
    FileManager* _fileManager;
    QThread* _fileManagerThread;
    RemoteControl* _remoteControl;
    VideoPlayerController* _videoPlayerController;
    DeviceType _deviceType;
    QString _filename;
    std::vector<std::vector<unsigned char>> _kuSens;
    std::vector<std::vector<unsigned char>> _stStrSens;
    std::vector<std::vector<unsigned char>> _endStrSens;
};

#endif  // CORE_H
