#ifndef DEFCORE_H
#define DEFCORE_H

#include <QThread>
#include <QSharedPointer>
#include <QStringList>
#include <QMutex>

#include <queue>
#include <utility>

#include "datatransfers/datatransfer_lan.h"
#include "datatransfers/datatransfer_can.h"

#include "modeltypes.h"
#include "settings.h"
#include "Definitions.h"


class Core;

class QTimer;

class cThreadClassList;
class cChannelsTable;
class cDeviceConfig_test;
class cDeviceCalibration;
class cEventManager;
class cEventManager;
class cCriticalSection;
class cDataTransfer;
class cDevice;
class cDeviceConfig;
class tDEV_AScanMeasure;
class tDEV_AScanHead;
class tDEV_AlarmHead;
class tDEV_BScan2Head;
class tDEV_PathStepData;

class Defcore : public QThread
{
    Q_OBJECT

public:
    explicit Defcore(QObject* parent = 0);
    ~Defcore();

    int schemeIndex() const;
    int schemeCount() const;
    double maxSpeed() const;

    QString getCurrentCalibrationName();

    QStringList calibrationNames();  // thread-safe
    void deleteCalibration(int index = 0);
    void setScreenShootKey(eDeviceSide side, CID channelId, int gateIndex, unsigned int key);
    unsigned int getScreenShootKey(eDeviceSide side, CID channelId, int gateIndex);
    void deleteScreenShootsOfCalibration(int index);
    void setCalibrationTemperature(eDeviceSide side, CID channelId, int value);
    int getCalibrationTemperature(eDeviceSide side, CID channelId);
    int calibrationIndex(void);
    int realCalibrationIndex(int index);
    void setCalibrationName(const QString& name, int index = 0);
    bool calibrateChannel(int& sens);
    int GetCurrentBScanSessionID();

    static int aScanThreshold();
    static int bScanThreshold();

    eDeviceSide currentSide();
    CID currentChannelId();
    int currentGateIndex();

    QString deviceSerialNumber() const;
    QString deviceFirmwareVersion() const;
    QString devicePldVersion() const;

    QStringList deviceSerialNumbers() const;
    QStringList deviceFirmwareVersions() const;
    QStringList devicePldVersions() const;

    static void callBack(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);
    void emitDataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);

    cChannelsTable* getChannelsTablePtr();

    eDeviceMode getDeviceMode();

    void SetFixBadSensState(bool State);
    void SetTimeToFixBadSens(int TimeToFixBadSens);

    bool getDefcoreFinit() const;
    eCoordSys getSystemCoordType() const;
    tSensValidRangesList getSensValidRanges() const;

public slots:
    void init();
    void connectToDevice();

    void onSetSchemeIndex(int index);
    void onSetCalibrationIndex(int index = 0);

    void onSetChannel(CID id);
    void onSetSide(DeviceSide side);
    void onSetTvg(int value);
    void onSetPrismDelay(int value);
    void onSetSens(int value);
    void onSetStartGate(int value);
    void onSetEndGate(int value);
    void onSetGateIndex(int gateIndex);
    void onSetMark(int value);
    void onSetCalibrationMode(eCalibrationMode mode);
    void onChangeCalibrationMode(eCalibrationMode mode);
    void onSetSearchMode();
    void onDeviceUpdate(bool resetStrokes);
    void onSetStartNotch(int value);
    void onSetSizeNotch(int value);
    void onSetLevelNotch(int value);

    void onCalibrationToRailType();
    void onResetSens();
    void onCalibrationType1(int& sens);
    void onCalibrationType2();
    void onAutoCalibrationTimeDelay();
    void onSetControlMode(eDeviceControlMode mode, int additive);
    void onChangeLanguage();

    void onPauseModeOn();
    void onPauseModeOff();

    void onSetUmuLineToCompleteControl();
    void onSetUmuLineToScannerControl();

    void onTestSensLevels();

    void onSetRegistrationThreshold(int value);

    void onSetAv17CoordX(int Coord);
    void onSetAv17CoordY(int Coord);
    void onEnableSMChSensAutoCalibration(bool isEnable);

    void onDeviceEnableFiltration(bool isEnable);
    void onSetDeviceFilterParams(tBScan2FilterParamId prmId, int value);

