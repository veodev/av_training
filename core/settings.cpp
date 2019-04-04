#include <QSettings>
#include <QCoreApplication>
#include "settings.h"
#include "Definitions.h"

static void saveStringListToSection(const QStringList& list, const QString& section)
{
    QSettings settings;
    settings.beginWriteArray(section + "s");
    for (int i = 0; i < list.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue(section, list[i]);
    }
    settings.endArray();
}

static QStringList restoreStringListFromSection(const QString& section)
{
    QStringList list;
    QSettings settings;
    int size = settings.beginReadArray(section + "s");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        list.append(settings.value(section).toString());
    }
    settings.endArray();

    return list;
}

QString manufactureName()
{
    return QObject::tr("Radioavionica");
}

QString applicationName()
{
#if defined(TARGET_AVICON31)
    return QObject::tr("testcon");
#elif defined(TARGET_AVICONDB)
    return QObject::tr("multichannel");
#elif defined(TARGET_AVICONDBHS)
    return QObject::tr("singlechannel");
#elif defined(TARGET_AVICON15)
    return QObject::tr("avicon-15");
#elif defined(TARGET_AVICON31E)
    return QObject::tr("avicon-31E");
#else
#error "unknown application name"
#endif
}

void setApplicationSettingsProperties()
{
    QCoreApplication::setOrganizationName(manufactureName());
    QCoreApplication::setOrganizationDomain("radioavionica.ru");
    QCoreApplication::setApplicationName(applicationName());
}

void saveLanguage(const QString& language)
{
    QSettings settings;
    settings.setValue("language", language);
}

QString restoreLanguage()
{
    QSettings settings;
    return settings.value("language").toString();
}

void saveVolume(double volume)
{
    QSettings settings;
    settings.setValue("volume", volume);
}

double getVolume()
{
    QSettings settings;
    return settings.value("volume", 50).toDouble();
}

bool getAScanPlotOptimization()
{
    QSettings settings;
    QString key("ascanplotoptimization");
    bool value = false;

    if (settings.contains(key)) {
        value = settings.value(key).toBool();
    }
    else {
        settings.setValue(key, value);
    }

    return value;
}

void saveOperatorsToSettings(const QStringList& list)
{
    saveStringListToSection(list, "operator");
}

QStringList restoreOperatorsToSettings()
{
    return restoreStringListFromSection("operator");
}

void saveLinesToSettings(const QStringList& list)
{
    saveStringListToSection(list, "line");
}

QStringList restoreLinesToSettings()
{
    return restoreStringListFromSection("line");
}

void saveSchemeNumber(int index)
{
    QSettings settings;
    settings.setValue("scheme", index);
}

int restoreSchemeNumber()
{
    QSettings settings;
    return settings.value("scheme", 0).toInt();
}

void setCduSerialNumber(const QString& serialNumber)
{
    QSettings settings;
    return settings.setValue("cduserialnumber", serialNumber);
}

QString cduSerialNumber()
{
    QSettings settings;
    return settings.value("cduserialnumber", "00000").toString();
}

void saveEncoderCorrection(float encoderCorrection)
{
    QSettings settings;
    settings.setValue("encodercorrection", encoderCorrection);
}

float encoderCorrection()
{
    QSettings settings;
    return settings.value("encodercorrection", 1.89f).toDouble();
}

void saveBscanScale(double scale)
{
    QSettings settings;
    settings.setValue("bscanscale", scale);
}

double restoreBscanScale()
{
    QSettings settings;
    return settings.value("bscanscale", 2).toDouble();
}

void saveBrightness(double value)
{
    QSettings settings;
    settings.setValue("brightness", value);
}

double restoreBrightness()
{
    QSettings settings;
    return settings.value("brightness", 30).toDouble();
}

void saveBScanDotSize(int value)
{
    QSettings settings;
    settings.setValue("bscandotsize", value);
}

int restoreBScanDotSize()
{
    QSettings settings;
    return settings.value("bscandotsize", 2).toInt();
}

void saveMScanDotSize(int value)
{
    QSettings settings;
    settings.setValue("mscandotsize", value);
}

int restoreMScanDotSize()
{
    QSettings settings;
    return settings.value("mscandotsize", 5).toInt();
}

void saveBridgesToSettings(const QStringList& list)
{
    saveStringListToSection(list, "bridge");
}

QStringList restoreBridgesToSettings()
{
    return restoreStringListFromSection("bridge");
}

void savePlatformsToSettings(const QStringList& list)
{
    saveStringListToSection(list, "platform");
}

QStringList restorePlatformsToSettings()
{
    return restoreStringListFromSection("platform");
}

void saveMiscToSettings(const QStringList& list)
{
    saveStringListToSection(list, "misc");
}

QStringList restoreMiscToSettings()
{
    return restoreStringListFromSection("misc");
}

void saveNoiseReduction(bool value)
{
    QSettings settings;
    settings.setValue("noisereduction", value);
}

