#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QMap>
#include "lateralpanels/scanlateralpanel.h"
#include "Device_definitions.h"
#include "modeltypes.h"
#include "coredefinitions.h"


namespace Ui
{
class MainWindow;
}

class Core;
class Av17Page;
class Av17DefView;
class EKASUIOptions;
class WifiManagerPage;
class BluetoothManagerPage;
class TrackMarksPage;
class SensAutoReset;
class RegistrationPage;
class DefectPage;
class ScannerDefectRegistrationPage;
class BoltJointOptions;
class SwitchOperatorWidget;
class WrongCalibrationMsg;
class QStackedLayout;
class ExternalKeyboard;
class QStateMachine;
class QState;
class BScanPlot;
class QuickBar;
class ElectricTestPage;
class WaitMessagePage;
class Report;
class ScreenShotsServiceView;
class LimitsForSensSpinBoxNumber;
class TestExternalKeyboardWidget;
class BatteryOptions;
class ScreenHeaterOptions;
class ExternalTemperatureOptions;
class NotificationOptions;
class ScannerEncodersPage;
class VideoCameraPage;
class SystemPasswordForm;
class DeviceOptions;
class PermissionsPage;
class PasswordManagerPage;
class MemoryImportPage;
class KeyCombinationsOptions;
class PathEncoderEmulatorOptions;
class QGeoPositionInfo;
class OptionsListPage;
class QPushButton;
class SwitchTypePage;
class SplashScreen;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void addNewCalibration();
    void renameCalibration();
    void deleteCalibration();
    void addServiceMarksBridge();
    void editServiceMarksBridge();
    void deleteServiceMarksBridge();
    void addServiceMarksPlatform();
    void editServiceMarksPlatform();
    void deleteServiceMarksPlatform();
    void addServiceMarksMisc();
    void editServiceMarksMisc();
    void deleteServiceMarksMisc();
    void addSwitchNumber();
    void addBranch();
    void addSlip();
    void addRailroadCrossing();
    void quitFromApplication();
    void rebootSystem();
    void powerOffSystem();
    void switchToServiceSystemMode();
    void switchToUpdateSystemMode();
    void switchToElTestSystemMode();
    void changeOrganizationName();
    void changeDepartmentName();
    void changeCalibrationDate();
    void changeCalibrationValidity();
    void changeCertificatNumber();
    void changeFTPServer();
    void changeFTPPath();
    void changeFTPLogin();
    void changeFTPPassword();
    void readPreviousExitCode(int exitCode);
    void setElectricTest(bool isElTest);

public slots:
    void init();

signals:
    void doSetScanChannel();
    void doSetHandChannel();
    void doSetHeadScannerChannel();
    void doNewCalibration();
    void doRecalibration();
    void trackMarkExternalKeyboard_released();
    void serviceMarkExternalKeyboard_released();
    void backFromNotesMenuPageState();
    void backFromServiceMarkPage();
    void messageChanged(const QString& message = "");
    void progressChanged(qreal percent);
    void inited();
    void registrationServiceButtonPressed();
    void doStartBoltJoint();
    void doStopBoltJoint();
    void languageChanged();
    void noUseHeadScanner();
    void backToMenuRegOffState();
    void trackMarksSelected();
    void doSchemeChanged();
    void acNotificationEnabled(bool);

protected:
    bool event(QEvent* e);
    void keyPressEvent(QKeyEvent* ke);

