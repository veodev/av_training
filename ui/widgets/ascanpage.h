#ifndef ASCANPAGE_H
#define ASCANPAGE_H

#include <QSharedPointer>
#include <QModelIndex>
#include <QWidget>
#include "modeltypes.h"
#include "Device_definitions.h"
#include "dc_definitions.h"
#include "coredefinitions.h"

namespace Ui
{
class AScanPage;
}

class SpinBoxList;
class SpinBoxNumber;
class QPushButton;
class QAbstractItemModel;
class BScanPlot;
class SpinBoxNumberDelegateUs;
class SpinBoxNumberDelegateGates;

class AScanPage : public QWidget
{
    Q_OBJECT

    friend class SpinBoxNumberDelegateUs;
    friend class SpinBoxNumberDelegateGates;

public:
    explicit AScanPage(QWidget* parent = 0);
    ~AScanPage();

    void setBScanDotSize(int value);
    void setControlsAreaVisible(bool isVisible = true);
    bool controlsAreaVisible();
    void setModel(QAbstractItemModel* channelsModel);
    void setHeadScanModel(QAbstractItemModel* headScanChannelsModel);

    enum Modes
    {
        unknownMode,
        evaluation,
        hand,
        calibrationModeScan,
        calibrationModeHand,
        headScanMode,
    };

    void setMode(Modes mode);

    void setH(float h);
    void visibleH(bool enable = true);
    void setR(float r);
    void visibleR(bool enable = true);
    void setL(float l);
    void setN(float n);
    void setKd(float kd);
    void setGain(float gain);
    void setGain(float gain, float gainForControlSection);
    void setGainBase(float gain);
    void setModeTitle(const QString& mode);
    void setBScanPlot(const BScanPlot& plot, int index);
    void blockControlsArea(bool isBlock);
    void configureUiForAviconDbAviconDbHs();
    void configureUiForAvicon15();
    void visibleControlsForRegistration(bool enable = false);
    void setCurrentChannelType(eChannelType channelType);
    void checkHandScanRegTimer();
    void updateChannelNote();
    void addMode(ModeSetted mode);
    void setProbePulseLocation(bool isProbePulseFromAbove);
    void setFillingAScanState(bool isFillingAScan);
    void updateNoiseReductionState();
    void restoreBScanLevel();

    // Electric test methods
    void setElectricTest(bool isElTest);
    void timerElectricTestStart();
    void timerElectricTestStop();
    void setUmuConnectionStatus(bool isConnected);
    //

    void setDrawMode(int mode);
signals:
    void shootScreen(unsigned int key);
    void shootScreen(DeviceSide side, CID channelId, int gateIndex, int gain, int sens, float tvg, float timeDelay, int surface, int startGate, int endGate);
    void screenShootButtonReleased(int key, ChannelType channelType);
    void screenShootButtonReleased(unsigned int key);
    void startHandScanRegistration();
    void stopHandScanRegistration(DeviceSide side, int surface, CID channelId, int gain, int sens, float tvg, float prismDelay);
    void blockHandScanButtons(bool isBlock);
    void bscanScaleChanged(double value);
    void doSetControlledRail(int index, const QString& value, const QVariant& userData);
    void startTimer();
    void configurationRequired(bool isRequired);

public slots:
    void setVisible(bool visible);
    void onSetControledRail(int index, const QString& value, const QVariant& userData);
    void onHotKeyReleased(Actions action);

protected:
    bool event(QEvent* e);

private:
    void retranslateUi();

    void updateBscanVerticalScale();
    void updateGates();
    void updateChannelMode();
    void updateTimeDelay();
    void updateSens();
    void updateTvg();
    void updateNotchParams();
    void enableNotchControls(bool isEnable);
    void updateMark();
    void updateIsPlayAlarm();

    void updateControlsAreaEnableState();

    enum RulerMode
    {
        rulerModeUs = 0,
        rulerModeMm = 1,
        rulerModeNone = 2,
        rulerModesCount
    };
    void setRulerMode(AScanPage::RulerMode mode = rulerModeNone);

    void noiseReductionFilter(unsigned char* array);
    void reset();

    void calibrateButtonEffectSetOk();
    void calibrateButtonEffectSetFailed();

    void bScanReplot();

    inline float pixelToDelayFactor() const;
    inline float delayToDepthFactor() const;
    inline float pixelToDepthFactor() const;

    inline float delayToPixel(float delay) const;
    inline float depthToDelay(float depth) const;
    inline float delayToDepth(float delay) const;
    inline float pixelToDelay(float delay) const;

    int aScanLen() const;

    void enableToneForAllChannels();

    void blockControlsAreaForRegistration(bool isBlock);
    void deleteScreenShoot(unsigned int key);
    void makeScreenShoot();
    void setCalibrationTemperature();
    void refreshMarkerBoarders();

    void changeSensForElectricTest();
    void changeTvgForElectricTest();

    void checkCalibrationTemperature();