bool restoreNoiseReduction()
{
    QSettings settings;
    return settings.value("noisereduction", true).toBool();
}

QString umuIpAddr()
{
    QSettings settings;
    return settings.value("umuipaddr", "192.168.100.100").toString();
}

int umuPortOut()
{
    QSettings settings;
    return settings.value("umuportout", 43000).toInt();
}

int umuPortIn()
{
    QSettings settings;
    return settings.value("umuportin", 43001).toInt();
}

void saveProbePulseLocation(bool isProbePulseFromAbove)
{
    QSettings settings;
    settings.setValue("isProbePulseFromAbove", isProbePulseFromAbove);
}

bool restoreProbePulseLocation()
{
    QSettings settings;
    return settings.value("isProbePulseFromAbove", false).toBool();
}

void saveIncreaseTracks(bool isIncreaseTracks)
{
    QSettings settings;
    settings.setValue("isIncreaseTracks", isIncreaseTracks);
}

bool restoreIncreaseTracks()
{
    QSettings settings;
    return settings.value("isIncreaseTracks", false).toBool();
}

void saveAdditiveForSens(int additive)
{
    QSettings settings;
    settings.setValue("additiveForSens", additive);
}

int restoreAdditiveForSens()
{
    QSettings settings;
    return settings.value("additiveForSens", 0).toInt();
}

int limitOfCountLogFiles()
{
    QSettings settings;
    return settings.value("limitOfCountLogFiles", 10).toInt();
}

bool invertEncoder()
{
    QSettings settings;
    return settings.value("invertEncoder", false).toBool();
}

void saveLastCalibrationForScheme(int schemeNumber, int calibrationIndex)
{
    QSettings settings;
    settings.setValue("calibrationIndexForScheme_" + QString::number(schemeNumber), calibrationIndex);
}

int restoreLastCalibrationForScheme(int schemeNumber)
{
    QSettings settings;
    return settings.value("calibrationIndexForScheme_" + QString::number(schemeNumber), 0).toInt();
}

void saveLastServiceDate(const QString& lastServiceDate)
{
    QSettings settings;
    settings.setValue("lastServiceDate", lastServiceDate);
}

QString restoreLastServiceDate()
{
    QSettings settings;
    return settings.value("lastServiceDate", QObject::tr("not performed")).toString();
}

void saveGainForControlSection(const QMap<int, int>& map)
{
    QSettings settings;
    QList<int> listOfKeys = map.uniqueKeys();
    QList<int> listOfValues = map.values();

    settings.beginWriteArray("gainMapKeys");
    for (int i = 0; i < listOfKeys.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("gainMapKeys", listOfKeys[i]);
    }
    settings.endArray();

    settings.beginWriteArray("gainMapValues");
    for (int i = 0; i < listOfValues.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("gainMapValues", listOfValues[i]);
    }
    settings.endArray();
}

QMap<int, int> restoreGainForControlSection()
{
    QSettings settings;
    QList<int> listOfKeys;
    QList<int> listOfValues;
    QMap<int, int> mapOfGains;

    int size = settings.beginReadArray("gainMapKeys");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        listOfKeys.append(settings.value("gainMapKeys").toInt());
    }
    settings.endArray();

    size = settings.beginReadArray("gainMapValues");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        listOfValues.append(settings.value("gainMapValues").toInt());
    }
    settings.endArray();

    for (int i = 0; i < size; ++i) {
        mapOfGains.insert(listOfKeys[i], listOfValues[i]);
    }
    return mapOfGains;
}

void saveKeyCombinations(const QMap<int, int>& map)
{
    QSettings settings;
    QList<int> listOfKeys = map.uniqueKeys();
    QList<int> listOfValues = map.values();

    settings.beginWriteArray("keyCombinationsKeys");
    for (int i = 0; i < listOfKeys.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("keyCombinationsKeys", listOfKeys[i]);
    }
    settings.endArray();

    settings.beginWriteArray("keyCombinationsValues");
    for (int i = 0; i < listOfValues.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("keyCombinationsValues", listOfValues[i]);
    }
    settings.endArray();
}

QMap<int, int> restoreKeyCombinations()
{
    QSettings settings;
    QList<int> listOfKeys;
    QList<int> listOfValues;
    QMap<int, int> mapOfKeyCombinations;

    int size = settings.beginReadArray("keyCombinationsKeys");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        listOfKeys.append(settings.value("keyCombinationsKeys").toInt());
    }
    settings.endArray();

    size = settings.beginReadArray("keyCombinationsValues");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        listOfValues.append(settings.value("keyCombinationsValues").toInt());
    }
    settings.endArray();

    for (int i = 0; i < size; ++i) {
        mapOfKeyCombinations.insert(listOfKeys[i], listOfValues[i]);
    }
    return mapOfKeyCombinations;
}


void saveLastKm(int km)
{
    QSettings settings;
    settings.setValue("lastKm", km);
}


