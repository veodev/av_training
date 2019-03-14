#include <QPushButton>
#include <QtMath>
#include <QDir>

#include "ascanpage.h"
#include "ui_ascanpage.h"

#include "settings.h"
#include "spinboxnumber.h"
#include "spinboxlist.h"
#include "core.h"
#include "debug.h"
#include "roles.h"
#include "channelscontroller.h"
#include "styles.h"
#include "notifier.h"
#include "accessories.h"
#include "limitsforsens.h"
#include "registration.h"
#include "appdatapath.h"
#include "ChannelsIds.h"

#define X_RANGE_COUNT 232
#define MARKER_WIDTH 10
#if !defined TARGET_AVICON15
#define MINIMUM_SENS -80
#define MAXIMUM_SENS 80
#else
#define MINIMUM_SENS -60
#define MAXIMUM_SENS 60
#endif
#define CHECK_HAND_SCAN_TIME_TIMER 100

static const int bScanReplotInterval = 60;

class SpinBoxNumberDelegateUs : public SpinBoxNumberDelegateBase
{
public:
    explicit SpinBoxNumberDelegateUs(AScanPage* aScanPage)
        : _aScanPage(aScanPage)
    {
    }

    ~SpinBoxNumberDelegateUs() {}

    virtual QString value(qreal value)
    {
        switch (_aScanPage->_rulerMode) {
        case AScanPage::rulerModeMm: {
            return (QString::number(static_cast<double>(_aScanPage->delayToDepth(value)), 'f', 1) + QObject::tr(" mm"));
        } break;
        case AScanPage::rulerModeNone:
            return QString(QObject::tr("%1")).arg(_aScanPage->delayToPixel(value));
            break;
        default:
            break;
        }
        return QString(QObject::tr("%1 us")).arg(value);
    }

private:
    AScanPage* _aScanPage;
};

class SpinBoxNumberDelegateGates : public SpinBoxNumberDelegateBase
{
public:
    explicit SpinBoxNumberDelegateGates(AScanPage* aScanPage)
        : _aScanPage(aScanPage)
        , _defaultGate(0)
        , _isDefaultGateEnabled(true)
    {
    }

    ~SpinBoxNumberDelegateGates() {}

    void setDefaultGate(qreal value)
    {
        _defaultGate = value;
    }

    void setDefaultGateEnable(bool value)
    {
        _isDefaultGateEnabled = value;
    }

    virtual QString value(qreal value)
    {
        if (_isDefaultGateEnabled == true) {
            switch (_aScanPage->_rulerMode) {
            case AScanPage::rulerModeMm:
                return QString(QObject::tr("%1 (%2) mm")).arg(format(value)).arg(format(_defaultGate));
                break;
            case AScanPage::rulerModeNone:
                return QString(QObject::tr("%1 (%2)")).arg(_aScanPage->delayToPixel(value)).arg(_aScanPage->delayToPixel(_defaultGate));
                break;
            default:
                break;
            }
            return QString(QObject::tr("%1 (%2) us")).arg(value).arg(_defaultGate);
        }

        switch (_aScanPage->_rulerMode) {
        case AScanPage::rulerModeMm:
            return QString(QObject::tr("%1 mm")).arg(format(value));
            break;
        case AScanPage::rulerModeNone:
            return QString(QObject::tr("%1")).arg(_aScanPage->delayToPixel(value));
            break;
        default:
            break;
        }
        return QString(QObject::tr("%1 us")).arg(value);
    }

private:
    QString format(qreal value)
    {
        return QString::number(static_cast<double>(_aScanPage->delayToDepth(value)), 'f', 1);
    }

private:
    AScanPage* _aScanPage;
    qreal _defaultGate;
    bool _isDefaultGateEnabled;
};

