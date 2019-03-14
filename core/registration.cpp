#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QBuffer>
#include <cmath>

#include "core.h"
#include "defcore.h"
#include "channelscontroller.h"
#include "settings.h"
#include "debug.h"
#include "registration.h"
#include "roles.h"
#include "appdatapath.h"
#include "DataContainer.h"
#include "temperature.h"
#include "settings.h"
#include "notifier.h"
#include "Device_definitions.h"

const int UPDATE_INTERVAL_MSEC = 60000;
const int RECORD_SPEED_INTERVAL_MSEC = 10000;
const int UPDATE_GPS_DISTANCE_IN_METERS = 10;

#if defined TARGET_AVICON31
const QString prefix = ".a31";
#elif defined(TARGET_AVICONDB)
const QString prefix = ".F17m";
#elif defined(TARGET_AVICONDBHS)
const QString prefix = ".F17s";
#elif defined(TARGET_AVICON15)
const QString prefix = ".a15";
#elif defined(TARGET_AVICON31E)
const QString prefix = ".a31e";
#endif

Registration::Registration(QObject* parent)
    : QObject(parent)
    , _dataContainer(nullptr)
    , _defcore(nullptr)
    , _channelsModel(nullptr)
    , _isPause(false)
    , _isCheckAcousticContact(restoreStateAcousticContactControl())
    , _isCloseHeader(false)
    , _regarStatus(restoreRegarStatus())
#if defined TARGET_AVICON31
    , _geoPosition(GeoPosition::instance("/dev/ttymxc1"))
#elif defined TARGET_AVICON15
    , _geoPosition(GeoPosition::instance())
#endif
    , _tempTrackMarks(nullptr)
    , _latitude(qSNaN())
    , _longitude(qSNaN())
    , _updateTimer(this)
    , _recordSpeedTimer(this)
    , _modesElapsedTimer(new QElapsedTimer())
    , _lastSpeed(0)
    , _maxSpeed(0)
    , _lastChannelType(ctCompInsp)
    , _isGpsAntennaConnect(false)
    , _countSatellites(0)
    , _deviceType(static_cast<DeviceType>(restoreDeviceType()))
{
    _isTemp = false;
}

Registration::~Registration()
{
    stop(true);
    disconnect(_defcore, &Defcore::doBScan2Data, this, &Registration::onBScan2Data);
    disconnect(_defcore, &Defcore::doRailTypeResult, this, &Registration::onRailTypeResult);
    disconnect(_channelsModel, &QAbstractItemModel::dataChanged, this, &Registration::onDataChanged);

    _modesElapsedTimer->invalidate();
    delete _modesElapsedTimer;
    _updateTimer.stop();
    _recordSpeedTimer.stop();

    qDebug() << "Registration deleted!";
}

void Registration::setDefcore(Defcore* defcore)
{
    Q_ASSERT(defcore != nullptr);
    _defcore = defcore;

    ASSERT(connect(defcore, &Defcore::doBScan2Data, this, &Registration::onBScan2Data));
    ASSERT(connect(defcore, &Defcore::doRailTypeResult, this, &Registration::onRailTypeResult));

    _maxSpeed = _defcore->maxSpeed();
}

void Registration::setModel(QAbstractItemModel* channelsModel)
{
    Q_ASSERT(channelsModel);

    if (_channelsModel == channelsModel) {
        return;
    }
    if (_channelsModel != nullptr) {
        disconnect(_channelsModel, &QAbstractItemModel::dataChanged, this, &Registration::onDataChanged);
    }
    _channelsModel = channelsModel;
    if (_channelsModel != nullptr) {
        ASSERT(connect(_channelsModel, &QAbstractItemModel::dataChanged, this, &Registration::onDataChanged, Qt::UniqueConnection));
    }
}