int restoreLastKm()
{
    QSettings settings;
    return settings.value("lastKm", 0).toInt();
}


void saveLastPk(int pk)
{
    QSettings settings;
    settings.setValue("lastPk", pk);
}


int restoreLastPk()
{
    QSettings settings;
    return settings.value("lastPk", 0).toInt();
}


void saveLastM(int m)
{
    QSettings settings;
    settings.setValue("lastM", m);
}


int restoreLastM()
{
    QSettings settings;
    return settings.value("lastM", 0).toInt();
}


void saveMinimalBatteryPercent(int minimalPercent)
{
    QSettings settings;
    settings.setValue("minimalBatteryPercent", minimalPercent);
}


int restoreMinimalBatteryPercent()
{
    QSettings settings;
    return settings.value("minimalBatteryPercent", 0).toInt();
}


void saveTimeOffset(int timeOffset)
{
    QSettings settings;
    settings.setValue("timeOffset", timeOffset);
}


int restoreTimeOffset()
{
    QSettings settings;
    return settings.value("timeOffset", -100).toInt();
}

void saveOrganizationName(const QString& organizationName)
{
    QSettings settings;
    settings.setValue("organizationName", organizationName);
}

QString restoreOrganizationName()
{
    QSettings settings;
    return settings.value("organizationName", QObject::tr("unsetted")).toString();
}

void saveDepartmentName(const QString& departmentName)
{
    QSettings settings;
    settings.setValue("departmentName", departmentName);
}

QString restoreDepartmentName()
{
    QSettings settings;
    return settings.value("departmentName", QObject::tr("unsetted")).toString();
}

void saveCalibrationDate(const QString& calibrationDate)
{
    QSettings settings;
    settings.setValue("calibrationDate", calibrationDate);
}

QString restoreCalibrationDate()
{
    QSettings settings;
    return settings.value("calibrationDate", QObject::tr("dd.mm.yyyy")).toString();
}

void saveCalibrationValidityDate(const QString& calibrationValidityDate)
{
    QSettings settings;
    settings.setValue("calibrationValidityDate", calibrationValidityDate);
}

QString restoreCalibrationValidityDate()
{
    QSettings settings;
    return settings.value("calibrationValidityDate", QObject::tr("dd.mm.yyyy")).toString();
}

void saveCertificateNumber(const QString& certificateNumber)
{
    QSettings settings;
    settings.setValue("certificateNumber", certificateNumber);
}

QString restoreCertificateNumber()
{
    QSettings settings;
    return settings.value("certificateNumber", "").toString();
}

void saveStateCheckCalibrationAlgorithm(bool isEnabled)
{
    QSettings settings;
    settings.setValue("stateCheckCalibrationAlgorithm", isEnabled);
}

bool restoreStateCheckCalibrationAlgorithm()
{
    QSettings settings;
    return settings.value("stateCheckCalibrationAlgorithm", false).toBool();
}

void saveFiltrationEnable(bool isEnabled)
{
    QSettings settings;
    settings.setValue("stateFiltrationEnable", isEnabled);
}


bool restoreFiltrationEnable()
{
    QSettings settings;
    return settings.value("stateFiltrationEnable", false).toBool();
}

void saveFilterMinPacketSize(double value)
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        settings.setValue("filterMinPacketSize_Minus12dB", value);
        break;
    case -6:
        settings.setValue("filterMinPacketSize_Minus6dB", value);
        break;
    }
}

int restoreFilterMinPacketSize()
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        return settings.value("filterMinPacketSize_Minus12dB", FILTER_PARAM_MIN_PACKET_SIZE_MINUS_12_DB).toInt();
    case -6:
        return settings.value("filterMinPacketSize_Minus6dB", FILTER_PARAM_MIN_PACKET_SIZE_MINUS_6_DB).toInt();
    }
    return -1;
}

void saveFilterMaxCoordSpacing(double value)
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        settings.setValue("filterMaxCoordSpacing_Minus12dB", value);
        break;
    case -6:
        settings.setValue("filterMaxCoordSpacing_Minus6dB", value);
        break;
    }
}

int restoreFilterMaxCoordSpacing()
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        return settings.value("filterMaxCoordSpacing_Minus12dB", FILTER_PARAM_COORD_DELTA_MAX_MINUS_12_DB).toInt();
    case -6:
        return settings.value("filterMaxCoordSpacing_Minus6dB", FILTER_PARAM_COORD_DELTA_MAX_MINUS_6_DB).toInt();
    }
    return -1;
}

void saveFilterMaxDelayDelta(double value)
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        settings.setValue("filterMaxDelayDelta_Minus12dB", value);
        break;
    case -6:
        settings.setValue("filterMaxDelayDelta_Minus6dB", value);
        break;
    }
}

int restoreFilterMaxDelayDelta()
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        return settings.value("filterMaxDelayDelta_Minus12dB", FILTER_PARAM_DELAY_DELTA_MAX_MINUS_12_DB).toInt();
    case -6:
        return settings.value("filterMaxDelayDelta_Minus6dB", FILTER_PARAM_DELAY_DELTA_MAX_MINUS_6_DB).toInt();
    }
    return -1;
}