AScanPage::AScanPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AScanPage)
    , _isConnected(false)
    , _isModelIndexValid(false)
    , _displayThresholdBscan(new SpinBoxNumber(this))
    , _scaleBscan(new SpinBoxNumber(this))
    , _spinBoxNumberDelegateUs(new SpinBoxNumberDelegateUs(this))
    , _spinBoxNumberDelegateStartGate(new SpinBoxNumberDelegateGates(this))
    , _spinBoxNumberDelegateEndGate(new SpinBoxNumberDelegateGates(this))
    , _timeDelaySpinBox(new SpinBoxNumber(this))
    , _sensSpinBoxNumber(new SpinBoxNumber(this))
    , _gateStartSpinBoxNumber(new SpinBoxNumber(this))
    , _gateEndSpinBoxNumber(new SpinBoxNumber(this))
    , _markSpinBoxNumber(new SpinBoxNumber(this))
    , _tvgSpinBoxNumber(new SpinBoxNumber(this))
    , _notchStartSpinBoxNumber(new SpinBoxNumber(this))
    , _notchSizeSpinBoxNumber(new SpinBoxNumber(this))
    , _notchLevelSpinBoxNumber(new SpinBoxNumber(this))
    , _rulerModeList(new SpinBoxList(this))
    , _setZeroDbButton(new QPushButton(this))
    , _calibrateChannelButton(new QPushButton(this))
    , _autoTimeDelayButton(new QPushButton(this))
    , _toneSwitchOffButton(new QPushButton(this))
    , _screenShootViewButton(new QPushButton(this))
    , _toneOnlyForChannelButton(new QPushButton(this))
    , _surfaceList(new SpinBoxList(this))
    , _railList(new SpinBoxList(this))
    , _registrationTimerSpinBoxNumber(new SpinBoxNumber(this))
    , _saveAScanButton(new QPushButton(this))
    , _startOfRegistrationButton(new QPushButton(this))
    , _calibrationModeSpinBoxList(new SpinBoxList(this))
    , _rulerMode(rulerModeNone)
    , _noiseReduction(restoreNoiseReduction())
    , _isStartRegChecked(false)
    , _isBScanTimeContinue(false)
    , _isElTest(false)
    , _isElTestActive(false)
    , _cid(-1)
    , _deviceSide(NoneDeviceSide)
    , _gateIndex(-1)
    , _bScanPlotIndex(0)
    , _multiply(1)
    , _aScanScale(1)
    , _enterAngle(0)
    , _bScanLevel(0)
    , _channelsModel(nullptr)
    , _mode(unknownMode)
    , _calibrationMode(cmSens)
    , _bScanReplotTimer(new QTimer(this))
    , _handScanTimer(new QTimer(this))
    , _checkHandScanTimeTimer(new QTimer(this))
    , _currentChannelType(ctCompInsp)
    , _deviceType(static_cast<DeviceType>(restoreDeviceType()))
{
    ui->setupUi(this);
    restoreBScanLevel();

    ui->hLabel->setPalette(Styles::cellType1Palette());
    ui->rLabel->setPalette(Styles::cellType1Palette());
    ui->modeLabel->setPalette(Styles::cellType1Palette());
    ui->lLabel->setPalette(Styles::cellType1Palette());
    ui->nLabel->setPalette(Styles::cellType1Palette());
    ui->gainLabel->setPalette(Styles::cellType1Palette());
    ui->gainBaseLabel->setPalette(Styles::cellType1Palette());
    ui->kdLabel->setPalette(Styles::cellType1Palette());

    _bScanReplotTimer->setInterval(bScanReplotInterval);

    ASSERT(connect(_handScanTimer, &QTimer::timeout, this, &AScanPage::onHandScanTimerOff));

    _checkHandScanTimeTimer->setInterval(CHECK_HAND_SCAN_TIME_TIMER);
    ASSERT(connect(_checkHandScanTimeTimer, &QTimer::timeout, this, &AScanPage::onCheckHandScanTimerOff));

    Core& core = Core::instance();

    ASSERT(connect(&core, &Core::doConnectionStatusDisconnected, this, &AScanPage::onConnectionStatusDisconnected));
    ASSERT(connect(&core, &Core::doConnectionStatusConnecting, this, &AScanPage::onConnectionStatusConnecting));
    ASSERT(connect(&core, &Core::doConnectionStatusConnected, this, &AScanPage::onConnectionStatusConnected));

    ui->controlsArea->setDisabled(true);

    ChannelsController* channelsController = core.channelsController();
    ASSERT(connect(channelsController, &ChannelsController::doChannelSetted, this, &AScanPage::onChannelSetted));

    ASSERT(connect(channelsController, &ChannelsController::doHeadChannelSetted, this, &AScanPage::onHeadChannelSetted));

    Q_ASSERT(_timeDelaySpinBox != nullptr);
    ASSERT(connect(_timeDelaySpinBox, &SpinBoxNumber::valueChanged, this, &AScanPage::timeDelayControlChanged));
    _timeDelaySpinBox->enableCaption();
    _timeDelaySpinBox->setPrecision(3);
    _timeDelaySpinBox->setMinimum(0.2);
    _deviceType == AviconDB ? _timeDelaySpinBox->setMaximum(100) : _timeDelaySpinBox->setMaximum(15);
    _timeDelaySpinBox->setStepBy(0.1);
    _timeDelaySpinBox->setSuffix(tr(" us"));
    _timeDelaySpinBox->setValue(0, false);
    _timeDelaySpinBox->setEnabled(false);

    Q_ASSERT(_sensSpinBoxNumber != nullptr);
    ASSERT(connect(_sensSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::sensControlChanged));
    _sensSpinBoxNumber->enableCaption();
    _sensSpinBoxNumber->setMinimum(MINIMUM_SENS);
    _sensSpinBoxNumber->setMaximum(MAXIMUM_SENS);
    _sensSpinBoxNumber->setValue(0, false);

    QFont font;
    font.setPointSize(22);
    font.setBold(true);
    font.setWeight(75);

    ASSERT(connect(_setZeroDbButton, &QPushButton::toggled, this, &AScanPage::setZeroDbButtonPressed));
    _setZeroDbButton->setCheckable(true);
    _setZeroDbButton->setMinimumHeight(60);
    _setZeroDbButton->setFont(font);
    _setZeroDbButton->setStyleSheet(Styles::button());
    _setZeroDbButton->setFocusPolicy(Qt::NoFocus);

    ASSERT(connect(_calibrateChannelButton, &QPushButton::pressed, this, &AScanPage::calibrateChannelButtonPressed));
    _calibrateChannelButton->setMinimumHeight(60);
    _calibrateChannelButton->setFont(font);
    _calibrateChannelButton->setStyleSheet(Styles::areaButton());
    _calibrateChannelButton->setFocusPolicy(Qt::NoFocus);

    _autoTimeDelayButton->setEnabled(false);
    ASSERT(connect(_autoTimeDelayButton, &QPushButton::pressed, this, &AScanPage::autoTimeDelayButtonPressed));
    _autoTimeDelayButton->setMinimumHeight(60);
    _autoTimeDelayButton->setFont(font);
    _autoTimeDelayButton->setStyleSheet(Styles::button());
    _autoTimeDelayButton->setFocusPolicy(Qt::NoFocus);

    _toneSwitchOffButton->setCheckable(true);
    _toneSwitchOffButton->setMinimumHeight(60);
    _toneSwitchOffButton->setFont(font);
    _toneSwitchOffButton->setStyleSheet(Styles::areaCheckedButton());
    _toneSwitchOffButton->setFocusPolicy(Qt::NoFocus);
    ASSERT(connect(_toneSwitchOffButton, &QPushButton::toggled, this, &AScanPage::toneSwitchOffButtonPressed));

    _toneOnlyForChannelButton->setCheckable(true);
    _toneOnlyForChannelButton->setMinimumHeight(60);
    _toneOnlyForChannelButton->setFont(font);
    _toneOnlyForChannelButton->setStyleSheet(Styles::areaCheckedButton());
    _toneOnlyForChannelButton->setFocusPolicy(Qt::NoFocus);
    ASSERT(connect(_toneOnlyForChannelButton, &QPushButton::toggled, this, &AScanPage::toneOnlyForChannelButtonPressed));

    _saveAScanButton->setMinimumHeight(60);
    _saveAScanButton->setFont(font);
    _saveAScanButton->setStyleSheet(Styles::button());
    _saveAScanButton->setFocusPolicy(Qt::NoFocus);
    ASSERT(connect(_saveAScanButton, &QPushButton::released, this, &AScanPage::saveAScanButtonReleased));

    _startOfRegistrationButton->setCheckable(true);
    _startOfRegistrationButton->setMinimumHeight(60);
    _startOfRegistrationButton->setFont(font);
    _startOfRegistrationButton->setStyleSheet(Styles::button());
    _startOfRegistrationButton->setFocusPolicy(Qt::NoFocus);
    ASSERT(connect(_startOfRegistrationButton, &QPushButton::released, this, &AScanPage::startOfRegistrationButtonRelease));

    ASSERT(connect(_gateStartSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::gateStartValueChanged));
    _gateStartSpinBoxNumber->enableCaption();
    _gateStartSpinBoxNumber->setPrecision(4);
    _gateStartSpinBoxNumber->setMinimum(0);
    _gateStartSpinBoxNumber->setMaximum(pixelToDelay(X_RANGE_COUNT));
    _gateStartSpinBoxNumber->setValue(0, false);
    _gateStartSpinBoxNumber->setDelegate(_spinBoxNumberDelegateStartGate);

    ASSERT(connect(_gateEndSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::gateEndValueChanged));
    _gateEndSpinBoxNumber->enableCaption();
    _gateEndSpinBoxNumber->setPrecision(4);
    _gateEndSpinBoxNumber->setMinimum(0);
    _gateEndSpinBoxNumber->setMaximum(pixelToDelay(X_RANGE_COUNT));
    _gateEndSpinBoxNumber->setValue(0, false);
    _gateEndSpinBoxNumber->setDelegate(_spinBoxNumberDelegateEndGate);

    ui->aScanPlot->enableGates();

    ASSERT(connect(_markSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::markValueChanged));
    _markSpinBoxNumber->enableCaption();
    _markSpinBoxNumber->setPrecision(3);
    _markSpinBoxNumber->setMinimum(0);
    _markSpinBoxNumber->setMaximum(pixelToDelay(X_RANGE_COUNT));
    _markSpinBoxNumber->setValue(0, false);
    _markSpinBoxNumber->setDelegate(_spinBoxNumberDelegateUs);

    ASSERT(connect(_tvgSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::tvgControlChanged));
    _tvgSpinBoxNumber->enableCaption();
    _tvgSpinBoxNumber->setMinimum(0);
    _tvgSpinBoxNumber->setMaximum(40);
    _tvgSpinBoxNumber->setValue(0, false);
    _tvgSpinBoxNumber->setDelegate(_spinBoxNumberDelegateUs);

    ASSERT(connect(_notchStartSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::notchStartChanged));
    _notchStartSpinBoxNumber->enableCaption();
    _notchStartSpinBoxNumber->setDelegate(_spinBoxNumberDelegateUs);

    ASSERT(connect(_notchSizeSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::notchSizeChanged));
    _notchSizeSpinBoxNumber->enableCaption();
    _notchSizeSpinBoxNumber->setDelegate(_spinBoxNumberDelegateUs);

    ASSERT(connect(_notchLevelSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &AScanPage::notchLevelChanged));
    _notchLevelSpinBoxNumber->setMinimum(0);
    _notchLevelSpinBoxNumber->setMaximum(12);
    _notchLevelSpinBoxNumber->setStepBy(1);
    _notchLevelSpinBoxNumber->enableCaption();

    _surfaceList->enableCaption();

    _railList->enableCaption();

    _registrationTimerSpinBoxNumber->enableCaption();
    _registrationTimerSpinBoxNumber->setMinimum(4);
    _registrationTimerSpinBoxNumber->setMaximum(40);
    _registrationTimerSpinBoxNumber->setStepBy(4);
    _registrationTimerSpinBoxNumber->setValue(4, false);

    ASSERT(connect(_rulerModeList, &SpinBoxList::valueChanged, this, &AScanPage::rulerModeValueChanged));
    _rulerModeList->enableCaption();

    Q_ASSERT(_screenShootViewButton != nullptr);
    _screenShootViewButton->setMinimumHeight(60);
    _screenShootViewButton->setFont(font);
    _screenShootViewButton->setStyleSheet(Styles::button());
    _screenShootViewButton->setFocusPolicy(Qt::NoFocus);

    ASSERT(connect(_screenShootViewButton, &QPushButton::released, this, &AScanPage::onScreenShootViewButtonReleased));

    ASSERT(connect(_calibrationModeSpinBoxList, &SpinBoxList::valueChanged, this, &AScanPage::calibrationModeChanged));
    _calibrationModeSpinBoxList->setMinimumHeight(60);
    _calibrationModeSpinBoxList->setFont(font);
    _calibrationModeSpinBoxList->setFocusPolicy(Qt::NoFocus);

    if (getAScanPlotOptimization()) {
        ui->aScanPlot->setSignalCurvePen(QPen(Qt::blue), 0);
        ui->aScanPlot->setSignalCurvePen(QPen(Qt::red), 1);
        ui->aScanPlot->setSignalCurveBrush(QBrush(Qt::NoBrush), 0);
        ui->aScanPlot->setSignalCurveBrush(QBrush(Qt::NoBrush), 1);
        ui->aScanPlot->setSignalCurveRenderAntialiased(false, 0);
        ui->aScanPlot->setSignalCurveRenderAntialiased(false, 1);
        ui->aScanPlot->setTvgCurvePen(QPen(Qt::darkRed), 0);
        ui->aScanPlot->setTvgCurvePen(QPen(Qt::darkRed), 1);
        ui->aScanPlot->setTvgCurveRenderAntialiased(false, 0);
        ui->aScanPlot->setTvgCurveRenderAntialiased(false, 1);
        ui->aScanPlot->setGatesSymbolPen(QPen(Qt::darkGreen));
    }

    ui->aScanPlot->setMaximumMarkerValue(0, 0);
    ui->aScanPlot->setMaximumMarkerVisible(false, true);

    ui->aScanPlot->enableSignal(true, 0);
    ui->aScanPlot->setSignalCurvePen(QPen(Qt::red), 1);
    ui->aScanPlot->setSignalCurveBrush(QBrush(Qt::NoBrush), 1);
    ui->aScanPlot->setSignalCurveRenderAntialiased(true, 1);
    ui->aScanPlot->setActiveSignal(1);

    visibleR(false);
    visibleH();

    _displayThresholdBscan->enableCaption();
    _displayThresholdBscan->setMinimum(-12);
    _displayThresholdBscan->setMaximum(18);
    _displayThresholdBscan->setStepBy(2);
    _displayThresholdBscan->setValue(0);
    ASSERT(connect(_displayThresholdBscan, &SpinBoxNumber::valueChanged, this, &AScanPage::onThresholdBscanChanged));

    ASSERT(connect(_scaleBscan, &SpinBoxNumber::valueChanged, this, &AScanPage::onChangeBscanScale));
    _scaleBscan->enableCaption();
    _scaleBscan->setPrecision(2);
    _scaleBscan->setMinimum(0.5);
    _scaleBscan->setMaximum(20.0);
    _scaleBscan->setStepBy(0.5);
    _scaleBscan->setValue(restoreBscanScale(), false);

    setRulerMode(rulerModeUs);
    setFillingAScanState(restoreAScanFillingState());
    retranslateUi();
}

AScanPage::~AScanPage()
{
    qDebug() << "Deleting AScanPage...";
    Core& core = Core::instance();
    ChannelsController* channelsController = core.channelsController();
    Q_ASSERT(channelsController != nullptr);
    disconnect(channelsController, &ChannelsController::doChannelSetted, this, &AScanPage::onChannelSetted);
    disconnect(channelsController, &ChannelsController::doHeadChannelSetted, this, &AScanPage::onHeadChannelSetted);
    disconnect(_handScanTimer, &QTimer::timeout, this, &AScanPage::onHandScanTimerOff);
    disconnect(_checkHandScanTimeTimer, &QTimer::timeout, this, &AScanPage::onCheckHandScanTimerOff);
    disconnect(&core, &Core::doConnectionStatusDisconnected, this, &AScanPage::onConnectionStatusDisconnected);
    disconnect(&core, &Core::doConnectionStatusConnecting, this, &AScanPage::onConnectionStatusConnecting);
    disconnect(&core, &Core::doConnectionStatusConnected, this, &AScanPage::onConnectionStatusConnected);
    disconnect(&core, &Core::doAScanMeas, this, &AScanPage::onAScanMeas);
    disconnect(&core, &Core::doAScanData, this, &AScanPage::onAScanData);
    disconnect(&core, &Core::doTVGData, this, &AScanPage::onTVGData);
    disconnect(&core, &Core::doBScan2Data, this, &AScanPage::onBScan2Data);
    disconnect(&core, &Core::doBScanDisplayThresholdChanged, this, &AScanPage::bScanDisplayThresholdChanged);

    disconnect(_bScanReplotTimer, &QTimer::timeout, this, &AScanPage::bScanReplotTimeout);
    _handScanTimer->stop();
    _checkHandScanTimeTimer->stop();

    if (_spinBoxNumberDelegateUs != nullptr) {
        delete _spinBoxNumberDelegateUs;
    }
    delete _spinBoxNumberDelegateStartGate;
    delete _spinBoxNumberDelegateEndGate;
    delete _bScanReplotTimer;
    delete _handScanTimer;
    delete _checkHandScanTimeTimer;
    delete _timeDelaySpinBox;
    delete _sensSpinBoxNumber;
    delete _setZeroDbButton;
    delete _calibrateChannelButton;
    delete _autoTimeDelayButton;
    delete _toneSwitchOffButton;
    delete _toneOnlyForChannelButton;
    delete _saveAScanButton;
    delete _startOfRegistrationButton;
    delete _gateStartSpinBoxNumber;
    delete _gateEndSpinBoxNumber;
    delete _markSpinBoxNumber;
    delete _tvgSpinBoxNumber;
    delete _notchStartSpinBoxNumber;
    delete _notchSizeSpinBoxNumber;
    delete _notchLevelSpinBoxNumber;
    delete _surfaceList;
    delete _railList;
    delete _registrationTimerSpinBoxNumber;
    delete _rulerModeList;
    delete _screenShootViewButton;
    delete _calibrationModeSpinBoxList;
    delete ui;
    qDebug() << "AScanPage deleted!";
}

