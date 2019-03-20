#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMap>
class QString;
class QStringList;

QString manufactureName();
QString applicationName();
QString organisationName();

void setApplicationSettingsProperties();

void saveLanguage(const QString& language);
QString restoreLanguage();

void saveVolume(double volume);
double getVolume();

bool getAScanPlotOptimization();

void saveOperatorsToSettings(const QStringList& list);
QStringList restoreOperatorsToSettings();

void saveLinesToSettings(const QStringList& list);
QStringList restoreLinesToSettings();

void saveSchemeNumber(int index);
int restoreSchemeNumber();

void saveEncoderCorrection(float encoderCorrection);
float encoderCorrection();

void saveHorizontalScannerEncoderStep(double encoderCorrection);
double restoreHorizontalScannerEncoderStep();

void saveVerticalScannerEncoderStep(double encoderCorrection);
double restoreVerticalScannerEncoderStep();

void saveBscanScale(double scale);
double restoreBscanScale();

void saveOpacityImportantArea(int opacity);
int restoreOpacityImportantArea();

void saveColorImportantAreaIndex(int colorIndex);
int restoreColorImportantAreaIndex();

void saveColorImportantAreaColor(int r, int g, int b);
int restoreColorImportantAreaColorR();
int restoreColorImportantAreaColorG();
int restoreColorImportantAreaColorB();

void saveBrightness(double value);
double restoreBrightness(void);

void setCduSerialNumber(const QString& serialNumber);
QString cduSerialNumber();

void setDefectoscopeSerialNumber(const QString& serialNumber);
QString defectoscopeSerialNumber();

void saveOrganizationName(const QString& organizationName);
QString restoreOrganizationName();

void saveDepartmentName(const QString& departmentName);
QString restoreDepartmentName();

void saveBScanDotSize(int value);
int restoreBScanDotSize();

void saveProbePulseLocation(bool isProbePulseFromAbove);
bool restoreProbePulseLocation();

void saveAScanFillingState(bool isFillingAScan);
bool restoreAScanFillingState();

void saveTrolleyLeftSide(bool isTrolleyLeftSideWork);
bool restoreTrolleyLeftSide();

void saveRuLocation(bool isBehind);
bool restoreRuLocation();

void saveStateCheckCalibrationAlgorithm(bool isEnabled);
bool restoreStateCheckCalibrationAlgorithm();

void saveFiltrationEnable(bool isEnabled);
bool restoreFiltrationEnable();

void saveFilterMinPacketSize(double value);
int restoreFilterMinPacketSize();

void saveFilterMaxCoordSpacing(double value);
int restoreFilterMaxCoordSpacing();

void saveFilterMaxDelayDelta(double value);
int restoreFilterMaxDelayDelta();

void saveFilterMaxSameDelayConsecutive(double value);
int restoreFilterMaxSameDelayConsecutive();

void saveMScanDotSize(int value);
int restoreMScanDotSize();

void saveBridgesToSettings(const QStringList& list);
QStringList restoreBridgesToSettings();

void savePlatformsToSettings(const QStringList& list);
QStringList restorePlatformsToSettings();

void saveMiscToSettings(const QStringList& list);
QStringList restoreMiscToSettings();

void saveLastCalibrationForScheme(int schemeNumber, int calibrationIndex);
int restoreLastCalibrationForScheme(int schemeNumber);

void saveNoiseReduction(bool value);
bool restoreNoiseReduction();

void saveIncreaseTracks(bool isIncreaseTracks);
bool restoreIncreaseTracks();

void saveStateAcousticContactControl(bool isEnabled);
bool restoreStateAcousticContactControl();

void saveHeadScannerPageState(bool isEnabled);
bool restoreHeadScannerPageState();

void saveEkasuiPageState(bool isEnabled);
bool restoreEkasuiPageState();

void saveHeadScannerUseState(bool isUsed);
bool restoreHeadScannerUseState();

void saveHorizontalZone(int value);
int restoreHorizontalZone();

void saveVerticalStartPosition(int value);
int restoreVerticalStartPosition();

void saveAdditiveForSens(int additive);
int restoreAdditiveForSens();

void saveStartDelay(int delay);
int restoreStartDelay();

void saveAcousticThreshold(int threshold);
int restoreAcousticThreshold();

void saveLastServiceDate(const QString& lastServiceDate);
QString restoreLastServiceDate();

void saveCalibrationDate(const QString& calibrationDate);
QString restoreCalibrationDate();

void saveCalibrationValidityDate(const QString& calibrationValidityDate);
QString restoreCalibrationValidityDate();

void saveCertificateNumber(const QString& certificateNumber);
QString restoreCertificateNumber();

void saveGainForControlSection(const QMap<int, int>& map);
QMap<int, int> restoreGainForControlSection();

void saveKeyCombinations(const QMap<int, int>& map);
QMap<int, int> restoreKeyCombinations();

void saveLastKm(int km);
int restoreLastKm();

void saveLastPk(int pk);
int restoreLastPk();

void saveLastM(int m);
int restoreLastM();

void saveMinimalBatteryPercent(int minimalPercent);
int restoreMinimalBatteryPercent();

void saveTimeOffset(int timeOffset);
int restoreTimeOffset();

QString umuIpAddr();
int umuPortOut();
int umuPortIn();

int limitOfCountLogFiles();
bool invertEncoder();


void saveRegistrationThreshold(int value);
int restoreRegistrationThreshold(void);

void saveScreenHeaterStatus(bool isEnabled);
bool restoreScreenHeaterStatus();

void saveScreenHeaterTemp(int temp);
int restoreScreenHeaterTemp();

void saveFtpServer(const QString& server);
QString restoreFtpServer();

void saveFtpPath(const QString& path);
QString restoreFtpPath();
QString restoreFtpRootPath();