void saveFilterMaxSameDelayConsecutive(double value)
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        settings.setValue("filterMaxSameDelayConsecutive_Minus12dB", value);
        break;
    case -6:
        settings.setValue("filterMaxSameDelayConsecutive_Minus6dB", value);
        break;
    }
}

int restoreFilterMaxSameDelayConsecutive()
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        return settings.value("filterMaxSameDelayConsecutive_Minus12dB", FILTER_PARAM_SAME_DELAY_MAX_MINUS_12_DB).toInt();
    case -6:
        return settings.value("filterMaxSameDelayConsecutive_Minus6dB", FILTER_PARAM_SAME_DELAY_MAX_MINUS_6_DB).toInt();
    }
    return -1;
}

void saveStateAcousticContactControl(bool isEnabled)
{
    QSettings settings;
    settings.setValue("stateAcousticContactControl", isEnabled);
}

bool restoreStateAcousticContactControl()
{
    QSettings settings;
    return settings.value("stateAcousticContactControl", true).toBool();
}

void saveStartDelay(int delay)
{
    QSettings settings;
    settings.setValue("startDelay", delay);
}

int restoreStartDelay()
{
    QSettings settings;
    return settings.value("startDelay", 0).toInt();
}

void saveAcousticThreshold(int threshold)
{
    QSettings settings;
    settings.setValue("acousticThreshold", threshold);
}

int restoreAcousticThreshold()
{
    QSettings settings;
    return settings.value("acousticThreshold", 0).toInt();
}

void saveHeadScannerPageState(bool isEnabled)
{
    QSettings settings;
    settings.setValue("headScanPageState", isEnabled);
}

bool restoreHeadScannerPageState()
{
    QSettings settings;
    return settings.value("headScanPageState", false).toBool();
}

void saveEkasuiPageState(bool isEnabled)
{
    QSettings settings;
    settings.setValue("ekasuiPageState", isEnabled);
}

bool restoreEkasuiPageState()
{
    QSettings settings;
    return settings.value("ekasuiPageState", true).toBool();
}

void saveHeadScannerUseState(bool isUsed)
{
    QSettings settings;
    settings.setValue("headScanUseState", isUsed);
}

bool restoreHeadScannerUseState()
{
    QSettings settings;
    return settings.value("headScanUseState", false).toBool();
}

void saveHorizontalScannerEncoderStep(double encoderCorrection)
{
    QSettings settings;
    settings.setValue("horizontalScannerEncoderStep", encoderCorrection);
}

double restoreHorizontalScannerEncoderStep()
{
    QSettings settings;
    return settings.value("horizontalScannerEncoderStep", 0.52).toDouble();
}

void saveVerticalScannerEncoderStep(double encoderCorrection)
{
    QSettings settings;
    settings.setValue("verticalScannerEncoderStep", encoderCorrection);
}

double restoreVerticalScannerEncoderStep()
{
    QSettings settings;
    return settings.value("verticalScannerEncoderStep", 1.0).toDouble();
}

void saveRegistrationThreshold(int value)
{
    QSettings settings;
    settings.setValue("registrationThreshold", value);
}

int restoreRegistrationThreshold()
{
    QSettings settings;
    return settings.value("registrationThreshold", -6).toInt();
}

void saveFtpServer(const QString& server)
{
    QSettings settings;
    settings.setValue("ftpServer", server);
}

QString restoreFtpServer()
{
    QSettings settings;
    return settings.value("ftpServer", "").toString();
}

void saveFtpPath(const QString& path)
{
    QSettings settings;
    settings.setValue("ftpPath", path);
}

QString restoreFtpPath()
{
    QSettings settings;
    return ("/" + cduSerialNumber() + settings.value("ftpPath", "/").toString());
}

QString restoreFtpRootPath()
{
    return "/";
}

void saveFtpLogin(const QString& login)
{
    QSettings settings;
    settings.setValue("ftpLogin", login);
}

QString restoreFtpLogin()
{
    QSettings settings;
    return settings.value("ftpLogin", "").toString();
}

void saveFtpPassword(const QString& password)
{
    QSettings settings;
    settings.setValue("ftpPassword", password);
}

QString restoreFtpPassword()
{
    QSettings settings;
    return settings.value("ftpPassword", "").toString();
}

void saveImportantAreaIndicateEnable(bool isEnabled)
{
    QSettings settings;
    settings.setValue("stateImportantAreaIndicateEnable", isEnabled);
}

bool restoreImportantAreaIndicateEnable()
{
    QSettings settings;
    return settings.value("stateImportantAreaIndicateEnable", false).toBool();
}