void Registration::start(const QString& operatorName, const QString& pathSectionName, const QString& trackNumber, TrackMarks* trackMarks, bool isReverseSides, bool isDirectionIncrease, int directionCode, bool isService, bool controledRail, const QString& fileName)
{
    if (operatorName.isEmpty()) {
        Notifier::instance().addNote(Qt::red, tr("Input fields are empty"));
    }

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    Q_UNUSED(controledRail);
#else
    Q_UNUSED(isReverseSides);
    Q_UNUSED(directionCode);
#endif
    Q_ASSERT(_defcore != nullptr);

    Core& core = Core::instance();
    disconnect(&core, &Core::doStartBoltJoint, this, &Registration::onStartBoltJoint);
    disconnect(&core, &Core::doStopBoltJoint, this, &Registration::onStopBoltJoint);
    ASSERT(connect(&core, &Core::doStartBoltJoint, this, &Registration::onStartBoltJoint, Qt::UniqueConnection));
    ASSERT(connect(&core, &Core::doStopBoltJoint, this, &Registration::onStopBoltJoint, Qt::UniqueConnection));

    core.disableAll(true);

    core.resetPathEncoder();

#if defined TARGET_AVICON31
    _dataContainer = new cDataContainer(static_cast<eFileID>(restoreSchemeNumber()), _defcore->getChannelsTablePtr());
#elif defined(TARGET_AVICONDB) || defined(TARGET_AVICONDBHS)
    _dataContainer = new cDataContainer(ftFilusX111W, _defcore->getChannelsTablePtr());
#elif defined(TARGET_AVICON15)
    _dataContainer = new cDataContainer(ftAvicon15_N8_Scheme1, _defcore->getChannelsTablePtr());
#elif defined TARGET_AVICON31E
    _dataContainer = new cDataContainer(ftAvicon31E, _defcore->getChannelsTablePtr());
#endif
    Q_ASSERT(_dataContainer != nullptr);

    _dataContainer->setRegArEnabled(_regarStatus);

    QString finalFileName;
    if (fileName.size() == 0) {
        finalFileName = makeRegistrationFileName(isService);
    }
    else {
        finalFileName = fileName;
    }
    _dataContainer->CreateFile(finalFileName);

    const QStringList& splitBySlashResult = finalFileName.split("/");
    QStringList splitByDotResult;
    if (splitBySlashResult.isEmpty() == false) {
        splitByDotResult = splitBySlashResult.last().split(".");
    }
    QString splitName;
    if (splitByDotResult.isEmpty() == false) {
        splitName = splitByDotResult.first();
    }

    Core::instance().videoRegistrationStart(splitName);
    qDebug() << "splitName: " << splitName;

    _isPause = false;

    _dataContainer->AddDeviceUnitInfo(dutCDU, cduSerialNumber(), APP_VERSION);
    if (_deviceType == Avicon31Default || _deviceType == Avicon31KZ) {
        _dataContainer->SetFlawDetectorNumber(defectoscopeSerialNumber());
    }

    QStringList deviceSerialNumbers = core.deviceSerialNumbers();
    QStringList deviceFirmwareVersions = core.deviceFirmwareVersions();

    for (int i = 0; i < deviceSerialNumbers.count(); ++i) {
        _dataContainer->AddDeviceUnitInfo(dutUMU, deviceSerialNumbers[i], deviceFirmwareVersions[i]);
    }

    _dataContainer->SetRailRoadName(restoreOrganizationName());
    _dataContainer->SetOrganizationName(restoreDepartmentName());
    _dataContainer->SetPathSectionName(pathSectionName);
    _dataContainer->SetOperatorName(operatorName);
    _dataContainer->SetRailPathTextNumber(trackNumber);

    const QTime& currentTime = QTime::currentTime();
    const QDate& currentDate = QDate::currentDate();
    _dataContainer->SetDateTime(currentDate.day(), currentDate.month(), currentDate.year(), currentTime.hour(), currentTime.minute());

    switch (core.getSystemCoordType()) {
    case csMetricRF: {
        tMRFCrd PostRus;
        PostRus.Km = static_cast<TMRussian*>(trackMarks)->getKm();
        PostRus.Pk = static_cast<TMRussian*>(trackMarks)->getPk();
        PostRus.mm = static_cast<TMRussian*>(trackMarks)->getM() * 1000;
        _dataContainer->SetStartMRFCrd(PostRus);
    } break;
    case csMetric1km: {
        tCaCrd PostMetric;
        PostMetric.XXX = static_cast<TMMetric1KM*>(trackMarks)->getKm();
        PostMetric.YYY = static_cast<TMMetric1KM*>(trackMarks)->getM();
        _dataContainer->SetStartCaCrd(core.getSystemCoordType(), PostMetric);
    } break;
    default:
        qDebug() << "Cannot use coord system!";
        break;
    }

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    Q_UNUSED(controledRail);
    _dataContainer->SetDirectionCode(directionCode);
    _dataContainer->SetCorrespondenceSides(isReverseSides ? csReverse : csDirect);
#else
    _dataContainer->SetWorkRailTypeA(static_cast<eDeviceSide>(controledRail + 1));
#endif

#if defined TARGET_AVICON31
    _dataContainer->SetMaintenanceServicesDate(getServiceDate().day(), getServiceDate().month(), getServiceDate().year());
    const QDate& calibrationDate = QDate::fromString(restoreCalibrationDate(), "dd.MM.yyyy");
    _dataContainer->SetCalibrationDate(calibrationDate.day(), calibrationDate.month(), calibrationDate.year());
#endif
    _dataContainer->SetCalibrationName(Core::instance().getCurrentCalibrationName());

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    _dataContainer->SetUseGPSTrack();

    if (restoreStateAcousticContactControl()) {
        _dataContainer->SetAcousticContact();
    }
#endif
#if defined TARGET_AVICON15
    _dataContainer->SetBScanTreshold_minus_6dB();
#endif

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    int regThreshold = restoreRegistrationThreshold();
    if (regThreshold == -6) {
        _dataContainer->SetBScanTreshold_minus_6dB();
    }
    else if (regThreshold == -12) {
        _dataContainer->SetBScanTreshold_minus_12dB();
    }
#endif

#if defined TARGET_AVICONDBHS
    _dataContainer->SetHandScanFileType();
#endif

    _dataContainer->SetSpeed();
#if defined TARGET_AVICON31  // TODO Fix rec or not temperature for different projects.
    if (restoreExternalTemperatureEnabled()) {
        _dataContainer->SetTemperature();
    }
#endif

    _isCloseHeader = _dataContainer->CloseHeader(isDirectionIncrease ? 1 : -1, (encoderCorrection() * 100));

    if (!isService && !makeTestRecordFileName().isEmpty()) {
        _dataContainer->SetTestRecordFile(makeTestRecordFileName());
    }

#if !defined TARGET_AVICONDBHS
    addParamsForAllScanChannels();
#endif

    _updateTimer.start(UPDATE_INTERVAL_MSEC);
    _recordSpeedTimer.start(RECORD_SPEED_INTERVAL_MSEC);
    ASSERT(connect(&core, &Core::externalTemperatureChanged, this, &Registration::onTemperatureChanged));
#if defined TARGET_AVICON31 || defined TARGET_AVICON15
    GeoPosition* geoPosition = _geoPosition.data();
    if (geoPosition != nullptr) {
        ASSERT(connect(geoPosition, &GeoPosition::positionUpdated, this, &Registration::satellitePositionUpdated));
        ASSERT(connect(geoPosition, &GeoPosition::antennaStatusChanged, this, &Registration::satelliteAntennaStatusChanged));
        ASSERT(connect(geoPosition, &GeoPosition::satellitesInUseUpdated, this, &Registration::satelliteInUseChanged));
    }
    addSensAllowedRanges(core.getSensValidRanges());
#elif defined TARGET_AVICONDB
    ASSERT(connect(&_updateTimer, &QTimer::timeout, this, &Registration::onAddLocalTime));
#endif
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E || defined TARGET_AVICON15
    ASSERT(connect(&_updateTimer, &QTimer::timeout, this, &Registration::onAddLocalAndSatteliteTime));
    ASSERT(connect(&_recordSpeedTimer, &QTimer::timeout, this, &Registration::recordSpeed));
#endif

    _modesElapsedTimer->start();
    core.testSensLevels();

#if defined TARGET_AVICON31 || defined TARGET_AVICON15
    satelliteAntennaStatusChanged(_geoPosition->antennaStatus());
#endif

    recordSpeed();

    _isTemp = false;

    core.disableAll(false);
    qDebug() << QString("Info| Registration started.");
}