void AScanPage::setBScanDotSize(int value)
{
    ui->bScanTape->setBScanDotSize(value);
}

void AScanPage::setControlsAreaVisible(bool isVisible)
{
    isVisible == true ? ui->controlsArea->show() : ui->controlsArea->hide();
}

bool AScanPage::controlsAreaVisible()
{
    return ui->controlsArea->isVisible();
}

void AScanPage::setModel(QAbstractItemModel* channelsModel)
{
    Q_ASSERT(channelsModel);

    if (_channelsModel != channelsModel) {
        if (_channelsModel != nullptr) {
            _channelsModel->disconnect(this);
        }
        _channelsModel = channelsModel;
        if (_channelsModel != nullptr) {
            ASSERT(connect(_channelsModel, &QAbstractItemModel::dataChanged, this, &AScanPage::dataChanged, Qt::UniqueConnection));
        }
    }
}

void AScanPage::setHeadScanModel(QAbstractItemModel* headScanChannelsModel)
{
    Q_ASSERT(headScanChannelsModel);
    _headScanChannelsModel = headScanChannelsModel;
    if (_headScanChannelsModel != nullptr) {
        _headScanChannelsModel->disconnect(this);
    }
    ASSERT(connect(_headScanChannelsModel, &QAbstractItemModel::dataChanged, this, &AScanPage::headScanDataChanged, Qt::UniqueConnection));
}

void AScanPage::setMode(AScanPage::Modes mode)
{
    if (_mode == mode) {
        return;
    }
    _mode = mode;

    setControlsAreaVisible(false);
    ui->controlsArea->reset();
    _calibrationModeSpinBoxList->setIndex(0);
    ui->controlsArea->addWidget(_gateStartSpinBoxNumber);
    ui->controlsArea->addWidget(_gateEndSpinBoxNumber);
    ui->controlsArea->addWidget(_rulerModeList);
    ui->controlsArea->addWidget(_sensSpinBoxNumber);
    ui->controlsArea->addWidget(_tvgSpinBoxNumber);
    ui->controlsArea->addWidget(_timeDelaySpinBox);

    switch (mode) {
    case evaluation:
        _timeDelaySpinBox->setEnabled(true);
        ui->controlsArea->addWidget(_markSpinBoxNumber);
        ui->controlsArea->addWidget(_toneSwitchOffButton);
        ui->controlsArea->addWidget(_toneOnlyForChannelButton);
        if (_deviceType != Avicon31Estonia) {
            ui->controlsArea->addWidget(_screenShootViewButton);
        }
        ui->gainBaseLabel->setVisible(false);
        ui->bScanTape->setVisible(true);
        if (_deviceType == AviconDB || _deviceType == AviconDBHS) {
            ui->controlsArea->addWidget(_notchStartSpinBoxNumber);
            ui->controlsArea->addWidget(_notchSizeSpinBoxNumber);
            ui->controlsArea->addWidget(_notchLevelSpinBoxNumber);
            ui->controlsArea->addWidget(_saveAScanButton);
        }
        break;
    case hand:
        _timeDelaySpinBox->setEnabled(true);
        ui->controlsArea->addWidget(_markSpinBoxNumber);
        ui->controlsArea->addWidget(_screenShootViewButton);
        ui->controlsArea->addWidget(_surfaceList);
        ui->controlsArea->addWidget(_railList);
        ui->controlsArea->addWidget(_registrationTimerSpinBoxNumber);
        ui->controlsArea->addWidget(_startOfRegistrationButton);
        ui->bScanTape->setBScanHorizontalScale(12);
        if (_deviceType == AviconDBHS) {
            ui->controlsArea->addWidget(_scaleBscan);
            ui->controlsArea->addWidget(_displayThresholdBscan);
            ui->controlsArea->addWidget(_saveAScanButton);
            ui->bScanTape->setEncoderCorrection(5);
        }
        else {
            ui->bScanTape->setEncoderCorrection(20);
        }
        ui->gainBaseLabel->setVisible(false);
        ui->bScanTape->setVisible(true);
        ui->bScanTape->setAllBscanCurvesVisible(true);
        break;
    case calibrationModeHand:
        ui->controlsArea->addWidget(_setZeroDbButton);
        ui->controlsArea->addWidget(_calibrateChannelButton);
        ui->controlsArea->addWidget(_calibrationModeSpinBoxList);
        ui->controlsArea->addWidget(_autoTimeDelayButton);
        _timeDelaySpinBox->setEnabled(false);
        ui->bScanTape->setVisible(false);
        ui->gainBaseLabel->setVisible(true);
        break;
    case calibrationModeScan:
        ui->controlsArea->addWidget(_setZeroDbButton);
        ui->controlsArea->addWidget(_calibrateChannelButton);
        ui->controlsArea->addWidget(_calibrationModeSpinBoxList);
        ui->controlsArea->addWidget(_autoTimeDelayButton);
        _timeDelaySpinBox->setEnabled(false);
        ui->gainBaseLabel->setVisible(true);
        _calibrateChannelButton->setVisible(true);
        ui->bScanTape->setVisible(false);
        break;
    case headScanMode:
        _timeDelaySpinBox->setEnabled(true);
        ui->controlsArea->addWidget(_markSpinBoxNumber);
        ui->gainBaseLabel->setVisible(false);
        ui->bScanTape->setVisible(false);
        break;
    case unknownMode:
    default:
        ui->bScanTape->setVisible(false);
        break;
    }
    setControlsAreaVisible(true);
}

void AScanPage::setVisible(bool visible)
{
    Core& core = Core::instance();
    if (visible == true) {
        ASSERT(connect(&core, &Core::doAScanMeas, this, &AScanPage::onAScanMeas));
        ASSERT(connect(&core, &Core::doAScanData, this, &AScanPage::onAScanData));
        ASSERT(connect(&core, &Core::doTVGData, this, &AScanPage::onTVGData));
        ASSERT(connect(&core, &Core::doBScan2Data, this, &AScanPage::onBScan2Data));
        ASSERT(connect(&core, &Core::doBScanDisplayThresholdChanged, this, &AScanPage::bScanDisplayThresholdChanged));
        ASSERT(connect(_bScanReplotTimer, &QTimer::timeout, this, &AScanPage::bScanReplotTimeout));
    }
    else {
        disconnect(&core, &Core::doAScanMeas, this, &AScanPage::onAScanMeas);
        disconnect(&core, &Core::doAScanData, this, &AScanPage::onAScanData);
        disconnect(&core, &Core::doTVGData, this, &AScanPage::onTVGData);
        disconnect(&core, &Core::doBScan2Data, this, &AScanPage::onBScan2Data);
        disconnect(&core, &Core::doBScanDisplayThresholdChanged, this, &AScanPage::bScanDisplayThresholdChanged);
        disconnect(_bScanReplotTimer, &QTimer::timeout, this, &AScanPage::bScanReplotTimeout);
        enableToneForAllChannels();
    }

    QWidget::setVisible(visible);
}

void AScanPage::onSetControledRail(int index, const QString& value, const QVariant& userData)
{
    _railList->setIndex(index);
    emit doSetControlledRail(index, value, userData);
}

void AScanPage::onHotKeyReleased(Actions action)
{
    if (this->isVisible() == false) {
        return;
    }

    switch (action) {
    case KyPlus:
        _sensSpinBoxNumber->setValue(_sensSpinBoxNumber->value() + _sensSpinBoxNumber->stepBy());
        break;
    case KyMinus:
        _sensSpinBoxNumber->setValue(_sensSpinBoxNumber->value() - _sensSpinBoxNumber->stepBy());
        break;
    case TvgPlus:
        _tvgSpinBoxNumber->setValue(_tvgSpinBoxNumber->value() + _tvgSpinBoxNumber->stepBy());
        break;
    case TvgMinus:
        _tvgSpinBoxNumber->setValue(_tvgSpinBoxNumber->value() - _tvgSpinBoxNumber->stepBy());
        break;
    default:
        break;
    }
}