void saveImportantAreaMinPacketSize(int value)
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        settings.setValue("stateImportantAreaMinPacketSize_Minus12dB", value);
        break;
    case -6:
        settings.setValue("stateImportantAreaMinPacketSize_Minus6dB", value);
        break;
    }
}

int restoreImportantAreaMinPacketSize()
{
    QSettings settings;
    switch (restoreRegistrationThreshold()) {
    case -12:
        return settings.value("stateImportantAreaMinPacketSize_Minus12dB", IMPORTANT_AREA_MIN_PACKET_SIZE_MINUS_12_DB).toInt();
    case -6:
        return settings.value("stateImportantAreaMinPacketSize_Minus6dB", IMPORTANT_AREA_MIN_PACKET_SIZE_MINUS_6_DB).toInt();
    }
    return -1;
}

void saveImportantAreaMinSignals0dB(int value)
{
    QSettings settings;
    settings.setValue("stateImportantAreaMinSignals0dB", value);
}

int restoreImportantAreaMinSignals0dB()
{
    QSettings settings;
    return settings.value("stateImportantAreaMinSignals0dB", 10).toInt();
}

void saveEvalCalibrMinZTMSpacingSize(int value)
{
    QSettings settings;
    settings.setValue("stateEvalCalibrMinZTMSpacingSize", value);
}

int restoreEvalCalibrMinZTMSpacingSize()
{
    QSettings settings;
    return settings.value("stateEvalCalibrMinZTMSpacingSize", 15).toInt();
}

void saveMinKdEchoPrm(int value)
{
    QSettings settings;
    settings.setValue("minKdEchoValue", value);
}

int restoreMinKdEchoPrm()
{
    QSettings settings;
    return settings.value("minKdEchoValue", -8).toInt();  //-6 15.05.2017
}

int restoreImportantAreaMinZTMSpacing()
{
    QSettings settings;
    return settings.value("importantAreaMinZTMSpacing", 10).toInt();
}

void saveImportantAreaMinZTMSpacing(int value)
{
    QSettings settings;
    settings.setValue("importantAreaMinZTMSpacing", value);
}

void saveHorizontalZone(int value)
{
    QSettings settings;
    settings.setValue("horizontalZone", value);
}

int restoreHorizontalZone()
{
    QSettings settings;
    return settings.value("horizontalZone", 92).toInt();
}

void saveVerticalStartPosition(int value)
{
    QSettings settings;
    settings.setValue("verticalStartPosition", value);
}

int restoreVerticalStartPosition()
{
    QSettings settings;
    return settings.value("verticalStartPosition", 9).toInt();
}

void saveHeadScannerParams(int XPathStep, int YPathStep, int StartY, int ScanWidth, int ScanHeight, int CurrentY, int ScannerOrientation, int WorkRail, int Threshold)
{
    // TODO: !!! unused params
    Q_UNUSED(XPathStep);
    Q_UNUSED(YPathStep);
    Q_UNUSED(StartY);
    Q_UNUSED(ScanWidth);
    QSettings settings;
    //    settings.setValue("XPathStep", XPathStep);
    //    settings.setValue("YPathStep", YPathStep);
    //    settings.setValue("StartY", StartY);
    settings.setValue("CurrentY", CurrentY);
    //    settings.setValue("ScanWidth", ScanWidth);
    settings.setValue("ScanHeight", ScanHeight);
    settings.setValue("ScannerOrientation", ScannerOrientation);
    settings.setValue("WorkRail", WorkRail);
    settings.setValue("Threshold", Threshold);
}

void restoreHeadScannerParams(int& XPathStep, int& YPathStep, int& StartY, int& ScanWidth, int& ScanHeight, int& CurrentY, int& ScannerOrientation, int& WorkRail, int& Threshold)
{
    QSettings settings;
    XPathStep = qRound(restoreHorizontalScannerEncoderStep() * 100);
    YPathStep = qRound(restoreVerticalScannerEncoderStep() * 100);
    StartY = restoreVerticalStartPosition();
    CurrentY = settings.value("CurrentY", 9).toInt();
    //    ScanWidth=restoreHorizontalZone();
    ScanWidth = 92;
    ScanHeight = settings.value("ScanHeight", 28).toInt();
    ScannerOrientation = settings.value("ScannerOrientation", 2).toInt();
    WorkRail = settings.value("WorkRail", 0).toInt();
    Threshold = settings.value("Threshold", 50).toInt();
}

void saveHeadScannerWorkRail(int workrail)
{
    QSettings settings;
    settings.setValue("WorkRail", workrail);
}

int restoreHeadScannerWorkRail()
{
    QSettings settings;
    return settings.value("WorkRail", 0).toInt();
}

void saveHeadScannerOrientation(int ScannerOrientation)
{
    QSettings settings;
    settings.setValue("ScannerOrientation", ScannerOrientation);
}

int restoreHeadScannerOrientation()
{
    QSettings settings;
    return settings.value("ScannerOrientation", 2).toInt();
}