void Registration::start()
{
    start("Unknown", "Unknown", "QString()", Core::instance().getTrackMarks(), false, false, 1, false, false, TEMP_FILENAME);
    _isTemp = true;
}

void Registration::stop(bool exit)
{
    if (_dataContainer != nullptr) {
        Core& core = Core::instance();
        Core::instance().videoRegistrationStop();
        disconnect(&core, &Core::doStartBoltJoint, this, &Registration::onStartBoltJoint);
        disconnect(&core, &Core::doStopBoltJoint, this, &Registration::onStopBoltJoint);

        _updateTimer.stop();
        _recordSpeedTimer.stop();
        disconnect(&core, &Core::externalTemperatureChanged, this, &Registration::onTemperatureChanged);
#if defined TARGET_AVICON31 || defined TARGET_AVICON15

        GeoPosition* geoPosition = _geoPosition.data();
        if (geoPosition != nullptr) {
            disconnect(geoPosition, &GeoPosition::positionUpdated, this, &Registration::satellitePositionUpdated);
            disconnect(geoPosition, &GeoPosition::antennaStatusChanged, this, &Registration::satelliteAntennaStatusChanged);
        }

#elif defined TARGET_AVICONDB
        disconnect(&_updateTimer, &QTimer::timeout, this, &Registration::onAddLocalTime);
        onAddLocalTime();
#else
        onAddLocalTime();
#endif

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E || defined TARGET_AVICON15
        disconnect(&_updateTimer, &QTimer::timeout, this, &Registration::onAddLocalAndSatteliteTime);
        onAddLocalAndSatteliteTime();
#endif

        _latitude = qSNaN();
        _longitude = qSNaN();
        _lastSpeed = 0;

        const QString& fileName = _dataContainer->GetFileName();
        _dataContainer->CloseFile();
        delete _dataContainer;
        _dataContainer = nullptr;

        if (_isTemp) {
            tempRegistrationCleanup();
        }
        else {
            emit doStopRegistration(fileName);
        }
        qDebug() << QString("Info| Registration stopped.");
    }

    _isPause = true;
    _isCloseHeader = false;
    if (!exit) {
        if (!_isTemp) {
            start();
        }
        else {
            _isTemp = false;
        }
    }
}

void Registration::addDefectLabel(int length, int depth, int width, const QString& defectCode, const QString& comment, const QString& link, DeviceSide side)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }

    QString label(defectCode);
    label += " H:" + QString::number(depth) + " L:" + QString::number(length) + " X:" + QString::number(width);
    if (!link.isEmpty()) {
        label += " (" + link + ")";
    }
    if (!comment.isEmpty()) {
        label += " [" + comment + "]";
    }

    qDebug() << "Defect label - " << label;
    _dataContainer->AddDefLabel(static_cast<eDeviceSide>(side), label);
}