private:
    void setupUi();
    void setupFiltration();
    void setupCheckCalibration();
    void setupImportantAreaIndicate();
    void hideStackedLayout();
    void hideStackedLayoutBesidesReport();
    void retranslateUi();
    void retranslateRegistrationPage();
    void retranslateRegAndServiceOnOffButton();
    void setupSetLanguagePage();
    void setupCalibrationPage();
    void setupLimitsBaseSensPage();
    void setupStackedWidget();
    void setupOptionsListPage();
    QPushButton* addButton(QString objectName, QString buttonName);
    void addLimitsBaseSensSpinBoxNumber(quint16 angle, eInspectionMethod method);
    void initFsm();
    void initConnectionStates();
    void initMemoryStates();
    void initBatteryStates();
    void initRegistrationStates();
    void initFsmRegOffStates();
    QState* initFsmRegOnStates();
    QState* initFsmRegOnServiceStates();
    void initBoltJointStates();
    void updateTrackMarksWidgets();
    void changeServiceMarksDefectCode();
    void changeServiceMarksDefectComment();
    void changeLSizeHeadScanner();
    void changeHSizeHeadScanner();
    void changeHeadScannerDefectComment();
    void changeDirectionCode();
    void setTrackNumber();
    void setEKASUILink();
    void setEKASUIHeadLink();
    void setDHead();
    void switchToMode(const QString& mode);
    void showVirtualKeyboardsWithLineEditAndValidatorNumbers(const QString& value);
    void showVirtualKeyboardsWithLineEditAndValidatorNone(const QString& value);
    void showVirtualKeyboardsWithPlainTextAndValidatorNone(const QString& value);
    void showVirtualKeyboardsWithLineEditAndValidatorDate(const QString& value);
    void checkLastServiceDate(bool showNotifier = false);
    void changeStateAcousticContact(bool isEnabled);
    void configureUiForAviconDbAviconDbHs();
    void configureUiForAviconDb();
    void configureUiForAviconDbHs();
    void configureUiForAvicon15();
    void configureUiForAvicon31Estonia();
    void configureBottomPanelForAndroid();
    void setupPowerOffLabel();
    void setLateralPanelsVisibility(bool visible);
    void setAcousticContactPanelsVisibility(bool visible);
    void clearLateralPanelsSelection();
    void setupTrackMarks(Core& core);
    void setVisibleSpeedLabel(bool isVisible);
    void hideServiceMarksForDBHS();
    void switchRegistrationControlObject(int index);
    void printExitCode();
    void setEnabledBoltJointState(bool isEnabled);
    void connectRemoteControlSignals();
    void disconnectRemoteControlSignals();
    void viewCurrentCoordinate();
    void showSplashScreen(QString message);

    void connectBottomPanelSignals();
    void setupLateralButtons();
    void lateralButtonClicked();