//Настройки датчиков температуры
void saveExternalTemperatureEnabled(bool value)
{
    QSettings settings;
    settings.setValue("ExternalTemperatureEnabled", value);
}

bool restoreExternalTemperatureEnabled()
{
    QSettings settings;
    return settings.value("ExternalTemperatureEnabled", true).toBool();
}


void saveExternalTempSensorType(int type)
{
    QSettings settings;
    settings.setValue("ExternalTempSensorType", type);
}

int restoreExternalTempSensorType()
{
    QSettings settings;
    return settings.value("ExternalTempSensorType", 3).toInt();
}

void saveInternalTempSensorType(int type)
{
    QSettings settings;
    settings.setValue("InternalTempSensorType", type);
}

int restoreInternalTempSensorType()
{
    QSettings settings;
    return settings.value("InternalTempSensorType", 2).toInt();
}

void saveVirtualKeybLanguageIdx(int value)
{
    QSettings settings;
    settings.setValue("VirtualKeybLanguageIdx", value);
}

int restoreVirtualKeybLanguageIdx()
{
    QSettings settings;
    return settings.value("VirtualKeybLanguageIdx", 2).toInt();
}

void saveTrolleyLeftSide(bool isTrolleyLeftSideWork)
{
    QSettings settings;
    settings.setValue("isTrolleyLeftSideWork", isTrolleyLeftSideWork);
}

bool restoreTrolleyLeftSide()
{
    QSettings settings;
    return settings.value("isTrolleyLeftSideWork", false).toBool();
}

void saveAScanFillingState(bool isFillingAScan)
{
    QSettings settings;
    settings.setValue("isFillingAScan", isFillingAScan);
}

bool restoreAScanFillingState()
{
    QSettings settings;
    return settings.value("isFillingAScan", true).toBool();
}

void saveRuLocation(bool isBehind)
{
    QSettings settings;
    settings.setValue("isBehind", isBehind);
}

bool restoreRuLocation()
{
    QSettings settings;
    return settings.value("isBehind", true).toBool();
}

void saveEKASUIRailroad(const QString& railroad)
{
    QSettings settings;
    settings.setValue("EKASUIRailroad", railroad);
}

QString restoreEKASUIRailroad()
{
    QSettings settings;
    return settings.value("EKASUIRailroad", "").toString();
}

void saveEKASUIPRED(const QString& pred)
{
    QSettings settings;
    settings.setValue("EKASUIPRED", pred);
}

QString restoreEKASUIPRED()
{
    QSettings settings;
    return settings.value("EKASUIPRED", "").toString();
}

int restoreEKASUIMode()
{
    QSettings settings;
    return settings.value("EKASUIMode", 0).toInt();
}

void saveEKASUIMode(int value)
{
    QSettings settings;
    settings.setValue("EKASUIMode", value);
}

void saveACLevel(int value)
{
    QSettings settings;
    settings.setValue("ACLevel", value);
}

int restoreACLevel()
{
    QSettings settings;
    return settings.value("ACLevel", 30).toInt();
}

void saveACFreq(int value)
{
    QSettings settings;
    settings.setValue("ACFreq", value);
}

int restoreACFreq()
{
    QSettings settings;
    return settings.value("ACFreq", 1).toInt();
}

void saveACGain(int value)
{
    QSettings settings;
    settings.setValue("ACGain", value);
}

int restoreACGain()
{
    QSettings settings;
    return settings.value("ACGain", 70).toInt();
}

void saveACVoiceNotificationEnabled(bool value)
{
    QSettings settings;
    settings.setValue("ACVoiceNotificationEnabled", value);
}

bool restoreACVoiceNotificationEnabled()
{
    QSettings settings;
    return settings.value("ACVoiceNotificationEnabled", false).toBool();
}

void saveAutoSensResetEnabled(bool value)
{
    QSettings settings;
    settings.setValue("AutoSensReset", value);
}

bool restoreAutoSensResetEnabled()
{
    QSettings settings;
    return settings.value("AutoSensReset", false).toBool();
}

void saveAutoSensResetDelay(int value)
{
    QSettings settings;
    settings.setValue("AutoSensResetDelay", value);
}

int restoreAutoSensResetDelay()
{
    QSettings settings;
    return settings.value("AutoSensResetDelay", 30).toInt();
}

void saveEKASUIControlObject(int value)
{
    QSettings settings;
    settings.setValue("EKASUIControlObject", value);
}

int restoreEKASUIControlObject()
{
    QSettings settings;
    return settings.value("EKASUIControlObject", 0).toInt();
}

void saveDeviceType(int devtype)
{
    QSettings settings;
    settings.setValue("DeviceType", devtype);
}

int restoreDeviceType()
{
    QSettings settings;
    return settings.value("DeviceType", 0).toInt();
}

void saveRegistrationType(int regtype)
{
    QSettings settings;
    settings.setValue("RegistrationType", regtype);
}

int restoreRegistrationType()
{
    QSettings settings;
    return settings.value("RegistrationType", 0).toInt();
}