void Registration::addTextLabel(const QString& label, DeviceSide side)
{
    Q_UNUSED(side);
    if (_dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding text label:" << label;
    _dataContainer->AddTextLabel(label);
}

void Registration::addRecalibrationLabel()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding recalibration label";
    _dataContainer->AddNeedRecalibrationLabel();
}

void Registration::addDefectMarker(eDeviceSide side, CID channel, int gateIndex, int centrCoord, int packetWidth, unsigned char stDelay, unsigned char edDelay)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddAutomaticSearchRes(side, channel, gateIndex, centrCoord, packetWidth, stDelay, edDelay);
}

void Registration::setMrfPost(const TMRussian* trackMarks)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding MRF post";
    tMRFPost Post;
    Post.Km[0] = trackMarks->getPostKm(0);
    Post.Km[1] = trackMarks->getPostKm(1);
    Post.Pk[0] = trackMarks->getPostPk(0);
    Post.Pk[1] = trackMarks->getPostPk(1);
    _dataContainer->AddMRFPost(Post);
}

void Registration::setCaPost(const TMMetric1KM* trackMarks)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding Ca Post";
    tCaCrd crd;
    crd.XXX = trackMarks->getKm();
    crd.YYY = trackMarks->getM();
    _dataContainer->AddCaPost(crd);
}

void Registration::addChangingSensBySpeed(int difference)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddChangeSensBySpeed(static_cast<char>(difference), static_cast<char>(_lastSpeed));
}

void Registration::addMenuMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMode(menuMode);
}

void Registration::addHandScanMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    // addMode(handMode);
}

void Registration::addHeadScanMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMode(headScannerMode);
}

void Registration::addBScanMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMode(bScanMode);
}

void Registration::addEvaluationMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    // addMode(evaluationMode);
}

void Registration::addMScanMode()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMode(mScanMode);
}

void Registration::addStartSwitch()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->SetStartSwitchShunter();
}

void Registration::addEndSwitch()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->SetEndSwitchShunter();
}

void Registration::addVideo(const QString& filename)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMedia(filename, mtVideo);
}

void Registration::addAudioComment(const QString& filename)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMedia(filename, mtAudio);
}

void Registration::addImage(const QString& filename)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    addMedia(filename, mtPhoto);
}

void Registration::addAScanScreen(DeviceSide side, CID channelId, int gateIndex, int gain, int sens, float tvg, float timeDelay, int surface, const QPixmap& pixmap, int startGate, int endGate)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding AScanScreen";
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "png");
    buffer.close();
    buffer.open(QIODevice::ReadOnly);

    sGate alarmGate;
    alarmGate.gStart = startGate;
    alarmGate.gEnd = endGate;

    _dataContainer->AddAScanFrame(static_cast<eDeviceSide>(side), surface, channelId, gateIndex, gain, sens, tvg, timeDelay, alarmGate, buffer.readAll().data(), bytes.size());
    buffer.close();
}

void Registration::changeOperator(const QString& operatorName)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    qDebug() << "Adding change operator:" << operatorName;
    _dataContainer->ChangeOperatorName(operatorName);
}

void Registration::setCurrentChannelType(eChannelType channelType)
{
    _lastChannelType = channelType;
}

void Registration::plugTheBscanData(bool isPlug)
{
    disconnect(_defcore, &Defcore::doBScan2Data, this, &Registration::onBScan2Data);
    if (isPlug) {
        ASSERT(connect(_defcore, &Defcore::doBScan2Data, this, &Registration::onBScan2Data, Qt::UniqueConnection));
    }
}

void Registration::addTestSensLevelsForChannel(eDeviceSide side, CID channel, int gateIndex, int value)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddSensFault(side, channel, gateIndex, value);
}

void Registration::addSensAllowedRanges(const tSensValidRangesList& ranges)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddSensAllowedRanges(ranges);
}

void Registration::addOperatorRemindLabel(int rail, int type, const QString& label, int length, int depth)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddOperatorRemindLabel((rail == 1) ? (dsRight) : (dsLeft), static_cast<eLabelType>(type), length, depth, label);
}

int Registration::GetMaxDisCoord()
{
    if (_isPause || _dataContainer == nullptr) {
        return 0;
    }
    return _dataContainer->GetMaxDisCoord();
}

int Registration::GetMaxFileCoord()
{
    if (_dataContainer == nullptr) {
        return 0;
    }
    return _dataContainer->GetMaxFileDisCoord();
}

int Registration::GetCurSysCoord()
{
    if (_isPause || _dataContainer == nullptr) {
        return 0;
    }
    return _dataContainer->GetCurSysCoord();
}

bool Registration::waitForAllProcessing()
{
    if (_dataContainer != nullptr) {
        return _dataContainer->waitForRegArProcessing();
    }
    return false;
}

void Registration::tempRegistrationCleanup()
{
    qDebug() << "Temp registration cleanup";
    QFile::remove(TEMP_FILENAME);
    sync();
}