private slots:
    void switchToPage(QWidget* widget);
    void switchToMenuPage(QWidget* widget);
    void leaveMenuPage();
    void clearMenuHistory();
    void switchToVisibleSatellitesPage();
    void setUmuInfo();
    void checkUmuFirmwareVersion();
    void setScanChannel(int index, LateralPanelType side);
    void setHandChannel(int index);
    void setHeadScanChannel(int index);
    void onBScanPlotSelected(int tapeIndex, int index);
    void on_showHideControlsButton_released();
    void on_backPushButton_released();
    void on_logButton_released();
    void on_calibrationButton_released();
    void onSetDateButton_released();
    void on_InfoButton_released();
    void onSetTimeButton_released();
    void onSoundSettingsButton_released();
    void on_quitButton_released();
    void on_rebootButton_released();
    void on_powerOffButton_released();
    void on_serviceModeButton_released();
    void on_updateModeButton_released();
    void on_eltestModeButton_released();
    void onSetLanguageButton_released();
    void on_coordinatesButton_released();
    void on_versionsButton_released();
    void on_aScanButton_released();
    void on_mScanButton_released();
    void on_bScanButton_released();
    void on_optionsButton_released();
    void on_calibrationsListButton_released();
    void on_tableOfCalibrationsButton_released();
    void on_registrationButton_released();
    void onChangeSchemeButton_released();
    void on_changeOperatorButton_released();
    void on_serviceMarksButton_released();
    void on_railTypeButton_released();
    void on_showHideDebugPanelButton_released();
    void on_debugButton_released();
    void on_tableOfHandChannelsButton_released();
    void on_leaveButton_released();
    void onBrightnessSettingsButton_released();
    void on_showMarkMenuPageButton_released();
    void onAScanSettingsButton_released();
    void onBScanSettingsButton_released();
    void onMScanSettingsButton_released();
    void onWifiManagerButton_released();
    void on_showVideoCameraPageButton_released();
    void on_showAudioRecorderPageButton_released();
    void on_serviceMarksDefectButton_released();
    void on_serviceMarksSwitchNumberButton_released();
    void on_serviceMarksBranchButton_released();
    void on_serviceMarksSlipButton_released();
    void on_serviceMarksPhotoVideButton_released();
    void on_serviceMarksAudioCommentsButton_released();
    void on_serviceMarksRailroadCrossingButton_released();
    void on_serviceMarksMiscButton_released();
    void on_serviceMarksBridgeButton_released();
    void on_serviceMarksBridgeOkButton_released();
    void on_serviceMarksBridgeAddButton_released();
    void on_serviceMarksBridgeEditButton_released();
    void on_serviceMarksBridgeDeleteButton_released();
    void on_serviceMarksPlatformButton_released();
    void on_serviceMarksPlatformOkButton_released();
    void on_serviceMarksPlatformAddButton_released();
    void on_serviceMarksPlatformEditButton_released();
    void on_serviceMarksPlatformDeleteButton_released();
    void on_serviceMarksMiscOkButton_released();
    void on_serviceMarksMiscAddButton_released();
    void on_serviceMarksMiscEditButton_released();
    void on_serviceMarksMiscDeleteButton_released();
    void on_selectCalibrationButton_released();
    void on_newCalibrationButton_released();
    void on_deleteCalibrationButton_released();
    void on_renameCalibrationButton_released();
    void on_recalibrationButton_released();
    void onSchemeChanged(int index = 0);
    void onInited();
    void externalKeyToggled(bool isPressed, int keyCode);
    void onBrightnessChanged(qreal value);
    void onConnectionDisconnectedStateEntered();
    void onConnectionConnectingStateEntered();
    void onConnectionConnectedStateEntered();
    void onUnknownMemoryStateEntered();
    void onMemoryIsOverStateEntered();
    void onLowMemoryStateEntered();
    void onEnoughMemoryStateEntered();
    void onLowBattery();
    void onRegistrationOffStateEntered();
    void onRegistrationOffBatteryIsOverStateEntered();
    void onRegistrationOnStateEntered();
    void onRegistrationOnStateExited();
    void onRegistrationOnServiceStateEntered();
    void onRegistrationOnServiceStateExited();
    void onStopRegistrationMemoryIsOver();
    void onInitialBoltJointStateEntered();
    void onStartBoltJointStateEntered();
    void onStopBoltJointStateEntered();
    void onMenuStateRegOffEntered();
    void onMenuStateRegOffExited();
    void onNewCalibrationStateRegOffEntered();
    void onRecalibrationStateRegOffEntered();
    void onCalibrationStateRegOffExited();
    void onCalibrationScanControlStateRegOffEntered();
    void onCalibrationHandControlStateRegOffEntered();
    void onScanStateRegOffEntered();
    void onBScanStateRegOffEntered();
    void onBScanStateRegOffExited();
    void onMScanStateRegOffEntered();
    void onMScanStateRegOffExited();
    void onEvaluationStateRegOffEntered();
    void onEvaluationStateRegOffExited();
    void onHandStateRegOffEntered();
    void onHandStateRegOffExited();
    void onHeadScannerBScanStateRegOffEntered();
    void onHeadScannerEvaluationStateRegOffEntered();
    void onHeadScannerViewResultStateRegOffEntered();
    void onHeadScanRegOptionsStartStateRegOnEntered();
    void onHeadScanRegOptionsFinishStateRegOnEntered();
    void onHeadScanRegOptionsFinishStateRegOnExited();
    void onHeadScannerBScanStateRegOnEntered();
    void onHeadScannerEvaluationStateRegOnEntered();
    void onHeadScannerViewResultStateRegOnEntered();
    void onServiceMarkStateRegOffEntered();
    void onServiceMarkStateRegOffExited();
    void onBScanStateRegOnEntered();
    void onBScanStateRegOnExited();
    void onMScanStateRegOnEntered();
    void onMScanStateRegOnExited();
    void onMenuStateRegOnEntered();
    void onMenuStateRegOnExited();
    void onEvaluationStateRegOnEntered();
    void onEvaluationStateRegOnExited();
    void onHandStateRegOnEntered();
    void onHandStateRegOnExited();
    void onTransitionFromHandToBScanStateTriggered();
    void onRollBackTrackMarkStateTriggered();
    void onNotesMenuPageStateRegOnEntered();
    void onNotesMenuPageStateRegOnExited();
    void onTrackMarkStateRegOnEntered();
    void onTrackMarkStateRegOnExited();
    void onServiceMarkStateRegOnEntered();
    void onServiceMarkStateRegOnServiceEntered();
    void onPauseStateRegOnEntered();
    void onPauseStateRegOnExited();
    void onBScanData(QSharedPointer<tDEV_BScan2Head> head);
    void onBScanDisplayThresholdChanged(int value);
    void on_encoderCorrectionButton_released();
    void onChangedSpeed(double);
    void onChangedBscanScale(double);
    void onBScanDotSizeValueChanged(qreal value);
    void onBScanProbePulseLocationChanged(int index, const QString& value, const QVariant& userData);
    void onViewCurrentCoordinateChanged(int index, const QString& value, const QVariant& userData);
    void onAScanFillingStateChanged(int index, const QString& value, const QVariant& userData);
    void onNoiseReductionStateChanged(int index, const QString& value, const QVariant& userData);
    void onHeadScannerUseSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onHorizontalZoneChanged(qreal value);
    void onVerticalStartPositionChanged(qreal value);
    void onMScanDotSizeValueChanged(qreal value);
    void shootScreen();
    void on_memoryButton_released();
    void onRemoveRegistrationFiles(const QString& info);
    void onMoveRegistrationFiles(const QString& info);
    void onOpenBScanFileViewer();
    void onOpenImageViewer(const QString& dir, const QString& file);
    void onOpenAudioCommentsPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack);
    void onOpenVideoPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack);
    void onReport(const QString& fileName);
    void onRemoveImageFiles(const QString& info);
    void onRemoveCommentFile(const QString& info);
    void onRemoveVideoFile(const QString& info);
    void onChangedDistance(int value);
    void onBoltJointSettingsButton_released();
    void on_playButton_released();
    void on_notifierButton_released();
    void on_maxBaseSensButton_released();
    void onDateTimeChanged(const QDateTime& datetime);
    void on_controlSectionForwardButton_released();
    void on_controlSectionBackwardButton_released();
    void onShootScreen(unsigned int key);
    void onShootScreen(DeviceSide side, CID channelId, int gateIndex, int gain, int sens, float tvg, float timeDelay, int surface, int startGate, int endGate);
    void onScreenShotButtonReleased(unsigned int key);
    void onBlockHandScanButtons(bool isBlock);
    void blockControlsForScreenShoot(bool isBlock);
    void blockControlsForExitFromApp();
    void onConfigurationRequired(bool isRequired);
    void onSetControlledRail(int index, const QString& value, const QVariant& userData);
    void onBScanPageFpsChanged(double value);
    void onBatterySettingsButton_released();
    void onMinimalLevelBatteryChanged(qreal minimalPercent);
    void onPowerOffSystem();
    void onOperatingOrganizationButton_released();
    void on_changeOrganizationNameButton_released();
    void on_changeDepartmentNameButton_released();
    void onCalibrationInfoButton_released();
    void on_changeCalibrationDateButton_released();
    void on_changeCalibrationValidityButton_released();
    void on_changeCertificatNumberButton_released();
    void on_startSwitchButton_released();
    void on_endSwitchButton_released();
    void onAcousticContactOptionsPageButton_released();
    void onHeadScanOptionsPageButton_released();
    void on_scannerEncodersCorrectionButton_released();
    void onRegistrationThresholdSpinBoxChanged(qreal value);
    void onScreenHeaterSettingsButton_released();
    void onFtpSettingsButton_released();
    void on_editFTPServerButton_released();
    void on_editFTPPathButton_released();
    void on_editFTPLoginButton_released();
    void on_editFTPPasswordButton_released();
    void onImpotantAreaSettingPushButton_released();
    void onAcStateSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onACVoiceNotificationChanged(int index, const QString& value, const QVariant& userData);
    void onImportantAreaIndicationChanged(int index, const QString& value, const QVariant& userData);
    void onDefectCreated();