void saveFtpLogin(const QString& login);
QString restoreFtpLogin();

void saveFtpPassword(const QString& password);
QString restoreFtpPassword();

void saveImportantAreaIndicateEnable(bool isEnabled);
bool restoreImportantAreaIndicateEnable();

void saveImportantAreaMinPacketSize(int value);
int restoreImportantAreaMinPacketSize(void);

void saveImportantAreaMinSignals0dB(int value);
int restoreImportantAreaMinSignals0dB();

void saveEvalCalibrMinZTMSpacingSize(int value);
int restoreEvalCalibrMinZTMSpacingSize(void);

void saveMinKdEchoPrm(int value);
int restoreMinKdEchoPrm(void);

void saveImportantAreaMinZTMSpacing(int value);
int restoreImportantAreaMinZTMSpacing(void);

void saveHeadScannerParams(int XPathStep, int YPathStep, int StartY, int ScanWidth, int ScanHeight, int CurrentY, int ScannerOrientation, int WorkRail, int Threshold);
void restoreHeadScannerParams(int& XPathStep, int& YPathStep, int& StartY, int& ScanWidth, int& ScanHeight, int& CurrentY, int& ScannerOrientation, int& WorkRail, int& Threshold);

void saveHeadScannerWorkRail(int workrail);
int restoreHeadScannerWorkRail();

void saveHeadScannerOrientation(int ScannerOrientation);
int restoreHeadScannerOrientation();

void saveExternalTemperatureEnabled(bool value);
bool restoreExternalTemperatureEnabled();

void saveExternalTempSensorType(int type);
int restoreExternalTempSensorType();

void saveInternalTempSensorType(int type);
int restoreInternalTempSensorType();

void saveInternalLM75Bus(int value);
int restoreInternalLM75Bus();

void saveExternalLM75Bus(int value);
int restoreExternalLM75Bus();

void saveInternalLM75Addr(const QString& value);
QString restoreInternalLM75Addr();

void saveExternalLM75Addr(const QString& value);
QString restoreExternalLM75Addr();

void saveVirtualKeybLanguageIdx(int value);
int restoreVirtualKeybLanguageIdx();

void saveEKASUIRailroad(const QString& railroad);
QString restoreEKASUIRailroad();

void saveEKASUIPRED(const QString& pred);
QString restoreEKASUIPRED();

void saveEKASUIMode(int value);
int restoreEKASUIMode();

void saveACLevel(int value);
int restoreACLevel();

void saveACFreq(int value);
int restoreACFreq();

void saveACGain(int value);
int restoreACGain();

void saveACVoiceNotificationEnabled(bool value);

bool restoreACVoiceNotificationEnabled();

void saveAutoSensResetEnabled(bool value);
bool restoreAutoSensResetEnabled();

void saveAutoSensResetDelay(int value);
int restoreAutoSensResetDelay();

void saveEKASUIControlObject(int value);
int restoreEKASUIControlObject();

void saveDeviceType(int devtype);
int restoreDeviceType();

void saveRegistrationType(int devtype);
int restoreRegistrationType();

void saveElectricTestStatus(bool isActivate);
bool restoreElectricTestStatus();

void saveCountCrashes(int count);
int restoreCountCrashes();

void saveCountCduReboot(int count);
int restoreCountCduReboot();

void saveCountUmuDisconnect(int count);
int restoreCountUmuDisconnect();

void saveEKASUIRegistrationLastDirection(const QString& value);
QString restoreEKASUIRegistrationLastDirection();

void saveEKASUIRegistrationLastStation(const QString& value);
QString restoreEKASUIRegistrationLastStation();

void saveEKASUIRegistrationLastTrackNumber(const QString& value);
QString restoreEKASUIRegistrationLastTrackNumber();

void saveDefaultRegistrationLastOperator1(const QString& value);
QString restoreDefaultRegistrationLastOperator1();

void saveDefaultRegistrationLastOperator2(const QString& value);
QString restoreDefaultRegistrationLastOperator2();

void saveDefaultRegistrationLastOperator3(const QString& value);
QString restoreDefaultRegistrationLastOperator3();

void saveDefaultRegistrationLastLine(const QString& value);
QString restoreDefaultRegistrationLastLine();

void saveDefaultRegistrationLastTrackNumber(const QString& value);
QString restoreDefaultRegistrationLastTrackNumber();

void saveDefaultRegistrationLastDirectionCode(const QString& value);
QString restoreDefaultRegistrationLastDirectionCode();

void saveEKASUICarId(const QString& value);
QString restoreEKASUICarId();

void saveFactoryPassword(const QString& value);
QString restoreFactoryPassword();

void saveLabPassword(const QString& value);
QString restoreLabPassword();

void saveManagerPassword(const QString& value);
QString restoreManagerPassword();

void saveEKASUIPassword(const QString& value);
QString restoreEKASUIPassword();

void saveNotificationDistance(int distance);
int restoreNotificationDistance();

void saveTypeViewCoordinateForBScan(int viewType);
int restoreTypeViewCoordinateForBScan();

void saveRegarStatus(bool isEnabled);
bool restoreRegarStatus();

void saveAutoGainAdjustmentStatus(bool isEnabled);
bool restoreAutoGainAdjustmentStatus();

void saveParallelVideoBrowsingStatus(bool isEnabled);
bool restoreParallelVideoBrowsingStatus();

void saveRcTcpServerPort(quint16 port);
quint16 restoreRcTcpServerPort();

void saveTrainingPcTcpServerPort(quint16 port);
quint16 restoreTrainingPcTcpServerPort();

void saveTrainingPcTcpServerIpAddress(QString ipAddress);
QString restoreTrainingPcTcpServerIpAddress();

#endif  // SETTINGS_H