void Registration::addMode(ModeSetted mode)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    unsigned short time = _modesElapsedTimer->restart() / 1000;

    if (_isCloseHeader) {
        _dataContainer->AddMode(mode, time);
    }
}

void Registration::addMode(ModeSetted mode, bool isAddedChannelInfo, eDeviceSide side, CID channel, int gateIndex)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    unsigned short time = _modesElapsedTimer->restart() / 1000;

    if (_isCloseHeader) {
        _dataContainer->AddMode(mode, time, isAddedChannelInfo, side, channel, gateIndex);
    }
}

void Registration::recordSpeed()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _lastSpeed >= _maxSpeed ? _dataContainer->AddSpeedState(_lastSpeed, ssOverSpeed) : _dataContainer->AddSpeedState(_lastSpeed, ssNormalSpeed);
}

void Registration::onStartHandScanRegistration()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
#if defined TARGET_AVICONDBHS
    setCurrentChannelType(ctHandScan);
#endif
    _dataContainer->StartHandScanDataSave();
    plugTheBscanData(true);
}

void Registration::onStopHandScanRegistration(DeviceSide side, int surface, CID channel, int gain, int sens, float tvg, float timeDelay)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
#if defined TARGET_AVICONDBHS
    setCurrentChannelType(ctCompInsp);
#endif
    _dataContainer->EndHandScanDataSave(static_cast<eDeviceSide>(side), surface, channel, gain, sens, tvg, timeDelay);
    plugTheBscanData(false);
}

void Registration::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }

    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        const QModelIndex& index = _channelsModel->index(row, 0);
        if (index.data(ChannelTypeRole).toInt() != HandChannel) {
            if (index.isValid()) {
                for (int role : roles) {
                    switch (role) {
                    case TimeDelayRole:
                        _dataContainer->AddPrismDelay(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), _channelsModel->data(index, TimeDelayRole).toReal());
                        break;
                    case SensRole:
                        _dataContainer->AddSens(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(SensRole).toInt());
                        break;
                    case GainRole:
                        _dataContainer->AddGain(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(GainRole).toInt());
                        break;
                    case TvgRole:
                        _dataContainer->AddTVG(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(TvgRole).toInt());
                        break;
                    case StartGateRole:
                        _dataContainer->AddStGate(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(StartGateRole).toInt());
                        break;
                    case EndGateRole:
                        _dataContainer->AddEndGate(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(EndGateRole).toInt());
                        break;
                    case SensLevelsRole:
                        if (_deviceType == Avicon31Default || _deviceType == Avicon15) {
                            addTestSensLevelsForChannel(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(SensLevelsRole).toInt());
                        }
                        break;
                    case IsPlayAlarmToneRole:
                        _dataContainer->AddHeadPh(static_cast<eDeviceSide>(index.data(SideRole).toInt()), index.data(ChannelIdRole).toInt(), index.data(GateIndexRole).toInt(), index.data(IsPlayAlarmToneRole).toBool());
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

void Registration::pause(bool value)
{
    _isPause = value;

    if (_dataContainer == nullptr) {
        return;
    }

    if (_isPause) {
        addMode(pauseMode);
        addTextLabel(tr("Pause"));
        plugTheBscanData(false);
    }
    else {
        plugTheBscanData(true);
    }
}

bool Registration::status()
{
    return _dataContainer != nullptr;
}

void Registration::onBScan2Data(QSharedPointer<tDEV_BScan2Head> head)
{
    Q_ASSERT(head.data() != 0);

    if (_isPause || _dataContainer == nullptr) {
        return;
    }

    if (head.data()->ChannelType != _lastChannelType) {
        return;
    }

    if (Core::instance().getDeviceMode() != dmSearch) {
        return;
    }

    if (head.data()->BScanSessionID != _defcore->GetCurrentBScanSessionID()) {
        return;
    }

    if (head.data()->PathEncodersIndex != 0) {
        return;
    }
    /*
        if ((_dataContainer->GetScanDataSaveMode() == smContinuousScan) && (head.data()->Simulator[0])) {
            return;
        }
    */
    if ((_dataContainer->GetScanDataSaveMode() == smHandScan) && (!head.data()->Simulator[0])) {
        return;
    }

    // qDebug() << head.data()->XDisCrd[0];
    _dataContainer->AddCoord(head.data()->Dir[0], head.data()->XSysCrd[0], head.data()->XDisCrd[0]);


    for (const tDEV_BScan2HeadItem& item : head.data()->Items) {
        Q_ASSERT(item.Side < 3);
        tEchoBlock echoBlock;
        // memset(&echoBlock, 0, sizeof(tEchoBlock));  // TODO: check performance and stability
        for (unsigned int i = 0; i < (item.Signals).size(); ++i) {
            echoBlock[i].Delay = item.Signals[i].Delay;
            echoBlock[i].Ampl = item.Signals[i].Ampl[AmplDBIdx_int] & 0x0f;
        }
        _dataContainer->AddEcho(item.Side, item.Channel, item.Signals.size(), echoBlock);
    }

    if (_isCheckAcousticContact && !head.data()->ACItems.empty()) {
        for (const tDEV_ACHeadItem& item : head.data()->ACItems) {
            _dataContainer->AddAcousticContactState(item.Side, item.Channel, item.ACValue);
        }
    }
}

void Registration::onStartBoltJoint()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddStBoltStyk();
}

void Registration::onStopBoltJoint()
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddEdBoltStyk();
}

void Registration::onRailTypeResult(tCalibrationToRailTypeResult* result)
{
    Q_UNUSED(result);
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddSetRailType();
}

void Registration::satellitePositionUpdated(const QGeoPositionInfo& info)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    if (!info.timestamp().time().isNull()) {
        _timeFromSatellite.setHMS(info.timestamp().time().hour(), info.timestamp().time().minute(), info.timestamp().time().second());
    }
    QGeoCoordinate coordinate = info.coordinate();
    if (coordinate.isValid()) {
        _currentGeoCoord = coordinate;
    }
    if (_lastGeoCoordinate.isValid() == false || _lastGeoCoordinate.distanceTo(info.coordinate()) >= UPDATE_GPS_DISTANCE_IN_METERS) {
        _lastGeoCoordinate = info.coordinate();
        _dataContainer->AddSatelliteCoordAndSpeed(_lastGeoCoordinate.latitude(), _lastGeoCoordinate.longitude(), info.attribute(QGeoPositionInfo::GroundSpeed));
    }
    double latitude = coordinate.latitude();
    double longitude = coordinate.longitude();
    if ((_longitude != longitude) || (_latitude != latitude)) {
        _latitude = latitude;
        _longitude = longitude;
        _dataContainer->AddSatelliteCoord(latitude, longitude);
    }
    _dataContainer->AddSCReceiverStatus(_isGpsAntennaConnect, info.isValid(), _countSatellites);
}