    void drawSpan(const BScanDisplaySpan& span);

private slots:
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void headScanDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onConnectionStatusDisconnected();
    void onConnectionStatusConnecting();
    void onConnectionStatusConnected();
    void onAScanMeas(QSharedPointer<tDEV_AScanMeasure> measure);
    void onAScanData(QSharedPointer<tDEV_AScanHead> head, QSharedPointer<tUMU_AScanData> data);
    void onTVGData(QSharedPointer<tDEV_AScanHead> head, QSharedPointer<tUMU_AScanData> data);
    void onBScan2Data(QSharedPointer<tDEV_BScan2Head> head);
    void bScanDisplayThresholdChanged(int value);
    void onChannelSetted(const QModelIndex& index);
    void onHeadChannelSetted(const QModelIndex& index);
    void timeDelayControlChanged(qreal value);
    void sensControlChanged(qreal value);
    void gateStartValueChanged(qreal value);
    void gateEndValueChanged(qreal value);
    void tvgControlChanged(qreal value);
    void notchStartChanged(qreal value);
    void notchSizeChanged(qreal value);
    void notchLevelChanged(qreal value);
    void markValueChanged(qreal value);
    void rulerModeValueChanged(int index, const QString& value, const QVariant& userData);
    void onScreenShootViewButtonReleased();
    void calibrateChannelButtonPressed();
    void autoTimeDelayButtonPressed();
    void setZeroDbButtonPressed(bool checked);
    void toneSwitchOffButtonPressed(bool checked);
    void toneOnlyForChannelButtonPressed(bool checked);
    void saveAScanButtonReleased();
    void startOfRegistrationButtonRelease();
    void calibrationModeChanged(int index, const QString& value, const QVariant& userData);
    void calibrateButtonEffectTimeout();
    void bScanReplotTimeout();
    void onHandScanTimerOff();
    void onCheckHandScanTimerOff();
    void onDataContainerData(pDaCo_DateFileItem item);
    void fillBScanTape();
    void onThresholdBscanChanged(qreal value);
    void onChangeBscanScale(const qreal& value);
    void onTimerElTestTimeout();

private:
    Ui::AScanPage* ui;

    bool _isConnected;
    bool _isModelIndexValid;

    SpinBoxNumber* _displayThresholdBscan;
    SpinBoxNumber* _scaleBscan;
    SpinBoxNumberDelegateUs* _spinBoxNumberDelegateUs;
    SpinBoxNumberDelegateGates* _spinBoxNumberDelegateStartGate;
    SpinBoxNumberDelegateGates* _spinBoxNumberDelegateEndGate;
    SpinBoxNumber* _timeDelaySpinBox;
    SpinBoxNumber* _sensSpinBoxNumber;
    SpinBoxNumber* _gateStartSpinBoxNumber;
    SpinBoxNumber* _gateEndSpinBoxNumber;
    SpinBoxNumber* _markSpinBoxNumber;
    SpinBoxNumber* _tvgSpinBoxNumber;
    SpinBoxNumber* _notchStartSpinBoxNumber;
    SpinBoxNumber* _notchSizeSpinBoxNumber;
    SpinBoxNumber* _notchLevelSpinBoxNumber;
    SpinBoxList* _rulerModeList;
    QPushButton* _setZeroDbButton;
    QPushButton* _calibrateChannelButton;
    QPushButton* _autoTimeDelayButton;
    QPushButton* _toneSwitchOffButton;
    QPushButton* _screenShootViewButton;
    QPushButton* _toneOnlyForChannelButton;
    SpinBoxList* _surfaceList;
    SpinBoxList* _railList;
    SpinBoxNumber* _registrationTimerSpinBoxNumber;
    QPushButton* _saveAScanButton;
    QPushButton* _startOfRegistrationButton;
    SpinBoxList* _calibrationModeSpinBoxList;
    RulerMode _rulerMode;

    bool _noiseReduction;
    bool _isStartRegChecked;
    bool _isBScanTimeContinue;

    // Electric test members
    bool _isElTest;
    bool _isElTestActive;
    bool _isUmuConnected;
    QTimer* _timerElTest;
    //

    CID _cid;
    DeviceSide _deviceSide;
    ChannelType _channelType;
    int _gateIndex;
    int _bScanPlotIndex;
    int _multiply;
    int _aScanScale;
    int _enterAngle;
    int _bScanLevel;

    QAbstractItemModel* _channelsModel;
    QAbstractItemModel* _headScanChannelsModel;

    QModelIndex _modelIndex;
    QModelIndex _headScanModelIndex;

    Modes _mode;
    eCalibrationMode _calibrationMode;

    QTimer* _bScanReplotTimer;
    QTimer* _handScanTimer;
    QTimer* _checkHandScanTimeTimer;
    QMap<int, int> _mapGainOfControlSection;
    eChannelType _currentChannelType;
    DeviceType _deviceType;

    std::pair<unsigned short, unsigned short> getGates(eDeviceSide side);
    bool getInversion(CID chan);
    unsigned char getPlotIndex(CID chan);
    bool isChannelZero(CID chan);
};

#endif  // ASCANPAGE_H