bool AScanPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void AScanPage::retranslateUi()
{
    _timeDelaySpinBox->setCaption(tr("t.delay"));
    _timeDelaySpinBox->setSuffix(tr(" us"));
    _sensSpinBoxNumber->setCaption(tr("Sens."));
    _sensSpinBoxNumber->setSuffix(tr(" () dB"));
    _gateStartSpinBoxNumber->setCaption(tr("Gate start"));
    _gateStartSpinBoxNumber->refresh();
    _gateEndSpinBoxNumber->setCaption(tr("Gate end"));
    _gateEndSpinBoxNumber->refresh();
    _markSpinBoxNumber->setCaption(tr("Mark"));
    _markSpinBoxNumber->refresh();
    _tvgSpinBoxNumber->setCaption(tr("TVG"));
    _tvgSpinBoxNumber->refresh();
    _notchStartSpinBoxNumber->setCaption(tr("Notch start"));
    _notchStartSpinBoxNumber->refresh();
    _notchSizeSpinBoxNumber->setCaption(tr("Notch size"));
    _notchSizeSpinBoxNumber->refresh();
    _notchLevelSpinBoxNumber->setCaption(tr("Notch level"));
    _notchLevelSpinBoxNumber->setSuffix(tr(" dB"));
    _rulerModeList->setCaption(tr("Ruler Mode"));
    _rulerModeList->clear();
    _rulerModeList->addItem(tr("us"), rulerModeUs);
    _rulerModeList->addItem(tr("mm"), rulerModeMm);
#ifdef DEBUG
    _rulerModeList->addItem(tr("counts"), rulerModeNone);
#endif
    _setZeroDbButton->setText(tr("Set 0 dB"));
    _calibrateChannelButton->setText(tr("Calibrate"));
    _autoTimeDelayButton->setText(tr("Auto t.delay"));
    _toneSwitchOffButton->setStyleSheet("border-image: url(\":/tone_on.png\")");
    _screenShootViewButton->setText(tr("Prev. calib."));
    _toneOnlyForChannelButton->setStyleSheet("border-image: url(\":/only_tone_on.png\")");
    _surfaceList->clear();
    _surfaceList->setCaption(tr("Surface"));
    *_surfaceList << tr("Top") << tr("Gauge head") << tr("Field head") << tr("Gauge web") << tr("Field web") << tr("Gauge pen") << tr("Field pen");
    _railList->clear();
    _railList->setCaption(tr("Rail"));
    _railList->addItem(tr("Left"), QString("Left"));
    _railList->addItem(tr("Right"), QString("Right"));
    _registrationTimerSpinBoxNumber->setCaption(tr("Timer"));
    _saveAScanButton->setText(tr("Save A-Scan"));
    _startOfRegistrationButton->setText(tr("Start reg."));
    _calibrationModeSpinBoxList->clear();
    _calibrationModeSpinBoxList->addItem(tr("By depth"), cmSens);
    _calibrationModeSpinBoxList->addItem(tr("By beam"), cmPrismDelay);
    _displayThresholdBscan->setCaption(tr("B-Scan level"));
    _displayThresholdBscan->setSuffix(tr(" dB"));
    _scaleBscan->setCaption(tr("B-Scan scale"));
    _scaleBscan->setSuffix(tr(" s"));
}

void AScanPage::updateBscanVerticalScale()
{
    ui->bScanTape->setBScanVerticalScale(_channelsModel->data(_modelIndex, StartBscangateRole).toInt(), _channelsModel->data(_modelIndex, EndBscangateRole).toInt());
}

void AScanPage::updateGates()
{
    QAbstractItemModel* model;
    QModelIndex index;

    if (_mode != headScanMode) {
        model = _channelsModel;
        index = _modelIndex;
    }
    else {
        model = _headScanChannelsModel;
        index = _headScanModelIndex;
    }

    if (model == nullptr) {
        return;
    }

    float startGateUs = model->data(index, StartGateRole).toFloat();
    float endGateUs = model->data(index, EndGateRole).toFloat();
    float defaultStartGateUs = model->data(index, DefaultStartGateRole).toFloat();
    float defaultEndGateUs = model->data(index, DefaultEndGateRole).toFloat();

    if (_gateStartSpinBoxNumber != nullptr) {
        float startGate = startGateUs;
        float defaultStartGate = defaultStartGateUs;
        if (_mode == unknownMode || _mode == evaluation) {
            _spinBoxNumberDelegateStartGate->setDefaultGate(defaultStartGate);
            _spinBoxNumberDelegateStartGate->setDefaultGateEnable(true);
        }
        else {
            _spinBoxNumberDelegateStartGate->setDefaultGateEnable(false);
        }
        _gateStartSpinBoxNumber->setValue(startGate, false);
        _gateStartSpinBoxNumber->setDisabled(model->data(index, IsStartGateLocked).toBool());
    }
    if (_gateEndSpinBoxNumber != nullptr) {
        float endGate = endGateUs;
        float defaultEndGate = defaultEndGateUs;
        if (_mode == unknownMode || _mode == evaluation) {
            _spinBoxNumberDelegateEndGate->setDefaultGate(defaultEndGate);
            _spinBoxNumberDelegateEndGate->setDefaultGateEnable(true);
        }
        else {
            _spinBoxNumberDelegateEndGate->setDefaultGateEnable(false);
        }
        _gateEndSpinBoxNumber->setValue(endGate, false);

        if (_mode == calibrationModeScan || _mode == calibrationModeHand) {
            _calibrationMode == cmSens ? _gateEndSpinBoxNumber->setDisabled(true) : _gateEndSpinBoxNumber->setEnabled(true);
        }
        else {
            _gateEndSpinBoxNumber->setDisabled(model->data(index, IsEndGateLocked).toBool());
        }
    }

    ui->aScanPlot->setGate(Core::instance().evaluationGateLevel(), ::round(delayToPixel(startGateUs)), ::round(delayToPixel(endGateUs)));

    if (_mode != headScanMode) {
        ui->bScanTape->setGatesScale(_channelsModel->data(_modelIndex, EndBscangateRole).toInt());
        bool isProbePulseFromAbove = ui->bScanTape->getProbePulseLocation();
        int maxGateSize = ui->bScanTape->getMaxGateSize();
#ifdef TARGET_AVICONDB
        if (isProbePulseFromAbove == true) {
            _channelsModel->data(_modelIndex, ChannelIdRole).toInt() != N55MSLW ? ui->bScanTape->setGate(0, maxGateSize - startGateUs, maxGateSize - endGateUs, 0, 0) : ui->bScanTape->setGate(0, startGateUs, endGateUs, 0, 0);
        }
        else {
            _channelsModel->data(_modelIndex, ChannelIdRole).toInt() == N55MSLU ? ui->bScanTape->setGate(0, maxGateSize - startGateUs, maxGateSize - endGateUs, 0, 0) : ui->bScanTape->setGate(0, startGateUs, endGateUs, 0, 0);
        }
#else
        isProbePulseFromAbove == true ? ui->bScanTape->setGate(0, maxGateSize - startGateUs, maxGateSize - endGateUs, 0, 0) : ui->bScanTape->setGate(0, startGateUs, endGateUs, 0, 0);
#endif
    }
}

void AScanPage::setRulerMode(AScanPage::RulerMode mode)
{
    _rulerMode = mode;
    float factor = 1;
    int xRange = X_RANGE_COUNT;
    switch (mode) {
    case rulerModeUs:
        factor = pixelToDelayFactor();
        xRange = aScanLen();
        break;
    case rulerModeMm:
        factor = pixelToDepthFactor();
        xRange = delayToDepth(aScanLen());
        break;
    case rulerModeNone:
    default:
        break;
    }
    _gateStartSpinBoxNumber->setMaximum(xRange);
    _gateEndSpinBoxNumber->setMaximum(xRange);
    _markSpinBoxNumber->setMaximum(xRange);
    ui->aScanPlot->setFactor(factor);
    ui->aScanPlot->setXRange(0, xRange);
    _gateStartSpinBoxNumber->refresh();
    _gateEndSpinBoxNumber->refresh();
    _markSpinBoxNumber->refresh();
    _tvgSpinBoxNumber->refresh();
    refreshMarkerBoarders();
}

void AScanPage::noiseReductionFilter(unsigned char* array)
{
    Q_ASSERT(array != NULL);
    if (_noiseReduction) {
        for (unsigned char i = 0; i < X_RANGE_COUNT; ++i) {
            if (array[i] < 16) {
                array[i] = 0;
            }
            else if (array[i] < 32) {
                array[i] = 32 * (array[i] - 15) / 16;
            }
        }
    }
}

void AScanPage::reset()
{
    ui->aScanPlot->reset();
    ui->bScanTape->clearTape();
    ui->bScanTape->resetGates();
    _setZeroDbButton->setChecked(false);

    setH(0);
    setR(0);
    setL(0);
    setN(0);
    setKd(0);
    setModeTitle("");
}

void AScanPage::calibrateButtonEffectSetOk()
{
    _calibrateChannelButton->setText(tr("Calibrated"));
    _calibrateChannelButton->setStyleSheet(CALIBRATION_SET_OK);
    QTimer::singleShot(2000, this, &AScanPage::calibrateButtonEffectTimeout);
}

void AScanPage::calibrateButtonEffectSetFailed()
{
    _calibrateChannelButton->setText(tr("Failed"));
    _calibrateChannelButton->setStyleSheet(CALIBRATION_SET_FAILED);
    QTimer::singleShot(2000, this, &AScanPage::calibrateButtonEffectTimeout);
}

void AScanPage::bScanReplot()
{
    if (ui->bScanTape->getIsInvalid()) {
        fillBScanTape();
    }
    ui->bScanTape->replotBScanPlot();

    _isBScanTimeContinue = true;
    if (_bScanReplotTimer->isActive() == false) {
        _bScanReplotTimer->start();
    }
}

float AScanPage::pixelToDelayFactor() const
{
    return (_aScanScale / 10.0f);
}

float AScanPage::delayToDepthFactor() const
{
    float divider = 2.F;
    _mode == headScanMode ? divider = 1.F : divider = 2.F;
    if (_enterAngle != 0) {
        return ((3.26F * cos(static_cast<float>(_enterAngle) * M_PI / 180.F)) / divider);
    }
    return (5.9F / divider);
}

float AScanPage::pixelToDepthFactor() const
{
    return (pixelToDelayFactor() * delayToDepthFactor());
}

float AScanPage::delayToPixel(float delay) const
{
    return (delay / pixelToDelayFactor());
}

float AScanPage::depthToDelay(float depth) const
{
    return (depth / delayToDepthFactor());
}

float AScanPage::delayToDepth(float delay) const
{
    return (delay * delayToDepthFactor());
}

float AScanPage::pixelToDelay(float delay) const
{
    return (delay * pixelToDelayFactor());
}

int AScanPage::aScanLen() const
{
    return (_aScanScale * X_RANGE_COUNT) / 10;
}

void AScanPage::enableToneForAllChannels()
{
    if (_toneOnlyForChannelButton->isChecked()) {
        toneOnlyForChannelButtonPressed(false);
    }
}

void AScanPage::blockControlsAreaForRegistration(bool isBlock)
{
    _gateStartSpinBoxNumber->setDisabled(isBlock);
    _gateEndSpinBoxNumber->setDisabled(isBlock);
    _tvgSpinBoxNumber->setDisabled(isBlock);
    _sensSpinBoxNumber->setDisabled(isBlock);
    _timeDelaySpinBox->setDisabled(isBlock);
    _markSpinBoxNumber->setDisabled(isBlock);
    _autoTimeDelayButton->setDisabled(isBlock);
    _surfaceList->setDisabled(isBlock);
    _railList->setDisabled(isBlock);
    _registrationTimerSpinBoxNumber->setDisabled(isBlock);
    _screenShootViewButton->setDisabled(isBlock);
    _rulerModeList->setDisabled(isBlock);
    _saveAScanButton->setDisabled(isBlock);
}