void Registration::satelliteAntennaStatusChanged(GeoPosition::AntennaStatus antennaStatus)
{
    Q_UNUSED(antennaStatus);
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    setAntennaStatus(antennaStatus);
}

void Registration::satelliteInUseChanged(const QList<QGeoSatelliteInfo>& satellites)
{
    if (satellites.count() > 0) {
        _countSatellites = satellites.count();
    }
}

void Registration::setSatelliteInUseCount(int countSatellites)
{
    _countSatellites = countSatellites;
}

void Registration::setAntennaStatus(GeoPosition::AntennaStatus status)
{
    switch (status) {
    case GeoPosition::AntennaStatusConnected:
        _isGpsAntennaConnect = true;
        break;
    case GeoPosition::AntennaStatusDisconnected:
        _isGpsAntennaConnect = false;
        break;
    default:
        break;
    }
}

void Registration::setGeoPositionInfo(const QGeoPositionInfo& info)
{
    satellitePositionUpdated(info);
}

void Registration::modifyRegistrationFileHeader(const QString& operatorName, const QString& pathSectionName, const QString& trackNumber, TrackMarks* trackMarks, bool isReverseSides, bool isDirectionIncrease, int directionCode)
{
    _dataContainer->StartModifyHeader();
    _dataContainer->SetOperatorName(operatorName);
    _dataContainer->SetPathSectionName(pathSectionName);
    _dataContainer->SetRailPathTextNumber(trackNumber);
    switch (Core::instance().getSystemCoordType()) {
    case csMetricRF: {
        tMRFCrd PostRus;
        PostRus.Km = static_cast<TMRussian*>(trackMarks)->getKm();
        PostRus.Pk = static_cast<TMRussian*>(trackMarks)->getPk();
        PostRus.mm = static_cast<TMRussian*>(trackMarks)->getM() * 1000;
        _dataContainer->SetStartMRFCrd(PostRus);
    } break;
    case csMetric1km: {
        tCaCrd PostMetric;
        PostMetric.XXX = static_cast<TMMetric1KM*>(trackMarks)->getKm();
        PostMetric.YYY = static_cast<TMMetric1KM*>(trackMarks)->getM();
        _dataContainer->SetStartCaCrd(Core::instance().getSystemCoordType(), PostMetric);
    } break;
    default:
        qDebug() << "Cannot use coord system!";
        break;
    }

    _dataContainer->SetCorrespondenceSides(isReverseSides ? csReverse : csDirect);
    _dataContainer->SetDirectionCode(directionCode);
    _dataContainer->EndModifyHeader(isDirectionIncrease ? 1 : -1);
}

int Registration::openFile(const QString &fileName)
{
    if(_dataContainer != nullptr){
        return _dataContainer->openFile(fileName);
    }
    else{
        start();
        return _dataContainer->openFile(fileName);
    }
}

void Registration::closeFile()
{
    _dataContainer->closeViewFile();
}

void Registration::distanceCalculate(int &km, int &pk, int &m, int disCoord, int sysCoord, int &direct)
{
    _dataContainer->distanceCalculate(km, pk, m, disCoord, sysCoord, direct);
}

