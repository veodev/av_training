#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clock.h"
#include "scanlateralpaneltapeitem.h"
#include "registration.h"
#include "screen.h"
#include "wrongcalibrationmsg.h"
#include "bscanmovementcontrols.h"

#include "av17defview.h"
#include "av17page.h"

#include "wifimanagerpage.h"
#include "bluetooth/bluetoothmanagerpage.h"
#include "boltjointoptions.h"
#include "ekasuioptions.h"
#include "optionslistpage.h"

#include "trackmarks/trackmarksrussian.h"

#include "trackmarks/trackmarksmetric1km.h"
#include "sensautoreset.h"
#include "registration/registrationpage.h"
#include "defect/defectpage.h"
#include "defect/scannerdefectregistrationpage.h"

#include "selectors/switchoperatorwidget.h"

#include "settings.h"
#include "debug.h"
#include "languageswitcher.h"
#include "core.h"
#include "defcore.h"
#include "externalkeyboard.h"
#include "virtualkeyboards.h"
#include "languageswitcher.h"
#include "channelscontroller.h"
#include "messagepage.h"
#include "styles.h"
#include "appdatapath.h"
#include "battery.h"
#include "notifier.h"
#include "accessories.h"
#include "quickbar.h"
#include "screenshotsserviceview.h"
#include "limitsforsensspinboxnumber.h"
#include "splashscreen.h"
#include "waitmessagepage.h"
#include "report.h"
#include "defectmarker.h"
#include "battery.h"
#include "electrictestpage.h"
#include "limitsforsens.h"
#include "ekasui/EK_ASUIdata.h"
#include "testexternalkeyboardwidget.h"

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include "androidJNI.h"
#endif

#include <tmrussian.h>
#include "registration/defaultregistrationpage.h"
#include "registration/ekasuiregistrationpage.h"

#include "defect/ekasuidefectpage.h"
#include "defect/defaultdefectpage.h"

#include "defect/ekasuiscannerdefectregistrationpage.h"
#include "defect/defaultscannerdefectregistrationpage.h"

#include "batteryoptions.h"
#include "screenheateroptions.h"
#include "externaltemperatureoptions.h"
#include "scannerencoderspage.h"
#include "videocamerapage.h"
#include "systempasswordform.h"
#include "deviceoptions.h"
#include "notificationoptions.h"
#include "memoryimportpage.h"
#include "permissionspage.h"
#include "passwordmanagerpage.h"
#include "remotecontrol/remotecontrol.h"
#include "keycombinationsoptions.h"
#include "pathencoderemulatoroptions.h"
#include "switchtypepage.h"
#include "ftp/ftpuploader.h"

#ifdef TARGET_AVICON15
const static int batteryUpdateInterval = 3000;
#endif

const char SECOND_DIGIT_FIRMWARE_VERSION = 6;
const int MINIMAL_BRIGHTNESS = 30;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , _stackedLayout(nullptr)
    , _quickBar(nullptr)
    , _waitMessagePage(nullptr)
    , _report(nullptr)
    , _screenShotsServiceView(nullptr)
    , _testExternalKeyboard(nullptr)
    , _electricTestPage(nullptr)
    , _av17Page(nullptr)
    , _av17DefView(nullptr)
    , _batteryOptions(nullptr)
    , _screenHeaterOptions(nullptr)
    , _keyCombinationsOptions(nullptr)
    , _pathEncoderEmulatorOptions(nullptr)
    , _optionsListPage(nullptr)
    , _switchTypePage(nullptr)
    , _boltJointOptions(nullptr)
    , _ekasuiOptions(nullptr)
    , _externalTemperatureOptions(nullptr)
    , _scannerEncodersPage(nullptr)
    , _videoCameraPage(nullptr)
    , _systemPasswordForm(nullptr)
    , _wifiManagerPage(nullptr)
    , _bluetoothManagerPage(nullptr)
    , _trackMarksPage(nullptr)
    , _deviceOptions(nullptr)
    , _notificationOptions(nullptr)
    , _memoryImportPage(nullptr)
    , _sensAutoReset(nullptr)
    , _registrationPage(nullptr)
    , _defectPage(nullptr)
    , _permissionsPage(nullptr)
    , _passwordManagerPage(nullptr)
    , _scannerDefectRegistrationPage(nullptr)
    , _switchOperatorWidget(nullptr)
    , _stateMachine(nullptr)
    , _connectionState(nullptr)
    , _connectionDisconnectedState(nullptr)
    , _connectionConnectingState(nullptr)
    , _connectionConnectedState(nullptr)
    , _memoryState(nullptr)
    , _memoryUnknownState(nullptr)
    , _memoryIsOverState(nullptr)
    , _memoryLowState(nullptr)
    , _memoryEnoughState(nullptr)
    , _batteryState(nullptr)
    , _batteryEnoughState(nullptr)
    , _batteryIsOverState(nullptr)
    , _registrationState(nullptr)
    , _registrationOffState(nullptr)
    , _registrationOffBatteryIsOverState(nullptr)
    , _registrationOnState(nullptr)
    , _registrationOnServiceState(nullptr)
    , _externalKeyboard(new ExternalKeyboard(this))
    , _checkTimerForAvailableMemory(nullptr)
    , _currentCalibrationIndex(0)
    , _keyForScreenShoot(0)
    , _isSaveAScan(false)
    , _sideForAScanScreen(NoneDeviceSide)
    , _channelIdForAScanScreen(0)
    , _gateIndexForAScanScreen(0)
    , _gainForAScanScreen(0)
    , _sensForAScanScreen(0)
    , _tvgForAScanScreen(0)
    , _timeDelayForAScanScreen(0)
    , _surfaceForAScanScreen(0)
    , _startGateForAScanScreen(0)
    , _endGateForAScanScreen(0)
    , _pathToSaveScreenShoot("")
    , _wrongcalibrationmsg(nullptr)
    , _enabledBoltJointState(true)
    , _speedLimit(false)
    , _deviceType(static_cast<DeviceType>(restoreDeviceType()))
    , _registrationType(static_cast<RegistrationType>(restoreRegistrationType()))
#ifdef ANDROID
    , _batteryLevelRequestTimer(nullptr)
#endif
    , _exitCode(0)
    , _isElTest(false)
    , _isElTestActivate(restoreElectricTestStatus())
    , _isUmuConnected(false)
    , _scanChannelsCountLeftPanel(0)
    , _scanChannelsCountRightPanel(0)
    , _lastScanChannelLeftPanel(0)
    , _lastScanChannelRightPanel(0)
    , _handChannelsCount(0)
    , _lastHandChannel(0)
    , _timerElTest(nullptr)
    , _typeView(static_cast<ViewCoordinate>(restoreTypeViewCoordinateForBScan()))
    , _value(false)
{
    qRegisterMetaType<eInspectionMethod>("eInspectionMethod");
}

MainWindow::~MainWindow()
{
    qDebug() << "Deleting MainWindow...";
    qDebug() << "Waiting for registration processing...";
    showSplashScreen(tr("Shutdown... Please wait!"));

    if (Core::instance().registration().waitForAllProcessing()) {
        qDebug() << "Registration processing finished!";
    }
    else {
        qDebug() << "Registration processing timeout!";
    }

    if (_deviceType == Avicon31Estonia) {
        disconnectRemoteControlSignals();
    }
    disconnect(&Core::instance(), &Core::doUmuConnectionStatusChanged, this, &MainWindow::onUmuConnectionStatusChanged);
    disconnect(ui->battery, &BatteryWidget::lowBattery, this, &MainWindow::onLowBattery);
    disconnect(ui->coordinatesPage, &SatelliteCoordinatesPage::switchToVisibleSatellitesPage, this, &MainWindow::switchToVisibleSatellitesPage);
    disconnect(ui->encoderPage, &EncoderPage::changedSpeed, this, &MainWindow::onChangedSpeed);
    disconnect(ui->encoderPage, &EncoderPage::changedDistance, this, &MainWindow::onChangedDistance);
    disconnect(ui->memoryPage, &MemoryPage::doMemoryInfo, this, &MainWindow::onMemoryInfo);
#ifdef DEBUG
    disconnect(&Core::instance(), &Core::doBScan2Data, this, &MainWindow::onBScanData);
#endif
    disconnect(_bluetoothManagerPage, &BluetoothManagerPage::connectionStatus, this, &MainWindow::onBluetoothConnection);
    disconnect(_bluetoothManagerPage, &BluetoothManagerPage::setDeviceAsAudio, this, &MainWindow::onBluetoothSetDeviceAsAudio);
    disconnect(_wifiManagerPage, &WifiManagerPage::connectToWifi, this, &MainWindow::onConnectToWifi);
    disconnect(_wifiManagerPage, &WifiManagerPage::disconnectToWifi, this, &MainWindow::onDisconnectToWifi);
    disconnect(_wifiManagerPage, &WifiManagerPage::qualityPercent, this, &MainWindow::onQualityWifiPercent);

    _menuHistory.clear();

    ui->stackedWidget->removeWidget(_av17DefView);
    ui->stackedWidget->removeWidget(_av17Page);

    delete _stackedLayout;

    delete _stateMachine;
    delete _switchOperatorWidget;
    delete _scannerDefectRegistrationPage;
    delete _passwordManagerPage;
    delete _permissionsPage;
    delete _defectPage;
    delete _registrationPage;
    delete _sensAutoReset;
    delete _memoryImportPage;
    delete _notificationOptions;
    delete _deviceOptions;
    delete _trackMarksPage;
    delete _bluetoothManagerPage;
    delete _wifiManagerPage;
    delete _systemPasswordForm;
    delete _videoCameraPage;
    delete _scannerEncodersPage;
    delete _externalTemperatureOptions;
    delete _ekasuiOptions;
    delete _boltJointOptions;
    delete _keyCombinationsOptions;
    delete _screenHeaterOptions;
    delete _batteryOptions;
    delete _optionsListPage;
    delete _switchTypePage;
    delete _av17DefView;
    delete _av17Page;
    delete _electricTestPage;
    delete _testExternalKeyboard;
    delete _screenShotsServiceView;
    delete _report;
    delete _waitMessagePage;
    delete _quickBar;

    delete ui;


    qDebug() << "MainWindow deleted!";
}

void MainWindow::setupFTP()
{
    ui->ftpServerEdit->setText(restoreFtpServer());
    ui->ftpPathEdit->setText(restoreFtpRootPath());
    ui->ftpLoginEdit->setText(restoreFtpLogin());
    ui->ftpPasswordEdit->setText(restoreFtpPassword());
}

void MainWindow::setupMediaPlayers()
{
    ASSERT(connect(ui->imageViewerPage, &ImageViewerPage::doRemoveFiles, this, &MainWindow::onRemoveImageFiles));
    ASSERT(connect(ui->imageViewerPage, &ImageViewerPage::noFiles, this, &MainWindow::leaveMenuPage));

    ASSERT(connect(ui->audioPlayerPage, &AudioPlayerPage::doRemoveFile, this, &MainWindow::onRemoveCommentFile));
    ASSERT(connect(ui->audioPlayerPage, &AudioPlayerPage::noFiles, this, &MainWindow::leaveMenuPage));

    ASSERT(connect(ui->videoPlayerPage, &VideoPlayerPage::doRemoveFile, this, &MainWindow::onRemoveVideoFile));
    ASSERT(connect(ui->videoPlayerPage, &VideoPlayerPage::noFiles, this, &MainWindow::leaveMenuPage));
}

void MainWindow::setupBrightnessControls()
{
    ui->setBrightnessSpinBoxNumber->setMinimum(0);
    ui->setBrightnessSpinBoxNumber->setMaximum(100);
    ui->setBrightnessSpinBoxNumber->setStepBy(1);
    ui->setBrightnessSpinBoxNumber->setValue(restoreBrightness(), false);
    onBrightnessChanged(restoreBrightness());
    ASSERT(connect(ui->setBrightnessSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &MainWindow::onBrightnessChanged));
}

void MainWindow::init()
{
    emit messageChanged(tr("Core initing ..."));
    emit progressChanged(14);
    Core& core = Core::instance();
    emit messageChanged(tr("Main window initing ..."));
    emit progressChanged(21);
    ui = new Ui::MainWindow;
    emit progressChanged(27);
    ui->setupUi(this);
    emit progressChanged(30);
    setupLateralButtons();
    ui->bottomPanel->hide();
    connectBottomPanelSignals();
    _testExternalKeyboard = new TestExternalKeyboardWidget();
    emit progressChanged(35);
    setupStackedWidget();
    emit progressChanged(66);
    core.checkExtraLogs();
    emit progressChanged(67);
    ASSERT(connect(ui->memoryPage, &MemoryPage::doMemoryInfo, this, &MainWindow::onMemoryInfo));
    emit progressChanged(68);
    ASSERT(connect(&core, &Core::heaterStateChanged, this, &MainWindow::onHeaterStateChanged));
    ASSERT(connect(&core, &Core::internalTemperatureChanged, _screenHeaterOptions, &ScreenHeaterOptions::onScreenTemperatureChanged));
    core.resetTemperatureCache();
    core.defectmarker();
    emit progressChanged(70);

    ASSERT(connect(&core, &Core::doSchemeChanged, this, &MainWindow::onSchemeChanged));
    ASSERT(connect(this, &MainWindow::languageChanged, &core, &Core::onLanguageChanged));
    ASSERT(connect(&core, &Core::doInited, this, &MainWindow::onInited));
    ASSERT(connect(&core, &Core::doUpdateViewedMark, this, &MainWindow::onUpdateViewedMark));
    ASSERT(connect(&core, &Core::doSetBScanMode, this, &MainWindow::onSetBScanMode));

#ifdef DEBUG
    ASSERT(connect(&core, &Core::doBScan2Data, this, &MainWindow::onBScanData));
#endif

    ASSERT(connect(&core, &Core::doBScanDisplayThresholdChanged, this, &MainWindow::onBScanDisplayThresholdChanged));

    emit messageChanged(tr("Ui initing ..."));
    setupUi();
    qDebug() << QString("AVICON app version: ") << APP_VERSION;
    qDebug() << QString("AVICON COMMIT HASH: ") << AVICON_HASH;
    qDebug() << QString("DEFCORE COMMIT HASH: ") << DEFCORE_HASH;
    emit progressChanged(80);

    emit progressChanged(85);
    retranslateUi();

    emit progressChanged(87);
    _optionsListPage->setItemVisible("headScanOptionsPageButton", restoreHeadScannerPageState());
    _optionsListPage->setItemVisible("ekasuiButton", restoreEkasuiPageState());
    ui->scannerEncodersCorrectionButton->setHidden(!restoreHeadScannerPageState());

    emit progressChanged(89);

    setupTrackMarks(core);

    ui->serviceMarksBridgesList->clear();
    ui->serviceMarksBridgesList->addItems(restoreBridgesToSettings());
    ui->serviceMarksPlatformsList->clear();
    ui->serviceMarksPlatformsList->addItems(restorePlatformsToSettings());
    ui->serviceMarksMiscList->clear();
    ui->serviceMarksMiscList->addItems(restoreMiscToSettings());

    ui->controledRailBarLabel->setText(_registrationPage->getControlRail() ? tr("Left rail") : tr("Right rail"));
    emit progressChanged(90);
    ASSERT(connect(ui->showHideControlsButton, &PushButtonWithLongPress::longPress, this, &MainWindow::shootScreen));
    ASSERT(connect(_externalKeyboard, &ExternalKeyboard::toggled, this, &MainWindow::externalKeyToggled));
    ASSERT(connect(ui->encoderPage, &EncoderPage::changedSpeed, this, &MainWindow::onChangedSpeed));
    ASSERT(connect(ui->encoderPage, &EncoderPage::changedDistance, this, &MainWindow::onChangedDistance));
    ASSERT(connect(ui->bScanPage, &BScanPage::bscanScaleChanged, this, &MainWindow::onChangedBscanScale));
    ASSERT(connect(ui->aScanPage, &AScanPage::bscanScaleChanged, this, &MainWindow::onChangedBscanScale));
    ASSERT(connect(ui->aScanPage, QOverload<uint>::of(&AScanPage::shootScreen), this, QOverload<uint>::of(&MainWindow::onShootScreen)));
    ASSERT(connect(ui->aScanPage, QOverload<DeviceSide, CID, int, int, int, float, float, int, int, int>::of(&AScanPage::shootScreen), this, QOverload<DeviceSide, CID, int, int, int, float, float, int, int, int>::of(&MainWindow::onShootScreen)));
    ASSERT(connect(ui->aScanPage, QOverload<uint>::of(&AScanPage::screenShootButtonReleased), this, &MainWindow::onScreenShotButtonReleased));
    ASSERT(connect(ui->aScanPage, &AScanPage::blockHandScanButtons, this, &MainWindow::onBlockHandScanButtons));
    ASSERT(connect(ui->aScanPage, &AScanPage::configurationRequired, this, &MainWindow::onConfigurationRequired));
#if defined TARGET_AVICONDBHS
    ASSERT(connect(ui->aScanPage, &AScanPage::doSetControlledRail, this, &MainWindow::onSetControlledRail));
#endif
    ASSERT(connect(ui->memoryPage, &MemoryPage::removeFiles, this, &MainWindow::onRemoveRegistrationFiles));
    ASSERT(connect(ui->memoryPage, &MemoryPage::moveFiles, this, &MainWindow::onMoveRegistrationFiles));
    ASSERT(connect(ui->memoryPage, &MemoryPage::doOpenBScanFileViewer, this, &MainWindow::onOpenBScanFileViewer));
    ASSERT(connect(ui->memoryPage, &MemoryPage::doOpenImageViewer, this, &MainWindow::onOpenImageViewer));
    ASSERT(connect(ui->memoryPage, &MemoryPage::doOpenAudioPlayer, this, &MainWindow::onOpenAudioCommentsPlayer));
    ASSERT(connect(ui->memoryPage, &MemoryPage::doOpenVideoPlayer, this, &MainWindow::onOpenVideoPlayer));
#if defined TARGET_AVICON31
    ASSERT(connect(ui->memoryPage, &MemoryPage::doReport, this, &MainWindow::onReport));
#endif

    ASSERT(connect(_switchTypePage, &SwitchTypePage::doStartSwitchControl, this, &MainWindow::onStartSwitchControl));
    ASSERT(connect(_switchTypePage, &SwitchTypePage::doStopSwitchControl, this, &MainWindow::onStopSwitchControl));

    emit progressChanged(91);
    setupMediaPlayers();

    ASSERT(connect(ui->coordinatesPage, &SatelliteCoordinatesPage::switchToVisibleSatellitesPage, this, &MainWindow::switchToVisibleSatellitesPage));

    setupBrightnessControls();
    onDateTimeChanged(QDateTime::currentDateTime());

    ASSERT(connect(ui->battery, &BatteryWidget::lowBattery, this, &MainWindow::onLowBattery));
    ASSERT(connect(&core, &Core::externalTemperatureChanged, ui->temperature, &Temperature::onTemperatureChanged));
    ASSERT(connect(&core, &Core::doUmuConnectionStatusChanged, this, &MainWindow::onUmuConnectionStatusChanged));
    ASSERT(connect(&core, &Core::doPowerOffOverheat, this, &MainWindow::powerOffSystem));
    ASSERT(connect(&core, &Core::doChangeBrightnessOverheat, this, &MainWindow::onBrightnessChanged));

    ASSERT(connect(_wifiManagerPage, &WifiManagerPage::connectToWifi, this, &MainWindow::onConnectToWifi));
    ASSERT(connect(_wifiManagerPage, &WifiManagerPage::disconnectToWifi, this, &MainWindow::onDisconnectToWifi));
    ASSERT(connect(_wifiManagerPage, &WifiManagerPage::qualityPercent, this, &MainWindow::onQualityWifiPercent));
    ASSERT(connect(_bluetoothManagerPage, &BluetoothManagerPage::connectionStatus, this, &MainWindow::onBluetoothConnection));
    ASSERT(connect(_bluetoothManagerPage, &BluetoothManagerPage::setDeviceAsAudio, this, &MainWindow::onBluetoothSetDeviceAsAudio));
    ASSERT(connect(_videoCameraPage, &VideoCameraPage::doBackFromVideoCameraPage, this, &MainWindow::on_backPushButton_released));

    emit progressChanged(93);
    initFsm();

    connect(_registrationPage, &RegistrationPage::correctRegistration, this, &MainWindow::onCorrectRegistration);
    this->onChangedBscanScale(restoreBscanScale());

    emit messageChanged("");
    emit progressChanged(96);

    if (_deviceType == Avicon31Estonia) {
        connectRemoteControlSignals();
        core.listenRemoteControl();
    }

    core.initTemperatureManager();
    core.start();

    emit progressChanged(100);

#if defined TARGET_AVICON31
    move(-72, 0);
    core.setCurrentDirection(_registrationPage->getDirection());
    showFullScreen();
#elif defined TARGET_AVICONDB
    showFullScreen();
#else
    showMaximized();
#endif
    ASSERT(connect(&core, &Core::permissionsChanged, this, &MainWindow::onAccessLevelChanged));
    ASSERT(connect(_permissionsPage, &PermissionsPage::configurePasswordsPressed, this, &MainWindow::onConfigurePasswordsPagePressed));
    onAccessLevelChanged();
    ASSERT(connect(ui->bScanPage, &BScanPage::fpsChanged, this, &MainWindow::onBScanPageFpsChanged));
#if defined ANDROID && defined TARGET_AVICON15
    ASSERT(connect(&core, &Core::doBatteryChargePercent, (ui->battery), &BatteryWidget::percents));
    ASSERT(connect(&core, &Core::doBatteryVoltage, (ui->battery), &BatteryWidget::voltage));
    ASSERT(connect(&core, &Core::doBatteryChargePercent, (ui->cduBattery), &BatteryWidget::percents));
    ASSERT(connect(&core, &Core::doBatteryVoltage, (ui->cduBattery), &BatteryWidget::voltage));

    _batteryLevelRequestTimer = new QTimer(this);
    _batteryLevelRequestTimer->setInterval(batteryUpdateInterval);
    ASSERT(connect(_batteryLevelRequestTimer, &QTimer::timeout, this, &MainWindow::onBatteryLevelRequestTimeout));
    _batteryLevelRequestTimer->start(batteryUpdateInterval);
#endif
    ui->memoryPage->checkMemory();
    ui->changeOperatorButton->setVisible(true);
    setupFTP();
    emit inited();
    printExitCode();
    if (_isElTest == true) {
        ui->aScanPage->setElectricTest(_isElTest);
        ASSERT(connect(_electricTestPage, &ElectricTestPage::doStartElTest, this, &MainWindow::onStartElTest));
        ASSERT(connect(_electricTestPage, &ElectricTestPage::doStopElTest, this, &MainWindow::onStopElTest));
        if (_isElTestActivate == true) {
            _electricTestPage->setCountCrashes(restoreCountCrashes());
            if (_exitCode == 0) {
                saveCountCduReboot(restoreCountCduReboot() + 1);
                qDebug() << QString("COUNT OF CDU REBOOT: ") << QString::number(restoreCountCduReboot());
            }
            ASSERT(connect(this, &MainWindow::doSchemeChanged, this, &MainWindow::onModelsReadyStartElTest));
        }
        _electricTestPage->show();
    }
}

void MainWindow::addNewCalibration()
{
    const QString& newCalibrationName = VirtualKeyboards::instance()->value();
    if (newCalibrationName.isEmpty() == false) {
        _newCalibrationName = newCalibrationName;
        ui->calibrationsListWidget->addItem(newCalibrationName);
        ui->calibrationsListWidget->setCurrentRow(ui->calibrationsListWidget->count() - 1);
        _currentCalibrationIndex = ui->calibrationsListWidget->currentRow();
        ui->calibrationLabel->setText(newCalibrationName);
        emit doNewCalibration();
    }
}

void MainWindow::renameCalibration()
{
    const QString& newCalibrationName = VirtualKeyboards::instance()->value();
    if (newCalibrationName.isEmpty() == false) {
        QListWidgetItem* item = ui->calibrationsListWidget->currentItem();
        if (item != 0) {
            item[0].setText(newCalibrationName);
            Core::instance().setCalibrationName(newCalibrationName, ui->calibrationsListWidget->currentRow());
            if (_currentCalibrationIndex == ui->calibrationsListWidget->currentRow()) {
                ui->calibrationLabel->setText(newCalibrationName);
            }
        }
    }
}

void MainWindow::deleteCalibration()
{
    Core& core = Core::instance();
    int currentRow = ui->calibrationsListWidget->currentRow();
    QListWidgetItem* item = ui->calibrationsListWidget->currentItem();
    if (item != 0) {
        core.deleteCalibration(currentRow);
        qDeleteAll(ui->calibrationsListWidget->selectedItems());
    }
    if (ui->calibrationsListWidget->count() > 0) {
        ui->calibrationsListWidget->setCurrentRow(0);
        core.setCalibrationIndex(0);
        ui->calibrationLabel->setText(ui->calibrationsListWidget->currentItem()->text());
    }
    else {
        VirtualKeyboards::instance()->setValue(QString("auto"));
        addNewCalibration();
        Notifier::instance().addNote(Qt::red, QString(tr("Auto create calibartion: ")) + QString("\"auto\""));
    }
}