void AScanPage::deleteScreenShoot(unsigned int key)
{
    QDir dir(registrationServiceScreenShoots());
    dir.remove(QString::number(key) + QString(".png"));
}

void AScanPage::makeScreenShoot()
{
    deleteScreenShoot(Core::instance().getScreenShootKey(static_cast<eDeviceSide>(_deviceSide), _cid, _gateIndex));
    unsigned int key = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    Core::instance().setScreenShootKey(static_cast<eDeviceSide>(_deviceSide), _cid, _gateIndex, key);
    emit shootScreen(key);
}

void AScanPage::setCalibrationTemperature()
{
    Core::instance().setCalibrationTemperature(static_cast<eDeviceSide>(_deviceSide), _cid);  // TODO Add gate index
}

void AScanPage::refreshMarkerBoarders()
{
    switch (_rulerMode) {
    case AScanPage::rulerModeMm:
        ui->aScanPlot->setLeftMarkerValue(delayToDepth(ui->aScanPlot->leftMarkerValue()));
        ui->aScanPlot->setRightMarkerValue(delayToDepth(ui->aScanPlot->rightMarkerValue()));
        break;
    default:
        ui->aScanPlot->setLeftMarkerValue(depthToDelay(ui->aScanPlot->leftMarkerValue()));
        ui->aScanPlot->setRightMarkerValue(depthToDelay(ui->aScanPlot->rightMarkerValue()));
        break;
    }
}

void AScanPage::changeSensForElectricTest()
{
    int currentSens = _channelsModel->data(_modelIndex, SensRole).toInt();
    int currentTvg = _channelsModel->data(_modelIndex, TvgRole).toInt();
    int tmpSens = currentSens;
    for (int i = 0; i < 120; ++i) {
        if (!_isUmuConnected || !_isElTestActive) {
            _timerElTest->stop();
            Core::instance().channelsController()->setSens(currentSens);
            Core::instance().channelsController()->setTvg(currentTvg);
            break;
        }
        delay(30);
        if (i < 40 || i >= 100) {
            tmpSens = tmpSens + 1;
        }
        else if (i >= 40 && i < 100) {
            tmpSens = tmpSens - 1;
        }
        changeTvgForElectricTest();
        Core::instance().channelsController()->setSens(tmpSens);
        delay(30);
    }
    Core::instance().channelsController()->setSens(currentSens);
}

void AScanPage::changeTvgForElectricTest()
{
    int currentTvg = _channelsModel->data(_modelIndex, TvgRole).toInt();
    int tmpTvg = currentTvg;
    for (int i = 0; i < 40; ++i) {
        if (!_isUmuConnected || !_isElTestActive) {
            _timerElTest->stop();
            Core::instance().channelsController()->setTvg(currentTvg);
            break;
        }
        (i < 20) ? (++tmpTvg) : (--tmpTvg);
        delay(30);
        Core::instance().channelsController()->setTvg(tmpTvg);
        delay(30);
    }
    Core::instance().channelsController()->setTvg(currentTvg);
    delay(30);
}

void AScanPage::checkCalibrationTemperature()
{
    int externalTemperature = Core::instance().getExternalTemperature();
    int calibrationTemperature = _channelsModel->data(_modelIndex, CalibrationTemperatureRole).toInt();
    qDebug() << "Ext temp = " << externalTemperature << " Calib temp = " << calibrationTemperature;
    (abs(externalTemperature - calibrationTemperature) >= 10) ? ui->gainLabel->setStyleSheet("background-color: red; color: white") : ui->gainLabel->setStyleSheet("");
}

void AScanPage::drawSpan(const BScanDisplaySpan& span)
{
    for (const BScanDisplayData& currentItem : span) {
        size_t size = currentItem.Signals.size();
        ui->bScanTape->nextStep(currentItem.Dir);
        for (unsigned int sig = 0; sig < size; ++sig) {
            CID chan = currentItem.Signals[sig].Channel;
            eDeviceSide side = currentItem.Signals[sig].Side;
            if (_cid == chan && static_cast<int>(side) == _deviceSide) {
                const auto& gate = getGates(side);
                isChannelZero(chan) ? ui->bScanTape->addSignalsN0EMS(currentItem.Signals[sig].Signals, gate.first, gate.second, currentItem.Signals[sig].Count, _multiply, getPlotIndex(chan), getInversion(chan))
                                    : ui->bScanTape->addSignalsNORMAL(currentItem.Signals[sig].Signals, currentItem.Signals[sig].Count, _multiply, getPlotIndex(chan), getInversion(chan));
            }
        }
    }
}

void AScanPage::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (_modelIndex.row() >= topLeft.row() && _modelIndex.row() <= bottomRight.row()) {
        for (int role : roles) {
            switch (role) {
            case TimeDelayRole:
                updateTimeDelay();
                break;
            case SensRole:
            case GainRole:
                updateSens();
                break;
            case TvgRole:
                updateTvg();
                break;
            case MarkRole:
                updateMark();
                break;
            case IsPlayAlarmToneRole:
                updateIsPlayAlarm();
                break;
            case StartNotchRole:
            case SizeNotchRole:
            case LevelNotchRole:
                updateNotchParams();
                break;
            case StartGateRole:
            case EndGateRole:
                updateGates();
                ui->aScanPlot->replot();
                bScanReplot();
                break;
            default:
                break;
            }
        }
    }
}

void AScanPage::headScanDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (_headScanModelIndex.row() >= topLeft.row() && _headScanModelIndex.row() <= bottomRight.row()) {
        for (int role : roles) {
            switch (role) {
            case TimeDelayRole:
                updateTimeDelay();
                break;
            case SensRole:
            case GainRole:
                updateSens();
                break;
            case TvgRole:
                updateTvg();
                break;
            case MarkRole:
                updateMark();
                break;
            case StartGateRole:
            case EndGateRole:
                updateGates();
                ui->aScanPlot->replot();
                break;
            default:
                break;
            }
        }
    }
}

void AScanPage::onConnectionStatusDisconnected()
{
    _isConnected = false;
    updateControlsAreaEnableState();
}

void AScanPage::onConnectionStatusConnecting()
{
    _isConnected = false;
    updateControlsAreaEnableState();
}

void AScanPage::onConnectionStatusConnected()
{
    _isConnected = true;
    updateControlsAreaEnableState();
}

void AScanPage::updateChannelMode()
{
    QString channelMode;

    Q_ASSERT(_channelsModel != nullptr);

    QAbstractItemModel* model = _channelsModel;
    QModelIndex index = _modelIndex;

    if (_mode == headScanMode) {
        model = _headScanChannelsModel;
        index = _headScanModelIndex;
        channelMode += tr("Scanner ");
    }

    ChannelType channelType = static_cast<ChannelType>(model->data(index, ChannelTypeRole).toInt());
    _channelType = channelType;
    if (channelType == HandChannel) {
        channelMode += tr("Hand ");
    }

    switch (_deviceSide) {
    case LeftDeviceSide:
        channelMode += tr("left ");
        break;
    case RightDeviceSide:
        channelMode += tr("right ");
        break;
    default:
        break;
    }

    int angle = model->data(index, AngleRole).toInt();
    channelMode += QString::number(angle) + "° ";

    if (channelType == ScanChannel) {
        Direction direction = static_cast<Direction>(model->data(index, DirectionRole).toInt());
        switch (direction) {
        case ForwardDirection:
            channelMode += tr("forw ");
            break;
        case BackwardDirection:
            channelMode += tr("back ");
            break;
        default:
            break;
        }
    }

    InspectMethod inspectMethod = static_cast<InspectMethod>(model->data(index, MethodRole).toInt());
    switch (inspectMethod) {
    case Echo:
        channelMode += tr("echo ");
        break;
    case MirrorShadow:
        channelMode += tr("MSM ");
        break;
    case Mirror:
        channelMode += tr("mirr ");
        break;
    case Shadow:
        channelMode += tr("shad ");
        break;
    default:
        break;
    }

    eControlZone zone = static_cast<eControlZone>(model->data(index, ZoneRole).toInt());
    switch (zone) {
    case czWeb:
        channelMode += tr("(web) ");
        break;
    case czBase:
        channelMode += tr("(base) ");
        break;
    case czHeadWork:
    case czHeadLeftWork:
    case czHeadAndWork:
        channelMode += tr("(gauge side) ");
        break;
    case czHeadUnWork:
    case czHeadLeftUnWork:
    case czHeadAndUnWork:
        channelMode += tr("(field side) ");
        break;
    default:
        break;
    }

#ifdef DEBUG
    channelMode += QString("(%1)").arg(_cid);
#endif

    switch (static_cast<ChannelFrequency>(model->data(index, FrequencyChannelRole).toInt())) {
    case f2_5MHz:
        channelMode += QString(" 2.5") + QString(tr(" MHz"));
        break;
    case f5MHz:
        channelMode += QString(" 5") + QString(tr(" MHz"));
        break;
    }

    setModeTitle(channelMode);
}

void AScanPage::updateTimeDelay()
{
    if (_channelsModel == nullptr) {
        return;
    }

    float timeDelay = 0;
    _mode == headScanMode ? timeDelay = _headScanChannelsModel->data(_headScanModelIndex, TimeDelayRole).toFloat() : timeDelay = _channelsModel->data(_modelIndex, TimeDelayRole).toFloat();
    _timeDelaySpinBox->setValue(timeDelay / 10, false);
}