void saveElectricTestStatus(bool isActivate)
{
    QSettings settings;
    settings.setValue("ElectricTestStatus", isActivate);
}

bool restoreElectricTestStatus()
{
    QSettings settings;
    return settings.value("ElectricTestStatus", false).toBool();
}

void saveCountCrashes(int count)
{
    QSettings settings;
    settings.setValue("CountCrashes", count);
}

int restoreCountCrashes()
{
    QSettings settings;
    return settings.value("CountCrashes", 0).toInt();
}

void saveCountCduReboot(int count)
{
    QSettings settings;
    settings.setValue("CountCduReboot", count);
}

int restoreCountCduReboot()
{
    QSettings settings;
    return settings.value("CountCduReboot", 0).toInt();
}

void saveCountUmuDisconnect(int count)
{
    QSettings settings;
    settings.setValue("CountUmuDisconnect", count);
}

int restoreCountUmuDisconnect()
{
    QSettings settings;
    return settings.value("CountUmuDisconnect", 0).toInt();
}

void saveOpacityImportantArea(int opacity)
{
    QSettings settings;
    settings.setValue("OpacityImportatnArea", opacity);
}

int restoreOpacityImportantArea()
{
    QSettings settings;
    return settings.value("OpacityImportatnArea", 25).toInt();
}

void saveColorImportantAreaIndex(int colorIndex)
{
    QSettings settings;
    settings.setValue("ColorImportantArea", colorIndex);
}

int restoreColorImportantAreaIndex()
{
    QSettings settings;
    return settings.value("ColorImportantArea", 6).toInt();
}

void saveEKASUIRegistrationLastDirection(const QString& value)
{
    QSettings settings;
    settings.setValue("EKASUIRegistrationLastDirection", value);
}

QString restoreEKASUIRegistrationLastDirection()
{
    QSettings settings;
    return settings.value("EKASUIRegistrationLastDirection", "").toString();
}

void saveEKASUIRegistrationLastStation(const QString& value)
{
    QSettings settings;
    settings.setValue("EKASUIRegistrationLastStation", value);
}

QString restoreEKASUIRegistrationLastStation()
{
    QSettings settings;
    return settings.value("EKASUIRegistrationLastStation", "").toString();
}

void saveEKASUIRegistrationLastTrackNumber(const QString& value)
{
    QSettings settings;
    settings.setValue("EKASUIRegistrationLastTrackNumber", value);
}

QString restoreEKASUIRegistrationLastTrackNumber()
{
    QSettings settings;
    return settings.value("EKASUIRegistrationLastTrackNumber", "").toString();
}

void saveDefaultRegistrationLastLine(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastLine", value);
}

QString restoreDefaultRegistrationLastLine()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastLine", "").toString();
}

void saveDefaultRegistrationLastTrackNumber(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastTrackNumber", value);
}

QString restoreDefaultRegistrationLastTrackNumber()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastTrackNumber", "").toString();
}

void saveDefaultRegistrationLastDirectionCode(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastDirectionCode", value);
}

QString restoreDefaultRegistrationLastDirectionCode()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastDirectionCode", "").toString();
}

void saveEKASUICarId(const QString& value)
{
    QSettings settings;
    settings.setValue("EKASUICarId", value);
}

QString restoreEKASUICarId()
{
    QSettings settings;
    return settings.value("EKASUICarId", "").toString();
}

void saveFactoryPassword(const QString& value)
{
    QSettings settings;
    settings.setValue("FactoryPassword", value);
}

QString restoreFactoryPassword()
{
    QSettings settings;
    return settings.value("FactoryPassword", "12345").toString();
}

void saveLabPassword(const QString& value)
{
    QSettings settings;
    settings.setValue("LabPassword", value);
}

QString restoreLabPassword()
{
    QSettings settings;
    return settings.value("LabPassword", "12345").toString();
}

void saveManagerPassword(const QString& value)
{
    QSettings settings;
    settings.setValue("ManagerPassword", value);
}

QString restoreManagerPassword()
{
    QSettings settings;
    return settings.value("ManagerPassword", "12345").toString();
}

void saveEKASUIPassword(const QString& value)
{
    QSettings settings;
    settings.setValue("EKASUIPassword", value);
}

QString restoreEKASUIPassword()
{
    QSettings settings;
    return settings.value("EKASUIPassword", "12345").toString();
}

QString organisationName()
{
    QString str;
    switch (restoreDeviceType()) {
    case -1:  // Unknown
        str = QObject::tr("Not set!");
        break;
    case 0:  // Avicon31Default
        str = QObject::tr("Radioavionica");
        break;
    case 1:  // Avicon31KZ
        str = QObject::tr("TOO Radioavionica-Asia");
        break;
    case 2:  // AviconDB
        str = QString();
        break;
    case 3:  // AviconDBHS
        str = QString();
        break;
    case 4:  // Avicon15
        str = QObject::tr("Radioavionica");
        break;
    case 5:  // Avicon31 Estonia
        str = QObject::tr("Radioavionica");
        break;
    }
    return str;
}