void Registration::changeLastSpeed(float speed)
{
    _lastSpeed = speed;
}

void Registration::delegateSensToModel(int disCoord, std::vector<std::vector<unsigned char> > &kuSens, std::vector<std::vector<unsigned char> > &stStrSens, std::vector<std::vector<unsigned char> > &endStrSens)
{
    _dataContainer->delegateSensToModel(disCoord, kuSens, stStrSens, endStrSens);
}

int Registration::CIDToChannel(int side, int cid)
{
    return _dataContainer->CIDToChannel(side, cid);
}

void Registration::addStartSwitchControlLabel(const QString& number, SwitchType type, SwitchDirectionType directionType, SwitchFixedDirection fixedDirection, DeviceDirection deviceDirection, eDeviceSide side, QString& switchVariantText)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddExtendedStartSwitchLabel(number, type, directionType, fixedDirection, deviceDirection, side, switchVariantText);
}

void Registration::addEndSwitchControlLabel(const QString& number, QString& switchVariantText)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddExtendedEndSwitchLabel(number, switchVariantText);
}

void Registration::onAddLocalAndSatteliteTime()
{
    const QTime& current = QTime::currentTime();
    _dataContainer->AddNewTime(0, current.hour(), current.minute());
    _dataContainer->AddNewTime(1, _timeFromSatellite.hour(), _timeFromSatellite.minute());
}

void Registration::onAddLocalTime()
{
    const QTime& current = QTime::currentTime();
    _dataContainer->AddNewTime(0, current.hour(), current.minute());
}

void Registration::onTemperatureChanged(qreal value)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    if (restoreExternalTemperatureEnabled()) {
        _dataContainer->AddTemperature(0, value);
    }
}

QString Registration::makeRegistrationFileName(bool isService)
{
    if (isService) {
        QDir dir(registrationServicePath());
        QStringList fileList;
        QString pattern = QString("s") + QString::number(restoreSchemeNumber());
        if (dir.exists()) {
            fileList = dir.entryList(QDir::Files);
        }
        for (const QString& fileName : fileList) {
            if (fileName.contains(pattern)) {
                dir.remove(fileName);
            }
        }
        _currentFileName = QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString("-s") + QString::number(restoreSchemeNumber());
        _currentFilePath = registrationServicePath();
        return _currentFilePath + _currentFileName + prefix;
    }

    _currentFileName = QDateTime::currentDateTime().toString("yyMMdd-hhmmss");
    _currentFilePath = registrationsPath();
    return _currentFilePath + _currentFileName + prefix;
}

QString Registration::makeTestRecordFileName()
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
            fileName = fileInfo.absoluteFilePath();
            break;
        }
    }
    return fileName;
}

QDate Registration::getServiceDate()
{
    QDir dir(registrationServicePath());
    if (dir.exists() && dir.entryList(QDir::Files).count() != 0) {
        return dir.entryInfoList().first().created().date();
    }
    return QDate();
}

void Registration::addParamsForAllScanChannels()
{
    const ChannelsController* channelsController = Core::instance().channelsController();
    const QAbstractItemModel* scanChannelsModel = channelsController->scanChannelsModel();

    for (int i = 0; i < scanChannelsModel->rowCount(); ++i) {
        const QModelIndex& index = scanChannelsModel->index(i, 0);

        QVariant value;

        value = scanChannelsModel->data(index, ChannelIdRole);
        Q_ASSERT(value.isValid() == true);
        CID channelId = value.toInt();

        value = scanChannelsModel->data(index, SideRole);
        Q_ASSERT(value.isValid() == true);
        eDeviceSide side = static_cast<eDeviceSide>(value.toInt());

        value = scanChannelsModel->data(index, GateIndexRole);
        Q_ASSERT(value.isValid() == true);
        int gateIndex = value.toInt();

        value = scanChannelsModel->data(index, SensRole);
        Q_ASSERT(value.isValid() == true);
        int sens = value.toInt();

        value = scanChannelsModel->data(index, GainRole);
        Q_ASSERT(value.isValid() == true);
        int gain = value.toInt();

        value = scanChannelsModel->data(index, TvgRole);
        Q_ASSERT(value.isValid() == true);
        int tvg = value.toInt();

        value = scanChannelsModel->data(index, StartGateRole);
        Q_ASSERT(value.isValid() == true);
        int startGate = value.toInt();

        value = scanChannelsModel->data(index, EndGateRole);
        Q_ASSERT(value.isValid() == true);
        int endGate = value.toInt();

        value = scanChannelsModel->data(index, TimeDelayRole);
        Q_ASSERT(value.isValid() == true);
        int prismDelay = value.toInt();

        _dataContainer->AddSens(side, channelId, gateIndex, sens);
        _dataContainer->AddGain(side, channelId, gateIndex, gain);
        _dataContainer->AddTVG(side, channelId, tvg);
        _dataContainer->AddStGate(side, channelId, gateIndex, startGate);
        _dataContainer->AddEndGate(side, channelId, gateIndex, endGate);
        _dataContainer->AddPrismDelay(side, channelId, prismDelay);
    }
}