void AScanPage::updateSens()
{
    QAbstractItemModel* model;
    QModelIndex index;
    _mode == headScanMode ? (model = _headScanChannelsModel, index = _headScanModelIndex) : (model = _channelsModel, index = _modelIndex);
    int currentGain = model->data(index, GainRole).toInt();
    int currentSens = model->data(index, SensRole).toInt();
    if (currentGain > 0 && currentGain < MAXIMUM_SENS) {
        _sensSpinBoxNumber->setMinimum(MINIMUM_SENS);
        _sensSpinBoxNumber->setMaximum(MAXIMUM_SENS);
    }
    else if (currentGain == 0) {
        _sensSpinBoxNumber->setMinimum(model->data(index, SensRole).toInt());
    }
    else if (currentGain == MAXIMUM_SENS) {
        _sensSpinBoxNumber->setMaximum(model->data(index, SensRole).toInt());
    }
    _sensSpinBoxNumber->setValue(model->data(index, SensRole).toInt(), false);
    _sensSpinBoxNumber->setSuffix(tr(" (%1) dB").arg(model->data(index, RecommendedSensRole).toInt()));
    int key = createKeyFromSchemeAndSideAndChannelAndGateIndex(restoreSchemeNumber(), _deviceSide, _cid, _gateIndex);
    _mode == evaluation ? setGain(currentGain, _mapGainOfControlSection[key]) : setGain(currentGain);
    _setZeroDbButton->isChecked() ? (_setZeroDbButton->setText(tr("Kb ") + QString::number(currentGain) + tr("dB")), setGainBase(currentGain)) : setGainBase(currentGain - currentSens);
}

void AScanPage::updateTvg()
{
    if (_channelsModel == nullptr) {
        return;
    }

    float tvg = 0;
    _mode == headScanMode ? tvg = _headScanChannelsModel->data(_headScanModelIndex, TvgRole).toFloat() : tvg = _channelsModel->data(_modelIndex, TvgRole).toFloat();
    _tvgSpinBoxNumber->setValue(tvg, false);
}

void AScanPage::updateNotchParams()
{
    if (_channelsModel == nullptr) {
        return;
    }
    _notchStartSpinBoxNumber->setValue(_channelsModel->data(_modelIndex, StartNotchRole).toInt(), false);
    _notchSizeSpinBoxNumber->setValue(_channelsModel->data(_modelIndex, SizeNotchRole).toInt(), false);
    _notchLevelSpinBoxNumber->setValue(_channelsModel->data(_modelIndex, LevelNotchRole).toInt(), false);

    _notchStartSpinBoxNumber->setMinimum(_tvgSpinBoxNumber->maximum());
    _notchStartSpinBoxNumber->setMaximum(_channelsModel->data(_modelIndex, EndGateRole).toInt() - _notchSizeSpinBoxNumber->value());
    _notchSizeSpinBoxNumber->setMaximum(_channelsModel->data(_modelIndex, EndGateRole).toInt() - _notchStartSpinBoxNumber->value());
}

void AScanPage::enableNotchControls(bool isEnable)
{
    _notchStartSpinBoxNumber->setEnabled(isEnable);
    _notchSizeSpinBoxNumber->setEnabled(isEnable);
    _notchLevelSpinBoxNumber->setEnabled(isEnable);
}

void AScanPage::updateMark()
{
    if (_channelsModel == nullptr) {
        return;
    }
    float mark = 0;
    _mode == headScanMode ? mark = _headScanChannelsModel->data(_headScanModelIndex, MarkRole).toFloat() : mark = _channelsModel->data(_modelIndex, MarkRole).toFloat();
    _markSpinBoxNumber->setValue(mark, false);
}

void AScanPage::updateIsPlayAlarm()
{
    QVariant value = _channelsModel->data(_modelIndex, IsPlayAlarmToneRole).toBool();
    Q_ASSERT(value.isValid() == true);
    bool IsPlayAlarmTone = !value.toBool();
    _toneSwitchOffButton->setChecked(IsPlayAlarmTone);
}

void AScanPage::updateControlsAreaEnableState()
{
#ifdef DEBUG
    ui->controlsArea->setEnabled(true);
    return;
#endif
    (_isConnected == true && _isModelIndexValid == true) ? ui->controlsArea->setEnabled(true) : ui->controlsArea->setDisabled(true);
}

void AScanPage::onAScanMeas(QSharedPointer<tDEV_AScanMeasure> measure)
{
    Q_ASSERT(measure.data() != 0);

    if (measure.data()->Channel != _cid || measure.data()->Side != static_cast<eDeviceSide>(_deviceSide)) {
        return;
    }

    setH(measure->ParamH);
    setR(measure->ParamR);
    setL(measure->ParamL);
    setN(measure->ParamN);
    setKd(measure->ParamKd);
    if (_noiseReduction && measure->ParamA < _bScanLevel) {
        measure->ParamA = 0;
    }
    ui->aScanPlot->setMaximumMarkerValue(measure->ParamA, delayToPixel(measure->ParamM));
    ui->aScanPlot->replot();
}

void AScanPage::onAScanData(QSharedPointer<tDEV_AScanHead> head, QSharedPointer<tUMU_AScanData> data)
{
    Q_ASSERT(head.data() != 0);
    Q_ASSERT(data.data() != 0);

    if (head.data()->Channel != _cid || head.data()->Side != static_cast<eDeviceSide>(_deviceSide)) {
        return;
    }

    unsigned char* ptr = reinterpret_cast<unsigned char*>(data.data()->Data);
    noiseReductionFilter(ptr);
    ui->aScanPlot->setSignalData(ptr);
    ui->aScanPlot->replot();
}

void AScanPage::onTVGData(QSharedPointer<tDEV_AScanHead> head, QSharedPointer<tUMU_AScanData> data)
{
    Q_ASSERT(head.data() != 0);
    Q_ASSERT(data.data() != 0);

    if (head.data()->Channel != _cid || head.data()->Side != static_cast<eDeviceSide>(_deviceSide)) {
        return;
    }

    ui->aScanPlot->enableTvg(true);
    ui->aScanPlot->setTvg(reinterpret_cast<unsigned char*>(data.data()->Data));
    ui->aScanPlot->replot();
}

std::pair<unsigned short, unsigned short> AScanPage::getGates(eDeviceSide side)
{
    int startGate, endGate;
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    int startGateLeft, endGateLeft, startGateRight, endGateRight;
    Core::instance().channelsController()->getGatesForMsmChannel(startGateLeft, endGateLeft, startGateRight, endGateRight);
    if (side == dsLeft) {
        startGate = startGateLeft;
        endGate = endGateLeft;
    }
    else if (side == dsRight) {
        startGate = startGateRight;
        endGate = endGateRight;
    }
#elif defined TARGET_AVICON15 || defined TARGET_AVICONDB
    Core::instance().channelsController()->getGatesForMsmChannel(startGate, endGate);
#endif
    return std::pair<unsigned short, unsigned short>(startGate, endGate);
}

bool AScanPage::getInversion(CID chan)
{
    bool invert = false;
#if defined TARGET_AVICONDBs
    // Ось сигнала В-развертки для канала N55MSLU направлена сверху вниз
    // а для канала N55MSLW снизу  независимо от режима В - развертки "зондирующий импульс сверху/снизу.
    if (ui->bScanTape->getProbePulseLocation()) {
        // Преворот оси Y В-развертки для канала N55MSLW
        chan == N55MSLW ? invert = true : invert = false;
    }
    else {
        // Преворот оси Y В-развертки для канала N55MSLU
        chan == N55MSLU ? invert = true : invert = false;
    }
#endif
    return invert;
}

unsigned char AScanPage::getPlotIndex(CID chan)
{
    unsigned char plotIndex = _bScanPlotIndex;
    if (chan == F42E || chan == B42E) {
        plotIndex = _bScanPlotIndex % 2;
    }
#if defined TARGET_AVICONDB
    if (chan == F45E_WP || chan == B45E_WP) {
        plotIndex = _bScanPlotIndex % 2;
    }
#endif
    return plotIndex;
}

bool AScanPage::isChannelZero(CID chan)
{
#if defined TARGET_AVICONDB
    return chan == 61;
#else
    return chan == N0EMS;
#endif
}

void AScanPage::onBScan2Data(QSharedPointer<tDEV_BScan2Head> headSharedPtr)
{
    tDEV_BScan2Head* head = headSharedPtr.data();
    Q_ASSERT(head != 0);

    if (_currentChannelType != head->ChannelType) {
        return;
    }

    if (ui->bScanTape->getIsInvalid()) {
        fillBScanTape();
    }
    ui->bScanTape->nextStep(head->Dir[0]);
    for (const tDEV_BScan2HeadItem& item : head->Items) {
        if (item.Channel == _cid && item.Side == static_cast<eDeviceSide>(_deviceSide)) {
            const auto& gate = getGates(item.Side);

            const auto& signalSource = item.Signals;
            tDaCo_BScanSignalList signalsList;
            for (unsigned int i = 0; i < item.Signals.size(); ++i) {
                signalsList[i].Delay = signalSource[i].Delay;
                signalsList[i].Ampl = signalSource[i].Ampl[AmplDBIdx_int] & 0x0f;
            }

            isChannelZero(item.Channel) ? ui->bScanTape->addSignalsN0EMS(signalsList, gate.first, gate.second, item.Signals.size(), _multiply, getPlotIndex(item.Channel), getInversion(item.Channel))
                                        : ui->bScanTape->addSignalsNORMAL(signalsList, item.Signals.size(), _multiply, getPlotIndex(item.Channel), getInversion(item.Channel));
            continue;
        }
    }
}

void AScanPage::bScanDisplayThresholdChanged(int value)
{
    ui->bScanTape->setThresholdDb(value);
}

void AScanPage::onChannelSetted(const QModelIndex& index)
{
    if (index.isValid() == false) {
        return;
    }
    _modelIndex = index;

    enableToneForAllChannels();
    reset();
    blockControlsArea(!_isConnected);
    emit configurationRequired(true);

    if (_modelIndex.isValid()) {
        _cid = _channelsModel->data(_modelIndex, ChannelIdRole).toInt();
        _deviceSide = static_cast<DeviceSide>(_channelsModel->data(_modelIndex, SideRole).toInt());
        _gateIndex = _channelsModel->data(_modelIndex, GateIndexRole).toInt();
        _multiply = _channelsModel->data(_modelIndex, DelayMultiplyRole).toInt();
        _aScanScale = _channelsModel->data(_modelIndex, AScanScaleRole).toInt();
        _enterAngle = _channelsModel->data(_modelIndex, AngleRole).toInt();
        _mapGainOfControlSection = restoreGainForControlSection();
        enableNotchControls(_channelsModel->data(_modelIndex, UseNotchRole).toBool() && _gateIndex == 0);

        if (_mode == calibrationModeScan || _mode == calibrationModeHand) {
            Q_ASSERT(_channelsModel != 0);
            _channelsModel->setData(_modelIndex, false, IsAlarmedRole);
            bool isSlaveChannel = _channelsModel->data(_modelIndex, IsCopySlaveChannelRole).toBool();
            if (isSlaveChannel) {
                emit configurationRequired(!isSlaveChannel);
                makeScreenShoot();
                blockControlsArea(isSlaveChannel && _isConnected);
            }
        }

        _enterAngle == 0 ? _tvgSpinBoxNumber->setMaximum(20) : _tvgSpinBoxNumber->setMaximum(40);
        setRulerMode(_rulerMode);

        if (static_cast<ChannelType>(_channelsModel->data(_modelIndex, ChannelTypeRole).toInt()) == HandChannel) {
            updateBscanVerticalScale();
        }

        updateGates();
        updateTimeDelay();
        updateTvg();
        updateNotchParams();
        updateMark();
        updateSens();
        updateIsPlayAlarm();
        updateChannelMode();
        _markSpinBoxNumber->setValue(0);

        ui->aScanPlot->replot();
        bScanReplot();

        _isModelIndexValid = true;
        updateControlsAreaEnableState();
        _calibrationModeSpinBoxList->setIndex(0);
    }
    else {
        _cid = -1;
        _deviceSide = NoneDeviceSide;

        _isModelIndexValid = false;
        updateControlsAreaEnableState();
    }

    switch (_mode) {
    case calibrationModeHand:
        addMode(calibrationHandMode);
        break;
    case calibrationModeScan:
        addMode(calibrationScanMode);
        break;
    case evaluation:
        addMode(evaluationMode);
        break;
    case hand:
        addMode(handMode);
        break;
    default:

        break;
    }
}