void saveNotificationDistance(int distance)
{
    QSettings settings;
    settings.setValue("NotificationDistance", distance);
}

int restoreNotificationDistance()
{
    QSettings settings;
    return settings.value("NotificationDistance", 1).toInt();
}

void saveTypeViewCoordinateForBScan(int viewType)
{
    QSettings settings;
    settings.setValue("TypeViewCoordinateForBScan", viewType);
}

int restoreTypeViewCoordinateForBScan()
{
    QSettings settings;
    return settings.value("TypeViewCoordinateForBScan", 0).toInt();
}

void setDefectoscopeSerialNumber(const QString& serialNumber)
{
    QSettings settings;
    return settings.setValue("defectoscopeserialnumber", serialNumber);
}

QString defectoscopeSerialNumber()
{
    QSettings settings;
    return settings.value("defectoscopeserialnumber", "00000").toString();
}

void saveScreenHeaterStatus(bool isEnabled)
{
    QSettings settings;
    settings.setValue("ScreenHeaterStatus", isEnabled);
}

bool restoreScreenHeaterStatus()
{
    QSettings settings;
    return settings.value("ScreenHeaterStatus", false).toBool();
}

void saveScreenHeaterTemp(int temp)
{
    QSettings settings;
    settings.setValue("ScreenHeaterTemp", temp);
}

int restoreScreenHeaterTemp()
{
    QSettings settings;
    return settings.value("ScreenHeaterTemp", 5).toInt();
}

int restoreColorImportantAreaColorR()
{
    QSettings settings;
    return settings.value("ColorImportantAreaColorR", 64).toInt();
}

int restoreColorImportantAreaColorG()
{
    QSettings settings;
    return settings.value("ColorImportantAreaColorG", 255).toInt();
}

int restoreColorImportantAreaColorB()
{
    QSettings settings;
    return settings.value("ColorImportantAreaColorB", 64).toInt();
}

void saveColorImportantAreaColor(int r, int g, int b)
{
    QSettings settings;
    settings.setValue("ColorImportantAreaColorR", r);
    settings.setValue("ColorImportantAreaColorG", g);
    settings.setValue("ColorImportantAreaColorB", b);
}

void saveRegarStatus(bool isEnabled)
{
    QSettings settings;
    settings.setValue("RegarStatus", isEnabled);
}

bool restoreRegarStatus()
{
    QSettings settings;
    return settings.value("RegarStatus", false).toBool();
}

void saveAutoGainAdjustmentStatus(bool isEnabled)
{
    QSettings settings;
    settings.setValue("AutoGainAdjustmentStatus", isEnabled);
}

bool restoreAutoGainAdjustmentStatus()
{
    QSettings settings;
    return settings.value("AutoGainAdjustmentStatus", false).toBool();
}

void saveDefaultRegistrationLastOperator1(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastOpeartor1", value);
}

QString restoreDefaultRegistrationLastOperator1()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastOpeartor1", "").toString();
}

void saveDefaultRegistrationLastOperator2(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastOpeartor2", value);
}

QString restoreDefaultRegistrationLastOperator2()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastOpeartor2", "").toString();
}

void saveDefaultRegistrationLastOperator3(const QString& value)
{
    QSettings settings;
    settings.setValue("DefaultRegistrationLastOpeartor3", value);
}

QString restoreDefaultRegistrationLastOperator3()
{
    QSettings settings;
    return settings.value("DefaultRegistrationLastOpeartor3", "").toString();
}

void saveParallelVideoBrowsingStatus(bool isEnabled)
{
    QSettings settings;
    settings.setValue("ParallelVideoBrowsingStatus", isEnabled);
}

bool restoreParallelVideoBrowsingStatus()
{
    QSettings settings;
    return settings.value("ParallelVideoBrowsingStatus", false).toBool();
}

void saveRcTcpServerPort(quint16 port)
{
    QSettings settings;
    settings.setValue("RcTcpServerPort", port);
}

quint16 restoreRcTcpServerPort()
{
    QSettings settings;
    return static_cast<quint16>(settings.value("RcTcpServerPort", 49001).toInt());
}

void saveTrainingPcTcpServerPort(quint16 port)
{
    QSettings settings;
    settings.setValue("TrainingPcTcpServerPort", port);
}

quint16 restoreTrainingPcTcpServerPort()
{
    QSettings settings;
    return static_cast<quint16>(settings.value("TrainingPcTcpServerPort", 50003).toInt());
}

void saveTrainingPcTcpServerIpAddress(QString ipAddress)
{
    QSettings settings;
    settings.setValue("TrainingPcTcpServerIpAddress", ipAddress);
}

QString restoreTrainingPcTcpServerIpAddress()
{
    QSettings settings;
    return settings.value("TrainingPcTcpServerIpAddress", "127.0.0.1").toString();
}