void MainWindow::addServiceMarksBridge()
{
    const QString& newBridgeName = VirtualKeyboards::instance()->value();
    if (newBridgeName.isEmpty() == false) {
        ui->serviceMarksBridgesList->addItem(newBridgeName);
        ui->serviceMarksBridgesList->setCurrentRow(ui->serviceMarksBridgesList->count() - 1);
        saveBridgesToSettings(convertToQStringList(*(ui->serviceMarksBridgesList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::editServiceMarksBridge()
{
    const QString& newBridgeName = VirtualKeyboards::instance()->value();
    if (newBridgeName.isEmpty() == false) {
        QListWidgetItem* item = ui->serviceMarksBridgesList->currentItem();
        if (item != 0) {
            item[0].setText(newBridgeName);
            saveBridgesToSettings(convertToQStringList(*(ui->serviceMarksBridgesList)));
            Core::instance().updateRemoteMarks();
        }
    }
}

void MainWindow::deleteServiceMarksBridge()
{
    QListWidgetItem* item = ui->serviceMarksBridgesList->currentItem();
    if (item != 0) {
        qDeleteAll(ui->serviceMarksBridgesList->selectedItems());
        saveBridgesToSettings(convertToQStringList(*(ui->serviceMarksBridgesList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::addServiceMarksPlatform()
{
    const QString& newPlatformName = VirtualKeyboards::instance()->value();
    if (newPlatformName.isEmpty() == false) {
        ui->serviceMarksPlatformsList->addItem(newPlatformName);
        ui->serviceMarksPlatformsList->setCurrentRow(ui->serviceMarksPlatformsList->count() - 1);
        savePlatformsToSettings(convertToQStringList(*(ui->serviceMarksPlatformsList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::editServiceMarksPlatform()
{
    const QString& newPlatformName = VirtualKeyboards::instance()->value();
    if (newPlatformName.isEmpty() == false) {
        QListWidgetItem* item = ui->serviceMarksPlatformsList->currentItem();
        if (item != 0) {
            item[0].setText(newPlatformName);
            savePlatformsToSettings(convertToQStringList(*(ui->serviceMarksPlatformsList)));
            Core::instance().updateRemoteMarks();
        }
    }
}

void MainWindow::deleteServiceMarksPlatform()
{
    QListWidgetItem* item = ui->serviceMarksPlatformsList->currentItem();
    if (item != 0) {
        qDeleteAll(ui->serviceMarksPlatformsList->selectedItems());
        savePlatformsToSettings(convertToQStringList(*(ui->serviceMarksPlatformsList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::addServiceMarksMisc()
{
    const QString& newMiscName = VirtualKeyboards::instance()->value();
    if (newMiscName.isEmpty() == false) {
        ui->serviceMarksMiscList->addItem(newMiscName);
        ui->serviceMarksMiscList->setCurrentRow(ui->serviceMarksMiscList->count() - 1);
        saveMiscToSettings(convertToQStringList(*(ui->serviceMarksMiscList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::editServiceMarksMisc()
{
    const QString& newMiscName = VirtualKeyboards::instance()->value();
    if (newMiscName.isEmpty() == false) {
        QListWidgetItem* item = ui->serviceMarksMiscList->currentItem();
        if (item != 0) {
            item[0].setText(newMiscName);
            saveMiscToSettings(convertToQStringList(*(ui->serviceMarksMiscList)));
            Core::instance().updateRemoteMarks();
        }
    }
}

void MainWindow::deleteServiceMarksMisc()
{
    QListWidgetItem* item = ui->serviceMarksMiscList->currentItem();
    if (item != 0) {
        qDeleteAll(ui->serviceMarksMiscList->selectedItems());
        saveMiscToSettings(convertToQStringList(*(ui->serviceMarksMiscList)));
        Core::instance().updateRemoteMarks();
    }
}

void MainWindow::addSwitchNumber()
{
    const QString& switchNumber = VirtualKeyboards::instance()->value();
    if (switchNumber.isEmpty() == false) {
        Core::instance().registration().addTextLabel(tr("Switch number #%1").arg(switchNumber));
    }
}

void MainWindow::addBranch()
{
    const QString& branch = VirtualKeyboards::instance()->value();
    if (branch.isEmpty() == false) {
        Core::instance().registration().addTextLabel(tr("Branch: %1").arg(branch));
    }
}

void MainWindow::addSlip()
{
    const QString& slip = VirtualKeyboards::instance()->value();
    if (slip.isEmpty() == false) {
        Core::instance().registration().addTextLabel(tr("Slip: %1").arg(slip));
    }
}

void MainWindow::addRailroadCrossing()
{
    const QString& railroadCrossing = VirtualKeyboards::instance()->value();
    if (railroadCrossing.isEmpty() == false) {
        Core::instance().registration().addTextLabel(tr("RailroadCrossing: %1").arg(railroadCrossing));
    }
}

void MainWindow::quitFromApplication()
{
    showSplashScreen(tr("Shutdown..."));
    ui->statusBarWidget->hide();
    QTimer::singleShot(100, qApp, &QCoreApplication::quit);
}

void MainWindow::rebootSystem()
{
    blockControlsForExitFromApp();
    switchToMode("reboot");
}

void MainWindow::powerOffSystem()
{
#if defined IMX_DEVICE
    blockControlsForExitFromApp();
    switchToMode("poweroff");
#elif defined ATOM_DEVICE
    QProcess* process = new QProcess(this);
    bool ifStart = process->startDetached("/usr/local/scripts/menu_script.sh");
    if (ifStart == true) {
        qApp->quit();
        this->deleteLater();
    }
#else
    qApp->exit();
#endif
}

void MainWindow::switchToServiceSystemMode()
{
    blockControlsForExitFromApp();
    switchToMode("service");
}

void MainWindow::switchToUpdateSystemMode()
{
    blockControlsForExitFromApp();
    switchToMode("update");
}

void MainWindow::switchToElTestSystemMode()
{
    blockControlsForExitFromApp();
    switchToMode("eltest");
}

void MainWindow::changeOrganizationName()
{
    ui->organizationLineEdit->setText(VirtualKeyboards::instance()->value());
    ui->organizationLabel->setText(VirtualKeyboards::instance()->value());
    saveOrganizationName(VirtualKeyboards::instance()->value());
}

void MainWindow::changeDepartmentName()
{
    ui->departamentLineEdit->setText(VirtualKeyboards::instance()->value());
    ui->departmentLabel->setText(VirtualKeyboards::instance()->value());
    saveDepartmentName(VirtualKeyboards::instance()->value());
}

void MainWindow::changeCalibrationDate()
{
    ui->calibrationDateLineEdit->setText(VirtualKeyboards::instance()->value());
    saveCalibrationDate(VirtualKeyboards::instance()->value());
}

void MainWindow::changeCalibrationValidity()
{
    ui->calibrationValidityLineEdit->setText(VirtualKeyboards::instance()->value());
    ui->calibrationValidityLabel->setText(VirtualKeyboards::instance()->value());
    saveCalibrationValidityDate(VirtualKeyboards::instance()->value());
}

void MainWindow::changeCertificatNumber()
{
    ui->calibrationCertificateNumberLineEdit->setText(VirtualKeyboards::instance()->value());
    saveCertificateNumber(VirtualKeyboards::instance()->value());
}

void MainWindow::changeFTPServer()
{
    ui->ftpServerEdit->setText(VirtualKeyboards::instance()->value());
    saveFtpServer(ui->ftpServerEdit->text());
}

void MainWindow::changeFTPPath()
{
    ui->ftpPathEdit->setText(VirtualKeyboards::instance()->value());
    saveFtpPath(ui->ftpPathEdit->text());
}

void MainWindow::changeFTPLogin()
{
    ui->ftpLoginEdit->setText(VirtualKeyboards::instance()->value());
    saveFtpLogin(ui->ftpLoginEdit->text());
}

void MainWindow::changeFTPPassword()
{
    ui->ftpPasswordEdit->setText(VirtualKeyboards::instance()->value());
    saveFtpPassword(ui->ftpPasswordEdit->text());
}

void MainWindow::readPreviousExitCode(int exitCode)
{
    _exitCode = exitCode;
}

void MainWindow::setElectricTest(bool isElTest)
{
    _isElTest = isElTest;
}

bool MainWindow::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
        retranslateRegAndServiceOnOffButton();
    }
    return QMainWindow::event(e);
}

void MainWindow::keyPressEvent(QKeyEvent* ke)
{
    if (_deviceType == Avicon31Estonia) {
        Actions action = _keyCombinationsOptions->checkKey(ke);
        switch (action) {
        case Back:
            ui->backPushButton->released();
            break;
        case Menu:
            ui->menuButton->pressed();
            break;
        case ServiceMarks:
            ui->notesButton->pressed();
            break;
        case RailType:
            ui->railTypeButton->released();
            break;
        case BScan:
            ui->scanButton->pressed();
            break;
        default:
            break;
        }

        if (ke->key() == Qt::Key_Print) {
            shootScreen();
        }
    }

    if (_deviceType == Avicon15) {
        switch (ke->key()) {
        case KEY_1:
        case FN_KEY_1:
            if (!_pauseButtonState) {
                ui->backPushButton->released();
                ui->bottomPanel->hide();
            }
            break;
        case KEY_2:
        case FN_KEY_2:
            ui->menuButton->pressed();
            ui->bottomPanel->hide();
            break;
        case KEY_3:
        case FN_KEY_3:
            if (!_pauseButtonState) {
                ui->boltJointButton->released();
                ui->bottomPanel->hide();
            }
            break;
        case KEY_4:
        case FN_KEY_4:
            ui->notesButton->pressed();
            ui->bottomPanel->hide();
            break;
        case KEY_5:
            ui->railTypeButton->released();
            ui->bottomPanel->hide();
            break;
        case KEY_6:
        case FN_KEY_6:
            ui->handButton->pressed();
            ui->bottomPanel->hide();
            break;
        case KEY_7:
            ui->pauseButton->pressed();
            ui->bottomPanel->hide();
            break;
        case KEY_8:
        case FN_KEY_8:
            ui->scanButton->pressed();
            ui->bottomPanel->hide();
            break;
        case KEY_9:
            if (!_pauseButtonState) {
                ui->showHideControlsButton->released();
                ui->bottomPanel->hide();
            }
            break;
        case KEY_CALL:
            ui->bottomPanel->isHidden() ? ui->bottomPanel->show() : ui->bottomPanel->hide();
            break;
        default:
            break;
        }
    }
}

void MainWindow::setupColorSelection()
{
    int colorIndex = restoreColorImportantAreaIndex();
    ui->colorSelectionSpinBox->addColor(QColor(64, 90, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 128, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 160, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 180, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 200, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 220, 64));
    ui->colorSelectionSpinBox->addColor(QColor(64, 255, 64));
    ui->colorSelectionSpinBox->addColor(QColor(128, 160, 128));
    ui->colorSelectionSpinBox->addColor(QColor(128, 180, 128));
    ui->colorSelectionSpinBox->addColor(QColor(128, 200, 128));
    ui->colorSelectionSpinBox->addColor(QColor(128, 220, 128));
    ui->colorSelectionSpinBox->addColor(QColor(128, 255, 128));
    ui->colorSelectionSpinBox->addColor(QColor(128, 160, 32));
    ui->colorSelectionSpinBox->addColor(QColor(128, 180, 32));
    ui->colorSelectionSpinBox->addColor(QColor(128, 200, 32));
    ui->colorSelectionSpinBox->addColor(QColor(128, 220, 32));
    ui->colorSelectionSpinBox->addColor(QColor(128, 255, 32));
    ui->colorSelectionSpinBox->addColor(QColor(32, 160, 128));
    ui->colorSelectionSpinBox->addColor(QColor(32, 180, 128));
    ui->colorSelectionSpinBox->addColor(QColor(32, 200, 128));
    ui->colorSelectionSpinBox->addColor(QColor(32, 220, 128));
    ui->colorSelectionSpinBox->addColor(QColor(32, 255, 128));
    ui->colorSelectionSpinBox->addColor(QColor(32, 32, 32));
    ui->colorSelectionSpinBox->addColor(QColor(64, 64, 64));
    ui->colorSelectionSpinBox->addColor(QColor(128, 128, 128));
    ui->colorSelectionSpinBox->addColor(QColor(200, 200, 200));
    ui->colorSelectionSpinBox->setIndex(colorIndex);
    ASSERT(connect(ui->colorSelectionSpinBox, &SpinBoxColor::colorChanged, this, &MainWindow::onSetColor));
}

void MainWindow::setupUi()
{
    setupOptionsListPage();
    ui->heaterLabel->hide();
    ui->specialRailTypeLabel->hide();
    ui->specialRailTypeLabelLine->hide();
    ui->switchLabel->hide();
    ui->switchControlLabelLine->hide();
    ui->switchControlButton->hide();
    _optionsListPage->setItemVisible("keyCombinationsOptionsButton", false);
    _optionsListPage->setItemVisible("regarOptionsButton", false);
    _optionsListPage->setItemVisible("autoGainAdjustmentOptionsButton", false);
    _optionsListPage->setItemVisible("parallelVideoBrowsingOptionsButton", false);
    _optionsListPage->setItemEnabled("setLanguageButton", false);
    ui->viewTypeLabel->hide();
    ui->viewCurrentCoordinateSpinBox->hide();
    ui->remoteControlLabel->hide();
    ui->handScanDoubleButton->setVisible(_deviceType == Avicon31Estonia);
    ui->controledRailBarLabel->hide();
    ui->trolleyLeftSidePageButton->hide();

    ui->buiBatteryLabel->hide();
    setWindowTitle(applicationName());
    ui->scannerEncodersCorrectionButton->setVisible(restoreHeadScannerUseState());
    ui->cduSerialNumberGroupBox->setVisible(false);
    ui->defectoscopeSerialNumberGroupBox->setVisible(false);
    ui->battery->setVoltageVisible(false);
    ui->cduBattery->setPercentVisible(false);
    ui->cduBattery->setTextColor(QColor(Qt::black));
#if defined ANDROID && defined TARGET_AVICON15
    ui->buiBatteryLabel->show();
    ui->buiBatteryLabel->setStyleSheet("color: rgb(0, 255, 0); font: bold");
    ui->cduSerialNumberGroupBox->show();
    ui->cduSerialNumberLineEdit->setDisabled(true);
    ui->changeSerialNumber->setDisabled(true);
#endif

    ui->battery->setMinimalPercent(restoreMinimalBatteryPercent());
    ui->organizationLineEdit->setText(restoreOrganizationName());
    ui->departamentLineEdit->setText(restoreDepartmentName());
    ui->cduSerialNumberLineEdit->setText(cduSerialNumber());
    ui->defectoscopeSerialNumberLineEdit->setText(defectoscopeSerialNumber());
    ui->calibrationDateLineEdit->setText(restoreCalibrationDate());
    ui->calibrationValidityLineEdit->setText(restoreCalibrationValidityDate());
    ui->calibrationCertificateNumberLineEdit->setText(restoreCertificateNumber());
    ASSERT(connect(_batteryOptions, &BatteryOptions::minimalLevelBatteryChanged, this, &MainWindow::onMinimalLevelBatteryChanged));

    ui->registrationLabel->setText(QString(0x2B1C));
    setupSetLanguagePage();
    emit progressChanged(71);
    ui->debugPanel->setMinimumSize(QSize(100, 0));
    ui->debugPanel->setLineWidth(2);
    ui->debugPanel->hide();
    ui->powerOffLabel->hide();
#ifndef DEBUG
    ui->debugButton->hide();
#endif
    ui->debugPanel->layout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    ui->leftLateralButtons->layout()->setAlignment(Qt::AlignTop);
    ui->rightLateralButtons->layout()->setAlignment(Qt::AlignTop);

#if defined(IMX_DEVICE)
    ui->leftLateralButtons->setVisible(true);
    ui->rightLateralButtons->setVisible(true);
#else
    ui->leftLateralButtons->setVisible(false);
    ui->rightLateralButtons->setVisible(false);
#endif

    ui->handLateralButtonsView->setVisible(false);
    ui->headScanLateralButtonsView->setVisible(false);

    ui->leftScanLateralPanelView->setType(LeftPanel);
    ui->leftScanLateralPanelView->setDependedPanel(ui->rightScanLateralPanelView);

    ui->leftScanTapeAcousticContactView->setType(LeftPanel);
#if defined(IMX_DEVICE)
    ui->leftScanLateralPanelView->setDependedButtonsView(ui->leftScanLateralButtonsView);
    ui->headScanLateralPanelView->setDependedButtonsView(ui->headScanLateralButtonsView);
#endif
    ui->leftScanLateralPanelView->hide();
    ASSERT(connect(ui->leftScanLateralPanelView, &ScanLateralPanelView::clicked, this, &MainWindow::setScanChannel));

    ui->rightScanLateralPanelView->setType(RightPanel);
    ui->rightScanLateralPanelView->setDependedPanel(ui->leftScanLateralPanelView);

    ui->rightScanTapeAcousticContactView->setType(RightPanel);
#if defined(IMX_DEVICE)
    ui->rightScanLateralPanelView->setDependedButtonsView(ui->rightScanLateralButtonsView);
#endif
    ui->rightScanLateralPanelView->hide();
    ASSERT(connect(ui->rightScanLateralPanelView, &ScanLateralPanelView::clicked, this, &MainWindow::setScanChannel));

#if defined(IMX_DEVICE)
    ui->handLateralPanelView->setDependedButtonsView(ui->handLateralButtonsView);
#endif
    ui->handLateralPanelView->hide();
    ASSERT(connect(ui->handLateralPanelView, &HandLateralPanelView::clicked, this, &MainWindow::setHandChannel));

    ui->headScanLateralPanelView->hide();
    ASSERT(connect(ui->headScanLateralPanelView, &HandLateralPanelView::clicked, this, &MainWindow::setHeadScanChannel));
    ui->label_247->hide();
    ui->horizontalZoneSpinBox->hide();

    ui->aScanPage->setControlsAreaVisible(false);
    int bScanDotSize = restoreBScanDotSize();
    ui->aScanPage->setBScanDotSize(bScanDotSize);

    ui->bScanPage->setControlsAreaVisible(false);
    ui->bScanPage->setDotSize(bScanDotSize);

    ui->bScanDotSize->setMinimum(1);
    ui->bScanDotSize->setMaximum(3);
    ui->bScanDotSize->setStepBy(1);
    ui->bScanDotSize->setValue(bScanDotSize, false);
    ASSERT(connect(ui->bScanDotSize, &SpinBoxNumber::valueChanged, this, &MainWindow::onBScanDotSizeValueChanged));

    setupColorSelection();

    ui->importaintAreaIndicationSpinBox->addItem(tr("On"), true);
    ui->importaintAreaIndicationSpinBox->addItem(tr("Off"), false);
    ui->importaintAreaIndicationSpinBox->setIndex((restoreImportantAreaIndicateEnable() ? 0 : 1));
    ASSERT(connect(ui->importaintAreaIndicationSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onImportantAreaIndicationChanged));


    int opacity = restoreOpacityImportantArea();
    ui->opacitySpinBox->setPrecision(0);
    ui->opacitySpinBox->setSuffix("%");
    ui->opacitySpinBox->setMinMaxStepValue(0, 100, 5, restoreOpacityImportantArea());
    ASSERT(connect(ui->opacitySpinBox, &SpinBoxNumber::valueChanged, this, &MainWindow::onSetOpacity));
    ui->opacitySpinBox->setValue(opacity);

    bool isProbePulseFromAbove = restoreProbePulseLocation();
    int locationIndex = isProbePulseFromAbove ? 0 : 1;
    ui->bScanLocationProbePulse->addItem(tr("From above"), true);
    ui->bScanLocationProbePulse->addItem(tr("From below"), false);
    ui->bScanLocationProbePulse->setIndex(locationIndex);
    ASSERT(connect(ui->bScanLocationProbePulse, &SpinBoxList::valueChanged, this, &MainWindow::onBScanProbePulseLocationChanged));

    ViewCoordinate typeView = static_cast<ViewCoordinate>(restoreTypeViewCoordinateForBScan());
    ui->viewCurrentCoordinateSpinBox->addItem(tr("#### Km ## Pk ## m"), KmPkM);
    ui->viewCurrentCoordinateSpinBox->addItem(tr("####,### km"), KmCommaM);
    ui->viewCurrentCoordinateSpinBox->addItem(tr("####+##"), Hectometer);
    ui->viewCurrentCoordinateSpinBox->addItem(tr("###### m"), MeterOnly);
    ui->viewCurrentCoordinateSpinBox->setIndex(typeView);
    ASSERT(connect(ui->viewCurrentCoordinateSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onViewCurrentCoordinateChanged));

    bool isFillingAScan = restoreAScanFillingState();
    locationIndex = isFillingAScan ? 0 : 1;
    ui->fillingAScanSpinBox->addItem(tr("On"), true);
    ui->fillingAScanSpinBox->addItem(tr("Off"), false);
    ui->fillingAScanSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->fillingAScanSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onAScanFillingStateChanged));

    bool isFilter = restoreNoiseReduction();
    locationIndex = isFilter ? 0 : 1;
    ui->noiseReductionSpinBox->addItem(tr("On"), true);
    ui->noiseReductionSpinBox->addItem(tr("Off"), false);
    ui->noiseReductionSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->noiseReductionSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onNoiseReductionStateChanged));

    bool isTrolleyLeftSideWork = restoreTrolleyLeftSide();
    locationIndex = isTrolleyLeftSideWork ? 0 : 1;
    ui->workUnworkSideSpinBox->addItem(tr("Gauge"), true);
    ui->workUnworkSideSpinBox->addItem(tr("Field"), false);
    ui->workUnworkSideSpinBox->setIndex(locationIndex);
    ASSERT(connect(_registrationPage, &RegistrationPage::workSideChanged, this, &MainWindow::onWorkUnworkSideSpinBoxChanged));
    ASSERT(connect(ui->workUnworkSideSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onWorkUnworkSideSpinBoxChanged));

    bool regarStatus = restoreRegarStatus();
    locationIndex = regarStatus ? 0 : 1;
    ui->regarStatusSpinBox->addItem(tr("On"), true);
    ui->regarStatusSpinBox->addItem(tr("Off"), false);
    ui->regarStatusSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->regarStatusSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onRegarStatusChaged));

    bool autoGainAdjustmentStatus = restoreAutoGainAdjustmentStatus();
    locationIndex = autoGainAdjustmentStatus ? 0 : 1;
    ui->autoGainAdjustmentSpinBox->addItem(tr("On"), true);
    ui->autoGainAdjustmentSpinBox->addItem(tr("Off"), false);
    ui->autoGainAdjustmentSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->autoGainAdjustmentSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onAutoGainAdjustmentStatusChanged));

    bool parallelVideoBrowsingStatus = restoreParallelVideoBrowsingStatus();
    locationIndex = parallelVideoBrowsingStatus ? 0 : 1;
    ui->parallelVideoBrowsingSpinBox->addItem(tr("On"), true);
    ui->parallelVideoBrowsingSpinBox->addItem(tr("Off"), false);
    ui->parallelVideoBrowsingSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->parallelVideoBrowsingSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onParallelVideoBrowsingStatusChanged));

    int mScanDotSize = restoreMScanDotSize();
    ui->mScanPage->setDotSize(mScanDotSize);
    ui->mScanDotSize->setMinimum(1);
#if !defined ANDROID && !defined TARGET_AVICON15
    ui->mScanDotSize->setMaximum(10);
#else
    ui->mScanDotSize->setMaximum(20);
#endif

    ui->mScanDotSize->setStepBy(1);
    ui->mScanDotSize->setValue(mScanDotSize, false);
    ASSERT(connect(ui->mScanDotSize, &SpinBoxNumber::valueChanged, this, &MainWindow::onMScanDotSizeValueChanged));

    emit progressChanged(72);

    bool acState = restoreStateAcousticContactControl();
    ui->acStateSpinBox->addItem(tr("Off"), false);
    ui->acStateSpinBox->addItem(tr("On"), true);
    ui->acStateSpinBox->setIndex(acState);
    ASSERT(connect(ui->acStateSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onAcStateSpinBoxChanged));

    bool acNotificationState = restoreACVoiceNotificationEnabled();
    ui->voiceNotificationSpinBox->addItem(tr("Off"), false);
    ui->voiceNotificationSpinBox->addItem(tr("On"), true);
    ui->voiceNotificationSpinBox->setIndex(acNotificationState);
    ASSERT(connect(ui->voiceNotificationSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onACVoiceNotificationChanged));
    ASSERT(connect(this, &MainWindow::acNotificationEnabled, &Core::instance(), &Core::onACNotificationEnabled));

    bool headScanUseState = restoreHeadScannerUseState();
    ui->headScannerUseStateSpinBox->addItem(tr("No"), false);
    ui->headScannerUseStateSpinBox->addItem(tr("Yes"), true);
    ui->headScannerUseStateSpinBox->setIndex(headScanUseState);
    ASSERT(connect(ui->headScannerUseStateSpinBox, &SpinBoxList::valueChanged, this, &MainWindow::onHeadScannerUseSpinBoxChanged));

    int horizontalZone = restoreHorizontalZone();
    ui->horizontalZoneSpinBox->setMinMaxStepValue(50, 200, 1, horizontalZone);
    ASSERT(connect(ui->horizontalZoneSpinBox, &SpinBoxNumber::valueChanged, this, &MainWindow::onHorizontalZoneChanged));

    int verticalStartPosition = restoreVerticalStartPosition();
    ui->verticalStartPositionSpinBox->setMinMaxStepValue(0, 35, 1, verticalStartPosition);
    ASSERT(connect(ui->verticalStartPositionSpinBox, &SpinBoxNumber::valueChanged, this, &MainWindow::onVerticalStartPositionChanged));

    int registrationThreshold = restoreRegistrationThreshold();
    ui->registrationThresholdSpinBox->setMinMaxStepValue(-12, -6, 6, registrationThreshold);

    Core::instance().setRegistrationThreshold(static_cast<int>(registrationThreshold));
    ASSERT(connect(ui->registrationThresholdSpinBox, &SpinBoxNumber::valueChanged, this, &MainWindow::onRegistrationThresholdSpinBoxChanged));

    ui->boltJointLabel->hide();
    ui->boltJointLine->hide();

    emit progressChanged(73);


    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionBackwardButton->setVisible(false);

    ASSERT(connect(ui->setDatePage, &SetDate::leaveTheWidget, this, &MainWindow::leaveMenuPage));
    ASSERT(connect(ui->setDatePage, &SetDate::leaveTheWidget, this, &MainWindow::onDateChanged));
    ASSERT(connect(ui->setTimePage, &SetTime::leaveTheWidget, this, &MainWindow::leaveMenuPage));
    ASSERT(connect(ui->messagePage, &MessagePage::leaveTheWidget, this, &MainWindow::leaveMenuPage));
    ASSERT(connect(ui->bScanPage, &BScanPage::doBScanPlotSelected, this, &MainWindow::onBScanPlotSelected));

#if !defined TARGET_AVICON31
    _optionsListPage->setItemVisible("ftpSettingsButton", false);
    _optionsListPage->setItemVisible("screenHeaterSettingsButton", false);
#endif
    ASSERT(connect(ui->bScanPage, &BScanPage::doChangeDrawMode, this, &MainWindow::onChangeDrawMode));

#ifndef DEBUG
// ui->quitButton->hide();
#endif

#ifndef IMX_DEVICE
    ui->leaveButton->hide();
    _optionsListPage->setItemVisible("brightnessSettingsButton", false);
#endif

#if defined(TARGET_AVICONDB)
    ui->leaveButton->show();
    _optionsListPage->setItemVisible("brightnessSettingsButton", true);
#elif defined(TARGET_AVICONDBHS)
    ui->leaveButton->show();
    _optionsListPage->setItemVisible("brightnessSettingsButton", false);
    ui->scanButton->hide();
    ui->pauseButton->hide();
    ui->boltJointButton->hide();
#elif defined(TARGET_AVICON15)
    ui->leaveButton->setText(tr("Service mode"));
    ui->leaveButton->show();
    ui->powerOffLabel->show();
#endif

    emit progressChanged(74);
    Notifier::instance().setParent(ui->mainPanel);

#ifdef TARGET_AVICONDB
    _deviceType = AviconDB;
#endif

#ifdef TARGET_AVICONDBHS
    _deviceType = AviconDBHS;
#endif

#ifdef TARGET_AVICON15
    _deviceType = Avicon15;
#endif
    switch (_deviceType) {
    case AviconDB:
        configureUiForAviconDb();
        break;
    case AviconDBHS:
        configureUiForAviconDbHs();
        break;
    case Avicon15:
        configureUiForAvicon15();
        break;
    case Avicon31Estonia:
        configureUiForAvicon31Estonia();
        break;
    default:
        break;
    }
    _permissionsPage->setDeviceType(_deviceType);
}

void MainWindow::setupFiltration()
{
    bool filtrationState = restoreFiltrationEnable();
    qDebug() << QString("Filtration| state: ") << QString::number(filtrationState);
    if (filtrationState) {
        int minPacketSize = restoreFilterMinPacketSize();
        int maxCoordSpacing = restoreFilterMaxCoordSpacing();
        int maxDelayDelta = restoreFilterMaxDelayDelta();
        int maxSameDelayConsecutive = restoreFilterMaxSameDelayConsecutive();
        Core& core = Core::instance();
        core.deviceEnableFiltration(filtrationState);
        core.deviceSetFilterParams(fprmMinPacketSize, minPacketSize);
        core.deviceSetFilterParams(fprmMaxCoordSpacing, maxCoordSpacing);
        core.deviceSetFilterParams(fprmMaxDelayDelta, maxDelayDelta);
        core.deviceSetFilterParams(fprmMaxSameDelayConsecutive, maxSameDelayConsecutive);
        qDebug() << QString("Filtration| min packet size: ") << QString::number(minPacketSize);
        qDebug() << QString("Filtration| max coordinate spacing: ") << QString::number(maxCoordSpacing);
        qDebug() << QString("Filtration| max delay delta: ") << QString::number(maxDelayDelta);
        qDebug() << QString("Filtration| max same delay consecutive: ") << QString::number(maxSameDelayConsecutive);
    }
}

void MainWindow::setupCheckCalibration()
{
    bool stateCheckCalibrationAlgorithm = restoreStateCheckCalibrationAlgorithm();
    qDebug() << QString("Check calibration| state: ") << QString::number(stateCheckCalibrationAlgorithm);
    if (stateCheckCalibrationAlgorithm) {
        int evalCalibrMinZTMSpacing = restoreEvalCalibrMinZTMSpacingSize();
        int minKdEcho = restoreMinKdEchoPrm();
        Core& core = Core::instance();
        core.setEvalCalibrMinZTMSpacingSize(evalCalibrMinZTMSpacing);
        core.setMinKdEchoPrm(minKdEcho);
        qDebug() << QString("Check calibration| eval calibr min ZTM spacing: ") << QString::number(evalCalibrMinZTMSpacing);
        qDebug() << QString("Check calibration| min Kd echo: ") << QString::number(minKdEcho);
    }
}

void MainWindow::setupImportantAreaIndicate()
{
    bool stateEnableImportantAreaInicate = restoreImportantAreaIndicateEnable();
    qDebug() << QString("Important area indicate| state: ") << QString::number(stateEnableImportantAreaInicate);
    if (stateEnableImportantAreaInicate) {
        int importantAreaMinPacketSize = restoreImportantAreaMinPacketSize();
        int importantAreaMinSignals0dB = restoreImportantAreaMinSignals0dB();
        int importantAreaMinZTMSpacingPrm = restoreImportantAreaMinZTMSpacing();
        Core& core = Core::instance();
        core.enableImportantAreaIndication(stateEnableImportantAreaInicate);
        core.setImportantAreaMinPacketSize(importantAreaMinPacketSize);
        core.setImportantAreaMinSignals0dB(importantAreaMinSignals0dB);
        core.setImportantAreaMinSpacingSize(importantAreaMinZTMSpacingPrm);
        qDebug() << QString("Important area indicate| important area min packet size: ") << QString::number(importantAreaMinPacketSize);
        qDebug() << QString("Important area indicate| important area min signals 0dB: ") << QString::number(importantAreaMinSignals0dB);
        qDebug() << QString("Important area indicate| important area min ZTM spacing prm: ") << QString::number(importantAreaMinZTMSpacingPrm);
    }
}

void MainWindow::setupQuickBar()
{
    _quickBar = new QuickBar();
    _stackedLayout->addWidget(_quickBar);
    _stackedLayout->setCurrentWidget(_quickBar);
    _quickBar->hide();
    ASSERT(connect(_quickBar, &QuickBar::brightnessChanged, this, &MainWindow::onBrightnessChanged));
    ASSERT(connect(_quickBar, &QuickBar::volumeChanged, ui->soundSettingsPage, &SoundSettings::setVolume));
    ASSERT(connect(_quickBar, &QuickBar::backPushButtonReleased, ui->backPushButton, &QPushButton::released));
    ASSERT(connect(_quickBar, &QuickBar::menuButtonReleased, ui->menuButton, &QPushButton::pressed));
    ASSERT(connect(_quickBar, &QuickBar::boltJointButtonReleased, ui->boltJointButton, &QPushButton::released));
    ASSERT(connect(_quickBar, &QuickBar::notesButtonReleased, ui->notesButton, &QPushButton::pressed));
    ASSERT(connect(_quickBar, &QuickBar::railTypeButtonReleased, ui->railTypeButton, &QPushButton::released));
    ASSERT(connect(_quickBar, &QuickBar::handButtonReleased, ui->handButton, &QPushButton::pressed));
    ASSERT(connect(_quickBar, &QuickBar::pauseButtonReleased, ui->pauseButton, &QPushButton::pressed));
    ASSERT(connect(_quickBar, &QuickBar::scanButtonReleased, ui->scanButton, &QPushButton::pressed));
    ASSERT(connect(_quickBar, &QuickBar::showHideControlsButtonReleased, ui->showHideControlsButton, &QPushButton::released));

    ASSERT(connect(_quickBar, &QuickBar::wireOutputSetted, this, &MainWindow::onWireOutputSetted));
    ASSERT(connect(_quickBar, &QuickBar::btWireOutputSetted, this, &MainWindow::onBtWireOutputSetted));

    ASSERT(connect(_quickBar, &QuickBar::bluetoothButtonReleased, this, &MainWindow::onBluetoothQuickBarReleased));
    ASSERT(connect(_quickBar, &QuickBar::wifiButtonReleased, this, &MainWindow::onWifiQuickBarReleased));
    if (_deviceType == Avicon31Default || _deviceType == Avicon15) {
        ASSERT(connect(ui->statusBarWidget, &StatusBar::clicked, _quickBar, &QuickBar::show));
    }
}

void MainWindow::setupRegistrationPage(DeviceType devType, RegistrationType regType)
{
    Q_UNUSED(devType);
    if (regType == EKASUIRegistration) {
        _registrationPage = new EKASUIRegistrationPage();
        _scannerDefectRegistrationPage = new EKASUIScannerDefectRegistrationPage();
        _defectPage = new EKASUIDefectPage();
    }
    else {
        _registrationPage = new DefaultRegistrationPage();
        _scannerDefectRegistrationPage = new DefaultScannerDefectRegistrationPage();
        _defectPage = new DefaultDefectPage();
    }

    ui->stackedWidget->addWidget(_registrationPage);

    _switchOperatorWidget = new SwitchOperatorWidget();
    ui->stackedWidget->addWidget(_switchOperatorWidget);

    ASSERT(connect(_registrationPage, &RegistrationPage::operatorsListChanged, _switchOperatorWidget, &SwitchOperatorWidget::setOperatorsList));
    ASSERT(connect(_registrationPage, &RegistrationPage::activeOperatorChanged, _switchOperatorWidget, &SwitchOperatorWidget::setActiveOperator));

    ui->stackedWidget->addWidget(_defectPage);

    ASSERT(connect(_defectPage, &DefectPage::defectCreated, this, &MainWindow::onDefectCreated));

    ui->stackedWidget->addWidget(_scannerDefectRegistrationPage);

    ASSERT(connect(_scannerDefectRegistrationPage, &ScannerDefectRegistrationPage::nextHead, this, &MainWindow::onScannerRegistrationNext));
    ASSERT(connect(_scannerDefectRegistrationPage, &ScannerDefectRegistrationPage::saveResults, this, &MainWindow::onScannerRegistrationSave));
}

void MainWindow::setupStackedWidget()
{
    _av17Page = new Av17Page();
    _av17Page->setObjectName(QStringLiteral("av17Page"));
    ui->stackedWidget->addWidget(_av17Page);
    emit progressChanged(37);
    _av17DefView = new Av17DefView();
    _av17DefView->setObjectName(QStringLiteral("av17DefView"));
    ui->stackedWidget->addWidget(_av17DefView);
    ASSERT(connect(_av17Page, &Av17Page::doLoadBScan0Chan, _av17DefView, &Av17DefView::loadBScan0Chan));
    emit progressChanged(38);
    _scannerEncodersPage = new ScannerEncodersPage();
    ui->stackedWidget->addWidget(_scannerEncodersPage);
    _deviceOptions = new DeviceOptions();
    ui->stackedWidget->addWidget(_deviceOptions);
    emit progressChanged(39);
    _batteryOptions = new BatteryOptions();
    ui->stackedWidget->addWidget(_batteryOptions);
    emit progressChanged(40);
    _optionsListPage = new OptionsListPage();
    ui->stackedWidget->addWidget(_optionsListPage);
    _switchTypePage = new SwitchTypePage();
    ui->stackedWidget->addWidget(_switchTypePage);
    _screenHeaterOptions = new ScreenHeaterOptions();
    ui->stackedWidget->addWidget(_screenHeaterOptions);
    _keyCombinationsOptions = new KeyCombinationsOptions();
    ui->stackedWidget->addWidget(_keyCombinationsOptions);
    ASSERT(connect(_keyCombinationsOptions, &KeyCombinationsOptions::doHotKeyReleased, ui->bScanPage, &BScanPage::onHotKeyReleased));
    ASSERT(connect(_keyCombinationsOptions, &KeyCombinationsOptions::doHotKeyReleased, ui->aScanPage, &AScanPage::onHotKeyReleased));
    emit progressChanged(42);
    _pathEncoderEmulatorOptions = new PathEncoderEmulatorOptions();
    ui->stackedWidget->addWidget(_pathEncoderEmulatorOptions);
    emit progressChanged(43);
    _externalTemperatureOptions = new ExternalTemperatureOptions();
    ui->stackedWidget->addWidget(_externalTemperatureOptions);
    emit progressChanged(44);
    _videoCameraPage = new VideoCameraPage();
    ui->stackedWidget->addWidget(_videoCameraPage);
    emit progressChanged(45);
    _systemPasswordForm = new SystemPasswordForm();
    ui->stackedWidget->addWidget(_systemPasswordForm);
    emit progressChanged(46);
    _wifiManagerPage = new WifiManagerPage();
    ui->stackedWidget->addWidget(_wifiManagerPage);
    _bluetoothManagerPage = new BluetoothManagerPage();
    ui->stackedWidget->addWidget(_bluetoothManagerPage);
    emit progressChanged(47);
    _boltJointOptions = new BoltJointOptions();
    ui->stackedWidget->addWidget(_boltJointOptions);
    ASSERT(connect(_boltJointOptions, &BoltJointOptions::increaseTracks, ui->bScanPage, &BScanPage::setIncreaseTracks));
    ASSERT(connect(_boltJointOptions, &BoltJointOptions::additiveSensChanged, ui->bScanPage, &BScanPage::setAdditiveForSens));
    emit progressChanged(48);
    _ekasuiOptions = new EKASUIOptions();
    ui->stackedWidget->addWidget(_ekasuiOptions);
    ASSERT(connect(_ekasuiOptions, &EKASUIOptions::ekasuiChanged, this, &MainWindow::onEKASUIChanged));
    emit progressChanged(49);
    _sensAutoReset = new SensAutoReset();
    ui->stackedWidget->addWidget(_sensAutoReset);
    emit progressChanged(51);
    _stackedLayout = new QStackedLayout(ui->widgetForStackedLayout);
    _stackedLayout->setStackingMode(QStackedLayout::StackAll);
    emit progressChanged(52);
    _stackedLayout->addWidget(ui->mainPanel);
    emit progressChanged(53);
    _stackedLayout->setSpacing(0);
    _stackedLayout->setObjectName(QStringLiteral("stackedLayout"));
    _stackedLayout->setContentsMargins(0, 0, 0, 0);
    emit progressChanged(54);
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    _permissionsPage = new PermissionsPage();
    ui->stackedWidget->addWidget(_permissionsPage);
    _passwordManagerPage = new PasswordManagerPage();
    ui->stackedWidget->addWidget(_passwordManagerPage);
    emit progressChanged(55);
    _stackedLayout->addWidget(virtualKeyboards);
    _stackedLayout->setCurrentWidget(virtualKeyboards);
    virtualKeyboards->hide();
    emit progressChanged(56);
    setupQuickBar();
    emit progressChanged(58);
    _waitMessagePage = new WaitMessagePage();
    emit progressChanged(59);
    _stackedLayout->addWidget(_waitMessagePage);
    _stackedLayout->setCurrentWidget(_waitMessagePage);
    _waitMessagePage->hide();
    _report = new Report();
    _stackedLayout->addWidget(_report);
    _stackedLayout->setCurrentWidget(_report);
    _report->hide();
    emit progressChanged(60);
    _screenShotsServiceView = new ScreenShotsServiceView();
    emit progressChanged(61);
    _stackedLayout->addWidget(_screenShotsServiceView);
    _stackedLayout->setCurrentWidget(_screenShotsServiceView);
    _screenShotsServiceView->hide();
    emit progressChanged(62);
    _wrongcalibrationmsg = new WrongCalibrationMsg();
    emit progressChanged(63);
    _stackedLayout->addWidget(_wrongcalibrationmsg);
    _stackedLayout->setCurrentWidget(_wrongcalibrationmsg);
    _wrongcalibrationmsg->hide();
    emit progressChanged(64);
    setupRegistrationPage(_deviceType, _registrationType);
    _electricTestPage = new ElectricTestPage();
    _stackedLayout->addWidget(_electricTestPage);
    _stackedLayout->setCurrentWidget(_electricTestPage);
    _electricTestPage->hide();
    emit progressChanged(65);
    _notificationOptions = new NotificationOptions();
    ui->stackedWidget->addWidget(_notificationOptions);
    _memoryImportPage = new MemoryImportPage();
    ui->stackedWidget->addWidget(_memoryImportPage);
}

void MainWindow::setupOptionsListPage()
{
    QPushButton* setDateButton = addButton(QString("setDateButton"), QString(tr("Date")));
    ASSERT(connect(setDateButton, &QPushButton::released, this, &MainWindow::onSetDateButton_released));

    QPushButton* setTimeButton = addButton(QString("setTimeButton"), QString(tr("Time")));
    ASSERT(connect(setTimeButton, &QPushButton::released, this, &MainWindow::onSetTimeButton_released));

    QPushButton* setLanguageButton = addButton(QString("setLanguageButton"), QString());
    ASSERT(connect(setLanguageButton, &QPushButton::released, this, &MainWindow::onSetLanguageButton_released));

    QPushButton* changeSchemeButton = addButton(QString("changeSchemeButton"), QString(tr("Set Scheme")));
    ASSERT(connect(changeSchemeButton, &QPushButton::released, this, &MainWindow::onChangeSchemeButton_released));

    QPushButton* soundSettingsButton = addButton(QString("soundSettingsButton"), QString(tr("Sound")));
    ASSERT(connect(soundSettingsButton, &QPushButton::released, this, &MainWindow::onSoundSettingsButton_released));

    QPushButton* brightnessSettingsButton = addButton(QString("brightnessSettingsButton"), QString(tr("Brightness")));
    ASSERT(connect(brightnessSettingsButton, &QPushButton::released, this, &MainWindow::onBrightnessSettingsButton_released));

    QPushButton* aScanSettingsButton = addButton(QString("aScanSettingsButton"), QString(tr("A-Scan")));
    ASSERT(connect(aScanSettingsButton, &QPushButton::released, this, &MainWindow::onAScanSettingsButton_released));

    QPushButton* bScanSettingsButton = addButton(QString("bScanSettingsButton"), QString(tr("B-Scan")));
    ASSERT(connect(bScanSettingsButton, &QPushButton::released, this, &MainWindow::onBScanSettingsButton_released));

    QPushButton* mScanSettingsButton = addButton(QString("mScanSettingsButton"), QString(tr("M-Scan")));
    ASSERT(connect(mScanSettingsButton, &QPushButton::released, this, &MainWindow::onMScanSettingsButton_released));

    QPushButton* boltJointSettingsButton = addButton(QString("boltJointSettingsButton"), QString(tr("Bolt Joint")));
    ASSERT(connect(boltJointSettingsButton, &QPushButton::released, this, &MainWindow::onBoltJointSettingsButton_released));

    QPushButton* batterySettingsButton = addButton(QString("batterySettingsButton"), QString(tr("Battery")));
    ASSERT(connect(batterySettingsButton, &QPushButton::released, this, &MainWindow::onBatterySettingsButton_released));

    QPushButton* operatingOrganizationButton = addButton(QString("operatingOrganizationButton"), QString(tr("Operating organization")));
    ASSERT(connect(operatingOrganizationButton, &QPushButton::released, this, &MainWindow::onOperatingOrganizationButton_released));

    QPushButton* deviceOptionsButton = addButton(QString("deviceOptionsButton"), QString(tr("Device options")));
    ASSERT(connect(deviceOptionsButton, &QPushButton::released, this, &MainWindow::onDeviceOptionsButton_released));

    QPushButton* calibrationInfoButton = addButton(QString("calibrationInfoButton"), QString(tr("Information about calibration")));
    ASSERT(connect(calibrationInfoButton, &QPushButton::released, this, &MainWindow::onCalibrationInfoButton_released));

    QPushButton* impotantAreaSettingPushButton = addButton(QString("impotantAreaSettingPushButton"), QString(tr("Important area indication")));
    ASSERT(connect(impotantAreaSettingPushButton, &QPushButton::released, this, &MainWindow::onImpotantAreaSettingPushButton_released));

    QPushButton* acousticContactOptionsPageButton = addButton(QString("acousticContactOptionsPageButton"), QString(tr("Acoustic Contact")));
    ASSERT(connect(acousticContactOptionsPageButton, &QPushButton::released, this, &MainWindow::onAcousticContactOptionsPageButton_released));

    QPushButton* headScanOptionsPageButton = addButton(QString("headScanOptionsPageButton"), QString(tr("Head Scan Options")));
    ASSERT(connect(headScanOptionsPageButton, &QPushButton::released, this, &MainWindow::onHeadScanOptionsPageButton_released));

    QPushButton* screenHeaterSettingsButton = addButton(QString("screenHeaterSettingsButton"), QString(tr("Screen Heater")));
    ASSERT(connect(screenHeaterSettingsButton, &QPushButton::released, this, &MainWindow::onScreenHeaterSettingsButton_released));

    QPushButton* wifiManagerButton = addButton(QString("wifiManagerButton"), QString(tr("Wi-Fi")));
    ASSERT(connect(wifiManagerButton, &QPushButton::released, this, &MainWindow::onWifiManagerButton_released));

    QPushButton* notificationOptionsButton = addButton(QString("notificationOptionsButton"), QString(tr("Notification options")));
    ASSERT(connect(notificationOptionsButton, &QPushButton::released, this, &MainWindow::onNotificationOptionsButton_released));

    QPushButton* ftpSettingsButton = addButton(QString("ftpSettingsButton"), QString(tr("FTP Settings")));
    ASSERT(connect(ftpSettingsButton, &QPushButton::released, this, &MainWindow::onFtpSettingsButton_released));

    QPushButton* ekasuiButton = addButton(QString("ekasuiButton"), QString(tr("EK ASUI")));
    ASSERT(connect(ekasuiButton, &QPushButton::released, this, &MainWindow::onEkasuiButton_released));

    QPushButton* bluetoothOptionsButton = addButton(QString("bluetoothOptionsButton"), QString(tr("Bluetooth")));
    ASSERT(connect(bluetoothOptionsButton, &QPushButton::released, this, &MainWindow::onBluetoothOptionsButton_released));

    QPushButton* sensAutoResetButton = addButton(QString("sensAutoResetButton"), QString(tr("Sensitivity auto reset")));
    ASSERT(connect(sensAutoResetButton, &QPushButton::released, this, &MainWindow::onSensAutoResetButton_released));

    QPushButton* keyCombinationsOptionsButton = addButton(QString("keyCombinationsOptionsButton"), QString(tr("Key combinations")));
    ASSERT(connect(keyCombinationsOptionsButton, &QPushButton::released, this, &MainWindow::onKeyCombinationsOptionsButton_released));

    QPushButton* regarOptionsButton = addButton(QString("regarOptionsButton"), QString(tr("Auto-decryption")));
    ASSERT(connect(regarOptionsButton, &QPushButton::released, this, &MainWindow::onRegarOptionsButton_released));

    QPushButton* autoGainAdjustmentOptionsButton = addButton(QString("autoGainAdjustmentOptionsButton"), QString(tr("Auto gain adjustment")));
    ASSERT(connect(autoGainAdjustmentOptionsButton, &QPushButton::released, this, &MainWindow::onAutoGainAdjustmentOptionsButton_released));

    QPushButton* parallelVideoBrowsingOptionsButton = addButton(QString("parallelVideoBrowsingOptionsButton"), QString(tr("Parallel video browsing")));
    ASSERT(connect(parallelVideoBrowsingOptionsButton, &QPushButton::released, this, &MainWindow::onParallelVideoBrowsingOptionsButton_released));

    // TODO: fix when path encoder emulator works
    //     QPushButton* pathEncoderEmulatorOptionsButton = addButton(QString("pathEncoderEmulatorOptions"), QString(tr("Path encoder emulator")));
    //     ASSERT(connect(pathEncoderEmulatorOptionsButton, &QPushButton::released, this, &MainWindow::onPathEncoderEmulatorOptionsButton_released));
}

QPushButton* MainWindow::addButton(QString objectName, QString buttonName)
{
    QFont font;
    font.setPointSize(22);
    QPushButton* button = new QPushButton(this);
    button->setObjectName(objectName);
    button->setText(buttonName);
    button->setMinimumHeight(50);
    button->setFont(font);
    button->setFocusPolicy(Qt::NoFocus);
    _optionsListPage->addWidget(button);
    return button;
}

void MainWindow::hideStackedLayout()
{
    _screenShotsServiceView->hide();
    _wrongcalibrationmsg->hide();
    _waitMessagePage->hide();
    _report->hide();
    if (_deviceType == Avicon31Default || _deviceType == Avicon15) {
        if (_quickBar != nullptr) {
            _quickBar->hide();
        }
    }
}

void MainWindow::hideStackedLayoutBesidesReport()
{
    _screenShotsServiceView->hide();
    _wrongcalibrationmsg->hide();
    _waitMessagePage->hide();
    if (_deviceType == Avicon31Default || _deviceType == Avicon15) {
        if (_quickBar != nullptr) {
            _quickBar->hide();
        }
    }
}

void MainWindow::retranslateUi()
{
    ui->retranslateUi(this);
    Core& core = Core::instance();

    setupLimitsBaseSensPage();
    _optionsListPage->setItemText("setLanguageButton", QString(tr("Language: ")) + LanguageSwitcher::supportedLanguages()[LanguageSwitcher::currentLanguage()]);

    ui->cduSoftwareVersionLabel->setText(APP_VERSION);
    ui->osVersionLabel->setText(core.getOSVersion());
    ui->cduSerialNumberLabel->setText(cduSerialNumber());
    ui->defectoscopeSerialNumberLabel->setText(defectoscopeSerialNumber());
    ui->organizationLabel->setText(restoreOrganizationName());
    ui->departmentLabel->setText(restoreDepartmentName());
    ui->calibrationValidityLabel->setText(restoreCalibrationValidityDate());

    int schemesCount = core.schemesCount();
    int currentSchemeIndex = ui->schemesListWidget->currentRow();
    ui->schemesListWidget->clear();
    for (int i = 0; i < schemesCount; ++i) {
        ui->schemesListWidget->addItem(QString(tr("Scheme %1").arg(i + 1)));
    }
    ui->schemesListWidget->setCurrentRow(currentSchemeIndex);
    ui->schemeNumberLabel->setText(QString::number(currentSchemeIndex + 1));
    if (_deviceType == Avicon31Default) {
        checkLastServiceDate();
    }

#if defined ANDROID
//    configureBottomPanelForAndroid();
#endif
#if defined TARGET_AVICON15
    setupPowerOffLabel();
    ui->leaveButton->setText(tr("Service mode"));
#endif
    ui->modeLabel->setText(tr("Menu"));
    ui->registrationLabel->setStyleSheet("color:rgb(25, 227, 49); font:bold");
    ui->registrationLabel->setText(QString(0x25A0));
    updateTrackMarksWidgets();
}

void MainWindow::retranslateRegAndServiceOnOffButton()
{
    _registrationPage->setRegistrationState(false);
}

void MainWindow::setupSetLanguagePage()
{
    Languages languages = LanguageSwitcher::supportedLanguages();
    QString currentLanguage = LanguageSwitcher::currentLanguage().toLower();
    int currentIndex = 0;
    Languages::const_iterator iter = languages.constBegin();
    while (iter != languages.constEnd()) {
        ui->listWidget->addItem(iter.value());
        if (iter.key().toLower() == currentLanguage) {
            ui->listWidget->setCurrentRow(currentIndex);
        }
        ++currentIndex;
        ++iter;
    }
}

void MainWindow::setupCalibrationPage()
{
    Core& core = Core::instance();
    ui->calibrationsListWidget->clear();
    ui->calibrationsListWidget->addItems(core.calibrationNamesList());
    TRACE_VALUE(core.currentCalibrationIndex());
    TRACE_VALUE(ui->calibrationsListWidget->count());
    Q_ASSERT(core.currentCalibrationIndex() >= 0 && core.currentCalibrationIndex() < ui->calibrationsListWidget->count());
    ui->calibrationsListWidget->setCurrentRow(core.currentCalibrationIndex());
    ui->calibrationLabel->setText(ui->calibrationsListWidget->currentItem()->text());
    _currentCalibrationIndex = core.currentCalibrationIndex();
}

void MainWindow::setupLimitsBaseSensPage()
{
    QMapIterator<QString, LimitsForSensSpinBoxNumber*> i(_limitsForSensSpinBoxes);
    while (i.hasNext()) {
        i.next();
        LimitsForSensSpinBoxNumber* item = i.value();
        item->setParent(nullptr);
        delete item;
    }
    _limitsForSensSpinBoxes.clear();

    QAbstractItemModel* model = Core::instance().channelsController()->channelsModel();
    if (model == nullptr) {
        return;
    }
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        int angle = model->data(index, AngleRole).toInt();
        eInspectionMethod method = static_cast<eInspectionMethod>(model->data(index, MethodRole).toInt());
        addLimitsBaseSensSpinBoxNumber(angle, method);
    }
}


void MainWindow::addLimitsBaseSensSpinBoxNumber(quint16 angle, eInspectionMethod method)
{
    Q_ASSERT(angle <= 360);
    int minimum = 30;
    QString methodString;
    switch (method) {
    case imEcho:
        methodString = "echo";
        (void) tr("echo");
        break;
    case imMirrorShadow:
        methodString = "msm";
        (void) tr("msm");
        break;
    case imMirror:
        methodString = "mirr";
        minimum = 50;
        (void) tr("mirr");
        break;
    case imShadow:
        methodString = "shad";
        (void) tr("shad");
        break;
    case imNotSet:
        QDEBUG << "Unknown inspection method.";
        Q_ASSERT(false);
        break;
    }

    QString objectName = QString("%1%2").arg(methodString).arg(angle);

    if (_limitsForSensSpinBoxes.contains(objectName) == false) {
        LimitsForSensSpinBoxNumber* item = new LimitsForSensSpinBoxNumber(angle, method, ui->limitsBaseSensPage);
        item->setObjectName(objectName);
        item->setMinimumSize(QSize(0, 50));
        item->setMaximumSize(QSize(16777215, 50));
        item->enableCaption();
        item->setMinimum(minimum);
        item->setMaximum(80);
        item->setValue(Core::instance().limitsForSens().sens(angle, method), false);
        item->setCaption(QString::number(angle) + QString(0x00B0) + QString(" %1").arg(tr(methodString.toLatin1())));
        item->setSuffix(tr(" dB"));
        ASSERT(connect(item, &LimitsForSensSpinBoxNumber::valueChanged, &(Core::instance().limitsForSens()), &LimitsForSens::onSetSens));
        _limitsForSensSpinBoxes[objectName] = item;
        ui->limitsBaseSensGridLayout->addWidget(item, (_limitsForSensSpinBoxes.count() - 1) / 2, (_limitsForSensSpinBoxes.count() - 1) & 0x00000001);
    }
}

void MainWindow::initFsm()
{
    _stateMachine = new QStateMachine(QState::ParallelStates, this);

    initConnectionStates();
    initMemoryStates();
    initBatteryStates();
    initRegistrationStates();
    initBoltJointStates();

    _stateMachine->start();
}

void MainWindow::initConnectionStates()
{
    _connectionState = new QState(_stateMachine);
    _connectionState->setObjectName("connectionState");

    _connectionDisconnectedState = new QState(_connectionState);
    _connectionDisconnectedState->setObjectName("connectionDisconnectedState");
    ASSERT(connect(_connectionDisconnectedState, &QState::entered, this, &MainWindow::onConnectionDisconnectedStateEntered));
    _connectionState->setInitialState(_connectionDisconnectedState);

    _connectionConnectingState = new QState(_connectionState);
    _connectionConnectingState->setObjectName("connectionConnectingState");
    ASSERT(connect(_connectionConnectingState, &QState::entered, this, &MainWindow::onConnectionConnectingStateEntered));

    _connectionConnectedState = new QState(_connectionState);
    _connectionConnectedState->setObjectName("connectionConnectedState");
    ASSERT(connect(_connectionConnectedState, &QState::entered, this, &MainWindow::onConnectionConnectedStateEntered));

    Core& core = Core::instance();
    _connectionDisconnectedState->addTransition(&core, &Core::doConnectionStatusConnecting, _connectionConnectingState);
    _connectionConnectingState->addTransition(&core, &Core::doConnectionStatusConnected, _connectionConnectedState);
}

void MainWindow::initMemoryStates()
{
    _memoryState = new QState(_stateMachine);
    _memoryState->setObjectName("memoryState");

    _memoryUnknownState = new QState(_memoryState);
    _memoryUnknownState->setObjectName("memoryUnknownState");
    ASSERT(connect(_memoryUnknownState, &QState::entered, this, &MainWindow::onUnknownMemoryStateEntered));
    _memoryState->setInitialState(_memoryUnknownState);

    _memoryIsOverState = new QState(_memoryState);
    _memoryIsOverState->setObjectName("memoryIsOverState");
    ASSERT(connect(_memoryIsOverState, &QState::entered, this, &MainWindow::onMemoryIsOverStateEntered));

    _memoryLowState = new QState(_memoryState);
    _memoryLowState->setObjectName("memoryLowState");
    ASSERT(connect(_memoryLowState, &QState::entered, this, &MainWindow::onLowMemoryStateEntered));

    _memoryEnoughState = new QState(_memoryState);
    _memoryEnoughState->setObjectName("memoryEnoughState");
    ASSERT(connect(_memoryEnoughState, &QState::entered, this, &MainWindow::onEnoughMemoryStateEntered));

    ASSERT(_memoryUnknownState->addTransition(ui->memoryPage, &MemoryPage::memoryIsOver, _memoryIsOverState));
    ASSERT(_memoryUnknownState->addTransition(ui->memoryPage, &MemoryPage::lowMemory, _memoryLowState));
    ASSERT(_memoryUnknownState->addTransition(ui->memoryPage, &MemoryPage::enoughMemory, _memoryEnoughState));

    ASSERT(_memoryIsOverState->addTransition(ui->memoryPage, &MemoryPage::lowMemory, _memoryLowState));
    ASSERT(_memoryIsOverState->addTransition(ui->memoryPage, &MemoryPage::enoughMemory, _memoryEnoughState));

    ASSERT(_memoryLowState->addTransition(ui->memoryPage, &MemoryPage::memoryIsOver, _memoryIsOverState));
    ASSERT(_memoryLowState->addTransition(ui->memoryPage, &MemoryPage::enoughMemory, _memoryEnoughState));

    ASSERT(_memoryEnoughState->addTransition(ui->memoryPage, &MemoryPage::memoryIsOver, _memoryIsOverState));
    ASSERT(_memoryEnoughState->addTransition(ui->memoryPage, &MemoryPage::lowMemory, _memoryLowState));
}

void MainWindow::initBatteryStates()
{
    _batteryState = new QState(_stateMachine);
    _batteryState->setObjectName("batteryState");

    _batteryEnoughState = new QState(_batteryState);
    _batteryEnoughState->setObjectName("batteryEnoughState");
    _batteryState->setInitialState(_batteryEnoughState);

    _batteryIsOverState = new QState(_batteryState);
    _batteryIsOverState->setObjectName("batteryIsOverState");
    ASSERT(_batteryEnoughState->addTransition(ui->battery, &BatteryWidget::batteryIsOver, _batteryIsOverState));
}

void MainWindow::initRegistrationStates()
{
    _registrationState = new QState(_stateMachine);
    initFsmRegOffStates();
    _registrationState->setInitialState(_registrationOffState);
    initFsmRegOnStates();
    initFsmRegOnServiceStates();
    ASSERT(_registrationOffState->addTransition(_registrationPage, &RegistrationPage::startStopRegistration, _registrationOnState));
    ASSERT(_registrationOnState->addTransition(_registrationPage, &RegistrationPage::startStopRegistration, _registrationOffState));
    ASSERT(_registrationOffState->addTransition(ui->battery, &BatteryWidget::batteryIsOver, _registrationOffBatteryIsOverState));
    ASSERT(_registrationOffState->addTransition(_registrationPage, &RegistrationPage::startStopService, _registrationOnServiceState));
    ASSERT(_registrationOnServiceState->addTransition(_registrationPage, &RegistrationPage::startStopService, _registrationOffState));
    QSignalTransition* transRegOn = _registrationOnState->addTransition(_memoryIsOverState, &QState::entered, _registrationOffState);
    QSignalTransition* transRegOnService = _registrationOnServiceState->addTransition(_memoryIsOverState, &QState::entered, _registrationOffState);
    Q_ASSERT(transRegOn != nullptr);
    Q_ASSERT(transRegOnService != nullptr);
    ASSERT(connect(transRegOn, &QSignalTransition::triggered, this, &MainWindow::onStopRegistrationMemoryIsOver));
    ASSERT(connect(transRegOnService, &QSignalTransition::triggered, this, &MainWindow::onStopRegistrationMemoryIsOver));
    QSignalTransition* transRegOnBattery = _registrationOnState->addTransition(_batteryIsOverState, &QState::entered, _registrationOffState);
    QSignalTransition* transRegOnServiceBattery = _registrationOnServiceState->addTransition(_batteryIsOverState, &QState::entered, _registrationOffState);
    Q_ASSERT(transRegOnBattery != nullptr);
    Q_ASSERT(transRegOnServiceBattery != nullptr);
    ASSERT(connect(transRegOnBattery, &QSignalTransition::triggered, this, &MainWindow::onStopRegistrationMemoryIsOver));
    ASSERT(connect(transRegOnServiceBattery, &QSignalTransition::triggered, this, &MainWindow::onStopRegistrationMemoryIsOver));
}

void MainWindow::initFsmRegOffStates()
{
    _registrationOffState = new QState(_registrationState);
    _registrationOffState->setObjectName("registrationOffState");
    ASSERT(connect(_registrationOffState, &QState::entered, this, &MainWindow::onRegistrationOffStateEntered));

    _registrationOffBatteryIsOverState = new QState(_registrationState);
    _registrationOffBatteryIsOverState->setObjectName("registrationOffBatteryIsOverState");
    ASSERT(connect(_registrationOffBatteryIsOverState, &QState::entered, this, &MainWindow::onRegistrationOffBatteryIsOverStateEntered));

    QState* menuState = new QState(_registrationOffState);
    menuState->setObjectName("menuState");
    ASSERT(connect(menuState, &QState::entered, this, &MainWindow::onMenuStateRegOffEntered));
    ASSERT(connect(menuState, &QState::exited, this, &MainWindow::onMenuStateRegOffExited));
    _registrationOffState->setInitialState(menuState);

    QState* newCalibrationState = new QState(_registrationOffState);
    newCalibrationState->setObjectName("newCalibrationState");
    ASSERT(connect(newCalibrationState, &QState::entered, this, &MainWindow::onNewCalibrationStateRegOffEntered));
    ASSERT(connect(newCalibrationState, &QState::exited, this, &MainWindow::onCalibrationStateRegOffExited));

    QState* newCalibrationScanControlState = new QState(newCalibrationState);
    newCalibrationScanControlState->setObjectName("newCalibrationScanControlState");
    ASSERT(connect(newCalibrationScanControlState, &QState::entered, this, &MainWindow::onCalibrationScanControlStateRegOffEntered));

    QState* newCalibrationHandControlState = new QState(newCalibrationState);
    newCalibrationHandControlState->setObjectName("newCalibrationHandControlState");
    newCalibrationState->setInitialState(newCalibrationScanControlState);
    ASSERT(connect(newCalibrationHandControlState, &QState::entered, this, &MainWindow::onCalibrationHandControlStateRegOffEntered));

    QState* recalibrationState = new QState(_registrationOffState);
    recalibrationState->setObjectName("recalibrationState");
    ASSERT(connect(recalibrationState, &QState::entered, this, &MainWindow::onRecalibrationStateRegOffEntered));
    ASSERT(connect(recalibrationState, &QState::exited, this, &MainWindow::onCalibrationStateRegOffExited));

    QState* recalibrationScanControlState = new QState(recalibrationState);
    recalibrationScanControlState->setObjectName("recalibrationScanControlState");
    ASSERT(connect(recalibrationScanControlState, &QState::entered, this, &MainWindow::onCalibrationScanControlStateRegOffEntered));

    QState* recalibrationHandControlState = new QState(recalibrationState);
    recalibrationHandControlState->setObjectName("recalibrationHandControlState");
    ASSERT(connect(recalibrationHandControlState, &QState::entered, this, &MainWindow::onCalibrationHandControlStateRegOffEntered));
    recalibrationState->setInitialState(recalibrationScanControlState);

    QState* scanState = new QState(_registrationOffState);
    scanState->setObjectName("scanState");
    ASSERT(connect(scanState, &QState::entered, this, &MainWindow::onScanStateRegOffEntered));

    QState* bScanState = new QState(scanState);
    bScanState->setObjectName("bScanState");
    ASSERT(connect(bScanState, &QState::entered, this, &MainWindow::onBScanStateRegOffEntered));
    ASSERT(connect(bScanState, &QState::exited, this, &MainWindow::onBScanStateRegOffExited));
    scanState->setInitialState(bScanState);

    QState* mScanState = new QState(scanState);
    mScanState->setObjectName("mScanState");
    ASSERT(connect(mScanState, &QState::entered, this, &MainWindow::onMScanStateRegOffEntered));
    ASSERT(connect(mScanState, &QState::exited, this, &MainWindow::onMScanStateRegOffExited));

    QState* evaluationdState = new QState(scanState);
    evaluationdState->setObjectName("evaluationdState");
    ASSERT(connect(evaluationdState, &QState::entered, this, &MainWindow::onEvaluationStateRegOffEntered));
    ASSERT(connect(evaluationdState, &QState::exited, this, &MainWindow::onEvaluationStateRegOffExited));

    QState* handState = new QState(_registrationOffState);
    handState->setObjectName("handState");
    ASSERT(connect(handState, &QState::entered, this, &MainWindow::onHandStateRegOffEntered));
    ASSERT(connect(handState, &QState::exited, this, &MainWindow::onHandStateRegOffExited));

    QState* headScannerBScanState = new QState(_registrationOffState);
    headScannerBScanState->setObjectName("headScannerBScanState");
    ASSERT(connect(headScannerBScanState, &QState::entered, this, &MainWindow::onHeadScannerBScanStateRegOffEntered));

    QState* headScannerEvaluationState = new QState(_registrationOffState);
    headScannerEvaluationState->setObjectName("headScannerEvaluationState");
    ASSERT(connect(headScannerEvaluationState, &QState::entered, this, &MainWindow::onHeadScannerEvaluationStateRegOffEntered));

    QState* headScannerViewResultState = new QState(_registrationOffState);
    headScannerViewResultState->setObjectName("headScannerViewResultState");
    ASSERT(connect(headScannerViewResultState, &QState::entered, this, &MainWindow::onHeadScannerViewResultStateRegOffEntered));

    QState* serviceMarkState = new QState(_registrationOffState);
    serviceMarkState->setObjectName("serviceMarkState");
    ASSERT(connect(serviceMarkState, &QState::entered, this, &MainWindow::onServiceMarkStateRegOffEntered));
    ASSERT(connect(serviceMarkState, &QState::exited, this, &MainWindow::onServiceMarkStateRegOffExited));

    ASSERT(menuState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(menuState->addTransition(ui->handScanDoubleButton, &QPushButton::released, handState));
    ASSERT(menuState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(menuState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(menuState->addTransition(this, &MainWindow::doNewCalibration, newCalibrationState));
    ASSERT(menuState->addTransition(this, &MainWindow::doRecalibration, recalibrationState));
#if !defined TARGET_AVICONDBHS
    ASSERT(menuState->addTransition(ui->selectCalibrationButton, &QPushButton::pressed, bScanState));
#else
    ASSERT(menuState->addTransition(ui->selectCalibrationButton, &QPushButton::pressed, handState));
#endif
    ASSERT(newCalibrationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(newCalibrationScanControlState->addTransition(ui->handButton, &QPushButton::pressed, newCalibrationHandControlState));

#if defined(TARGET_AVICONDBHS)
    ASSERT(newCalibrationScanControlState->addTransition(newCalibrationScanControlState, &QState::entered, newCalibrationHandControlState));
    ASSERT(newCalibrationHandControlState->addTransition(ui->handButton, &QPushButton::pressed, handState));
#endif
    ASSERT(newCalibrationScanControlState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(newCalibrationHandControlState->addTransition(ui->scanButton, &QPushButton::pressed, newCalibrationScanControlState));
    ASSERT(recalibrationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(recalibrationScanControlState->addTransition(ui->handButton, &QPushButton::pressed, recalibrationHandControlState));
#if defined(TARGET_AVICONDBHS)
    ASSERT(recalibrationScanControlState->addTransition(recalibrationScanControlState, &QState::entered, recalibrationHandControlState));
    ASSERT(recalibrationHandControlState->addTransition(ui->handButton, &QPushButton::pressed, handState));
#endif


    ASSERT(recalibrationScanControlState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(recalibrationHandControlState->addTransition(ui->scanButton, &QPushButton::pressed, recalibrationScanControlState));

    ASSERT(bScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(bScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    if (_deviceType != Avicon31Estonia) {
        ASSERT(bScanState->addTransition(ui->scanButton, &QPushButton::pressed, mScanState));
    }

    ASSERT(bScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationdState));
    ASSERT(bScanState->addTransition(this, &MainWindow::backToMenuRegOffState, menuState));

    ASSERT(mScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(mScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(mScanState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(mScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationdState));

    ASSERT(evaluationdState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(evaluationdState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(evaluationdState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(handState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(handState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));

    if (_deviceType == Avicon31Default) {
        ASSERT(handState->addTransition(ui->handButton, &QPushButton::pressed, headScannerBScanState));
        ASSERT(handState->addTransition(this, &MainWindow::backToMenuRegOffState, menuState));

        ASSERT(headScannerBScanState->addTransition(this, &MainWindow::noUseHeadScanner, handState));
        ASSERT(headScannerBScanState->addTransition(this, &MainWindow::doSetHeadScannerChannel, headScannerEvaluationState));
        ASSERT(headScannerBScanState->addTransition(_av17Page, &Av17Page::doStartAv17DefView, headScannerViewResultState));
        ASSERT(headScannerBScanState->addTransition(ui->backPushButton, &QPushButton::released, bScanState));
        ASSERT(headScannerBScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));

        ASSERT(headScannerEvaluationState->addTransition(ui->backPushButton, &QPushButton::released, headScannerBScanState));

        ASSERT(headScannerViewResultState->addTransition(ui->backPushButton, &QPushButton::released, headScannerBScanState));
        ASSERT(headScannerViewResultState->addTransition(_av17DefView, &Av17DefView::doNextButtonPressed, bScanState));
    }
}

QState* MainWindow::initFsmRegOnStates()
{
    _registrationOnState = new QState(_registrationState);
    _registrationOnState->setObjectName("registrationOnState");
    ASSERT(connect(_registrationOnState, &QState::entered, this, &MainWindow::onRegistrationOnStateEntered));
    ASSERT(connect(_registrationOnState, &QState::exited, this, &MainWindow::onRegistrationOnStateExited));

    QState* registrationOnInnerState = new QState(_registrationOnState);
    _registrationOnState->setObjectName("registrationOnState");
    _registrationOnState->setInitialState(registrationOnInnerState);

    QState* menuState = new QState(registrationOnInnerState);
    menuState->setObjectName("menuState");
    ASSERT(connect(menuState, &QState::entered, this, &MainWindow::onMenuStateRegOnEntered));
    ASSERT(connect(menuState, &QState::exited, this, &MainWindow::onMenuStateRegOnExited));

    QState* recalibrationState = new QState(_registrationOnState);
    recalibrationState->setObjectName("recalibrationState");
    ASSERT(connect(recalibrationState, &QState::entered, this, &MainWindow::onRecalibrationStateRegOffEntered));
    ASSERT(connect(recalibrationState, &QState::exited, this, &MainWindow::onCalibrationStateRegOffExited));

    QState* recalibrationScanControlState = new QState(recalibrationState);
    recalibrationScanControlState->setObjectName("recalibrationScanControlState");
    ASSERT(connect(recalibrationScanControlState, &QState::entered, this, &MainWindow::onCalibrationScanControlStateRegOffEntered));

    QState* recalibrationHandControlState = new QState(recalibrationState);
    recalibrationHandControlState->setObjectName("recalibrationHandControlState");
    ASSERT(connect(recalibrationHandControlState, &QState::entered, this, &MainWindow::onCalibrationHandControlStateRegOffEntered));
    recalibrationState->setInitialState(recalibrationScanControlState);

    QState* bScanState = new QState(registrationOnInnerState);
    bScanState->setObjectName("bScanState");
#if !defined TARGET_AVICONDBHS
    registrationOnInnerState->setInitialState(bScanState);
#endif
    ASSERT(connect(bScanState, &QState::entered, this, &MainWindow::onBScanStateRegOnEntered));
    ASSERT(connect(bScanState, &QState::exited, this, &MainWindow::onBScanStateRegOnExited));

    QState* mScanState = new QState(registrationOnInnerState);
    mScanState->setObjectName("mScanState");
    ASSERT(connect(mScanState, &QState::entered, this, &MainWindow::onMScanStateRegOnEntered));
    ASSERT(connect(mScanState, &QState::exited, this, &MainWindow::onMScanStateRegOnExited));

    QState* evaluationState = new QState(registrationOnInnerState);
    evaluationState->setObjectName("evaluationState");
    ASSERT(connect(evaluationState, &QState::entered, this, &MainWindow::onEvaluationStateRegOnEntered));

    QState* handState = new QState(registrationOnInnerState);
    handState->setObjectName("handState");
#if defined TARGET_AVICONDBHS
    registrationOnInnerState->setInitialState(handState);
#endif
    ASSERT(connect(handState, &QState::entered, this, &MainWindow::onHandStateRegOnEntered));
    ASSERT(connect(handState, &QState::exited, this, &MainWindow::onHandStateRegOnExited));
#if defined TARGET_AVICON31
    QState* headScanRegOptionsStartState = new QState(_registrationOnState);
    handState->setObjectName("headScanRegOptionsStartState");
    ASSERT(connect(headScanRegOptionsStartState, &QState::entered, this, &MainWindow::onHeadScanRegOptionsStartStateRegOnEntered));

    QState* headScanRegOptionsFinishState = new QState(_registrationOnState);
    handState->setObjectName("headScanRegOptionsFinishState");
    ASSERT(connect(headScanRegOptionsFinishState, &QState::entered, this, &MainWindow::onHeadScanRegOptionsFinishStateRegOnEntered));
    ASSERT(connect(headScanRegOptionsFinishState, &QState::exited, this, &MainWindow::onHeadScanRegOptionsFinishStateRegOnExited));

    QState* headScannerBScanState = new QState(_registrationOnState);
    headScannerBScanState->setObjectName("headScannerBScanState");
    ASSERT(connect(headScannerBScanState, &QState::entered, this, &MainWindow::onHeadScannerBScanStateRegOnEntered));

    QState* headScannerEvaluationState = new QState(_registrationOnState);
    headScannerEvaluationState->setObjectName("headScannerEvaluationState");
    ASSERT(connect(headScannerEvaluationState, &QState::entered, this, &MainWindow::onHeadScannerEvaluationStateRegOnEntered));

    QState* headScannerViewResultState = new QState(_registrationOnState);
    headScannerViewResultState->setObjectName("headScannerViewResultState");
    ASSERT(connect(headScannerViewResultState, &QState::entered, this, &MainWindow::onHeadScannerViewResultStateRegOnEntered));
#endif
    QHistoryState* markHistoryState = new QHistoryState(registrationOnInnerState);
    markHistoryState->setObjectName("markHistoryState");

    QState* notesMenuPageState = new QState(_registrationOnState);
    notesMenuPageState->setObjectName("notesMenuPageState");
    ASSERT(connect(notesMenuPageState, &QState::entered, this, &MainWindow::onNotesMenuPageStateRegOnEntered));
    ASSERT(connect(notesMenuPageState, &QState::exited, this, &MainWindow::onNotesMenuPageStateRegOnExited));

    QState* trackMarkState = new QState(_registrationOnState);
    trackMarkState->setObjectName("trackMarkState");
    ASSERT(connect(trackMarkState, &QState::entered, this, &MainWindow::onTrackMarkStateRegOnEntered));
    ASSERT(connect(trackMarkState, &QState::exited, this, &MainWindow::onTrackMarkStateRegOnExited));

    QState* serviceMarkState = new QState(_registrationOnState);
    serviceMarkState->setObjectName("serviceMarkState");
    ASSERT(connect(serviceMarkState, &QState::entered, this, &MainWindow::onServiceMarkStateRegOnEntered));

    QState* pauseState = new QState(_registrationOnState);
    pauseState->setObjectName("pauseState");
    ASSERT(connect(pauseState, &QState::entered, this, &MainWindow::onPauseStateRegOnEntered));
    ASSERT(connect(pauseState, &QState::exited, this, &MainWindow::onPauseStateRegOnExited));

    ASSERT(menuState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(menuState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(menuState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(menuState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(menuState->addTransition(this, &MainWindow::doRecalibration, recalibrationState));

#if defined(TARGET_AVICONDBHS)
    ASSERT(recalibrationScanControlState->addTransition(recalibrationScanControlState, &QState::entered, recalibrationHandControlState));
    ASSERT(recalibrationHandControlState->addTransition(ui->handButton, &QPushButton::pressed, handState));
#endif

    ASSERT(recalibrationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(recalibrationScanControlState->addTransition(ui->handButton, &QPushButton::pressed, recalibrationHandControlState));
    ASSERT(recalibrationScanControlState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(recalibrationHandControlState->addTransition(ui->scanButton, &QPushButton::pressed, recalibrationScanControlState));

    if (_deviceType != Avicon31Estonia) {
        ASSERT(bScanState->addTransition(ui->scanButton, &QPushButton::pressed, mScanState));
    }
    ASSERT(bScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(bScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(bScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationState));

    ASSERT(mScanState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(mScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(mScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(mScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationState));

    ASSERT(evaluationState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(evaluationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(evaluationState->addTransition(ui->handButton, &QPushButton::pressed, handState));

    QSignalTransition* trans = handState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState);
    Q_ASSERT(trans != nullptr);
    ASSERT(connect(trans, &QSignalTransition::triggered, this, &MainWindow::onTransitionFromHandToBScanStateTriggered));
    ASSERT(handState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
#if defined TARGET_AVICON31
    ASSERT(handState->addTransition(ui->handButton, &QPushButton::pressed, headScanRegOptionsStartState));

    ASSERT(headScanRegOptionsStartState->addTransition(this, &MainWindow::noUseHeadScanner, handState));
    ASSERT(headScanRegOptionsStartState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(headScanRegOptionsStartState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(headScanRegOptionsStartState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(headScanRegOptionsStartState->addTransition(_scannerDefectRegistrationPage, &ScannerDefectRegistrationPage::startWork, headScannerBScanState));
    ASSERT(headScanRegOptionsStartState->addTransition(ui->backPushButton, &QPushButton::released, bScanState));

    ASSERT(headScannerBScanState->addTransition(this, &MainWindow::doSetHeadScannerChannel, headScannerEvaluationState));
    ASSERT(headScannerBScanState->addTransition(_av17Page, &Av17Page::doStartAv17DefView, headScannerViewResultState));
    ASSERT(headScannerBScanState->addTransition(ui->backPushButton, &QPushButton::released, bScanState));

    ASSERT(headScannerEvaluationState->addTransition(ui->backPushButton, &QPushButton::released, headScannerBScanState));

    ASSERT(headScannerViewResultState->addTransition(ui->backPushButton, &QPushButton::released, headScannerBScanState));
    ASSERT(headScannerViewResultState->addTransition(_av17DefView, &Av17DefView::doNextButtonPressed, headScanRegOptionsFinishState));

    ASSERT(headScanRegOptionsFinishState->addTransition(ui->backPushButton, &QPushButton::released, headScannerViewResultState));
    ASSERT(headScanRegOptionsFinishState->addTransition(_scannerDefectRegistrationPage, &ScannerDefectRegistrationPage::saveResults, bScanState));
#endif

    ASSERT(notesMenuPageState->addTransition(this, &MainWindow::trackMarksSelected, markHistoryState));
    ASSERT(notesMenuPageState->addTransition(_switchOperatorWidget, &SwitchOperatorWidget::closeWidget, markHistoryState));

    ASSERT(notesMenuPageState->addTransition(this, &MainWindow::backFromNotesMenuPageState, markHistoryState));
    ASSERT(notesMenuPageState->addTransition(ui->trackMarksButton, &QPushButton::pressed, trackMarkState));
    ASSERT(notesMenuPageState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(notesMenuPageState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(notesMenuPageState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(notesMenuPageState->addTransition(ui->startSwitchButton, &QPushButton::pressed, markHistoryState));
    ASSERT(notesMenuPageState->addTransition(ui->endSwitchButton, &QPushButton::pressed, markHistoryState));
    ASSERT(notesMenuPageState->addTransition(_switchTypePage, &SwitchTypePage::doStartSwitchControl, bScanState));

    ASSERT(trackMarkState->addTransition(this, &MainWindow::trackMarksSelected, markHistoryState));
    ASSERT(trackMarkState->addTransition(this, &MainWindow::trackMarkExternalKeyboard_released, markHistoryState));
    trans = trackMarkState->addTransition(this, &MainWindow::backFromNotesMenuPageState, markHistoryState);
    Q_ASSERT(trans != nullptr);
    ASSERT(connect(trans, &QSignalTransition::triggered, this, &MainWindow::onRollBackTrackMarkStateTriggered));
    ASSERT(trackMarkState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(trackMarkState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(trackMarkState->addTransition(ui->handButton, &QPushButton::pressed, handState));

    ASSERT(serviceMarkState->addTransition(this, &MainWindow::trackMarksSelected, markHistoryState));
    ASSERT(serviceMarkState->addTransition(this, &MainWindow::backFromServiceMarkPage, markHistoryState));
    ASSERT(serviceMarkState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(serviceMarkState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(serviceMarkState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(serviceMarkState->addTransition(ui->startSwitchButton, &QPushButton::pressed, markHistoryState));
    ASSERT(serviceMarkState->addTransition(ui->endSwitchButton, &QPushButton::pressed, markHistoryState));
    ASSERT(serviceMarkState->addTransition(_switchTypePage, &SwitchTypePage::doStartSwitchControl, bScanState));

    ASSERT(pauseState->addTransition(ui->pauseButton, &QPushButton::pressed, markHistoryState));
    ASSERT(pauseState->addTransition(_waitMessagePage, &WaitMessagePage::clickWidget, markHistoryState));
    ASSERT(pauseState->addTransition(this, &MainWindow::backFromNotesMenuPageState, markHistoryState));

    ASSERT(registrationOnInnerState->addTransition(ui->notesButton, &QPushButton::pressed, notesMenuPageState));
    ASSERT(registrationOnInnerState->addTransition(this, &MainWindow::trackMarkExternalKeyboard_released, trackMarkState));
    ASSERT(registrationOnInnerState->addTransition(this, &MainWindow::serviceMarkExternalKeyboard_released, serviceMarkState));
    ASSERT(registrationOnInnerState->addTransition(ui->pauseButton, &QPushButton::pressed, pauseState));

    return _registrationOnState;
}

QState* MainWindow::initFsmRegOnServiceStates()
{
    _registrationOnServiceState = new QState(_registrationState);
    _registrationOnServiceState->setObjectName("registrationOnServiceState");
    ASSERT(connect(_registrationOnServiceState, &QState::entered, this, &MainWindow::onRegistrationOnServiceStateEntered));
    ASSERT(connect(_registrationOnServiceState, &QState::exited, this, &MainWindow::onRegistrationOnServiceStateExited));

    QState* registrationOnInnerState = new QState(_registrationOnServiceState);
    _registrationOnServiceState->setInitialState(registrationOnInnerState);

    QState* menuState = new QState(registrationOnInnerState);
    menuState->setObjectName("menuState");
    ASSERT(connect(menuState, &QState::entered, this, &MainWindow::onMenuStateRegOnEntered));
    ASSERT(connect(menuState, &QState::exited, this, &MainWindow::onMenuStateRegOnExited));

    QState* recalibrationState = new QState(_registrationOnServiceState);
    recalibrationState->setObjectName("recalibrationState");
    ASSERT(connect(recalibrationState, &QState::entered, this, &MainWindow::onRecalibrationStateRegOffEntered));
    ASSERT(connect(recalibrationState, &QState::exited, this, &MainWindow::onCalibrationStateRegOffExited));

    QState* recalibrationScanControlState = new QState(recalibrationState);
    recalibrationScanControlState->setObjectName("recalibrationScanControlState");
    ASSERT(connect(recalibrationScanControlState, &QState::entered, this, &MainWindow::onCalibrationScanControlStateRegOffEntered));

    QState* recalibrationHandControlState = new QState(recalibrationState);
    recalibrationHandControlState->setObjectName("recalibrationHandControlState");
    ASSERT(connect(recalibrationHandControlState, &QState::entered, this, &MainWindow::onCalibrationHandControlStateRegOffEntered));
    recalibrationState->setInitialState(recalibrationScanControlState);

    QState* bScanState = new QState(registrationOnInnerState);
    bScanState->setObjectName("bScanState");
    registrationOnInnerState->setInitialState(bScanState);
    ASSERT(connect(bScanState, &QState::entered, this, &MainWindow::onBScanStateRegOnEntered));

    QState* mScanState = new QState(registrationOnInnerState);
    mScanState->setObjectName("mScanState");
    ASSERT(connect(mScanState, &QState::entered, this, &MainWindow::onMScanStateRegOnEntered));

    QState* evaluationState = new QState(registrationOnInnerState);
    evaluationState->setObjectName("evaluationState");
    ASSERT(connect(evaluationState, &QState::entered, this, &MainWindow::onEvaluationStateRegOnEntered));
    ASSERT(connect(evaluationState, &QState::exited, this, &MainWindow::onEvaluationStateRegOnExited));

    QState* handState = new QState(registrationOnInnerState);
    handState->setObjectName("handState");
    ASSERT(connect(handState, &QState::entered, this, &MainWindow::onHandStateRegOnEntered));
    ASSERT(connect(handState, &QState::exited, this, &MainWindow::onHandStateRegOnExited));

    QHistoryState* markHistoryState = new QHistoryState(registrationOnInnerState);
    markHistoryState->setObjectName("markHistoryState");

    QState* serviceMarkState = new QState(_registrationOnServiceState);
    serviceMarkState->setObjectName("serviceMarkState");
    ASSERT(connect(serviceMarkState, &QState::entered, this, &MainWindow::onServiceMarkStateRegOnServiceEntered));

    QState* pauseState = new QState(_registrationOnServiceState);
    pauseState->setObjectName("pauseState");
    ASSERT(connect(pauseState, &QState::entered, this, &MainWindow::onPauseStateRegOnEntered));
    ASSERT(connect(pauseState, &QState::exited, this, &MainWindow::onPauseStateRegOnExited));

    ASSERT(menuState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(menuState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(menuState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(menuState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(menuState->addTransition(ui->notesButton, &QPushButton::pressed, serviceMarkState));
    ASSERT(menuState->addTransition(this, &MainWindow::doRecalibration, recalibrationState));

    ASSERT(recalibrationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));

    ASSERT(recalibrationScanControlState->addTransition(ui->handButton, &QPushButton::pressed, recalibrationHandControlState));

    ASSERT(recalibrationScanControlState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));

    ASSERT(recalibrationHandControlState->addTransition(ui->scanButton, &QPushButton::pressed, recalibrationScanControlState));

    ASSERT(bScanState->addTransition(ui->scanButton, &QPushButton::pressed, mScanState));
    ASSERT(bScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(bScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(bScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationState));
    ASSERT(bScanState->addTransition(ui->notesButton, &QPushButton::pressed, serviceMarkState));

    ASSERT(mScanState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(mScanState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(mScanState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(mScanState->addTransition(this, &MainWindow::doSetScanChannel, evaluationState));
    ASSERT(mScanState->addTransition(ui->notesButton, &QPushButton::pressed, serviceMarkState));

    ASSERT(evaluationState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(evaluationState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(evaluationState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(evaluationState->addTransition(ui->notesButton, &QPushButton::pressed, serviceMarkState));

    QSignalTransition* trans = handState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState);
    Q_ASSERT(trans != nullptr);
    ASSERT(connect(trans, &QSignalTransition::triggered, this, &MainWindow::onTransitionFromHandToBScanStateTriggered));
    ASSERT(handState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(handState->addTransition(ui->handButton, &QPushButton::pressed, handState));
    ASSERT(handState->addTransition(ui->notesButton, &QPushButton::pressed, serviceMarkState));

    ASSERT(serviceMarkState->addTransition(ui->notesButton, &QPushButton::pressed, markHistoryState));
    ASSERT(serviceMarkState->addTransition(this, &MainWindow::backFromServiceMarkPage, markHistoryState));
    ASSERT(serviceMarkState->addTransition(ui->controlSectionForwardButton, &QPushButton::pressed, markHistoryState));
    ASSERT(serviceMarkState->addTransition(ui->controlSectionBackwardButton, &QPushButton::pressed, markHistoryState));

    ASSERT(serviceMarkState->addTransition(ui->scanButton, &QPushButton::pressed, bScanState));
    ASSERT(serviceMarkState->addTransition(ui->menuButton, &QPushButton::pressed, menuState));
    ASSERT(serviceMarkState->addTransition(ui->handButton, &QPushButton::pressed, handState));

    ASSERT(pauseState->addTransition(ui->pauseButton, &QPushButton::pressed, markHistoryState));

    ASSERT(registrationOnInnerState->addTransition(this, &MainWindow::serviceMarkExternalKeyboard_released, serviceMarkState));
    ASSERT(registrationOnInnerState->addTransition(ui->pauseButton, &QPushButton::pressed, pauseState));

    return _registrationOnServiceState;
}

void MainWindow::initBoltJointStates()
{
    QState* boltJointState = new QState(_stateMachine);
    boltJointState->setObjectName("boltJointState");

    QState* initialBoltJointState = new QState(boltJointState);
    initialBoltJointState->setObjectName("initialBoltJointState");
    ASSERT(connect(initialBoltJointState, &QState::entered, this, &MainWindow::onInitialBoltJointStateEntered));
    boltJointState->setInitialState(initialBoltJointState);

    QState* startBoltJointState = new QState(boltJointState);
    startBoltJointState->setObjectName("startBoltJointState");
    ASSERT(connect(startBoltJointState, &QState::entered, this, &MainWindow::onStartBoltJointStateEntered));

    QState* stopBoltJointState = new QState(boltJointState);
    stopBoltJointState->setObjectName("stopBoltJointState");
    ASSERT(connect(stopBoltJointState, &QState::entered, this, &MainWindow::onStopBoltJointStateEntered));

    initialBoltJointState->addTransition(ui->boltJointButton, &QPushButton::released, startBoltJointState);
    initialBoltJointState->addTransition(this, &MainWindow::doStartBoltJoint, startBoltJointState);
    startBoltJointState->addTransition(ui->boltJointButton, &QPushButton::released, stopBoltJointState);
    stopBoltJointState->addTransition(ui->boltJointButton, &QPushButton::released, startBoltJointState);
    startBoltJointState->addTransition(this, &MainWindow::doStopBoltJoint, stopBoltJointState);
    stopBoltJointState->addTransition(this, &MainWindow::doStartBoltJoint, startBoltJointState);
}

void MainWindow::updateTrackMarksWidgets()
{
    if (Core::instance().getTrackMarks() != nullptr) {
        viewCurrentCoordinate();
        if (Core::instance().getTrackMarks()->getDirection() == 1) {
            ui->directionLabel->setText(QString(0x25b2));
        }
        else if (Core::instance().getTrackMarks()->getDirection() == -1) {
            ui->directionLabel->setText(QString(0x25bc));
        }
    }
}

void MainWindow::switchToMode(const QString& mode)
{
    QFile file("/etc/run");
    file.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly);
    file.write(mode.toLatin1());
    file.close();
#if defined TARGET_AVICON31
    quitFromApplication();
#endif
}

void MainWindow::showVirtualKeyboardsWithLineEditAndValidatorNumbers(const QString& value)
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::Numbers);
    virtualKeyboards->setValue(value);
    virtualKeyboards->show();
}

void MainWindow::showVirtualKeyboardsWithLineEditAndValidatorNone(const QString& value)
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(value);
    virtualKeyboards->show();
}

void MainWindow::showVirtualKeyboardsWithPlainTextAndValidatorNone(const QString& value)
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::PlainTextEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(value);
    virtualKeyboards->show();
}

void MainWindow::showVirtualKeyboardsWithLineEditAndValidatorDate(const QString& value)
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::Date);
    virtualKeyboards->setValue(value);
    virtualKeyboards->show();
}

void MainWindow::checkLastServiceDate(bool showNotifier)
{
    QDir dir(registrationServicePath());
    QFileInfoList fileList;
    QString pattern = QString("s") + QString::number(restoreSchemeNumber());

    if (dir.exists()) {
        fileList = dir.entryInfoList(QDir::Files);
    }

    QString fileName;
    for (const QFileInfo& fileInfo : fileList) {
        if (fileInfo.baseName().contains(pattern)) {
            ui->serviceDateLabel->setText(fileInfo.created().toString("dd.MM.yyyy"));
            fileName = fileInfo.baseName();
            break;
        }
    }

    if (fileName.isEmpty()) {
        ui->serviceDateLabel->setText(tr("not performed"));
        if (showNotifier) {
            Notifier::instance().addNote(Qt::red, tr("First of all, write control section of track!"));
        }
    }
}

void MainWindow::configureUiForAviconDbAviconDbHs()
{
    _optionsListPage->setItemVisible("changeSchemeButton", false);
    _optionsListPage->setItemVisible("wifiManagerButton", false);
    _optionsListPage->setItemVisible("regarOptionsButton", false);
    _optionsListPage->setItemVisible("autoGainAdjustmentOptionsButton", false);
    _optionsListPage->setItemVisible("bluetoothOptionsButton", false);
    ui->cduBattery->hide();
    ui->cduBatteryTitleLabel->hide();
    ui->schemeNumberTitleLabel->hide();
    ui->schemeNumberLabel->hide();
    ui->cduTemperatureTitleLabel->hide();
    ui->temperature->hide();
    ui->lastServiceDateTitleLabel->hide();
    ui->serviceDateLabel->hide();
    ui->calibrationValidityTitleLabel->hide();
    ui->calibrationValidityLabel->hide();
    ui->coordinatesButton->hide();
    ui->maxBaseSensButton->hide();
    ui->rebootButton->hide();
    ui->powerOffButton->hide();
    ui->serviceModeButton->hide();
    ui->updateModeButton->hide();
    ui->memoryPage->configureUiForAviconDbAviconDbHs();
    ui->aScanPage->configureUiForAviconDbAviconDbHs();
    ui->setTimePage->configureUiForAviconDbAviconDbHs();

    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    font.setWeight(75);
    ui->bottomPanel->setContentsMargins(5, 0, 5, 5);
    ui->backPushButton->setFont(font);
    ui->menuButton->setFont(font);
    ui->boltJointButton->setFont(font);
    ui->notesButton->setFont(font);
    ui->railTypeButton->setFont(font);
    ui->handButton->setFont(font);
    ui->pauseButton->setFont(font);
    ui->scanButton->setFont(font);
    ui->showHideControlsButton->setFont(font);
}

void MainWindow::configureUiForAviconDb()
{
    configureUiForAviconDbAviconDbHs();
    ui->serviceMarksAudioCommentsButton->hide();
    ui->serviceMarksPhotoVideButton->hide();
    ui->bScanPage->setBottomSignalWigetVisible(true);
    ui->trolleyLeftSidePageButton->show();
    ui->controledRailBarLabel->hide();
    _registrationPage->setDeviceType(AviconDB);
}

void MainWindow::configureUiForAviconDbHs()
{
    _optionsListPage->setItemVisible("mScanSettingsButton", false);
    _optionsListPage->setItemVisible("boltJointSettingsButton", false);
    configureUiForAviconDbAviconDbHs();
    ui->tableOfCalibrationsButton->hide();
    ui->encoderCorrectionButton->hide();
    ui->minThresholdLabel->hide();
    ui->registrationThresholdSpinBox->hide();
    ui->memoryPage->configureUiForAviconDbAviconDbHs();
    ui->aScanPage->configureUiForAviconDbAviconDbHs();
    ui->setTimePage->configureUiForAviconDbAviconDbHs();
    ui->controledRailBarLabel->show();
    ui->directionLabel->hide();
    ui->line_2->hide();
    ui->speedLabel->hide();
    _registrationPage->setDeviceType(AviconDBHS);
    ASSERT(connect(_registrationPage, &RegistrationPage::controlRailChanged, ui->aScanPage, &AScanPage::onSetControledRail));
}

void MainWindow::configureUiForAvicon15()
{
    _optionsListPage->setItemVisible("ftpSettingsButton", true);
    _optionsListPage->setItemVisible("autoGainAdjustmentOptionsButton", false);
    _optionsListPage->setItemVisible("regarOptionsButton", false);
    _optionsListPage->setItemVisible("changeSchemeButton", false);
    _optionsListPage->setItemVisible("setTimeButton", false);
    _optionsListPage->setItemVisible("setDateButton", false);
    _optionsListPage->setItemVisible("bluetoothOptionsButton", true);
    _optionsListPage->setItemVisible("impotantAreaSettingPushButton", false);
    _optionsListPage->setItemVisible("notificationOptionsButton", false);
    _optionsListPage->setItemVisible("acousticContactOptionsPageButton", false);
    _optionsListPage->setItemVisible("calibrationInfoButton", false);
    ui->statusBarWidget->setStyleSheet("background-color: black;");
    ui->bottomPanel->setAutoFillBackground(true);
    ui->schemeNumberTitleLabel->hide();
    ui->schemeNumberLabel->hide();
    ui->cduTemperatureTitleLabel->hide();
    ui->temperature->hide();
    ui->lastServiceDateTitleLabel->hide();
    ui->serviceDateLabel->hide();
    ui->calibrationValidityTitleLabel->hide();
    ui->calibrationValidityLabel->hide();
    ui->maxBaseSensButton->hide();
    ui->rebootButton->hide();
    ui->powerOffButton->hide();
    ui->serviceModeButton->hide();
    ui->updateModeButton->hide();
    ui->aScanPage->configureUiForAvicon15();
    ui->startSwitchButton->hide();
    ui->endSwitchButton->hide();
    _registrationPage->setDeviceType(Avicon15);
    ui->cduBattery->hide();
    ui->cduBatteryTitleLabel->hide();
    ui->memoryButtonImport->hide();
}

void MainWindow::configureUiForAvicon31Estonia()
{
    _optionsListPage->setItemVisible("parallelVideoBrowsingOptionsButton", true);
    _optionsListPage->setItemVisible("impotantAreaSettingPushButton", false);
    _optionsListPage->setItemVisible("autoGainAdjustmentOptionsButton", true);
    _optionsListPage->setItemVisible("regarOptionsButton", true);
    _optionsListPage->setItemVisible("keyCombinationsOptionsButton", true);
    _optionsListPage->setItemVisible("bluetoothOptionsButton", false);
    _optionsListPage->setItemVisible("boltJointSettingsButton", false);
    _optionsListPage->setItemVisible("screenHeaterSettingsButton", false);
    _optionsListPage->setItemVisible("batterySettingsButton", false);
    _optionsListPage->setItemVisible("wifiManagerButton", false);
    _optionsListPage->setItemVisible("mScanSettingsButton", false);
    _optionsListPage->setItemVisible("changeSchemeButton", false);
    _optionsListPage->setItemVisible("setDateButton", false);
    _optionsListPage->setItemVisible("setTimeButton", false);
    ui->cduBatteryTitleLabel->hide();
    ui->cduBattery->hide();
    ui->cduTemperatureTitleLabel->hide();
    ui->temperature->hide();
    ui->buiBatteryLabel->hide();
    _registrationPage->setDeviceType(Avicon31Estonia);
    ui->boltJointButton->hide();
    ui->pauseButton->hide();
    ui->showHideControlsButton->hide();
    ui->serviceMarksPhotoVideButton->hide();
    ui->serviceMarksAudioCommentsButton->hide();
    ui->cduBattery->hide();
    ui->cduBatteryTitleLabel->hide();
    ui->lastServiceDateTitleLabel->hide();
    ui->serviceDateLabel->hide();
    ui->bluetoothIndicator->hide();
    ui->wifiIndicator->hide();
    ui->maxBaseSensButton->hide();
    ui->handButton->hide();
    ui->bScanPage->setControlsAreaVisible(true);
    ui->aScanPage->setControlsAreaVisible(true);
    ui->viewTypeLabel->show();
    ui->viewCurrentCoordinateSpinBox->show();
    ui->memoryButton->hide();
    ui->memoryButtonImport->hide();
}

void MainWindow::configureBottomPanelForAndroid()
{
    ui->bottomPanel->setContentsMargins(5, 0, 5, 5);
    ui->bottomPanel->show();

    ui->backPushButton->setMaximumHeight(45);
    ui->backPushButton->setAutoFillBackground(true);
    ui->backPushButton->setText(ui->backPushButton->text() + QString("\n1"));

    ui->menuButton->setMaximumHeight(45);
    ui->menuButton->setAutoFillBackground(true);
    ui->menuButton->setText(ui->menuButton->text() + QString("\n2"));

    ui->boltJointButton->setMaximumHeight(45);
    ui->boltJointButton->setAutoFillBackground(true);
    ui->boltJointButton->setText(ui->boltJointButton->text() + QString("\n3"));

    ui->notesButton->setMaximumHeight(45);
    ui->notesButton->setAutoFillBackground(true);
    ui->notesButton->setText(ui->notesButton->text() + QString("\n4"));

    ui->railTypeButton->setMaximumHeight(45);
    ui->railTypeButton->setAutoFillBackground(true);
    ui->railTypeButton->setText(ui->railTypeButton->text() + QString("\n5"));

    ui->handButton->setMaximumHeight(45);
    ui->handButton->setAutoFillBackground(true);
    ui->handButton->setText(ui->handButton->text() + QString("\n6"));

    ui->pauseButton->setMaximumHeight(45);
    ui->pauseButton->setAutoFillBackground(true);
    ui->pauseButton->setText(ui->pauseButton->text() + QString("\n7"));

    ui->scanButton->setMaximumHeight(45);
    ui->scanButton->setAutoFillBackground(true);
    ui->scanButton->setText(ui->scanButton->text() + QString("\n8"));

    ui->showHideControlsButton->setMaximumHeight(45);
    ui->showHideControlsButton->setAutoFillBackground(true);
    ui->showHideControlsButton->setText(ui->showHideControlsButton->text() + QString("\n9"));
}

void MainWindow::setupPowerOffLabel()
{
    QString text(tr("To turn off, hold the power button. ") + QString(tr("\nIn the menu, select \"Power off\".")));
    ui->powerOffLabel->setText(text);
}

void MainWindow::setLateralPanelsVisibility(bool visible)
{
    ui->leftScanLateralPanelView->setVisible(visible);
    ui->rightScanLateralPanelView->setVisible(visible);
}

void MainWindow::setAcousticContactPanelsVisibility(bool visible)
{
    ui->leftScanTapeAcousticContactView->setVisible(visible);
    ui->rightScanTapeAcousticContactView->setVisible(visible);
}

void MainWindow::clearLateralPanelsSelection()
{
    ui->leftScanLateralPanelView->clearSelection();
    ui->rightScanLateralPanelView->clearSelection();
}

void MainWindow::setupTrackMarks(Core& core)
{
    switch (core.getSystemCoordType()) {
    case csMetricRF:
        _trackMarksPage = new TrackMarksRussian();
        _trackMarksPage->setTrackMarks(Core::instance().getTrackMarks());
        ASSERT(connect(_trackMarksPage, &TrackMarksPage::selected, this, &MainWindow::onTrackMarksSelected));
        ui->stackedWidget->addWidget(_trackMarksPage);
        break;
    case csMetric1km:
        _trackMarksPage = new TrackMarksMetric1KM();
        _trackMarksPage->setTrackMarks(Core::instance().getTrackMarks());
        ASSERT(connect(_trackMarksPage, &TrackMarksPage::selected, this, &MainWindow::onTrackMarksSelected));
        ui->stackedWidget->addWidget(_trackMarksPage);
        break;
    case csImperial:
    case csMetric0_1km:
    case csMetric1kmTurkish:
        break;
    }

    updateTrackMarksWidgets();
}

void MainWindow::setVisibleSpeedLabel(bool isVisible)
{
#if !defined TARGET_AVICONDBHS
    ui->speedLabel->setVisible(isVisible);
#endif
}

void MainWindow::hideServiceMarksForDBHS()
{
    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionForwardButton->setVisible(false);
    ui->serviceMarksBridgeButton->setVisible(false);
    ui->serviceMarksPlatformButton->setVisible(false);
    ui->serviceMarksSwitchNumberButton->setVisible(false);
    ui->serviceMarksRailroadCrossingButton->setVisible(false);
    ui->serviceMarksMiscButton->setVisible(false);
    ui->startSwitchButton->setVisible(false);
    ui->endSwitchButton->setVisible(false);
    ui->serviceMarksBranchButton->setVisible(false);
    ui->serviceMarksSlipButton->setVisible(false);
    ui->serviceMarksAudioCommentsButton->setVisible(false);
    ui->serviceMarksPhotoVideButton->setVisible(false);
}

void MainWindow::printExitCode()
{
    if (_exitCode != 0) {
        Notifier::instance().addNote(Qt::red, QString("Previous exit code: ") + QString::number(_exitCode));
        qDebug() << QString("PREVIOUS EXIT CODE: %1").arg(_exitCode);
        saveCountCrashes(restoreCountCrashes() + 1);
        qDebug() << QString("COUNT OF SOFTWARE CRASHES: ") << QString::number(restoreCountCrashes());
    }
}

void MainWindow::setEnabledBoltJointState(bool isEnabled)
{
    ui->boltJointButton->setEnabled(isEnabled);
    _enabledBoltJointState = isEnabled;
}


void MainWindow::connectRemoteControlSignals()
{
    Core* core = &Core::instance();
#ifndef TARGET_AVICON15
    ASSERT(connect(core, &Core::doRemoteControlConnected, this, &MainWindow::onRemoteControlConnected));
    ASSERT(connect(core, &Core::doRemoteControlDisconnected, this, &MainWindow::onRemoteControlDisconnected));
#endif
    ASSERT(connect(core, &Core::doUpdateRemoteState, this, &MainWindow::onUpdateRemoteState));
    ASSERT(connect(core, &Core::doTmSelectedFromRemoteControl, this, &MainWindow::onTmSelectedFromRemoteControl));
    ASSERT(connect(ui->encoderPage, &EncoderPage::changedSpeed, core, &Core::doChangedSpeed));
    ASSERT(connect(this, &MainWindow::trackMarksSelected, core, &Core::onTrackMarksSelected));
    ASSERT(connect(core, &Core::doSatellitesInUse, this, &MainWindow::onSatellitesInUse));
    ASSERT(connect(core, &Core::doSatellitesInfo, this, &MainWindow::onSatellitesInfo));

    // Remote control transitions
    ASSERT(_registrationOffState->addTransition(core, &Core::doStartRegistration, _registrationOnState));
    ASSERT(_registrationOnState->addTransition(core, &Core::doStopRegistration, _registrationOffState));
}

void MainWindow::disconnectRemoteControlSignals()
{
    Core* core = &Core::instance();
#ifndef TARGET_AVICON15
    disconnect(core, &Core::doRemoteControlConnected, this, &MainWindow::onRemoteControlConnected);
    disconnect(core, &Core::doRemoteControlDisconnected, this, &MainWindow::onRemoteControlDisconnected);
#endif
    disconnect(core, &Core::doUpdateRemoteState, this, &MainWindow::onUpdateRemoteState);
    disconnect(core, &Core::doTmSelectedFromRemoteControl, this, &MainWindow::onTmSelectedFromRemoteControl);
    disconnect(ui->encoderPage, &EncoderPage::changedSpeed, core, &Core::doChangedSpeed);
    disconnect(this, &MainWindow::trackMarksSelected, core, &Core::onTrackMarksSelected);
}

void MainWindow::viewCurrentCoordinate()
{
    switch (_typeView) {
    case KmPkM:
        ui->kmPicketLabel->setText(Core::instance().getTrackMarks()->getString());
        break;
    case KmCommaM:
        ui->kmPicketLabel->setText(Core::instance().getTrackMarks()->getKmString());
        break;
    case Hectometer:
        ui->kmPicketLabel->setText(Core::instance().getTrackMarks()->getHmString());
        break;
    case MeterOnly:
        ui->kmPicketLabel->setText(Core::instance().getTrackMarks()->getMeterString());
        break;
    }
}

void MainWindow::showSplashScreen(QString message)
{
    _splashScreen = new SplashScreen(this);
    _splashScreen->setGeometry(QGuiApplication::screens().first()->availableGeometry());
    _splashScreen->setLogo(QPixmap(":/logo"));
    _splashScreen->setOrganizationName(organisationName().toUpper());
    _splashScreen->setApplicationName(applicationName().toUpper());
    _splashScreen->setApplicationVersion(APP_VERSION);
    _splashScreen->hideProgressBar();
    _splashScreen->setMessage(message);
    _splashScreen->showFullScreen();
    delay(1000);
}

void MainWindow::connectBottomPanelSignals()
{
    ASSERT(connect(ui->backCustomButton, &CustomQLabel::clicked, ui->backPushButton, &QPushButton::released));
    ASSERT(connect(ui->menuCustomButton, &CustomQLabel::clicked, ui->menuButton, &QPushButton::pressed));
    ASSERT(connect(ui->boltJointCustomButton, &CustomQLabel::clicked, ui->boltJointButton, &QPushButton::released));
    ASSERT(connect(ui->marksCustomButton, &CustomQLabel::clicked, ui->notesButton, &QPushButton::pressed));
    ASSERT(connect(ui->railTypeCustomButton, &CustomQLabel::clicked, ui->railTypeButton, &QPushButton::released));
    ASSERT(connect(ui->handCustomButton, &CustomQLabel::clicked, ui->handButton, &QPushButton::pressed));
    ASSERT(connect(ui->pauseCustomButton, &CustomQLabel::clicked, ui->pauseButton, &QPushButton::pressed));
    ASSERT(connect(ui->searchCustomButton, &CustomQLabel::clicked, ui->scanButton, &QPushButton::pressed));
    ASSERT(connect(ui->optionsCustomButton, &CustomQLabel::clicked, ui->showHideControlsButton, &QPushButton::released));
}

void MainWindow::setupLateralButtons()
{
    ui->leftLateralButton0->setIndexAndSide(0, 0);
    ui->leftLateralButton1->setIndexAndSide(1, 0);
    ui->leftLateralButton2->setIndexAndSide(2, 0);
    ui->leftLateralButton3->setIndexAndSide(3, 0);
    ui->leftLateralButton4->setIndexAndSide(4, 0);
    ui->leftLateralButton5->setIndexAndSide(5, 0);
    ui->leftLateralButton6->setIndexAndSide(6, 0);
    ui->leftLateralButton7->setIndexAndSide(7, 0);
    ui->leftLateralButton8->setIndexAndSide(8, 0);
    ui->leftLateralButton9->setIndexAndSide(9, 0);

    ui->rightLateralButton0->setIndexAndSide(0, 1);
    ui->rightLateralButton1->setIndexAndSide(1, 1);
    ui->rightLateralButton2->setIndexAndSide(2, 1);
    ui->rightLateralButton3->setIndexAndSide(3, 1);
    ui->rightLateralButton4->setIndexAndSide(4, 1);
    ui->rightLateralButton5->setIndexAndSide(5, 1);
    ui->rightLateralButton6->setIndexAndSide(6, 1);
    ui->rightLateralButton7->setIndexAndSide(7, 1);
    ui->rightLateralButton8->setIndexAndSide(8, 1);
    ui->rightLateralButton9->setIndexAndSide(9, 1);

    connect(ui->leftLateralButton0, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton1, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton2, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton3, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton4, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton5, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton6, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton7, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton8, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->leftLateralButton9, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);

    connect(ui->rightLateralButton0, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton1, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton2, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton3, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton4, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton5, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton6, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton7, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton8, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
    connect(ui->rightLateralButton9, &CustomQLabel::clicked, this, &MainWindow::lateralButtonClicked);
}

void MainWindow::lateralButtonClicked()
{
    CustomQLabel* lateralButton = qobject_cast<CustomQLabel*>(QObject::sender());
    LateralPanelType lateralSide = static_cast<LateralPanelType>(lateralButton->getSide());
    int index = lateralButton->getIndex();
    if (ui->handLateralPanelView->isVisible() && !ui->leftScanLateralPanelView->isVisible() && !ui->rightScanLateralPanelView->isVisible()) {
        setHandChannel(index);
    }
    else if (!ui->handLateralPanelView->isVisible() && ui->leftScanLateralPanelView->isVisible() && ui->rightScanLateralPanelView->isVisible()) {
        setScanChannel(index, lateralSide);
    }
}

void MainWindow::changeStateAcousticContact(bool isEnabled)
{
    setAcousticContactPanelsVisibility(false);
    Core* core = &Core::instance();
    disconnect(core, &Core::doBScan2Data, ui->leftScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData);
    disconnect(core, &Core::doMScan2Data, ui->leftScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData);
    disconnect(core, &Core::doBScan2Data, ui->rightScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData);
    disconnect(core, &Core::doMScan2Data, ui->rightScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData);
    if (isEnabled) {
        ASSERT(connect(core, &Core::doBScan2Data, ui->leftScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData));
        ASSERT(connect(core, &Core::doMScan2Data, ui->leftScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData));
        ASSERT(connect(core, &Core::doBScan2Data, ui->rightScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData));
        ASSERT(connect(core, &Core::doMScan2Data, ui->rightScanTapeAcousticContactView, &ScanTapeAcousticContactView::onBScanData));
    }
}

void MainWindow::switchToPage(QWidget* widget)
{
    Q_ASSERT(widget);

    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    if (virtualKeyboards->isVisible() == true) {
        virtualKeyboards->leave();
    }
    ui->stackedWidget->setCurrentWidget(widget);
}

void MainWindow::switchToMenuPage(QWidget* widget)
{
    Q_ASSERT(widget);
    hideStackedLayout();
    _menuHistory.push(ui->stackedWidget->currentWidget());
    switchToPage(widget);
}

void MainWindow::leaveMenuPage()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    if (virtualKeyboards->isVisible() == true) {
        virtualKeyboards->leave();
        return;
    }
    if (_menuHistory.isEmpty() == false) {
        switchToPage(_menuHistory.pop());
        emit backFromServiceMarkPage();
    }
    else {
        emit backFromNotesMenuPageState();
    }
}

void MainWindow::clearMenuHistory()
{
    _menuHistory.clear();
}

void MainWindow::switchToVisibleSatellitesPage()
{
    switchToMenuPage(ui->visibleSatellitesPage);
}

void MainWindow::setUmuInfo()
{
    Core& core = Core::instance();
    QStringList deviceSerialNumbers = core.deviceSerialNumbers();
    QStringList deviceFirmwareVersions = core.deviceFirmwareVersions();
    QStringList devicePldVersions = core.devicePldVersions();

    QString deviceSerialNumbersResult;
    QString deviceFirmwareVersionsResult;
    QString devicePldVersionsResult;

    for (int i = 0; i < deviceSerialNumbers.count(); ++i) {
        if (i > 0) {
            deviceSerialNumbersResult.append(" | ");
            deviceFirmwareVersionsResult.append(" | ");
            devicePldVersionsResult.append(" | ");
        }
        deviceSerialNumbersResult.append(deviceSerialNumbers[i]);
        deviceFirmwareVersionsResult.append(deviceFirmwareVersions[i]);
        devicePldVersionsResult.append(devicePldVersions[i]);
    }

    ui->umuSerialNumberLabel->setText(deviceSerialNumbersResult);
    ui->umuFirmwareVersionLabel->setText(deviceFirmwareVersionsResult);
    ui->umuPldVersionLabel->setText(devicePldVersionsResult);
    qDebug() << QString("CDU SERIAL: ") << cduSerialNumber();
    qDebug() << QString("UMU SERIAL: ") << deviceSerialNumbersResult;
    qDebug() << QString("UMU FIRMWARE VERSION: ") << deviceFirmwareVersionsResult;
    qDebug() << QString("UMU PLD VERSION: ") << devicePldVersionsResult;
    if (_deviceType == Avicon31Default || _deviceType == Avicon31KZ) {
        checkUmuFirmwareVersion();
    }
}

void MainWindow::checkUmuFirmwareVersion()
{
    Core& core = Core::instance();
    QStringList deviceFirmwareVersions = core.deviceFirmwareVersions();
    if (deviceFirmwareVersions.isEmpty()) {
        return;
    }
    QString firmware = deviceFirmwareVersions.first();
    if (firmware.contains(".")) {
        QStringList parseFirmwareList = firmware.split(".");
        if (parseFirmwareList.isEmpty() == false && parseFirmwareList.at(1).toInt() < SECOND_DIGIT_FIRMWARE_VERSION) {
            Notifier::instance().addNote(Qt::red, QString(tr("Update UMU firmware to version 9.6.x or above!")));
        }
    }
}

void MainWindow::setScanChannel(int index, LateralPanelType side)
{
    bool rc = false;
    if (index >= 0) {
        switch (side) {
        case LeftPanel:
            rc = ui->leftScanLateralPanelView->select(index);
            break;
        case RightPanel:
            rc = ui->rightScanLateralPanelView->select(index);
            break;
        }
    }
    if (rc) {
        emit doSetScanChannel();
    }
}

void MainWindow::setHandChannel(int index)
{
    qDebug() << Q_FUNC_INFO << "index:" << index;
    ui->handLateralPanelView->select(index);
    emit doSetHandChannel();
}

void MainWindow::setHeadScanChannel(int index)
{
    qDebug() << Q_FUNC_INFO << "index:" << index;
    ui->headScanLateralPanelView->selectHeadScan(index);
    emit doSetHeadScannerChannel();
}

void MainWindow::onBScanPlotSelected(int tapeIndex, int index)
{
    Q_ASSERT(tapeIndex >= 0);
    ui->aScanPage->setBScanPlot(ui->bScanPage->bScanPlotByTapeIndex(static_cast<unsigned int>(tapeIndex)), index);
}

void MainWindow::onChangeDrawMode(int mode)
{
    ui->aScanPage->setDrawMode(mode);
}

void MainWindow::on_showHideControlsButton_released()
{
    if (_deviceType == Avicon31Estonia) {
        return;
    }
    if (ui->aScanPage->isVisible() == true) {
        ui->aScanPage->controlsAreaVisible() ? ui->aScanPage->setControlsAreaVisible(false) : ui->aScanPage->setControlsAreaVisible(true);
    }
    else if (ui->bScanPage->isVisible() == true) {
        ui->bScanPage->controlsAreaVisible() ? ui->bScanPage->setControlsAreaVisible(false) : ui->bScanPage->setControlsAreaVisible(true);
    }
#if defined TARGET_AVICON31
    else if (_av17DefView->isVisible() == true) {
        _av17DefView->controlsAreaVisible() ? _av17DefView->setControlsAreaVisible(false) : _av17DefView->setControlsAreaVisible(true);
    }
#endif
}

void MainWindow::on_backPushButton_released()
{
    if (_isElTest) {
        _electricTestPage->isHidden() ? _electricTestPage->show() : _electricTestPage->hide();
        return;
    }

    leaveMenuPage();
    hideStackedLayout();
}

void MainWindow::on_logButton_released()
{
    switchToMenuPage(ui->logPage);
}

void MainWindow::on_calibrationButton_released()
{
    switchToMenuPage(ui->calibrationMenuPage);
}

void MainWindow::onSetDateButton_released()
{
    switchToMenuPage(ui->setDatePage);
}

void MainWindow::on_InfoButton_released()
{
    switchToMenuPage(ui->infoPage);
}

void MainWindow::onSetTimeButton_released()
{
    switchToMenuPage(ui->setTimePage);
}

void MainWindow::onSoundSettingsButton_released()
{
    switchToMenuPage(ui->soundSettingsPage);
}

void MainWindow::on_quitButton_released()
{
    ui->messagePage->setInfoLabel(tr("Quit"));
    ui->messagePage->setCallback(this, &MainWindow::quitFromApplication);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_rebootButton_released()
{
    ui->messagePage->setInfoLabel(tr("Reboot"));
    ui->messagePage->setCallback(this, &MainWindow::rebootSystem);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_powerOffButton_released()
{
#ifdef IMX_DEVICE
    ui->messagePage->setInfoLabel(tr("Power Off"));
#else
    ui->messagePage->setInfoLabel(tr("Quit"));
#endif
    ui->messagePage->setCallback(this, &MainWindow::powerOffSystem);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_serviceModeButton_released()
{
    ui->messagePage->setInfoLabel(tr("Service mode"));
    ui->messagePage->setCallback(this, &MainWindow::switchToServiceSystemMode);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_updateModeButton_released()
{
    ui->messagePage->setInfoLabel(tr("Update mode"));
    ui->messagePage->setCallback(this, &MainWindow::switchToUpdateSystemMode);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_eltestModeButton_released()
{
    ui->messagePage->setInfoLabel(tr("Electric test mode"));
    ui->messagePage->setCallback(this, &MainWindow::switchToElTestSystemMode);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onSetLanguageButton_released()
{
    switchToMenuPage(ui->setLanguagePage);
}

void MainWindow::on_coordinatesButton_released()
{
    switchToMenuPage(ui->coordinatesPage);
}

void MainWindow::on_versionsButton_released()
{
    switchToMenuPage(ui->versionsPage);
}

void MainWindow::on_aScanButton_released()
{
    switchToMenuPage(ui->aScanPage);
}

void MainWindow::on_mScanButton_released()
{
    switchToMenuPage(ui->mScanPage);
}

void MainWindow::on_bScanButton_released()
{
    switchToMenuPage(ui->bScanPage);
}

void MainWindow::on_optionsButton_released()
{
    switchToMenuPage(_optionsListPage);
}

void MainWindow::on_calibrationsListButton_released()
{
    switchToMenuPage(ui->calibrationsListPage);
}

void MainWindow::on_tableOfCalibrationsButton_released()
{
    switchToMenuPage(ui->tableSettingsView);
}

void MainWindow::on_registrationButton_released()
{
    switchToMenuPage(_registrationPage);
    if (_deviceType == Avicon31Default) {
        checkLastServiceDate(true);
    }
}

void MainWindow::onChangeSchemeButton_released()
{
    switchToMenuPage(ui->changeSchemePage);
}

void MainWindow::on_changeOperatorButton_released()
{
    switchToMenuPage(_switchOperatorWidget);
}

void MainWindow::on_serviceMarksButton_released()
{
    switchToMenuPage(ui->serviceMarksPage);
    Core::instance().registration().addMenuMode();
    qDebug() << "Enter to serviceMarkPage";
}

void MainWindow::on_railTypeButton_released()
{
    if (_switchTypePage->switchControlStatus()) {
        if (_switchTypePage->getSpecialRailTypeStatus()) {
            ui->specialRailTypeLabel->hide();
            ui->specialRailTypeLabelLine->hide();
            _switchTypePage->specialRailTypeOff();
            Core::instance().setDynamicRailType(false);
            qDebug() << "SET DYNAMIC RAIL TYPE OFF";
        }
        else {
            ui->specialRailTypeLabel->show();
            ui->specialRailTypeLabelLine->show();
            _switchTypePage->specialRailTypeOn();
            Core::instance().setDynamicRailType(true);
            qDebug() << "SET DYNAMIC RAIL TYPE ON";
        }
    }
    else {
        Core::instance().calibrationToRailType();
    }
}

void MainWindow::on_debugButton_released()
{
    switchToMenuPage(ui->debugPage);
}

void MainWindow::on_tableOfHandChannelsButton_released()
{
    switchToMenuPage(ui->handChannelsView);
}

void MainWindow::on_leaveButton_released()
{
#if defined TARGET_AVICON31
    switchToMenuPage(ui->leavePage);
#else
    on_powerOffButton_released();
#endif
}

void MainWindow::onBrightnessSettingsButton_released()
{
    switchToMenuPage(ui->brightnessPage);
}

void MainWindow::on_showMarkMenuPageButton_released()
{
    switchToMenuPage(ui->markMenuPage);
}

void MainWindow::onAScanSettingsButton_released()
{
    switchToMenuPage(ui->aScanSettingsPage);
}

void MainWindow::onBScanSettingsButton_released()
{
    switchToMenuPage(ui->bScanSettingsPage);
}

void MainWindow::onMScanSettingsButton_released()
{
    switchToMenuPage(ui->mScanSettingsPage);
}

void MainWindow::onWifiManagerButton_released()
{
    switchToMenuPage(_wifiManagerPage);
}

void MainWindow::on_showVideoCameraPageButton_released()
{
    switchToMenuPage(_videoCameraPage);
    if (_deviceType == Avicon15) {
        ui->bottomPanel->hide();
    }
}

void MainWindow::on_showAudioRecorderPageButton_released()
{
    switchToMenuPage(ui->audioRecorderPage);
}

void MainWindow::on_serviceMarksDefectButton_released()
{
    _defectPage->clearFields();
    switchToMenuPage(_defectPage);
}


void MainWindow::on_serviceMarksSwitchNumberButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addSwitchNumber);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksBranchButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addBranch);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksSlipButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addSlip);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksPhotoVideButton_released()
{
    switchToMenuPage(_videoCameraPage);
    if (_deviceType == Avicon15) {
        ui->bottomPanel->hide();
    }
}

void MainWindow::on_serviceMarksAudioCommentsButton_released()
{
    switchToMenuPage(ui->audioRecorderPage);
}

void MainWindow::on_serviceMarksRailroadCrossingButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addRailroadCrossing);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksMiscButton_released()
{
    switchToMenuPage(ui->serviceMarksMiscPage);
}

void MainWindow::on_serviceMarksBridgeButton_released()
{
    switchToMenuPage(ui->serviceMarksBridgePage);
}

void MainWindow::on_serviceMarksBridgeOkButton_released()
{
    QListWidgetItem* item = ui->serviceMarksBridgesList->currentItem();
    QString bridgeName("");
    if (item != nullptr) {
        bridgeName = item->text();
        Core::instance().registration().addTextLabel(tr("Bridge: %1").arg(bridgeName));
    }
    leaveMenuPage();
}

void MainWindow::on_serviceMarksBridgeAddButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addServiceMarksBridge);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksBridgeEditButton_released()
{
    QListWidgetItem* item = ui->serviceMarksBridgesList->currentItem();
    if (item != nullptr) {
        VirtualKeyboards::instance()->setCallback(this, &MainWindow::editServiceMarksBridge);
        showVirtualKeyboardsWithLineEditAndValidatorNone(item->text());
    }
}

void MainWindow::on_serviceMarksBridgeDeleteButton_released()
{
    ui->messagePage->setInfoLabel(tr("Delete bridge"));
    ui->messagePage->setCallback(this, &MainWindow::deleteServiceMarksBridge);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_serviceMarksPlatformButton_released()
{
    switchToMenuPage(ui->serviceMarksPlatformPage);
}

void MainWindow::on_serviceMarksPlatformOkButton_released()
{
    QListWidgetItem* item = ui->serviceMarksPlatformsList->currentItem();
    QString platformName("");
    if (item != nullptr) {
        platformName = item->text();
        Core::instance().registration().addTextLabel(tr("Platform: %1").arg(platformName));
    }
    leaveMenuPage();
}

void MainWindow::on_serviceMarksPlatformAddButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addServiceMarksPlatform);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksPlatformEditButton_released()
{
    QListWidgetItem* item = ui->serviceMarksPlatformsList->currentItem();
    if (item != nullptr) {
        VirtualKeyboards::instance()->setCallback(this, &MainWindow::editServiceMarksPlatform);
        showVirtualKeyboardsWithLineEditAndValidatorNone(item->text());
    }
}

void MainWindow::on_serviceMarksPlatformDeleteButton_released()
{
    ui->messagePage->setInfoLabel(tr("Delete platform"));
    ui->messagePage->setCallback(this, &MainWindow::deleteServiceMarksPlatform);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_serviceMarksMiscOkButton_released()
{
    QListWidgetItem* item = ui->serviceMarksMiscList->currentItem();
    QString miscName("");
    if (item != nullptr) {
        miscName = item->text();
        Core::instance().registration().addTextLabel(tr("Miscellaneous: %1").arg(miscName));
    }
    leaveMenuPage();
}

void MainWindow::on_serviceMarksMiscAddButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addServiceMarksMisc);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_serviceMarksMiscEditButton_released()
{
    QListWidgetItem* item = ui->serviceMarksMiscList->currentItem();
    if (item != nullptr) {
        VirtualKeyboards::instance()->setCallback(this, &MainWindow::editServiceMarksMisc);
        showVirtualKeyboardsWithLineEditAndValidatorNone(item->text());
    }
}

void MainWindow::on_serviceMarksMiscDeleteButton_released()
{
    ui->messagePage->setInfoLabel(tr("Delete misc"));
    ui->messagePage->setCallback(this, &MainWindow::deleteServiceMarksMisc);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_selectCalibrationButton_released()
{
    int row = ui->calibrationsListWidget->currentRow();
    _currentCalibrationIndex = row;
    Core& core = Core::instance();
    core.setCalibrationIndex(row);
    core.testSensLevels();
    ui->calibrationLabel->setText(ui->calibrationsListWidget->currentItem()->text());
}

void MainWindow::on_newCalibrationButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::addNewCalibration);
    showVirtualKeyboardsWithLineEditAndValidatorNone("");
}

void MainWindow::on_deleteCalibrationButton_released()
{
    ui->messagePage->setInfoLabel(tr("Delete calibration"));
    ui->messagePage->setCallback(this, &MainWindow::deleteCalibration);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::on_renameCalibrationButton_released()
{
    QListWidgetItem* item = ui->calibrationsListWidget->currentItem();
    if (item != nullptr) {
        VirtualKeyboards::instance()->setCallback(this, &MainWindow::renameCalibration);
        showVirtualKeyboardsWithLineEditAndValidatorNone(item->text());
    }
}

void MainWindow::on_recalibrationButton_released()
{
    int row = ui->calibrationsListWidget->currentRow();
    Core::instance().setCalibrationIndex(row);
    ui->calibrationLabel->setText(ui->calibrationsListWidget->item(row)->text());
    emit doRecalibration();
}

void MainWindow::onSchemeChanged(int index)
{
    qDebug() << QString("Info| Scheme changed to %1").arg(index);
    Core& core = Core::instance();
    QAbstractItemModel* tapeModel = core.channelsController()->tapeModel();
    _scanChannelsCountLeftPanel = tapeModel->rowCount();
    _scanChannelsCountRightPanel = _scanChannelsCountLeftPanel;
    ui->leftScanLateralPanelView->setModel(tapeModel);
    ui->rightScanLateralPanelView->setModel(tapeModel);
    ui->bScanPage->setModel(tapeModel);

#if defined TARGET_AVICON31
    QAbstractItemModel* headScanChannelsModel = core.channelsController()->headScanChannelsModel();
    ui->headScanLateralPanelView->setModel(headScanChannelsModel);
#endif

#if defined TARGET_AVICON31 || TARGET_AVICON31E || TARGET_AVICONDB
    ui->leftScanTapeAcousticContactView->setModel(tapeModel);
    ui->rightScanTapeAcousticContactView->setModel(tapeModel);
#endif

    QAbstractItemModel* scanChannelsModel = core.channelsController()->scanChannelsModel();
    ui->tableSettingsView->setContoledRail(static_cast<ControledRail>(core.channelsController()->contoledRail()));
    ui->tableSettingsView->setModel(scanChannelsModel);

    QAbstractItemModel* handChannelsModel = core.channelsController()->handChannelsModel();
    _handChannelsCount = handChannelsModel->rowCount();
    ui->handLateralPanelView->setModel(handChannelsModel);
    ui->handChannelsView->setModel(handChannelsModel);

    QAbstractItemModel* channelsModel = core.channelsController()->channelsModel();
    ui->aScanPage->setModel(channelsModel);

#if defined TARGET_AVICON31
    ui->aScanPage->setHeadScanModel(headScanChannelsModel);
    _av17Page->setModel(headScanChannelsModel);
    _av17DefView->setModel(channelsModel);
#endif

    core.registration().setModel(channelsModel);

    ui->schemesListWidget->setCurrentRow(index);
    ui->schemeNumberLabel->setText(QString::number(index + 1));

    setupCalibrationPage();
    setupLimitsBaseSensPage();
    _waitMessagePage->hide();
    if (_deviceType == Avicon31Default) {
        checkLastServiceDate();
    }
    emit doSchemeChanged();
}

void MainWindow::onInited()
{
    setupFiltration();
    setupCheckCalibration();
    setupImportantAreaIndicate();
    changeStateAcousticContact(restoreStateAcousticContactControl());
    Core::instance().restoreACValues();
    Core::instance().useAutoGainAdjustment(restoreAutoGainAdjustmentStatus());
}

void MainWindow::externalKeyToggled(bool isPressed, int keyCode)
{
    switch (keyCode) {
    case 0:
        if (isPressed) {
            emit trackMarkExternalKeyboard_released();
            qDebug() << "TrackMark ExternalKeyboard Pressed";
        }
        break;
    case 1:
        if (isPressed) {
            emit serviceMarkExternalKeyboard_released();
            qDebug() << "ServiceMark ExternalKeyboard Pressed";
        }
        break;
    case 2:
        if (isPressed == true && _enabledBoltJointState == true) {
            emit doStartBoltJoint();
            qDebug() << "StartBoltJoint ExternalKeyboard Pressed";
        }
        else {
            emit doStopBoltJoint();
            qDebug() << "StartBoltJoint ExternalKeyboard Released";
        }
        break;
    }
}

void MainWindow::onBrightnessChanged(qreal value)
{
    if (value < 0) {
        value = 0;
    }
    else if (value > 100) {
        value = 100;
    }

    setScreenBrightness(value);
    if (Core::instance().isCduOverheated() && value > MINIMAL_BRIGHTNESS) {
        Notifier::instance().addNote(Qt::red, tr("Attention! Overheating CDU!"));
    }

    saveBrightness(value);
    ui->setBrightnessSpinBoxNumber->setValue(value, false);
    _quickBar->setBrightness(value);
}

void MainWindow::onConnectionDisconnectedStateEntered()
{
    ui->connectionStatus->setPalette(Styles::connectionStatusDisconnected());
#ifndef DEBUG
    ui->boltJointButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
#endif
}

void MainWindow::onConnectionConnectingStateEntered()
{
#if defined TARGET_AVICONDB || defined TARGET_AVICON31E
    _waitMessagePage->setText(tr("Please wait. UMU is connecting"));
    ASSERT(connect(_waitMessagePage, &WaitMessagePage::timeIsOut, this, &MainWindow::onTimeForConnectionOut));
    _waitMessagePage->startProcessTimer(30000);
    _waitMessagePage->show();
#endif
    ui->connectionStatus->setPalette(Styles::connectionStatusConnecting());
#ifndef DEBUG
    ui->boltJointButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
#endif
}

void MainWindow::onConnectionConnectedStateEntered()
{
#if defined TARGET_AVICONDB || defined TARGET_AVICON31E
    _waitMessagePage->hide();
    _waitMessagePage->stopProcessTimer();
    disconnect(_waitMessagePage, &WaitMessagePage::timeIsOut, this, &MainWindow::onTimeForConnectionOut);
#endif
    ui->connectionStatus->setPalette(Styles::connectionStatusConnected());
    ui->connectionStatus->setVisible(false);
    QTimer::singleShot(3000, this, &MainWindow::setUmuInfo);
    ui->pauseButton->setEnabled(true);
}

void MainWindow::onUnknownMemoryStateEntered()
{
    ui->audioRecorderPage->setRecordingEnabled(false);
}

void MainWindow::onMemoryIsOverStateEntered()
{
    Notifier::instance().addNote(Qt::red, tr("Disk space is over!"));
    ui->audioRecorderPage->setRecordingEnabled(false);
}

void MainWindow::onLowMemoryStateEntered()
{
    Notifier::instance().addNote(Qt::red, tr("Disk space is about to over!"));
    ui->audioRecorderPage->setRecordingEnabled(false);
}

void MainWindow::onEnoughMemoryStateEntered()
{
    ui->audioRecorderPage->setRecordingEnabled(true);
}

void MainWindow::onLowBattery()
{
#ifdef TARGET_AVICON15
    if (_stateMachine->configuration().contains(_connectionConnectedState)) {
        Notifier::instance().addNote(Qt::yellow, tr("Low UMU battery!"));
    }
#else
    Notifier::instance().addNote(Qt::yellow, tr("Low battery!"));
#endif
}

void MainWindow::onRegistrationOffStateEntered()
{
#if defined(TARGET_AVICONDB) || defined(TARGET_AVICONDBHS)
    ui->notesButton->setDisabled(true);
#endif
    ui->registrationLabel->setStyleSheet("color:rgb(25, 227, 49); font:bold");
    ui->registrationLabel->setText(QString(0x25A0));
    _registrationPage->setRegistrationState(false);
    ui->calibrationsListWidget->setEnabled(true);
    ui->selectCalibrationButton->setEnabled(true);
    ui->newCalibrationButton->setEnabled(true);
    ui->renameCalibrationButton->setEnabled(true);
    ui->deleteCalibrationButton->setEnabled(true);
    _optionsListPage->setItemEnabled("setDateButton", true);
    _optionsListPage->setItemEnabled("setTimeButton", true);
    _optionsListPage->setItemEnabled("acousticContactOptionsPageButton", true);
    _optionsListPage->setItemEnabled("changeSchemeButton", true);
    ui->encoderCorrectionButton->setEnabled(true);
    ui->memoryButton->setEnabled(true);
    ui->aScanPage->visibleControlsForRegistration(false);
    disconnect(ui->aScanPage, &AScanPage::startHandScanRegistration, &(Core::instance().registration()), &Registration::onStartHandScanRegistration);
    disconnect(ui->aScanPage, &AScanPage::stopHandScanRegistration, &(Core::instance().registration()), &Registration::onStopHandScanRegistration);
    if (_deviceType == Avicon31Default) {
        Core::instance().startTimerForCheckCalibrationTemperature();
        _wrongcalibrationmsg->resetCountBoltJoints();
    }
    Core::instance().updateRemoteState(false, _typeView);
    ui->switchControlButton->setVisible(false);
}

void MainWindow::onRegistrationOffBatteryIsOverStateEntered()
{
#ifdef TARGET_AVICON15
    if (_stateMachine->configuration().contains(_connectionConnectedState)) Notifier::instance().addNote(Qt::red, tr("UMU battery is over!"));
#else
    Notifier::instance().addNote(Qt::red, tr("Battery is over!\nShutdown."));
    QTimer::singleShot(5000, this, &MainWindow::onPowerOffSystem);
#endif
}

void MainWindow::onRegistrationOnStateEntered()
{
#if defined(TARGET_AVICONDB) || defined(TARGET_AVICONDBHS)
    ui->notesButton->setEnabled(true);
#endif
    ui->bScanPage->setMovementsMode(false);
    ui->registrationLabel->setStyleSheet("color:rgb(255, 0, 0); font:bold");
    ui->registrationLabel->setText(QString(0x25CF));

    _optionsListPage->setItemEnabled("setDateButton", false);
    _optionsListPage->setItemEnabled("setTimeButton", false);
    _optionsListPage->setItemEnabled("acousticContactOptionsPageButton", false);
    _optionsListPage->setItemEnabled("changeSchemeButton", false);
    ui->calibrationsListWidget->setEnabled(false);
    ui->selectCalibrationButton->setEnabled(false);
    ui->newCalibrationButton->setEnabled(false);
    ui->renameCalibrationButton->setEnabled(false);
    ui->deleteCalibrationButton->setEnabled(false);
    ui->encoderCorrectionButton->setEnabled(false);
    ui->encoderPage->resetCountStepsForOneMeter();
    ui->memoryButton->setEnabled(false);
    ui->aScanPage->visibleControlsForRegistration(true);
    ui->bScanPage->resetBscans();
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    ui->startSwitchButton->setVisible(true);
    ui->endSwitchButton->setVisible(true);
#endif
    ui->switchControlButton->setVisible(_deviceType == Avicon31Default || _deviceType == Avicon31KZ || _deviceType == Avicon15);

#if defined TARGET_AVICONDBHS
    hideServiceMarksForDBHS();
#endif

    _registrationPage->setRegistrationState(true);

    updateTrackMarksWidgets();
    Core& core = Core::instance();
    core.registration().stop(false);
    core.registration().start(_registrationPage->getOperator(),
                              _registrationPage->getLine(),
                              _registrationPage->getTrackNumber(),
                              Core::instance().getTrackMarks(),
                              _registrationPage->getLeftSide() ? true : false,
                              _registrationPage->getDirection() == 1 ? true : false,
                              _registrationPage->getDirectionName().toInt(),
                              false,
                              _registrationPage->getControlRail() ? true : false,
                              _registrationPage->getFileName());

    core.setSearchMode();
    ASSERT(connect(ui->aScanPage, &AScanPage::startHandScanRegistration, &(core.registration()), &Registration::onStartHandScanRegistration));
    ASSERT(connect(ui->aScanPage, &AScanPage::stopHandScanRegistration, &(core.registration()), &Registration::onStopHandScanRegistration));
    if (_deviceType == Avicon31Default) {
        core.startTimerForCheckCalibrationTemperature();
        ASSERT(connect(&(core.registration()), &Registration::doStopRegistration, this, &MainWindow::onReport));
        _wrongcalibrationmsg->resetCountBoltJoints();
    }
    Core::instance().updateRemoteState(true, _typeView);

    if (_registrationType == EKASUIRegistration) {
        Core::instance().getEkasui()->resetIncidentIndex();
    }
}

void MainWindow::onRegistrationOnStateExited()
{
    _switchTypePage->stopSwitchControl();
    Core& core = Core::instance();
    core.registration().stop(false);
    TrackMarks* tm = core.getTrackMarks();
    _registrationPage->updateTrackMarks();
    switch (Core::instance().getSystemCoordType()) {
    case csMetricRF:
        saveLastKm(static_cast<TMRussian*>(tm)->getKm());
        saveLastPk(static_cast<TMRussian*>(tm)->getPk());
        saveLastM(static_cast<TMRussian*>(tm)->getM());
        break;
    case csMetric1km:
    case csImperial:
    case csMetric0_1km:
    case csMetric1kmTurkish:
        break;
    }

    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionBackwardButton->setVisible(false);
    ui->switchControlButton->setVisible(false);

#if defined TARGET_AVICON31
    disconnect(&(core.registration()), &Registration::doStopRegistration, this, &MainWindow::onReport);
#endif
}

void MainWindow::onRegistrationOnServiceStateEntered()
{
    _optionsListPage->setItemEnabled("setDateButton", false);
    _optionsListPage->setItemEnabled("setTimeButton", false);
    _optionsListPage->setItemEnabled("acousticContactOptionsPageButton", false);
    _optionsListPage->setItemEnabled("changeSchemeButton", false);
    ui->bScanPage->setMovementsMode(false);
    ui->registrationLabel->setStyleSheet("color:rgb(255, 0, 0); font:bold");
    ui->registrationLabel->setText(QString(0x25CF));
    _registrationPage->setRegistrationState(true);
    ui->calibrationsListButton->setEnabled(true);
    ui->calibrationsListWidget->setEnabled(false);
    ui->selectCalibrationButton->setEnabled(false);
    ui->newCalibrationButton->setEnabled(false);
    ui->renameCalibrationButton->setEnabled(false);
    ui->deleteCalibrationButton->setEnabled(false);
    ui->encoderCorrectionButton->setEnabled(false);
    ui->memoryButton->setEnabled(false);
    ui->bScanPage->resetBscans();
    ui->controlSectionForwardButton->setVisible(true);
    ui->controlSectionBackwardButton->setVisible(true);
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    ui->startSwitchButton->setVisible(false);
    ui->endSwitchButton->setVisible(false);
#endif
    Core& core = Core::instance();
    core.getTrackMarks()->reset();
    core.getTrackMarks()->setDirection(ForwardDirection);

    updateTrackMarksWidgets();

    core.registration().start(_registrationPage->getOperator(), _registrationPage->getLine(), _registrationPage->getTrackNumber(), Core::instance().getTrackMarks(), false, true, 0, true, _registrationPage->getControlRail() ? true : false, "");
    saveLastServiceDate(QDateTime::currentDateTime().toString("dd.MM.yyyy"));
    ui->serviceDateLabel->setText(restoreLastServiceDate());
    core.setSearchMode();
}

void MainWindow::onRegistrationOnServiceStateExited()
{
    Core& core = Core::instance();
    core.registration().stop(false);
    core.saveGainForControlSectionOfTrack(restoreSchemeNumber());
    ui->serviceMarksAudioCommentsButton->setVisible(true);
    ui->serviceMarksPhotoVideButton->setVisible(true);
    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionBackwardButton->setVisible(false);
    clearMenuHistory();
}

void MainWindow::onStopRegistrationMemoryIsOver()
{
    Notifier::instance().addNote(Qt::red, tr("Registration has been stopped!"));
}

void MainWindow::onInitialBoltJointStateEntered()
{
    ui->boltJointLabel->hide();
    ui->boltJointLine->hide();
}

void MainWindow::onStartBoltJointStateEntered()
{
    ui->boltJointLabel->show();
    ui->boltJointLine->show();
    Core::instance().startBoltJoint(static_cast<int>(restoreAdditiveForSens()));
    ui->aScanPage->blockControlsArea(true);
}

void MainWindow::onStopBoltJointStateEntered()
{
    ui->boltJointLabel->hide();
    ui->boltJointLine->hide();
    Core::instance().stopBoltJoint();
    ui->aScanPage->blockControlsArea(false);
}

void MainWindow::onMenuStateRegOffEntered()
{
    setLateralPanelsVisibility(false);
    clearLateralPanelsSelection();
    ui->handLateralPanelView->setVisible(false);
    setAcousticContactPanelsVisibility(false);
    setVisibleSpeedLabel(true);
    ui->modeLabel->setText(tr("Menu"));
    emit doStopBoltJoint();
    setEnabledBoltJointState(false);
    hideStackedLayoutBesidesReport();
    switchToPage(ui->mainMenuPage);
    static bool isFirstRun = true;
    if (isFirstRun) {
        isFirstRun = false;
        switchToMenuPage(ui->infoPage);
    }
}

void MainWindow::onMenuStateRegOffExited()
{
    clearMenuHistory();
}

void MainWindow::onNewCalibrationStateRegOffEntered()
{
    Core& core = Core::instance();
    core.setCalibrationMode(_newCalibrationName);
    core.SetFixBadSensState(false);
    core.testSensLevels();
}

void MainWindow::onRecalibrationStateRegOffEntered()
{
    Core& core = Core::instance();
    core.setRecalibrationMode();
    core.SetFixBadSensState(false);
}

void MainWindow::onCalibrationStateRegOffExited()
{
    ui->aScanPage->setMode(AScanPage::evaluation);
    Core& core = Core::instance();
    core.setSearchMode();
}

void MainWindow::onCalibrationScanControlStateRegOffEntered()
{
    setLateralPanelsVisibility(true);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->aScanPage->setMode(AScanPage::calibrationModeScan);
    ui->modeLabel->setText(tr("Calib. scan"));

    ui->bScanPage->resetTapeStates();
    setLateralPanelsVisibility(true);

    ui->leftScanLateralPanelView->select(0);
    switchToPage(ui->aScanPage);
}

void MainWindow::onCalibrationHandControlStateRegOffEntered()
{
    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(true);
    setVisibleSpeedLabel(true);
    ui->aScanPage->setMode(AScanPage::calibrationModeHand);
    ui->modeLabel->setText(tr("Calib. hand"));
    ui->handLateralPanelView->select(0);
    switchToPage(ui->aScanPage);
}

void MainWindow::onScanStateRegOffEntered()
{
    Core::instance().setSearchMode();
}

void MainWindow::onBScanStateRegOffEntered()
{
    Core& core = Core::instance();
    if (core.registration().status()) {
        setLateralPanelsVisibility(true);
        clearLateralPanelsSelection();
        setAcousticContactPanelsVisibility(restoreStateAcousticContactControl());
        ui->handLateralPanelView->setVisible(false);
        ui->headScanLateralPanelView->setVisible(false);
        setVisibleSpeedLabel(true);
        ui->modeLabel->setText(tr("B-Scan"));
        switchToPage(ui->bScanPage);
        ui->bScanPage->plugTheBscan(true);
        if (_deviceType == Avicon31Default) {
            core.setUmuLineToCompleteControl();
        }
        if (_deviceType == Avicon31Default) {
            core.startTimerForCheckCalibrationTemperature();
            core.SetFixBadSensState(restoreAutoSensResetEnabled());
        }
    }
    else {
        emit backToMenuRegOffState();
        Notifier::instance().addNote(Qt::red, tr("Error! \nNo link with UMU!"));
    }
    hideStackedLayout();
    setEnabledBoltJointState(true);
}

void MainWindow::onBScanStateRegOffExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onMScanStateRegOffEntered()
{
    emit doStopBoltJoint();
    ui->bScanPage->resetTapeStates();
    clearLateralPanelsSelection();
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->modeLabel->setText(tr("M-Scan"));
    setEnabledBoltJointState(true);
    switchToPage(ui->mScanPage);
}

void MainWindow::onMScanStateRegOffExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onHandStateRegOffEntered()
{
    qDebug() << "Entered hand state";
    Core& core = Core::instance();
    if (core.registration().status()) {
        ui->bScanPage->plugTheBscan(false);
        setLateralPanelsVisibility(false);
        setAcousticContactPanelsVisibility(false);
        ui->handLateralPanelView->setVisible(true);
        ui->headScanLateralPanelView->setVisible(false);
        setVisibleSpeedLabel(false);
        ui->aScanPage->setCurrentChannelType(ctHandScan);
        ui->aScanPage->setMode(AScanPage::hand);
        ui->modeLabel->setText(tr("Hand"));
        if (_deviceType == Avicon31Default) {
            core.setUmuLineToCompleteControl();
        }
        ui->handLateralPanelView->select(0);
        switchToPage(ui->aScanPage);
    }
    else {
        emit backToMenuRegOffState();
        qDebug() << "No link with UMU - back to menu!";
        Notifier::instance().addNote(Qt::red, tr("Error! \nNo link with UMU!"));
    }
    hideStackedLayout();
    setEnabledBoltJointState(false);
}

void MainWindow::onHandStateRegOffExited()
{
    if (_stateMachine->configuration().contains(_connectionConnectedState)) {
        qDebug() << "Exited hand state";
        ui->aScanPage->setMode(AScanPage::evaluation);
        ui->aScanPage->setCurrentChannelType(ctCompInsp);
        Core::instance().restoreACValues();
    }
}

void MainWindow::onHeadScannerBScanStateRegOffEntered()
{
    if (!restoreHeadScannerUseState()) {
        emit noUseHeadScanner();
        return;
    }
#if defined TARGET_AVICON31
    Core::instance().stopTimerForCheckCalibrationTemperature();
#endif
    Core::instance().setUmuLineToScannerControl();
    ui->modeLabel->setText(tr("Scanner"));
    ui->headScanLateralPanelView->clearSelection();
    ui->headScanLateralPanelView->setVisible(true);
    ui->handLateralPanelView->setVisible(false);
    switchToPage(_av17Page);
}

void MainWindow::onHeadScannerEvaluationStateRegOffEntered()
{
    ui->headScanLateralPanelView->setVisible(true);
    ui->aScanPage->setMode(AScanPage::headScanMode);
    switchToPage(ui->aScanPage);
}

void MainWindow::onHeadScannerViewResultStateRegOffEntered()
{
    switchToPage(_av17DefView);
    ui->headScanLateralPanelView->setVisible(false);
}

void MainWindow::onHeadScanRegOptionsStartStateRegOnEntered()
{
    if (restoreHeadScannerUseState() == false) {
        emit noUseHeadScanner();
        return;
    }
    Core::instance().setUmuLineToScannerControl();
    ui->headScanLateralPanelView->setVisible(false);
    ui->handLateralPanelView->setVisible(false);
    ui->modeLabel->setText(tr("Scanner"));
    _scannerDefectRegistrationPage->switchState(1);
    _scannerDefectRegistrationPage->resetPage();
    switchToPage(_scannerDefectRegistrationPage);
}

void MainWindow::onHeadScanRegOptionsFinishStateRegOnEntered()
{
    QWidget* widget = ui->visiblePartOfApplication;
    _ekasuiScreenshot = QPixmap::grabWidget(widget);
    _scannerDefectRegistrationPage->switchState(0);
    switchToPage(_scannerDefectRegistrationPage);
}

void MainWindow::onHeadScanRegOptionsFinishStateRegOnExited()
{
    Q_ASSERT(_scannerDefectRegistrationPage);
    _scannerDefectRegistrationPage->switchState(0);
}

void MainWindow::onHeadScannerBScanStateRegOnEntered()
{
#if defined TARGET_AVICON31
    Core::instance().stopTimerForCheckCalibrationTemperature();
#endif
    ui->headScanLateralPanelView->clearSelection();
    ui->headScanLateralPanelView->setVisible(true);
    ui->handLateralPanelView->setVisible(false);
    Core::instance().registration().addHeadScanMode();
    switchToPage(_av17Page);
}

void MainWindow::onHeadScannerEvaluationStateRegOnEntered()
{
    ui->headScanLateralPanelView->setVisible(true);
    ui->aScanPage->setMode(AScanPage::headScanMode);
    switchToPage(ui->aScanPage);
}

void MainWindow::onHeadScannerViewResultStateRegOnEntered()
{
    switchToPage(_av17DefView);
    ui->headScanLateralPanelView->setVisible(false);
}

void MainWindow::onServiceMarkStateRegOffEntered()
{
    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);

    ui->serviceMarksDefectButton->setVisible(false);
    ui->serviceMarksBridgeButton->setVisible(false);
    ui->serviceMarksSwitchNumberButton->setVisible(false);
    ui->serviceMarksPlatformButton->setVisible(false);
    ui->serviceMarksBranchButton->setVisible(false);
    ui->serviceMarksRailroadCrossingButton->setVisible(false);
    ui->serviceMarksSlipButton->setVisible(false);
    ui->serviceMarksMiscButton->setVisible(false);
    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionBackwardButton->setVisible(false);
    ui->serviceMarksAudioCommentsButton->setVisible(true);
    ui->serviceMarksPhotoVideButton->setVisible(true);
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    ui->startSwitchButton->setVisible(false);
    ui->endSwitchButton->setVisible(false);
#endif

    switchToPage(ui->serviceMarksPage);
    hideStackedLayout();
}

void MainWindow::onServiceMarkStateRegOffExited()
{
    clearMenuHistory();
}


void MainWindow::onEvaluationStateRegOffEntered()
{
    setLateralPanelsVisibility(true);
    setAcousticContactPanelsVisibility(restoreStateAcousticContactControl());
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->aScanPage->setMode(AScanPage::evaluation);
    ui->modeLabel->setText(tr("A+B Scan"));
    setEnabledBoltJointState(true);
    switchToPage(ui->aScanPage);
    hideStackedLayout();
}

void MainWindow::onEvaluationStateRegOffExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onBScanStateRegOnEntered()
{
    Core& core = Core::instance();
    if (core.registration().status()) {
        ui->bScanPage->resetTapeStates();

        setLateralPanelsVisibility(true);
        clearLateralPanelsSelection();
        setAcousticContactPanelsVisibility(restoreStateAcousticContactControl());
        ui->handLateralPanelView->setVisible(false);
        ui->headScanLateralPanelView->setVisible(false);
        setVisibleSpeedLabel(true);
        ui->modeLabel->setText(tr("B-Scan"));
        switchToPage(ui->bScanPage);
        ui->bScanPage->plugTheBscan(true);
        core.registration().addBScanMode();
        if (_deviceType == Avicon31Default) {
            core.startTimerForCheckCalibrationTemperature();
            core.SetFixBadSensState(restoreAutoSensResetEnabled());
        }
        core.setSearchMode();
        if (_deviceType == Avicon31Default) {
            core.setUmuLineToCompleteControl();
        }
    }
    setEnabledBoltJointState(true);
    hideStackedLayout();
}

void MainWindow::onBScanStateRegOnExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onMScanStateRegOnEntered()
{
    setAcousticContactPanelsVisibility(restoreStateAcousticContactControl());
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->bScanPage->resetTapeStates();
    setLateralPanelsVisibility(true);
    clearLateralPanelsSelection();
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->modeLabel->setText(tr("M-Scan"));
    setEnabledBoltJointState(true);
    switchToPage(ui->mScanPage);
    Core::instance().registration().addMScanMode();
}

void MainWindow::onMScanStateRegOnExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onMenuStateRegOnEntered()
{
    setLateralPanelsVisibility(false);
    clearLateralPanelsSelection();
    setAcousticContactPanelsVisibility(false);
    ui->leftScanTapeAcousticContactView->clearView();
    ui->rightScanTapeAcousticContactView->clearView();
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->modeLabel->setText(tr("Menu"));
    emit doStopBoltJoint();
    setEnabledBoltJointState(false);
    hideStackedLayout();
    switchToPage(ui->mainMenuPage);
    Core::instance().registration().addMenuMode();
}

void MainWindow::onMenuStateRegOnExited()
{
    clearMenuHistory();
}

void MainWindow::onEvaluationStateRegOnEntered()
{
    setLateralPanelsVisibility(true);
    setAcousticContactPanelsVisibility(restoreStateAcousticContactControl());
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->aScanPage->setMode(AScanPage::evaluation);
    ui->modeLabel->setText(tr("A+B Scan"));
    Core::instance().registration().addEvaluationMode();
    setEnabledBoltJointState(true);
    switchToPage(ui->aScanPage);
}

void MainWindow::onEvaluationStateRegOnExited()
{
    setEnabledBoltJointState(false);
}

void MainWindow::onHandStateRegOnEntered()
{
    Core& core = Core::instance();
    if (core.registration().status()) {
        ui->bScanPage->plugTheBscan(false);
        setLateralPanelsVisibility(false);
        setAcousticContactPanelsVisibility(false);
        ui->handLateralPanelView->setVisible(true);
        setVisibleSpeedLabel(false);
        ui->aScanPage->setMode(AScanPage::hand);
        ui->aScanPage->setCurrentChannelType(ctHandScan);
#if defined TARGET_AVICONDBHS
        core.registration().setCurrentChannelType(ctCompInsp);
#else
        core.registration().setCurrentChannelType(ctHandScan);
#endif
        core.registration().plugTheBscanData(false);
        if (_deviceType == Avicon31Default) {
            core.setUmuLineToCompleteControl();
        }

        ui->modeLabel->setText(tr("Hand"));
        ui->handLateralPanelView->select(0);
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
        core.registration().onAddLocalAndSatteliteTime();
#elif defined TARGET_AVICON15
        core.registration().onAddLocalTime();
#endif
        core.registration().addHandScanMode();
        switchToPage(ui->aScanPage);
    }
    hideStackedLayout();
    setEnabledBoltJointState(false);
}

void MainWindow::onHandStateRegOnExited()
{
    ui->aScanPage->setMode(AScanPage::evaluation);
    ui->aScanPage->setCurrentChannelType(ctCompInsp);
    ui->aScanPage->checkHandScanRegTimer();
    Core& core = Core::instance();
    Registration& registration = core.registration();
    registration.setCurrentChannelType(ctCompInsp);
    registration.plugTheBscanData(true);
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    registration.onAddLocalAndSatteliteTime();
#elif defined TARGET_AVICON15
    registration.onAddLocalTime();
#endif
    core.restoreACValues();
}

void MainWindow::onTransitionFromHandToBScanStateTriggered()
{
    Core::instance().setSearchMode();
}

void MainWindow::onRollBackTrackMarkStateTriggered()
{
    updateTrackMarksWidgets();
}

void MainWindow::onNotesMenuPageStateRegOnEntered()
{
    QWidget* widget = ui->visiblePartOfApplication;
    _ekasuiScreenshot = QPixmap::grabWidget(widget);
    qDebug() << "Grab: " << _ekasuiScreenshot.width() << _ekasuiScreenshot.height();

    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);

    ui->serviceMarksDefectButton->setVisible(true);
    ui->serviceMarksBridgeButton->setVisible(true);
    ui->serviceMarksSwitchNumberButton->setVisible(true);
    ui->serviceMarksPlatformButton->setVisible(true);
    ui->serviceMarksBranchButton->setVisible(true);
    ui->serviceMarksRailroadCrossingButton->setVisible(true);
    ui->serviceMarksSlipButton->setVisible(true);
    ui->serviceMarksMiscButton->setVisible(true);
#if defined TARGET_AVICONDBHS
    hideServiceMarksForDBHS();
#endif
    switchToPage(ui->markMenuPage);
    Core::instance().registration().addMenuMode();
    qDebug() << "Enter to notes menu page";
}

void MainWindow::onNotesMenuPageStateRegOnExited()
{
    clearMenuHistory();
}

void MainWindow::onTrackMarkStateRegOnEntered()
{
    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    _trackMarksPage->saveTempTrackMarks();
    switchToPage(_trackMarksPage);
    disconnect(this, &MainWindow::trackMarkExternalKeyboard_released, this, &MainWindow::onSelectTrackMarkButton_released);
    ASSERT(connect(this, &MainWindow::trackMarkExternalKeyboard_released, this, &MainWindow::onSelectTrackMarkButton_released));

    _trackMarksPage->updateLabels();
    Core::instance().registration().addMenuMode();
    qDebug() << "Enter to trackMarkState";
}

void MainWindow::onTrackMarkStateRegOnExited() {}

void MainWindow::onServiceMarkStateRegOnEntered()
{
    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->serviceMarksDefectButton->setVisible(true);
    ui->serviceMarksBridgeButton->setVisible(true);
    ui->serviceMarksSwitchNumberButton->setVisible(true);
    ui->serviceMarksPlatformButton->setVisible(true);
    ui->serviceMarksBranchButton->setVisible(true);
    ui->serviceMarksRailroadCrossingButton->setVisible(true);
    ui->serviceMarksSlipButton->setVisible(true);
    ui->serviceMarksMiscButton->setVisible(true);
    ui->serviceMarksAudioCommentsButton->setVisible(true);
    ui->serviceMarksPhotoVideButton->setVisible(true);
    ui->controlSectionForwardButton->setVisible(false);
    ui->controlSectionBackwardButton->setVisible(false);
#if defined TARGET_AVICONDBHS
    hideServiceMarksForDBHS();
#endif
    switchToMenuPage(ui->serviceMarksPage);
    Core::instance().registration().addMenuMode();
    qDebug() << "Enter to serviceMarkState";
}

void MainWindow::onServiceMarkStateRegOnServiceEntered()
{
    setLateralPanelsVisibility(false);
    setAcousticContactPanelsVisibility(false);
    ui->handLateralPanelView->setVisible(false);
    setVisibleSpeedLabel(true);
    ui->serviceMarksDefectButton->setVisible(false);
    ui->serviceMarksBridgeButton->setVisible(false);
    ui->serviceMarksSwitchNumberButton->setVisible(false);
    ui->serviceMarksPlatformButton->setVisible(false);
    ui->serviceMarksBranchButton->setVisible(false);
    ui->serviceMarksRailroadCrossingButton->setVisible(false);
    ui->serviceMarksSlipButton->setVisible(false);
    ui->serviceMarksMiscButton->setVisible(false);
    ui->serviceMarksAudioCommentsButton->setVisible(false);
    ui->serviceMarksPhotoVideButton->setVisible(false);
    ui->controlSectionForwardButton->setVisible(true);
    ui->controlSectionBackwardButton->setVisible(true);
    switchToMenuPage(ui->serviceMarksPage);
}

void MainWindow::onPauseStateRegOnEntered()
{
    qDebug() << "Pause state reg on entered!";
    Core& core = Core::instance();
    ui->boltJointButton->setEnabled(false);
    ui->showHideControlsButton->setEnabled(false);
    ui->backPushButton->setEnabled(false);
    ui->railTypeButton->setEnabled(false);
    ui->registrationLabel->setText(QString(0x23F8));
    core.registration().pause(true);
    ui->bScanPage->plugTheBscan(false);
    core.pauseModeOn();
    Core::powerSaveMode(true);
    core.stopAudioOutput();
    ui->pauseButton->setStyleSheet(PAUSE_BUTTON_STATE_ON);
    _pauseButtonState = true;
#if defined TARGET_AVICONDB
    setScreenBrightness(10);
#endif
    _waitMessagePage->setClicked(true);
    _waitMessagePage->setText(tr("Pause"));
    _waitMessagePage->show();
}

void MainWindow::onPauseStateRegOnExited()
{
    qDebug() << "Pause state reg on exited!";
    Core& core = Core::instance();
    ui->boltJointButton->setEnabled(true);
    ui->showHideControlsButton->setEnabled(true);
    ui->backPushButton->setEnabled(true);
    ui->railTypeButton->setEnabled(true);
    ui->registrationLabel->setText(QString(0x25CF));
    core.registration().pause(false);
    ui->bScanPage->plugTheBscan(true);
    core.pauseModeOff();
    Core::powerSaveMode(false);
    core.resumeAudioOutput();
    ui->pauseButton->setStyleSheet(PAUSE_BUTTON_STATE_OFF);
    _pauseButtonState = false;
#if defined TARGET_AVICONDB
    setScreenBrightness(restoreBrightness());
#endif
    _waitMessagePage->setClicked(false);
    _waitMessagePage->hide();
}

void MainWindow::onBScanData(QSharedPointer<tDEV_BScan2Head> head)
{
    if (ui->debugPanel->isVisible()) {
        Q_ASSERT(head.data() != 0);
        ui->coord->setText(QString("coord:%1").arg(head.data()->XSysCrd[0]));
        ui->dir->setText(QString("dir:%1").arg(head.data()->Dir[0]));
    }
}

void MainWindow::onBScanDisplayThresholdChanged(int value)
{
    ui->thresholdDb->setText(QString(tr("%1 dB")).arg(value));
}

void MainWindow::on_showHideDebugPanelButton_released()
{
    ui->debugPanel->isVisible() ? ui->debugPanel->hide() : ui->debugPanel->show();
}

void MainWindow::on_encoderCorrectionButton_released()
{
    if (_deviceType == Avicon31Estonia) {
        Core::instance().setUmuLineToCompleteControl();
    }
    switchToMenuPage(ui->encoderPage);
}

void MainWindow::onChangedSpeed(double value)
{
    bool status = value >= Core::instance().maxSpeed();
    if (_speedLimit != status) {
        _speedLimit = status;
        _speedLimit ? ui->speedLabel->setStyleSheet("background-color: #ff0000; color: white;font: bold;") : ui->speedLabel->setStyleSheet("");
    }
    ui->speedLabel->setText(QString::number(value, 'f', 1) + tr(" km/h"));
}

void MainWindow::onChangedBscanScale(double value)
{
    ui->bscanScaleLabel->setText(QString::number(value, 'f', 1) + tr(" m"));
}

void MainWindow::onBScanDotSizeValueChanged(qreal value)
{
    ui->bScanPage->setDotSize(static_cast<int>(value));
    ui->aScanPage->setBScanDotSize(static_cast<int>(value));
    saveBScanDotSize(static_cast<int>(value));
}

void MainWindow::onBScanProbePulseLocationChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    bool isProbePulseFromAbove = userData.toBool();
    ui->bScanPage->setProbePulseLocation(isProbePulseFromAbove);
    ui->aScanPage->setProbePulseLocation(isProbePulseFromAbove);
    saveProbePulseLocation(isProbePulseFromAbove);
}

void MainWindow::onViewCurrentCoordinateChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    _typeView = static_cast<ViewCoordinate>(userData.toInt());
    saveTypeViewCoordinateForBScan(userData.toInt());
    updateTrackMarksWidgets();
    Core::instance().updateRemoteState(_stateMachine->configuration().contains(_registrationOnState), _typeView);
}


void MainWindow::onAScanFillingStateChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    bool isFillingAScan = userData.toBool();
    ui->aScanPage->setFillingAScanState(isFillingAScan);
    saveAScanFillingState(isFillingAScan);
}

void MainWindow::onNoiseReductionStateChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    saveNoiseReduction(userData.toBool());
    ui->aScanPage->updateNoiseReductionState();
}

void MainWindow::onHeadScannerUseSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    bool isUsed = userData.toBool();
    ui->scannerEncodersCorrectionButton->setVisible(isUsed);
    saveHeadScannerUseState(isUsed);
}

void MainWindow::onHorizontalZoneChanged(qreal value)
{
    ui->horizontalZoneSpinBox->setValue(value);
    saveHorizontalZone(static_cast<int>(value));
}

void MainWindow::onVerticalStartPositionChanged(qreal value)
{
    ui->verticalStartPositionSpinBox->setValue(value);
    saveVerticalStartPosition(static_cast<int>(value));
}

void MainWindow::onMScanDotSizeValueChanged(qreal value)
{
    ui->mScanPage->setDotSize(static_cast<int>(value));
    saveMScanDotSize(static_cast<int>(value));
}

void MainWindow::shootScreen()
{
    const int lineWidth = 10;
    QWidget* widget = ui->visiblePartOfApplication;
    QPixmap originalPixmap = QPixmap::grabWidget(widget);
    if (_keyForScreenShoot != 0 && !_isSaveAScan) {
        QString fileName = _pathToSaveScreenShoot + QString::number(_keyForScreenShoot) + QString(".png");
        originalPixmap.save(fileName, "png");
    }
    else if (_keyForScreenShoot == 0 && !_isSaveAScan) {
        QString fileName = screenshotsPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".png");
        originalPixmap.save(fileName, "png");
    }
    else if (_isSaveAScan) {
        Core::instance().registration().addAScanScreen(
            _sideForAScanScreen, _channelIdForAScanScreen, _gateIndexForAScanScreen, _gainForAScanScreen, _sensForAScanScreen, _tvgForAScanScreen, _timeDelayForAScanScreen, _surfaceForAScanScreen, originalPixmap, _startGateForAScanScreen, _endGateForAScanScreen);
    }
    QLabel* screenshotLabel = new QLabel();
    screenshotLabel->setFrameShape(QFrame::Box);
    screenshotLabel->setLineWidth(lineWidth);

    QBrush brush(QColor(255, 0, 0, 255));
    brush.setStyle(Qt::SolidPattern);

    QPalette palette;
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    screenshotLabel->setPalette(palette);

    QRect geometry = widget->geometry();
    screenshotLabel->setGeometry(geometry);
    screenshotLabel->move(pos().x() + ui->visiblePartOfApplication->pos().x(), pos().y());
    screenshotLabel->setPixmap(originalPixmap.scaled(QSize(geometry.width() - (lineWidth * 2), geometry.height() - (lineWidth * 2)), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QTimer::singleShot(2000, screenshotLabel, &QLabel::deleteLater);
    screenshotLabel->show();
    _keyForScreenShoot = 0;
    _isSaveAScan = false;
    _sideForAScanScreen = NoneDeviceSide;
    _channelIdForAScanScreen = 0;
    _gateIndexForAScanScreen = 0;
    _gainForAScanScreen = 0;
    _sensForAScanScreen = 0;
    _tvgForAScanScreen = 0;
    _timeDelayForAScanScreen = 0;
    _surfaceForAScanScreen = 0;
    _startGateForAScanScreen = 0;
    _endGateForAScanScreen = 0;
    blockControlsForScreenShoot(false);
}

void MainWindow::on_memoryButton_released()
{
    switchToMenuPage(ui->memoryPage);
}

void MainWindow::onRemoveRegistrationFiles(const QString& info)
{
    ui->messagePage->setCallback(ui->memoryPage, &MemoryPage::removeSelectedFiles);
    ui->messagePage->setInfoLabel(info);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onMoveRegistrationFiles(const QString& info)
{
    ui->messagePage->setCallback(ui->memoryPage, &MemoryPage::moveSelectedFiles);
    ui->messagePage->setInfoLabel(info);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onOpenBScanFileViewer()
{
    onBScanStateRegOffEntered();
    switchToMenuPage(ui->bScanPage);
    ui->bScanPage->plugTheBscan(true);
}

void MainWindow::onOpenImageViewer(const QString& dir, const QString& file)
{
    ui->imageViewerPage->setCurrentDirAndFile(dir, file);
    switchToMenuPage(ui->imageViewerPage);
    ui->imageViewerPage->updateViewer();
}

void MainWindow::onOpenAudioCommentsPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack)
{
    switchToMenuPage(ui->audioPlayerPage);
    ui->audioPlayerPage->onSetPlaylist(tracksLocation, playlist, currentTrack);
}

void MainWindow::onOpenVideoPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack)
{
    switchToMenuPage(ui->videoPlayerPage);
    ui->videoPlayerPage->onSetPlaylist(tracksLocation, playlist, currentTrack);
}

void MainWindow::onReport(const QString& fileName)
{
    _waitMessagePage->setText(tr("Please wait ... "));
    _waitMessagePage->show();
    qApp->processEvents();
    _report->makeReport(fileName);
    _waitMessagePage->hide();
    _report->show();
}

void MainWindow::onRemoveImageFiles(const QString& info)
{
    ui->messagePage->setCallback(ui->imageViewerPage, &ImageViewerPage::removeCurrentFile);
    ui->messagePage->setInfoLabel(info);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onRemoveCommentFile(const QString& info)
{
    ui->messagePage->setCallback(ui->audioPlayerPage, &AudioPlayerPage::onRemoveFile);
    ui->messagePage->setInfoLabel(info);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onRemoveVideoFile(const QString& info)
{
    ui->messagePage->setCallback(ui->videoPlayerPage, &VideoPlayerPage::onRemoveFile);
    ui->messagePage->setInfoLabel(info);
    switchToMenuPage(ui->messagePage);
}

void MainWindow::onChangedDistance(int value)
{
    Core& core = Core::instance();
    core.getTrackMarks()->addMeter(value);
    updateTrackMarksWidgets();
    core.handleNotifications();
    core.updateRemoteControlMeter();
}

void MainWindow::onBoltJointSettingsButton_released()
{
    switchToMenuPage(_boltJointOptions);
}

void MainWindow::on_playButton_released()
{
    Core::instance().playSound(ACNotify);
}

void MainWindow::on_notifierButton_released()
{
    Notifier::instance().addNote(Qt::green, "Some message");
}

void MainWindow::on_maxBaseSensButton_released()
{
    switchToMenuPage(ui->limitsBaseSensPage);
}

void MainWindow::onDateTimeChanged(const QDateTime& datetime)
{
    ui->dateLabel->setText(datetime.toUTC().toString("dd.MM.yyyy"));
}

void MainWindow::on_controlSectionForwardButton_released()
{
    Core::instance().registration().addTextLabel(tr("Control section forward"));
}

void MainWindow::on_controlSectionBackwardButton_released()
{
    Core::instance().registration().addTextLabel(tr("Control section backward"));
}

void MainWindow::onBScanPageFpsChanged(double value)
{
    if (ui->debugPanel->isVisible()) {
        ui->bScanPageFps->setText(QString("BScan fps:%1").arg(value));
    }
}

void MainWindow::onShootScreen(unsigned int key)
{
    blockControlsForScreenShoot(true);
    _keyForScreenShoot = key;
    _pathToSaveScreenShoot = registrationServiceScreenShoots();
    QTimer::singleShot(500, this, &MainWindow::shootScreen);
}

void MainWindow::onShootScreen(DeviceSide side, CID channelId, int gateIndex, int gain, int sens, float tvg, float timeDelay, int surface, int startGate, int endGate)
{
    blockControlsForScreenShoot(true);
    _isSaveAScan = true;
    _sideForAScanScreen = side;
    _channelIdForAScanScreen = channelId;
    _gateIndexForAScanScreen = gateIndex;
    _gainForAScanScreen = gain;
    _sensForAScanScreen = sens;
    _tvgForAScanScreen = tvg;
    _timeDelayForAScanScreen = timeDelay;
    _surfaceForAScanScreen = surface;
    _startGateForAScanScreen = startGate;
    _endGateForAScanScreen = endGate;

    QTimer::singleShot(500, this, &MainWindow::shootScreen);
}

void MainWindow::onScreenShotButtonReleased(unsigned int key)
{
    QString pathToViewScreenShoot = registrationServiceScreenShoots() + QString::number(key) + QString(".png");
    _screenShotsServiceView->showScreen(pathToViewScreenShoot);
    _screenShotsServiceView->show();
}

void MainWindow::onBlockHandScanButtons(bool isBlock)
{
    ui->handLateralButtonsView->blockButtons(isBlock);
    ui->handLateralPanelView->blockPanel(isBlock);
}

void MainWindow::blockControlsForScreenShoot(bool isBlock)
{
    ui->handLateralButtonsView->blockButtons(isBlock);
    ui->handLateralPanelView->blockPanel(isBlock);

    ui->leftScanLateralButtonsView->blockButtons(isBlock);
    ui->leftScanLateralPanelView->blockButtons(isBlock);

    ui->rightScanLateralButtonsView->blockButtons(isBlock);
    ui->rightScanLateralPanelView->blockButtons(isBlock);

    ui->menuButton->setDisabled(isBlock);
    ui->handButton->setDisabled(isBlock);
    ui->scanButton->setDisabled(isBlock);
    ui->showHideControlsButton->setDisabled(isBlock);
}

void MainWindow::blockControlsForExitFromApp()
{
    blockControlsForScreenShoot(true);
    ui->backPushButton->setDisabled(true);
    ui->boltJointButton->setDisabled(true);
    ui->notesButton->setDisabled(true);
    ui->railTypeButton->setDisabled(true);
    disconnect(_externalKeyboard, &ExternalKeyboard::toggled, this, &MainWindow::externalKeyToggled);
}

void MainWindow::onConfigurationRequired(bool isRequired)
{
    _waitMessagePage->hide();
    _waitMessagePage->setText("");
    if (!isRequired) {
        _waitMessagePage->setText(tr("No configuration required!"));
        _waitMessagePage->setClicked(true);
        _waitMessagePage->show();
    }
}

void MainWindow::onSetControlledRail(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    ui->controledRailBarLabel->setText(userData.toString());
}

void MainWindow::onBatterySettingsButton_released()
{
    Q_ASSERT(_batteryOptions);
    switchToMenuPage(_batteryOptions);
}

void MainWindow::onMinimalLevelBatteryChanged(qreal minimalPercent)
{
    QDEBUG << "Minimum battery level:" << minimalPercent;
    ui->battery->setMinimalPercent(static_cast<int>(minimalPercent));
    saveMinimalBatteryPercent(static_cast<int>(minimalPercent));
}

void MainWindow::onPowerOffSystem()
{
    Core::instance().umuPower(false);
    powerOffSystem();
}

void MainWindow::onOperatingOrganizationButton_released()
{
    switchToMenuPage(ui->operatingOrganizationPage);
}

void MainWindow::on_changeOrganizationNameButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeOrganizationName);
    showVirtualKeyboardsWithLineEditAndValidatorNone(ui->organizationLineEdit->text());
}

void MainWindow::on_changeDepartmentNameButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeDepartmentName);
    showVirtualKeyboardsWithLineEditAndValidatorNone(ui->departamentLineEdit->text());
}

void MainWindow::onCalibrationInfoButton_released()
{
    switchToMenuPage(ui->calibrationInfoPage);
}

void MainWindow::on_changeCalibrationDateButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeCalibrationDate);
    showVirtualKeyboardsWithLineEditAndValidatorDate(ui->calibrationDateLineEdit->text());
}

void MainWindow::on_changeCalibrationValidityButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeCalibrationValidity);
    showVirtualKeyboardsWithLineEditAndValidatorDate(ui->calibrationValidityLineEdit->text());
}

void MainWindow::on_changeCertificatNumberButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeCertificatNumber);
    showVirtualKeyboardsWithLineEditAndValidatorNone(ui->calibrationCertificateNumberLineEdit->text());
}

void MainWindow::on_startSwitchButton_released()
{
    Core::instance().registration().addStartSwitch();
    Core::instance().defectmarker().setSwitchShunterState(true);
}

void MainWindow::on_endSwitchButton_released()
{
    Core::instance().registration().addEndSwitch();
    Core::instance().defectmarker().setSwitchShunterState(false);
}

void MainWindow::onAcousticContactOptionsPageButton_released()
{
    switchToMenuPage(ui->acousticContactSettingsPage);
}

#if defined TARGET_AVICON31
void MainWindow::on_Av17PageButton_released()
{
    switchToMenuPage(_av17Page);
}
void MainWindow::onStartAv17DefView()
{
    switchToMenuPage(_av17DefView);
}
#endif

void MainWindow::onHeadScanOptionsPageButton_released()
{
    switchToMenuPage(ui->headScannerSettingsPage);
}

void MainWindow::on_scannerEncodersCorrectionButton_released()
{
    ui->bScanPage->plugTheBscan(false);
    Core::instance().setUmuLineToScannerControl();
    _scannerEncodersPage->resetEncodersDistances();
    switchToMenuPage(_scannerEncodersPage);
}

void MainWindow::onRegistrationThresholdSpinBoxChanged(qreal value)
{
    Core& core = Core::instance();
    core.setRegistrationThreshold(static_cast<int>(value));
    saveRegistrationThreshold(static_cast<int>(value));
    ui->aScanPage->restoreBScanLevel();

    // Reload filter parameters for other threshold
    // Threshold value checked inside settings function.
    core.deviceSetFilterParams(fprmMinPacketSize, restoreFilterMinPacketSize());
    core.deviceSetFilterParams(fprmMaxCoordSpacing, restoreFilterMaxCoordSpacing());
    core.deviceSetFilterParams(fprmMaxDelayDelta, restoreFilterMaxDelayDelta());
    core.deviceSetFilterParams(fprmMaxSameDelayConsecutive, restoreFilterMaxSameDelayConsecutive());
    core.setImportantAreaMinPacketSize(restoreImportantAreaMinPacketSize());
}

void MainWindow::onScreenHeaterSettingsButton_released()
{
    switchToMenuPage(_screenHeaterOptions);
}

void MainWindow::onFtpSettingsButton_released()
{
    switchToMenuPage(ui->ftpSettingsPage);
}

void MainWindow::on_editFTPServerButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeFTPServer);
    showVirtualKeyboardsWithPlainTextAndValidatorNone(ui->ftpServerEdit->text());
}

void MainWindow::on_editFTPPathButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeFTPPath);
    showVirtualKeyboardsWithPlainTextAndValidatorNone(ui->ftpPathEdit->text());
}

void MainWindow::on_editFTPLoginButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeFTPLogin);
    showVirtualKeyboardsWithPlainTextAndValidatorNone(ui->ftpLoginEdit->text());
}

void MainWindow::on_editFTPPasswordButton_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeFTPPassword);
    showVirtualKeyboardsWithPlainTextAndValidatorNone(ui->ftpPasswordEdit->text());
}

void MainWindow::onImpotantAreaSettingPushButton_released()
{
    switchToMenuPage(ui->importantAreaInicationPage);
}

void MainWindow::onAcStateSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    bool isEnabled = userData.toBool();
    saveStateAcousticContactControl(isEnabled);
    Core::instance().enableAC(isEnabled);
    Core::instance().registration().updateStateAcousticContact();
}

void MainWindow::onBatteryLevelRequestTimeout()
{
#if defined ANDROID && defined TARGET_AVICON15
    Core::instance().getUMUBatteryVoltage();
    Core::instance().getUMUBatteryPercent();
    int value = getBatteryLevelJNI();
    ui->buiBatteryLabel->setText("/" + QString::number(value) + " %");
#endif
}

void MainWindow::on_changeSerialNumber_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeCduSerialNumber);
    showVirtualKeyboardsWithLineEditAndValidatorNumbers(ui->cduSerialNumberLineEdit->text());
}


void MainWindow::changeCduSerialNumber()
{
    ui->cduSerialNumberLineEdit->setText(VirtualKeyboards::instance()->value());
    ui->cduSerialNumberLabel->setText(VirtualKeyboards::instance()->value());
    setCduSerialNumber(VirtualKeyboards::instance()->value());
}

void MainWindow::on_changeDefectoscopeSerialNumber_released()
{
    VirtualKeyboards::instance()->setCallback(this, &MainWindow::changeDefectoscopeSerialNumber);
    showVirtualKeyboardsWithLineEditAndValidatorNumbers(ui->defectoscopeSerialNumberLineEdit->text());
}

void MainWindow::onCorrectRegistration()
{
    Core::instance().registration().modifyRegistrationFileHeader(
        _registrationPage->getOperator(), _registrationPage->getLine(), _registrationPage->getTrackNumber(), Core::instance().getTrackMarks(), _registrationPage->getLeftSide() ? true : false, _registrationPage->getDirection() == 1 ? true : false, _registrationPage->getDirectionName().toInt());
}

void MainWindow::onDateChanged()
{
    onDateTimeChanged(QDateTime::currentDateTime());
}

void MainWindow::onHeaterStateChanged(bool _isOn)
{
    _isOn ? ui->heaterLabel->show() : ui->heaterLabel->hide();
}

void MainWindow::onMemoryInfo(unsigned long long total, unsigned long long user, unsigned long long system)
{
    ui->memoryUsageWidget->setMemoryInfo(total, user, system);
}

void MainWindow::onSetColor(const QColor& color, int index)
{
    saveColorImportantAreaIndex(index);
    saveColorImportantAreaColor(color.red(), color.green(), color.blue());
    ui->bScanPage->setImportaintAreaColor(color);
}

void MainWindow::onSetOpacity(qreal value)
{
    saveOpacityImportantArea(value);
    ui->bScanPage->setImportaintAreaOpacity(value);
}

void MainWindow::changeDefectoscopeSerialNumber()
{
    ui->defectoscopeSerialNumberLineEdit->setText(VirtualKeyboards::instance()->value());
    ui->defectoscopeSerialNumberLabel->setText(VirtualKeyboards::instance()->value());
    setDefectoscopeSerialNumber(VirtualKeyboards::instance()->value());
}

void MainWindow::on_selectSchemePushButton_released()
{
    Q_ASSERT(_waitMessagePage);
    _waitMessagePage->setText(tr("Please wait ... "));
    _waitMessagePage->show();
    qApp->processEvents();
    Core::instance().setSchemeIndex(ui->schemesListWidget->currentIndex().row());
    leaveMenuPage();
}

void MainWindow::on_selectLanguagePushButton_released()
{
    Q_ASSERT(_waitMessagePage);
    _waitMessagePage->setText(tr("Please wait ... "));
    _waitMessagePage->show();
    qApp->processEvents();
    QModelIndex index = ui->listWidget->currentIndex();
    Languages languages = LanguageSwitcher::supportedLanguages();
    QString language = index.model()->data(index).toString();
    Languages::const_iterator iter = languages.constBegin();
    while (iter != languages.constEnd()) {
        if (iter.value() == language) {
            LanguageSwitcher::switchLanguage(iter.key());
            saveLanguage(iter.key());
            break;
        }
        ++iter;
    }
    emit languageChanged();
    leaveMenuPage();
}

void MainWindow::onAccessLevelChanged()
{
    Core& core = Core::instance();
    const auto& permissions = core.getCurrentPermissions();

    if (permissions.find(factoryPermissions) != permissions.end()) {
        _optionsListPage->setItemVisible("deviceOptionsButton", true);
        _optionsListPage->setItemEnabled("setLanguageButton", true);
        ui->noiseReductionLabel->show();
        ui->noiseReductionSpinBox->show();
        ui->eltestModeButton->show();
        ui->memoryPage->setVisibleLogButton(true);
        ui->cduSerialNumberGroupBox->show();
        ui->defectoscopeSerialNumberGroupBox->show();
        ui->changeSerialNumber->setEnabled(true);
        ui->cduSerialNumberLineEdit->setEnabled(true);
        ui->defectoscopeSerialNumberLineEdit->setEnabled(true);
        ui->serviceModeButton->show();
        ui->eltestModeButton->show();
        ui->quitButton->show();
    }
    else {
        _optionsListPage->setItemVisible("deviceOptionsButton", false);
        _optionsListPage->setItemEnabled("setLanguageButton", false);
        ui->eltestModeButton->hide();
        ui->noiseReductionLabel->hide();
        ui->noiseReductionSpinBox->hide();
        ui->memoryPage->setVisibleLogButton(false);
        ui->cduSerialNumberGroupBox->hide();
        ui->defectoscopeSerialNumberGroupBox->hide();
        ui->changeSerialNumber->setEnabled(false);
        ui->cduSerialNumberLineEdit->setEnabled(false);
        ui->defectoscopeSerialNumberLineEdit->setEnabled(false);
        ui->serviceModeButton->hide();
        ui->eltestModeButton->hide();
        ui->quitButton->hide();
    }

    if (permissions.find(labPermissions) != permissions.end()) {
        ui->calibrationDateLineEdit->setEnabled(true);
        ui->changeCalibrationDateButton->setEnabled(true);
        ui->calibrationValidityLineEdit->setEnabled(true);
        ui->changeCalibrationValidityButton->setEnabled(true);
        ui->calibrationCertificateNumberLineEdit->setEnabled(true);
        ui->changeCertificatNumberButton->setEnabled(true);
    }
    else {
        ui->calibrationDateLineEdit->setEnabled(false);
        ui->changeCalibrationDateButton->setEnabled(false);
        ui->calibrationValidityLineEdit->setEnabled(false);
        ui->changeCalibrationValidityButton->setEnabled(false);
        ui->calibrationCertificateNumberLineEdit->setEnabled(false);
        ui->changeCertificatNumberButton->setEnabled(false);
    }

    _ekasuiOptions->enableSettingsEditing(permissions.find(ekasuiPermissions) != permissions.end());
}

void MainWindow::onTrackMarksSelected()
{
    disconnect(this, &MainWindow::trackMarkExternalKeyboard_released, this, &MainWindow::onSelectTrackMarkButton_released);
    updateTrackMarksWidgets();
    emit trackMarksSelected();
}

void MainWindow::onWorkUnworkSideSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    bool isTrolleyLeftSideWork = userData.toBool();
    ui->workUnworkSideSpinBox->setIndex(index);
    saveTrolleyLeftSide(isTrolleyLeftSideWork);
}

void MainWindow::onRegarStatusChaged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    bool regarStatus = userData.toBool();
    saveRegarStatus(regarStatus);
}

void MainWindow::onAutoGainAdjustmentStatusChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    bool autoGainAdjustmentStatus = userData.toBool();
    Core::instance().useAutoGainAdjustment(autoGainAdjustmentStatus);
    saveAutoGainAdjustmentStatus(autoGainAdjustmentStatus);
}

void MainWindow::onParallelVideoBrowsingStatusChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    bool parallelVideoBrowsingStatus = userData.toBool();
    ui->bScanPage->enableParallelVideoBrowsing(parallelVideoBrowsingStatus);
    saveParallelVideoBrowsingStatus(parallelVideoBrowsingStatus);
}

void MainWindow::onTimeForConnectionOut()
{
    Q_ASSERT(_waitMessagePage);
    _waitMessagePage->stopProcessTimer();
    _waitMessagePage->hide();
    Notifier::instance().addNote(Qt::red, tr("Error! \nNo link with UMU!"));
}

void MainWindow::on_trolleyLeftSidePageButton_released()
{
    switchToMenuPage(ui->trolleyLeftSidePage);
}

void MainWindow::onSelectTrackMarkButton_released()
{
    Q_ASSERT(_trackMarksPage);
    _trackMarksPage->selectTrackMark();
    disconnect(this, &MainWindow::trackMarkExternalKeyboard_released, this, &MainWindow::onSelectTrackMarkButton_released);
}

void MainWindow::onEkasuiButton_released()
{
    switchToMenuPage(_ekasuiOptions);
}

void MainWindow::onEKASUIChanged()
{
    // TODO: switchRegistrationControlObject(0);
}

void MainWindow::onBluetoothOptionsButton_released()
{
    switchToMenuPage(_bluetoothManagerPage);
}

void MainWindow::onACVoiceNotificationChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    qDebug() << "Voice notification:" << userData.toBool();
    saveACVoiceNotificationEnabled(userData.toBool());
    emit acNotificationEnabled(userData.toBool());
}

void MainWindow::onImportantAreaIndicationChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    qDebug() << "ImportantAreaIndication = " << userData.toBool();
    saveImportantAreaIndicateEnable(userData.toBool());
    ui->bScanPage->setImportaintAreaIndication(userData.toBool());
    setupImportantAreaIndicate();
}

void MainWindow::onDefectCreated()
{
    Core& core = Core::instance();
    Q_ASSERT(_registrationPage);
    Q_ASSERT(_defectPage);
    QString incidentFileName;
    int incidentIndex = -1;
    if (_registrationType == EKASUIRegistration) {
        EK_ASUI* ekasui = core.getEkasui();
        TMRussian* tmrussian = reinterpret_cast<TMRussian*>(core.getTrackMarks());
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        _ekasuiScreenshot.save(&buffer, "PNG");
        QGeoCoordinate currentCoord = core.registration().getCurrentGeoCoord();
        incidentIndex = ekasui->getIncidentIndex();
        incidentFileName = Core::instance().registration().getCurrentFilePath() + core.registration().getCurrentFileName() + "_" + QString::number(incidentIndex) + ".xml";
        qDebug() << "Creating ekasui incident" << incidentFileName;
        if (_registrationPage->getControlObject() == 0) {
            ekasui->CreateMainPathsIncidentXML(incidentFileName,
                                               core.deviceFirmwareVersion(),
                                               _registrationPage->getOperator(),
                                               restoreEKASUIRailroad(),
                                               restoreEKASUIPRED(),
                                               _registrationPage->getDirectionName(),
                                               _registrationPage->getTrackNumber(),
                                               _defectPage->getDefectLength(),
                                               _defectPage->getDefectDepth(),
                                               _defectPage->getDefectWidth(),
                                               _defectPage->getSpeedLimit(),
                                               tmrussian->getKm(),
                                               tmrussian->getPk(),
                                               tmrussian->getM(),
                                               _defectPage->getDefectCode(),
                                               _defectPage->getComment(),
                                               _defectPage->getLink(),
                                               _defectPage->getSide(),
                                               bytes.data(),
                                               bytes.size(),
                                               "",
                                               currentCoord.latitude(),
                                               currentCoord.longitude());
        }
        else {
            ekasui->CreateStationIncidentXML(incidentFileName,
                                             core.deviceFirmwareVersion(),
                                             _registrationPage->getOperator(),
                                             restoreEKASUIRailroad(),
                                             restoreEKASUIPRED(),
                                             _registrationPage->getStation(),
                                             _registrationPage->getTrackNumber(),
                                             _defectPage->getDefectLength(),
                                             _defectPage->getDefectDepth(),
                                             _defectPage->getDefectWidth(),
                                             _defectPage->getSpeedLimit(),
                                             tmrussian->getKm(),
                                             tmrussian->getPk(),
                                             tmrussian->getM(),
                                             _defectPage->getDefectCode(),
                                             _defectPage->getComment(),
                                             _defectPage->getLink(),
                                             _defectPage->getSide(),
                                             bytes.data(),
                                             bytes.size(),
                                             "",
                                             currentCoord.latitude(),
                                             currentCoord.longitude());
        }
    }

    core.registration().addDefectLabel(_defectPage->getDefectLength(), _defectPage->getDefectDepth(), _defectPage->getDefectWidth(), _defectPage->getDefectCode(), _defectPage->getComment(), _defectPage->getLink(), static_cast<DeviceSide>(_defectPage->getSide()));

    // TODO redo!!!!!!!!!!!!!!!!!!!!!!!!!!
    FTPUploader* ftpUploader = Core::instance().getFtpUploader();
    QString ftpServerAddress;
#if defined TARGET_AVICON31
    ftpServerAddress = restoreFtpServer();
#elif defined TARGET_AVICON15
    ftpServerAddress = _wifiManagerPage->getDefaultGetaway();
#endif
    ftpUploader->setParameters(ftpServerAddress + QString(":2221"), restoreFtpPath(), restoreFtpLogin(), restoreFtpPassword());
    const QString sourcePath = incidentFileName;
    const QString destinationPath = core.registration().getCurrentFileName() + "_" + QString::number(incidentIndex) + ".xml";
    qDebug() << "Source: " << sourcePath;
    qDebug() << "Destination: " << destinationPath;
    ftpUploader->uploadFile(sourcePath, destinationPath);
    leaveMenuPage();
}

void MainWindow::onSensAutoResetButton_released()
{
    switchToMenuPage(_sensAutoReset);
}

void MainWindow::onConnectToWifi()
{
    ui->wifiIndicator->show();
    if (_deviceType == Avicon15) {
        ui->wifiIndicator->setIndicatorPercent(100);
    }
}

void MainWindow::onDisconnectToWifi()
{
    ui->wifiIndicator->setIndicatorPercent(-1);
}

void MainWindow::onQualityWifiPercent(qreal percent)
{
    ui->wifiIndicator->setIndicatorPercent(percent);
}

void MainWindow::onBluetoothConnection(bool isConnect)
{
    if (!isConnect) {
        onWireOutputSetted();
        Notifier::instance().addNote(Qt::red, tr("Bluetooth disconnected.\nReconnect!"));
    }
    ui->bluetoothIndicator->changeBluetoothStatus(isConnect);
}

void MainWindow::onBluetoothSetDeviceAsAudio()
{
    onBtWireOutputSetted();
}

void MainWindow::onScannerRegistrationNext()
{
    Core& core = Core::instance();

    Q_ASSERT(_scannerDefectRegistrationPage);
    _scannerDefectRegistrationPage->switchState(2);
    core.SetAv17CoordX(0);

    ASSERT(connect(&core, &Core::doBScan2Data, _av17Page, &Av17Page::CountCenterOfDefect));
}

void MainWindow::onScannerRegistrationSave()
{
    eDeviceSide workrail;
    char workside;
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    av17config.WorkRail = _scannerDefectRegistrationPage->getWorkRail();
    if (av17config.WorkRail == 0)
        workrail = dsLeft;
    else if (av17config.WorkRail == 1)
        workrail = dsRight;
    else
        workrail = dsNone;
    av17config.ScannerOrientation = _scannerDefectRegistrationPage->getEdge();
    if (av17config.ScannerOrientation == 1)
        workside = 2;
    else
        workside = 1;

    saveHeadScannerParams(av17config.XPathStep, av17config.YPathStep, av17config.StartY, av17config.ScanWidth, av17config.ScanHeight, av17config.CurrentY, av17config.ScannerOrientation, av17config.WorkRail, av17config.threshold);

    Core& core = Core::instance();

    if (_registrationType == EKASUIRegistration) {
        TMRussian* tmrussian = reinterpret_cast<TMRussian*>(core.getTrackMarks());
        QGeoCoordinate currentCoord = core.registration().getCurrentGeoCoord();
        EK_ASUI* ekasui = core.getEkasui();
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        _ekasuiScreenshot.save(&buffer, "PNG");
        if (_registrationPage->getControlObject() == 0) {
            ekasui->CreateMainPathsIncidentXML(core.registration().getCurrentFilePath() + core.registration().getCurrentFileName() + ".xml",
                                               core.deviceFirmwareVersion(),
                                               _registrationPage->getOperator(),
                                               restoreEKASUIRailroad(),
                                               restoreEKASUIPRED(),
                                               _registrationPage->getDirectionName(),
                                               _registrationPage->getTrackNumber(),
                                               _scannerDefectRegistrationPage->getHSize(),
                                               _scannerDefectRegistrationPage->getDSize(),
                                               _scannerDefectRegistrationPage->getLSize(),
                                               _scannerDefectRegistrationPage->getSpeedLimit(),
                                               tmrussian->getKm(),
                                               tmrussian->getPk(),
                                               tmrussian->getM(),
                                               _scannerDefectRegistrationPage->getDefectCode(),
                                               _scannerDefectRegistrationPage->getComment(),
                                               _scannerDefectRegistrationPage->getLink(),
                                               _scannerDefectRegistrationPage->getWorkRail(),
                                               bytes.data(),
                                               bytes.size(),
                                               "",
                                               currentCoord.latitude(),
                                               currentCoord.longitude());
        }
        else {
            ekasui->CreateStationIncidentXML(core.registration().getCurrentFilePath() + core.registration().getCurrentFileName() + ".xml",
                                             core.deviceFirmwareVersion(),
                                             _registrationPage->getOperator(),
                                             restoreEKASUIRailroad(),
                                             restoreEKASUIPRED(),
                                             _registrationPage->getStation(),
                                             _registrationPage->getTrackNumber(),
                                             _scannerDefectRegistrationPage->getHSize(),
                                             _scannerDefectRegistrationPage->getDSize(),
                                             _scannerDefectRegistrationPage->getLSize(),
                                             _scannerDefectRegistrationPage->getSpeedLimit(),
                                             tmrussian->getKm(),
                                             tmrussian->getPk(),
                                             tmrussian->getM(),
                                             _scannerDefectRegistrationPage->getDefectCode(),
                                             _scannerDefectRegistrationPage->getComment(),
                                             _scannerDefectRegistrationPage->getLink(),
                                             _scannerDefectRegistrationPage->getWorkRail(),
                                             bytes.data(),
                                             bytes.size(),
                                             "",
                                             currentCoord.latitude(),
                                             currentCoord.longitude());
        }
    }

    core.registration().AddRailHeadScanerData(workrail,
                                              workside,
                                              2,
                                              av17config.threshold,
                                              av17data.Head._scanTime / 1000,
                                              _scannerDefectRegistrationPage->getDefectCode(),
                                              QString::number(_scannerDefectRegistrationPage->getHSize()) + " " + QString::number(_scannerDefectRegistrationPage->getLSize()),
                                              _scannerDefectRegistrationPage->getComment(),
                                              av17data.getBaWH().data(),
                                              av17data.getBaWH().size(),
                                              av17data.getBaLW().data(),
                                              av17data.getBaLW().size(),
                                              av17data.getBaLH().data(),
                                              av17data.getBaLH().size(),
                                              av17data.getBaBScan().data(),
                                              av17data.getBaBScan().size());
}

void MainWindow::onWireOutputSetted()
{
    Core::instance().setWireOutput();
}

void MainWindow::onBtWireOutputSetted()
{
    Core::instance().setBtWireOutput();
}

void MainWindow::onBluetoothQuickBarReleased()
{
    Q_ASSERT(_quickBar);
    _quickBar->hide();
    onBluetoothOptionsButton_released();
}

void MainWindow::onWifiQuickBarReleased()
{
    Q_ASSERT(_quickBar);
    _quickBar->hide();
    onWifiManagerButton_released();
}

void MainWindow::onDeviceOptionsButton_released()
{
    switchToMenuPage(_deviceOptions);
}

void MainWindow::onStartElTest()
{
    _timerElTest = new QTimer();
    _timerElTest->setInterval(2000);
    _timerElTest->setSingleShot(false);
    ASSERT(connect(_timerElTest, &QTimer::timeout, this, &MainWindow::onTimerElTestTimeout));
    ASSERT(connect(ui->aScanPage, &AScanPage::startTimer, this, &MainWindow::onStartTimerElTest));
    ASSERT(connect(&Core::instance(), &Core::doUmuConnectionStatusChanged, this, &MainWindow::onUmuConnectionStatusChanged));
    qDebug() << QString("START ELECTRIC TEST");
    _electricTestPage->setCountCrashes(restoreCountCrashes());
    _electricTestPage->setCountCduReboot(restoreCountCduReboot());
    _electricTestPage->setCountUmuDisconnections(restoreCountUmuDisconnect());
    _isElTestActivate = true;
    saveElectricTestStatus(_isElTestActivate);
    _timerElTest->start();
}

void MainWindow::onStopElTest()
{
    ui->aScanPage->timerElectricTestStop();
    if (_timerElTest->isActive()) {
        _timerElTest->stop();
        disconnect(_timerElTest, &QTimer::timeout, this, &MainWindow::onTimerElTestTimeout);
        disconnect(&Core::instance(), &Core::doModelsReady, this, &MainWindow::onModelsReadyStartElTest);
        disconnect(&Core::instance(), &Core::doUmuConnectionStatusChanged, this, &MainWindow::onUmuConnectionStatusChanged);
    }
    delete _timerElTest;
    qDebug() << QString("STOP ELECTRIC TEST");
    _isElTestActivate = false;
    saveElectricTestStatus(_isElTestActivate);
    _lastScanChannelLeftPanel = 0;
    _lastScanChannelRightPanel = 0;
    _lastHandChannel = 0;
    saveCountCrashes(0);
    saveCountCduReboot(0);
    saveCountUmuDisconnect(0);
}

void MainWindow::onTimerElTestTimeout()
{
    if (Core::instance().isUmuConnected() == false) {
        _timerElTest->stop();
        return;
    }

    if (_lastScanChannelLeftPanel != _scanChannelsCountLeftPanel) {
        if (_lastScanChannelLeftPanel == 0) {
            ui->scanButton->pressed();
        }
        setScanChannel(_lastScanChannelLeftPanel, LeftPanel);
        _timerElTest->stop();
        ui->aScanPage->timerElectricTestStart();
        ++_lastScanChannelLeftPanel;
        return;
    }

    if (_lastScanChannelRightPanel != _scanChannelsCountRightPanel) {
        setScanChannel(_lastScanChannelRightPanel, RightPanel);
        _timerElTest->stop();
        ui->aScanPage->timerElectricTestStart();
        ++_lastScanChannelRightPanel;
        return;
    }

    if (_lastHandChannel != _handChannelsCount) {
        if (_lastHandChannel == 0) {
            ui->handButton->pressed();
        }
        setHandChannel(_lastHandChannel);
        _timerElTest->stop();
        ui->aScanPage->timerElectricTestStart();
        ++_lastHandChannel;
        return;
    }
    else {
        ui->scanButton->pressed();
        _lastScanChannelLeftPanel = 0;
        _lastScanChannelRightPanel = 0;
        _lastHandChannel = 0;
    }
}

void MainWindow::onStartTimerElTest()
{
    if (_timerElTest->isActive() == false) {
        _timerElTest->start();
    }
}

void MainWindow::onModelsReadyStartElTest()
{
    if (_isElTestActivate == true) {
        _electricTestPage->startElectricTestAfterCrash();
    }
}

void MainWindow::onUmuConnectionStatusChanged(bool isConnected)
{
    if (!_isElTest && !isConnected && _deviceType == Avicon31Default) {
        Notifier::instance().addNote(Qt::red, QString(tr("UMU disconnected!!!")));
        return;
    }
    if (isConnected != _isUmuConnected) {
        _isUmuConnected = isConnected;
        if (_isUmuConnected == false) {
            saveCountUmuDisconnect(restoreCountUmuDisconnect() + 1);
            qDebug() << QString("COUNT UMU DISCONNECTED: ") << QString::number(restoreCountUmuDisconnect());
            _electricTestPage->setCountUmuDisconnections(restoreCountUmuDisconnect());
            _electricTestPage->stopElectricTest();
        }
        ui->aScanPage->setUmuConnectionStatus(_isUmuConnected);
    }
}

void MainWindow::on_permissionsPage_released()
{
    switchToMenuPage(_permissionsPage);
}

void MainWindow::onConfigurePasswordsPagePressed()
{
    switchToMenuPage(_passwordManagerPage);
}

void MainWindow::on_showTestRemoteButton_released()
{
#if defined DEBUG && !defined IMX_DEVICE
    disconnect(_testExternalKeyboard, &TestExternalKeyboardWidget::testTrackMarkButtonPressed, this, &MainWindow::trackMarkExternalKeyboard_released);
    disconnect(_testExternalKeyboard, &TestExternalKeyboardWidget::testServiceMarkButtonPressed, this, &MainWindow::serviceMarkExternalKeyboard_released);
    disconnect(_testExternalKeyboard, &TestExternalKeyboardWidget::testBoltJointButtonPressed, this, &MainWindow::doStartBoltJoint);
    disconnect(_testExternalKeyboard, &TestExternalKeyboardWidget::testBoltJointButtonReleased, this, &MainWindow::doStopBoltJoint);
    connect(_testExternalKeyboard, &TestExternalKeyboardWidget::testTrackMarkButtonPressed, this, &MainWindow::trackMarkExternalKeyboard_released);
    connect(_testExternalKeyboard, &TestExternalKeyboardWidget::testServiceMarkButtonPressed, this, &MainWindow::serviceMarkExternalKeyboard_released);
    connect(_testExternalKeyboard, &TestExternalKeyboardWidget::testBoltJointButtonPressed, this, &MainWindow::doStartBoltJoint);
    connect(_testExternalKeyboard, &TestExternalKeyboardWidget::testBoltJointButtonReleased, this, &MainWindow::doStopBoltJoint);
    _testExternalKeyboard->show();
#endif
}

void MainWindow::onNotificationOptionsButton_released()
{
    switchToMenuPage(_notificationOptions);
}

void MainWindow::on_memoryButtonImport_released()
{
    switchToMenuPage(_memoryImportPage);
}

void MainWindow::onStartSwitchControl()
{
    ui->switchLabel->show();
    ui->switchControlLabelLine->show();
}

void MainWindow::onStopSwitchControl()
{
    ui->switchLabel->hide();
    ui->switchControlLabelLine->hide();
    ui->specialRailTypeLabel->hide();
    ui->specialRailTypeLabelLine->hide();
    Core::instance().setDynamicRailType(false);
    // TODO stop special rail type in defcore method.
}

void MainWindow::onRemoteControlConnected()
{
    ui->remoteControlLabel->show();
    ui->coordinatesPage->setAntennaStatus(GeoPosition::AntennaStatusConnected);
}

void MainWindow::onRemoteControlDisconnected()
{
    ui->remoteControlLabel->hide();
    ui->coordinatesPage->setAntennaStatus(GeoPosition::AntennaStatusDisconnected);
}

void MainWindow::onUpdateRemoteState()
{
    Core::instance().updateRemoteState(_stateMachine->configuration().contains(_registrationOnState), _typeView);
}

void MainWindow::onTmSelectedFromRemoteControl()
{
    updateTrackMarksWidgets();
    Core::instance().onTrackMarksSelected();
}

void MainWindow::onSatellitesInUse(int countSatellites)
{
    ui->coordinatesPage->setSatellitesInUse(countSatellites);
}

void MainWindow::onSatellitesInfo(const QGeoPositionInfo& info)
{
    ui->coordinatesPage->setGeoPositionInfo(info);
}

void MainWindow::onKeyCombinationsOptionsButton_released()
{
    switchToMenuPage(_keyCombinationsOptions);
}

void MainWindow::onRegarOptionsButton_released()
{
    switchToMenuPage(ui->regarOptionsPage);
}

void MainWindow::onAutoGainAdjustmentOptionsButton_released()
{
    switchToMenuPage(ui->autoGainAdjustmentPage);
}

void MainWindow::onParallelVideoBrowsingOptionsButton_released()
{
    switchToMenuPage(ui->parallelVideoBrowsingOptionsPage);
}

void MainWindow::onPathEncoderEmulatorOptionsButton_released()
{
    switchToMenuPage(_pathEncoderEmulatorOptions);
}

void MainWindow::on_switchControlButton_released()
{
    _switchTypePage->setDeviceLeftSide(_registrationPage->getLeftSide());
    switchToMenuPage(_switchTypePage);
}

void MainWindow::onUpdateViewedMark()
{
    updateTrackMarksWidgets();
}

void MainWindow::onSetBScanMode(bool value)
{
    if (value) {
        ui->bScanPage->resetBscans();
        clearMenuHistory();
    }
    else {
        setLateralPanelsVisibility(false);
        clearLateralPanelsSelection();
        setAcousticContactPanelsVisibility(false);
        ui->leftScanTapeAcousticContactView->clearView();
        ui->rightScanTapeAcousticContactView->clearView();
        ui->handLateralPanelView->setVisible(false);
        setVisibleSpeedLabel(true);
        hideStackedLayout();
        switchToPage(ui->memoryPage);
        Core::instance().registration().addMenuMode();
    }
    ui->bScanPage->setMode(value);
    ui->leftScanLateralPanelView->setDisabled(value);
    ui->rightScanLateralPanelView->setDisabled(value);
    ui->notesButton->setDisabled(value);
    ui->railTypeButton->setDisabled(value);
    ui->handButton->setDisabled(value);
    ui->scanButton->setDisabled(value);
    ui->pauseButton->setDisabled(value);
    ui->menuButton->setDisabled(value);
    setEnabledBoltJointState(value);
    _value = value;
}