void AScanPage::onHeadChannelSetted(const QModelIndex& index)
{
    if (index.isValid() == false) {
        return;
    }
    _headScanModelIndex = index;

    enableToneForAllChannels();
    reset();

    if (_headScanModelIndex.isValid()) {
        _cid = _headScanChannelsModel->data(_headScanModelIndex, ChannelIdRole).toInt();
        _aScanScale = _headScanChannelsModel->data(_headScanModelIndex, AScanScaleRole).toInt();
        _enterAngle = _headScanChannelsModel->data(_headScanModelIndex, AngleRole).toInt();

        updateGates();
        updateTimeDelay();
        updateTvg();
        updateMark();
        updateSens();
        updateChannelMode();
        setRulerMode(_rulerMode);
        ui->aScanPlot->replot();
        _isModelIndexValid = true;
        updateControlsAreaEnableState();
    }
    else {
        _cid = -1;
        _isModelIndexValid = false;
        updateControlsAreaEnableState();
    }
}

void AScanPage::timeDelayControlChanged(qreal value)
{
    Core::instance().channelsController()->setTimeDelay(value);
}

void AScanPage::sensControlChanged(qreal value)
{
    Core::instance().channelsController()->setSens(value);
    if (_setZeroDbButton->isChecked() == true) {
        int gain = _channelsModel->data(_modelIndex, GainRole).toInt();
        _setZeroDbButton->setText(tr("Kb ") + QString::number(gain) + tr("dB"));
        setGainBase(gain);
    }
}

void AScanPage::gateStartValueChanged(qreal value)
{
    Core::instance().channelsController()->setStartGate(value);
}

void AScanPage::gateEndValueChanged(qreal value)
{
    Core::instance().channelsController()->setEndGate(value);
}

void AScanPage::tvgControlChanged(qreal value)
{
    Core::instance().channelsController()->setTvg(value);
}

void AScanPage::notchStartChanged(qreal value)
{
    Core::instance().channelsController()->setStartNotch(value);
}

void AScanPage::notchSizeChanged(qreal value)
{
    Core::instance().channelsController()->setSizeNotch(value);
}

void AScanPage::notchLevelChanged(qreal value)
{
    Core::instance().channelsController()->setLevelNotch(value);
}

void AScanPage::markValueChanged(qreal value)
{
    if (value == 0) {
        ui->aScanPlot->setLeftMarkerVisible(false);
        ui->aScanPlot->setRightMarkerVisible(false);
    }
    else {
        ui->aScanPlot->setLeftMarkerVisible(true);
        ui->aScanPlot->setRightMarkerVisible(true);
        switch (_rulerMode) {
        case AScanPage::rulerModeMm:
            ui->aScanPlot->setLeftMarkerValue(delayToDepth(value - (MARKER_WIDTH / 2)));
            ui->aScanPlot->setRightMarkerValue(delayToDepth(value + (MARKER_WIDTH / 2)));
            break;
        default:
            ui->aScanPlot->setLeftMarkerValue(value - (MARKER_WIDTH / 2));
            ui->aScanPlot->setRightMarkerValue(value + (MARKER_WIDTH / 2));
            break;
        }
    }
    Core::instance().channelsController()->setMark(value);
}

void AScanPage::rulerModeValueChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    RulerMode mode = static_cast<RulerMode>(userData.toInt());
    Q_ASSERT(mode >= 0 && mode < rulerModesCount);
    if (mode >= 0 && mode < rulerModesCount) {
        setRulerMode(mode);
        ui->aScanPlot->replot();
        bScanReplot();
    }
}

void AScanPage::onScreenShootViewButtonReleased()
{
    emit screenShootButtonReleased(Core::instance().getScreenShootKey(static_cast<eDeviceSide>(_deviceSide), _cid, _gateIndex));
}

void AScanPage::calibrateChannelButtonPressed()
{
    _calibrateChannelButton->setText(tr("in progress"));
    int angle = -1;
    int limitForSens = -1;

    if (_deviceType == Avicon31Default) {
        eInspectionMethod method = static_cast<eInspectionMethod>(_channelsModel->data(_modelIndex, MethodRole).toInt());
        angle = _channelsModel->data(_modelIndex, AngleRole).toInt();
        limitForSens = Core::instance().limitsForSens().sens(angle, method);
    }

    if (_setZeroDbButton->isChecked() == false) {
        if (Core::instance().calibrateChannel() == true) {
            calibrateButtonEffectSetOk();
            if (_deviceType == Avicon31Default) {
                setCalibrationTemperature();
                if (_channelsModel->data(_modelIndex, GainRole).toInt() - _channelsModel->data(_modelIndex, SensRole).toInt() > limitForSens) {
                    Notifier::instance().addNote(Qt::red, tr("The sensor is worn out."));
                }
                makeScreenShoot();
            }
        }
        else {
            calibrateButtonEffectSetFailed();
        }
    }
    else if (_setZeroDbButton->isChecked() == true && (_channelsModel->data(_modelIndex, GainRole).toInt() + _channelsModel->data(_modelIndex, RecommendedSensRole).toInt()) <= MAXIMUM_SENS) {
        Core::instance().calibrationType2();
        calibrateButtonEffectSetOk();
        if (_deviceType == Avicon31Default) {
            setCalibrationTemperature();
            if (_channelsModel->data(_modelIndex, GainRole).toInt() - _channelsModel->data(_modelIndex, SensRole).toInt() > limitForSens) {
                Notifier::instance().addNote(Qt::red, tr("The sensor is worn out."));
            }
            makeScreenShoot();
        }
        _setZeroDbButton->setChecked(false);
        _setZeroDbButton->setText(tr("Set 0 dB"));
    }
    else {
        calibrateButtonEffectSetFailed();
    }
    Core::instance().testSensLevels();
}

void AScanPage::autoTimeDelayButtonPressed()
{
    Core::instance().autoPrismDelayCalibration();
}

void AScanPage::setZeroDbButtonPressed(bool checked)
{
    if (checked == true) {
        Core::instance().resetSens();
        _setZeroDbButton->setText(tr("Kb ") + QString::number(_channelsModel->data(_modelIndex, GainRole).toInt()) + tr("dB"));
        _setZeroDbButton->setChecked(true);
        _sensSpinBoxNumber->setValue(0, false);
    }
    else {
        _setZeroDbButton->setText(tr("Set 0 dB"));
        _setZeroDbButton->setChecked(false);
        updateSens();
    }
}

void AScanPage::toneSwitchOffButtonPressed(bool checked)
{
    _channelsModel->setData(_modelIndex, !checked, IsPlayAlarmToneRole);
    checked ? _toneSwitchOffButton->setStyleSheet("border-image: url(\":/tone_off.png\")") : _toneSwitchOffButton->setStyleSheet("border-image: url(\":/tone_on.png\")");
}

void AScanPage::toneOnlyForChannelButtonPressed(bool checked)
{
    _toneOnlyForChannelButton->setChecked(checked);
    if (checked == true) {
        for (int i = 0; i < _channelsModel->rowCount(); ++i) {
            QModelIndex index = _channelsModel->index(i, 0);
            index != _modelIndex ? _channelsModel->setData(index, false, IsPlayAlarmToneRole) : _channelsModel->setData(index, true, IsPlayAlarmToneRole);
        }
        _toneOnlyForChannelButton->setStyleSheet("border-image: url(\":/only_tone_off.png\")");
    }
    else {
        for (int i = 0; i < _channelsModel->rowCount(); ++i) {
            QModelIndex index = _channelsModel->index(i, 0);
            _channelsModel->setData(index, true, IsPlayAlarmToneRole);
        }
        _toneOnlyForChannelButton->setStyleSheet("border-image: url(\":/only_tone_on.png\")");
    }
}

void AScanPage::saveAScanButtonReleased()
{
    emit shootScreen(_deviceSide,
                     _cid,
                     _gateIndex,
                     _channelsModel->data(_modelIndex, GainRole).toInt(),
                     _channelsModel->data(_modelIndex, SensRole).toInt(),
                     _channelsModel->data(_modelIndex, TvgRole).toFloat(),
                     _channelsModel->data(_modelIndex, TimeDelayRole).toFloat(),
                     _surfaceList->index(),
                     _channelsModel->data(_modelIndex, StartGateRole).toInt(),
                     _channelsModel->data(_modelIndex, EndGateRole).toInt());
}

void AScanPage::startOfRegistrationButtonRelease()
{
    _isStartRegChecked ? _isStartRegChecked = false : _isStartRegChecked = true;
    if (_isStartRegChecked) {
        blockControlsAreaForRegistration(true);
        _startOfRegistrationButton->setStyleSheet(HAND_SCAN_REG_ON);
        _startOfRegistrationButton->setText(QString::number(_registrationTimerSpinBoxNumber->value()) + tr(" sec"));
        _handScanTimer->start(static_cast<int>(_registrationTimerSpinBoxNumber->value() * 1000));
        _checkHandScanTimeTimer->start();
        emit startHandScanRegistration();
    }
    else {
        onHandScanTimerOff();
    }
}