#if defined TARGET_AVICON15
    void onUMUBatteryVoltageQuery(void);
    void onUMUBatteryChargePercentQuery(void);
#endif

    void onSetleftSideTrolleyPosition(tTrolleySide troleySide);
    void enableAll();
    void disableAll();
    void resetPathEncoder();
    void restoreACSettings();
    void onEnableAC(bool isEnabled);
    void stop();
    void createNewCalibration(const QString& name);
    void onUsePathEncoderEmulator(bool value);
    void onAutoGainAdjustmentEnabled(bool state);
    void onSetDynamicRailType(bool isEnabled);

signals:
    void doInited();
    void doSchemeChanged(int index);
    void doCalibrationChanged();

    void doRailTypeResult(tCalibrationToRailTypeResult* result);

    void doAScanMeas(QSharedPointer<tDEV_AScanMeasure>);
    void doAScanData(QSharedPointer<tDEV_AScanHead>, QSharedPointer<tUMU_AScanData>);
    void doTVGData(QSharedPointer<tDEV_AScanHead>, QSharedPointer<tUMU_AScanData>);
    void doAlarmData(QSharedPointer<tDEV_AlarmHead>);
    void doBScan2Data(QSharedPointer<tDEV_BScan2Head>);
    void doMScan2Data(QSharedPointer<tDEV_BScan2Head>);
    void doPathStepData(QSharedPointer<tDEV_PathStepData>);
    void doDeviceSpeed(unsigned int);

    void doConnectionStatusDisconnected();
    void doConnectionStatusConnecting();
    void doConnectionStatusConnected();

    void dataChanged(int groupIndex, eDeviceSide side, CID channel, int gateIndex, eValueID id, int value);

    void doDeviceSignalsPacket(QSharedPointer<tDEV_Packet>);
    void doDeviceSignalSpacing(QSharedPointer<tDEV_SignalSpacing>);

    void doDefect53_1(QSharedPointer<tDEV_Defect53_1>);

    void doBottomSignalAmpl(QSharedPointer<int>);

    void doUmuConnectionStatusChanged(bool isConnected);

#ifdef TARGET_AVICON15
    void doBatteryVoltage(double voltage);
    void doBatteryChargePercent(double percent);
#endif

protected:
    void run();
    void finit();

protected slots:
    void umuTimerTimeout();
    void umuConnectionTimerTimeout();

private:
    friend class Core;
    cChannelsTable* channelsTable() const;
    cDeviceConfig* deviceConfig() const;
    cDeviceConfig* deviceConfigHeadScan() const;
    cDeviceCalibration* deviceCalibration() const;

    void remapCalibrations();
    int getCalibrationCountBySchemeIndex(int schemeIndex);
    void createCalibrationIfDoesnotExist(int schemeIndex);
    bool createCalibration(int schemeIndex, const QString& name = QString("auto"));

    bool getFirstScanChannelDescriptionInScheme(sScanChannelDescription& scanChanDesc, int schemeNumber);
    bool getFirstHandChannelDescription(sHandChannelDescription& handChanDesc);

private:
    bool _stopThread;
    bool _interrupInit;
    bool _isDeviceStarted;
    bool _defcoreFinit;
    bool _defcoreTimerFinished;

    cThreadClassList* _threadList;
    cChannelsTable* _channelsTable;
    cDeviceConfig* _deviceConfig;
    cDeviceConfig* _deviceConfigHeadScan;
    cDeviceCalibration* _deviceCalibration;
    cEventManager* _deviceEventManager;
    cEventManager* _umuEventManager;
    cCriticalSection* _criticalSection;
    cCriticalSection* _criticalSectionDevice;
    cCriticalSection* _calibrationCriticalSection;
    cDevice* _device;

    QList<int> _filteredCalibrations;
    QStringList _calibrationNames;
    QMutex _calibrationMutex;

    QTimer* _auxilaryTimer;
    QTimer* _checkUmuConnectionTimer;

    int _schemeNumber;
    eDeviceMode _lastMode;

    static Defcore* _defcore;
};

#endif  // DEFCORE_H