bool Registration::addMedia(const QString& filename, int type)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QWARNING << "can't open media file: '" << filename << '\'';
        return false;
    }
    _dataContainer->AddMedia(file.readAll().data(), static_cast<tMediaType>(type), file.size());
    qDebug() << Q_FUNC_INFO << "filename:" << filename << "size:" << file.size() << "type:" << type;
    file.close();
    return true;
}

QGeoCoordinate Registration::getCurrentGeoCoord() const
{
    return _currentGeoCoord;
}

void Registration::updateStateAcousticContact()
{
    _isCheckAcousticContact = restoreStateAcousticContactControl();
}

QString Registration::getCurrentFilePath() const
{
    return _currentFilePath;
}

QString Registration::getCurrentFileName() const
{
    return _currentFileName;
}

bool Registration::getIsTemp() const
{
    return _isTemp;
}

void Registration::addQualityCalibrationRec(eDeviceSide Side, const tBadChannelList& BadChannelList, tQualityCalibrationRecType RecType)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    _dataContainer->AddQualityCalibrationRec(Side, BadChannelList, RecType);
}

void Registration::addMinBscanThresholdRec(int value)
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }
    switch (value) {
    case -6:
        _dataContainer->SetBScanTreshold_minus_6dB();
        break;
    case -12:
        _dataContainer->SetBScanTreshold_minus_12dB();
        break;
    }
}

void Registration::AddRailHeadScanerData(eDeviceSide Side,                                  // Side - Нить пути выполнения контроля: левая / правая
                                         char WorkSide,                                     // WorkSide - Рабочая грань: сторона сканера 1 / сторона сканера 2
                                         char ThresholdUnit,                                // ThresholdUnit - Единицы измерения порога формирования результирующего изображения: 1 - Децибеллы; 2 - % (от 0 до 100)
                                         char ThresholdValue,                               // ThresholdValue - Порог формирования результирующего изображения: дБ или % (от 0 до 100)
                                         int OperatingTime,                                 // OperatingTime - Время работы со сканером: сек.
                                         const QString& CodeDefect,                         // Заключение: код дефекта
                                         const QString& Sizes,                              // Заключение: размеры
                                         const QString& Comments,                           // Заключение: комментарии
                                         void* Cross_Cross_Section_Image_Ptr,               // Поперечное сечение - указатель на изображение в формате PNG
                                         unsigned int Cross_Cross_Section_Image_Size,       // Поперечное сечение - размер изображения в формате PNG
                                         void* Vertically_Cross_Section_Image_Ptr,          // Вертикальное сечение - указатель на изображение в формате PNG
                                         unsigned int Vertically_Cross_Section_Image_Size,  // Вертикальное сечение - размер изображения в формате PNG
                                         void* Horizontal_Cross_Section_Image_Ptr,          // Горизонтальное сечение - указатель на изображение в формате PNG
                                         unsigned int Horizontal_Cross_Section_Image_Size,  // Горизонтальное сечение - размер изображения в формате PNG
                                         void* Zero_Probe_Section_Image_Ptr,                // Донный сигнал - указатель на изображение в формате PNG
                                         unsigned int Zero_Probe_Section_Image_Size)        // Донный сигнал - размер изображения в формате PNG
{
    if (_isPause || _dataContainer == nullptr) {
        return;
    }

    _dataContainer->AddRailHeadScanerData(Side,                                 // Side - Нить пути выполнения контроля: левая / правая
                                          WorkSide,                             // WorkSide - Рабочая грань: сторона сканера 1 / сторона сканера 2
                                          ThresholdUnit,                        // ThresholdUnit - Единицы измерения порога формирования результирующего изображения: 1 - Децибеллы; 2 - % (от 0 до 100)
                                          ThresholdValue,                       // ThresholdValue - Порог формирования результирующего изображения: дБ или % (от 0 до 100)
                                          OperatingTime,                        // OperatingTime - Время работы со сканером: сек.
                                          CodeDefect,                           // Заключение: код дефекта
                                          Sizes,                                // Заключение: размеры
                                          Comments,                             // Заключение: комментарии
                                          Cross_Cross_Section_Image_Ptr,        // Поперечное сечение - указатель на изображение в формате PNG
                                          Cross_Cross_Section_Image_Size,       // Поперечное сечение - размер изображения в формате PNG
                                          Vertically_Cross_Section_Image_Ptr,   // Вертикальное сечение - указатель на изображение в формате PNG
                                          Vertically_Cross_Section_Image_Size,  // Вертикальное сечение - размер изображения в формате PNG
                                          Horizontal_Cross_Section_Image_Ptr,   // Горизонтальное сечение - указатель на изображение в формате PNG
                                          Horizontal_Cross_Section_Image_Size,  // Горизонтальное сечение - размер изображения в формате PNG
                                          Zero_Probe_Section_Image_Ptr,         // Донный сигнал - указатель на изображение в формате PNG
                                          Zero_Probe_Section_Image_Size);       // Донный сигнал - размер изображения в формате PNG
}