void AScanPage::calibrationModeChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    _calibrationMode = static_cast<eCalibrationMode>(userData.toInt());
    Q_ASSERT(_calibrationMode == cmSens || _calibrationMode == cmPrismDelay);

    switch (_calibrationMode) {
    case cmSens:  // by depth
        ui->hLabel->setVisible(true);
        ui->rLabel->setVisible(false);
        _timeDelaySpinBox->setEnabled(false);
        _autoTimeDelayButton->setEnabled(false);
        Core::instance().changeCalibrationMode(cmSens);
        break;
    case cmPrismDelay: {  // by beam
        if (Q_LIKELY(_enterAngle != 0)) {
            ui->rLabel->setVisible(true);
            ui->hLabel->setVisible(false);
        }
        else {
            ui->rLabel->setVisible(false);
            ui->hLabel->setVisible(true);
        }
        _timeDelaySpinBox->setEnabled(true);
        _autoTimeDelayButton->setEnabled(true);
        Core::instance().changeCalibrationMode(cmPrismDelay);
        break;
    }
    default:
        break;
    }
    updateGates();
}

void AScanPage::calibrateButtonEffectTimeout()
{
    _calibrateChannelButton->setText(tr("Calibrate"));
    _calibrateChannelButton->setStyleSheet(Styles::areaButton());
}

void AScanPage::bScanReplotTimeout()
{
    if (ui->bScanTape->getIsInvalid()) {
        fillBScanTape();
    }
    ui->bScanTape->replotBScanPlot();
}

void AScanPage::onHandScanTimerOff()
{
    blockControlsAreaForRegistration(false);
    _startOfRegistrationButton->setText(tr("Start reg."));
    _startOfRegistrationButton->setStyleSheet(HAND_SCAN_REG_OFF);
    emit stopHandScanRegistration(_deviceSide, _surfaceList->index(), _cid, _channelsModel->data(_modelIndex, GainRole).toInt(), _channelsModel->data(_modelIndex, SensRole).toInt(), _channelsModel->data(_modelIndex, TvgRole).toFloat(), _channelsModel->data(_modelIndex, TimeDelayRole).toFloat());
    _handScanTimer->stop();
    _checkHandScanTimeTimer->stop();
    _isStartRegChecked = false;
    emit blockHandScanButtons(_isStartRegChecked);
}

void AScanPage::onCheckHandScanTimerOff()
{
    int time = _handScanTimer->remainingTime() / 1000;
    if (time == static_cast<int>(_registrationTimerSpinBoxNumber->value())) {
        return;
    }
    _startOfRegistrationButton->setText(QString::number(time + 1) + tr(" sec"));
}

void AScanPage::onDataContainerData(pDaCo_DateFileItem pDaCoItem)
{
    /* size_t size = pDaCoItem->Signals.size();
     ui->bScanTape->nextStep(pDaCoItem->Dir);
     for (unsigned int sig = 0; sig < size; ++sig) {
         CID chan = pDaCoItem->Signals[sig].Channel;
         eDeviceSide side = pDaCoItem->Signals[sig].Side;

         if (_cid == chan && static_cast<int>(side) == _deviceSide) {
             const auto& gate = getGates(side);
             if (isChannelZero(chan)) {
                 ui->bScanTape->addSignalsN0EMS(pDaCoItem->Signals[sig].Signals, gate.first, gate.second, pDaCoItem->Signals[sig].Count, _multiply, getPlotIndex(chan), getInversion(chan));
             }
             else {
                 ui->bScanTape->addSignalsNORMAL(pDaCoItem->Signals[sig].Signals, pDaCoItem->Signals[sig].Count, _multiply, getPlotIndex(chan), getInversion(chan));
             }
         }
     }*/
}

void AScanPage::fillBScanTape()
{
    ui->bScanTape->clearTape();
    _mode == hand ? ui->bScanTape->setEncoderCorrection(25) : ui->bScanTape->setEncoderCorrection(encoderCorrection());
    if (_mode != hand) {
        int disCoordRange = qFloor((restoreBscanScale() * 1000.0f) / encoderCorrection());
        Core& core = Core::instance();
        int maxCoord = core.registration().GetMaxDisCoord();
        int minCoord = qMax(maxCoord - disCoordRange, 0);
        if (maxCoord != minCoord) {
            BScanDisplaySpan span;
            if (core.registration().getSpan(minCoord, maxCoord, span)) {
                drawSpan(span);
            }
        }
    }
    ui->bScanTape->setIsInvalid(false);
}

void AScanPage::onThresholdBscanChanged(qreal value)
{
    ui->bScanTape->setThreshold(value);
}

void AScanPage::onChangeBscanScale(const qreal& value)
{
    ui->bScanTape->setBScanHorizontalScale(value);
    ui->bScanTape->clearTape();
    emit bscanScaleChanged(value);
}

void AScanPage::onTimerElTestTimeout()
{
    if (!_isUmuConnected) {
        _timerElTest->stop();
        return;
    }

    Core::instance().channelsController()->setTvg(5);
    delay(100);
    Core::instance().channelsController()->setSens(_channelsModel->data(_modelIndex, RecommendedSensRole).toInt());
    delay(100);
    changeSensForElectricTest();
    _timerElTest->stop();
    emit startTimer();
}

void AScanPage::setH(float h)
{
    ui->hLabel->setText(QString(tr("H = ")) + QString::number(h));
}

void AScanPage::visibleH(bool enable)
{
    ui->hLabel->setVisible(enable);
}

void AScanPage::setR(float r)
{
    ui->rLabel->setText(QString(tr("R = ")) + QString::number(r));
}

void AScanPage::visibleR(bool enable)
{
    ui->rLabel->setVisible(enable);
}

void AScanPage::setL(float l)
{
    ui->lLabel->setText(QString(tr("L = ")) + QString::number(l));
}

void AScanPage::setN(float n)
{
    if (n >= 18) {
        ui->nLabel->setText(QString(tr("N ")) + QString(0x2265) + " 18");
    }
    else if (n < -12) {
        ui->nLabel->setText(QString(tr("N ")) + QString(0x2264) + " -12");
    }
    else {
        ui->nLabel->setText(QString(tr("N = ")) + QString::number(n));
    }
}

void AScanPage::setKd(float kd)
{
    ui->kdLabel->setText(QString(tr("Kd = ")) + QString::number(kd));
}

void AScanPage::setGain(float gain)
{
    ui->gainLabel->setText(QString(tr("Gain = ")) + QString::number(gain) + QString(tr(" dB")));
}

void AScanPage::setGain(float gain, float gainForControlSection)
{
#if defined(TARGET_AVICON31) || defined(TARGET_AVICON15) || defined(TARGET_AVICON31E)
    ui->gainLabel->setText(QString(tr("Gain = ")) + QString::number(gain) + "(" + QString::number(gainForControlSection) + ")" + QString(tr(" dB")));
#else
    Q_UNUSED(gainForControlSection);
    ui->gainLabel->setText(QString(tr("Gain = ")) + QString::number(gain) + QString(tr(" dB")));
#endif
}

void AScanPage::setGainBase(float gain)
{
    ui->gainBaseLabel->setText(QString(tr("Kb = ")) + QString::number(gain));
}

void AScanPage::setModeTitle(const QString& mode)
{
    ui->modeLabel->setText(mode);
}

void AScanPage::setDrawMode(int mode)
{
    ui->bScanTape->setDrawMode(mode);
}

void AScanPage::setElectricTest(bool isElTest)
{
    _isElTest = isElTest;
    if (_isElTest) {
        _timerElTest = new QTimer(this);
        _timerElTest->setInterval(1000);
        _timerElTest->setSingleShot(false);
        ASSERT(connect(_timerElTest, &QTimer::timeout, this, &AScanPage::onTimerElTestTimeout));
    }
}

void AScanPage::timerElectricTestStart()
{
    _isElTestActive = true;
    if (!_timerElTest->isActive()) {
        _timerElTest->start();
    }
    blockControlsArea(true);
}

void AScanPage::timerElectricTestStop()
{
    _isElTestActive = false;
    blockControlsArea(false);
}

void AScanPage::setUmuConnectionStatus(bool isConnected)
{
    _isUmuConnected = isConnected;
}

void AScanPage::setBScanPlot(const BScanPlot& plot, int index)
{
    _bScanPlotIndex = index;
    QVector<int> indexes;
    if (_cid == F42E || _cid == B42E) {
        indexes << (index % 2);
    }
    indexes << index;

    ui->bScanTape->cloneBScanPlot(plot, indexes);
    ui->bScanTape->clearTape();
    fillBScanTape();
}

void AScanPage::blockControlsArea(bool isBlock)
{
    ui->controlsArea->setDisabled(isBlock);
}

void AScanPage::configureUiForAviconDbAviconDbHs()
{
    _screenShootViewButton->hide();
}

void AScanPage::configureUiForAvicon15()
{
    configureUiForAviconDbAviconDbHs();
}

void AScanPage::visibleControlsForRegistration(bool enable)
{
    _saveAScanButton->setEnabled(enable);
    _startOfRegistrationButton->setEnabled(enable);
    _surfaceList->setEnabled(enable);
    _railList->setEnabled(enable);
    _registrationTimerSpinBoxNumber->setEnabled(enable);
}

void AScanPage::setCurrentChannelType(eChannelType channelType)
{
    _currentChannelType = channelType;
    if (_currentChannelType == ctHandScan) {
        _bScanPlotIndex = 0;
    }
}

void AScanPage::checkHandScanRegTimer()
{
    if (_handScanTimer->isActive()) {
        onHandScanTimerOff();
    }
}

void AScanPage::updateChannelNote()
{
    updateChannelMode();
}

void AScanPage::addMode(ModeSetted mode)
{
    Core::instance().registration().addMode(mode, true, static_cast<eDeviceSide>(_deviceSide), _cid, _gateIndex);
}

void AScanPage::setProbePulseLocation(bool isProbePulseFromAbove)
{
    ui->bScanTape->setProbePulseLocation(isProbePulseFromAbove);
}

void AScanPage::setFillingAScanState(bool isFillingAScan)
{
    ui->aScanPlot->setFillingAScanState(isFillingAScan);
}

void AScanPage::updateNoiseReductionState()
{
    _noiseReduction = restoreNoiseReduction();
}

void AScanPage::restoreBScanLevel()
{
    switch (restoreRegistrationThreshold()) {
    case -6:
        _bScanLevel = 16;
        break;
    case -12:
        _bScanLevel = 8;
        break;
    default:
        break;
    }
}