#if defined TARGET_AVICON31
    void on_Av17PageButton_released();
    void onStartAv17DefView();
#endif

    void onBatteryLevelRequestTimeout();
    void on_changeSerialNumber_released();
    void changeCduSerialNumber();
    void changeDefectoscopeSerialNumber();
    void on_selectSchemePushButton_released();
    void on_selectLanguagePushButton_released();
    void onAccessLevelChanged();
    void onTrackMarksSelected();
    void onWorkUnworkSideSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onRegarStatusChaged(int index, const QString& value, const QVariant& userData);
    void onAutoGainAdjustmentStatusChanged(int index, const QString& value, const QVariant& userData);
    void onParallelVideoBrowsingStatusChanged(int index, const QString& value, const QVariant& userData);
    void onTimeForConnectionOut();
    void on_trolleyLeftSidePageButton_released();
    void onSelectTrackMarkButton_released();
    void onEkasuiButton_released();
    void onEKASUIChanged();
    void onBluetoothOptionsButton_released();
    void onSensAutoResetButton_released();
    void onConnectToWifi();
    void onDisconnectToWifi();
    void onQualityWifiPercent(qreal percent);
    void onBluetoothConnection(bool isConnect);
    void onBluetoothSetDeviceAsAudio();
    void onScannerRegistrationNext();
    void onScannerRegistrationSave();
    void onWireOutputSetted();
    void onBtWireOutputSetted();
    void onBluetoothQuickBarReleased();
    void onWifiQuickBarReleased();
    void onDeviceOptionsButton_released();
    void onStartElTest();
    void onStopElTest();
    void onTimerElTestTimeout();
    void onStartTimerElTest();
    void onModelsReadyStartElTest();
    void onUmuConnectionStatusChanged(bool isConnected);
    void on_permissionsPage_released();
    void onConfigurePasswordsPagePressed();
    void on_showTestRemoteButton_released();
    void onNotificationOptionsButton_released();
    void on_memoryButtonImport_released();
    void onStartSwitchControl();
    void onStopSwitchControl();

    // Remote control slots
    void onRemoteControlConnected();
    void onRemoteControlDisconnected();
    void onUpdateRemoteState();
    void onTmSelectedFromRemoteControl();
    void onSatellitesInUse(int countSatellites);
    void onSatellitesInfo(const QGeoPositionInfo& info);

    void onKeyCombinationsOptionsButton_released();
    void onRegarOptionsButton_released();
    void onAutoGainAdjustmentOptionsButton_released();
    void onParallelVideoBrowsingOptionsButton_released();
    void onPathEncoderEmulatorOptionsButton_released();
    void onChangeDrawMode(int mode);

    void on_changeDefectoscopeSerialNumber_released();
    void onCorrectRegistration();
    void onDateChanged();
    void onHeaterStateChanged(bool _isOn);
    void onMemoryInfo(unsigned long long total, unsigned long long user, unsigned long long system);
    void onSetColor(const QColor& color, int index);
    void onSetOpacity(qreal value);

    void on_switchControlButton_released();

    void onUpdateViewedMark();
    void onSetBScanMode(bool value);

private:
    Ui::MainWindow* ui;
    QStackedLayout* _stackedLayout;
    QuickBar* _quickBar;
    WaitMessagePage* _waitMessagePage;
    Report* _report;
    ScreenShotsServiceView* _screenShotsServiceView;
    TestExternalKeyboardWidget* _testExternalKeyboard;

    ElectricTestPage* _electricTestPage;

    Av17Page* _av17Page;
    Av17DefView* _av17DefView;

    BatteryOptions* _batteryOptions;
    ScreenHeaterOptions* _screenHeaterOptions;
    KeyCombinationsOptions* _keyCombinationsOptions;
    PathEncoderEmulatorOptions* _pathEncoderEmulatorOptions;
    OptionsListPage* _optionsListPage;
    SwitchTypePage* _switchTypePage;

    BoltJointOptions* _boltJointOptions;
    EKASUIOptions* _ekasuiOptions;
    ExternalTemperatureOptions* _externalTemperatureOptions;
    ScannerEncodersPage* _scannerEncodersPage;
    VideoCameraPage* _videoCameraPage;
    SystemPasswordForm* _systemPasswordForm;
    WifiManagerPage* _wifiManagerPage;
    BluetoothManagerPage* _bluetoothManagerPage;
    TrackMarksPage* _trackMarksPage;
    DeviceOptions* _deviceOptions;
    NotificationOptions* _notificationOptions;
    MemoryImportPage* _memoryImportPage;

    SensAutoReset* _sensAutoReset;

    RegistrationPage* _registrationPage;
    DefectPage* _defectPage;
    PermissionsPage* _permissionsPage;
    PasswordManagerPage* _passwordManagerPage;
    ScannerDefectRegistrationPage* _scannerDefectRegistrationPage;
    SwitchOperatorWidget* _switchOperatorWidget;

    QStateMachine* _stateMachine;

    QState* _connectionState;
    QState* _connectionDisconnectedState;
    QState* _connectionConnectingState;
    QState* _connectionConnectedState;

    QState* _memoryState;
    QState* _memoryUnknownState;
    QState* _memoryIsOverState;
    QState* _memoryLowState;
    QState* _memoryEnoughState;

    QState* _batteryState;
    QState* _batteryEnoughState;
    QState* _batteryIsOverState;

    QState* _registrationState;
    QState* _registrationOffState;
    QState* _registrationOffBatteryIsOverState;
    QState* _registrationOnState;
    QState* _registrationOnServiceState;

    QStack<QWidget*> _menuHistory;
    ExternalKeyboard* _externalKeyboard;

    QMap<QString, LimitsForSensSpinBoxNumber*> _limitsForSensSpinBoxes;

    QString _newCalibrationName;

    QTimer* _checkTimerForAvailableMemory;
    int _currentCalibrationIndex;
    unsigned int _keyForScreenShoot;

    bool _isSaveAScan;
    DeviceSide _sideForAScanScreen;
    CID _channelIdForAScanScreen;
    int _gateIndexForAScanScreen;
    int _gainForAScanScreen;
    int _sensForAScanScreen;
    float _tvgForAScanScreen;
    float _timeDelayForAScanScreen;
    int _surfaceForAScanScreen;
    int _startGateForAScanScreen;
    int _endGateForAScanScreen;

    QString _pathToSaveScreenShoot;
    WrongCalibrationMsg* _wrongcalibrationmsg;

    bool _pauseButtonState = false;
    bool _enabledBoltJointState;
    bool _speedLimit;

    QPixmap _ekasuiScreenshot;
    DeviceType _deviceType;
    RegistrationType _registrationType;

    bool _value;

#ifdef ANDROID
    QTimer* _batteryLevelRequestTimer;
#endif
    void setupEKASUI(Core& core);
    void setupFTP();
    void setupMediaPlayers();
    void setupBrightnessControls();
    void setupQuickBar();
    void setupColorSelection();
    void setupRegistrationPage(DeviceType devType, RegistrationType regType);

    // Electric test members
    int _exitCode;
    bool _isElTest;
    bool _isElTestActivate;
    bool _isUmuConnected;
    int _scanChannelsCountLeftPanel;
    int _scanChannelsCountRightPanel;
    int _lastScanChannelLeftPanel;
    int _lastScanChannelRightPanel;
    int _handChannelsCount;
    int _lastHandChannel;
    QTimer* _timerElTest;
    ViewCoordinate _typeView;
    SplashScreen* _splashScreen;
};

#endif  // MAINWINDOW_H
