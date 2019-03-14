#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QObject>
#include <QGeoPositionInfo>
#include <QPointer>
#include <QAbstractItemModel>
#include <QTimer>
#include <QElapsedTimer>


#include "modeltypes.h"
#include "trackmarks.h"
#include "tmrussian.h"
#include "tmmetric1km.h"
#include "dc_definitions.h"
#include "coredefinitions.h"
#include "geoposition.h"
#include "DataContainer.h"

#define TEMP_FILENAME "tempfile.tmp"

class cDataContainer;
class TrackMarks;
class Defcore;
class GeoPosition;
class tDEV_AScanMeasure;
class tDEV_AScanHead;
class tDEV_AlarmHead;
class tDEV_BScan2Head;
class tDEV_PathStepData;

class Registration : public QObject
{
    Q_OBJECT
public:
    explicit Registration(QObject* parent = nullptr);
    ~Registration();
    void setDefcore(Defcore* defcore);
    void setModel(QAbstractItemModel* channelsModel);
    void start(const QString& operatorName, const QString& pathSectionName, const QString& trackNumber, TrackMarks* trackMarks, bool isReverseSides, bool isDirectionIncrease, int directionCode, bool isService, bool controledRail, const QString& fileName);
    void start();
    void stop(bool exit);
    void pause(bool value);
    bool status();
    void addDefectLabel(int length, int depth, int width, const QString& defectCode, const QString& comment, const QString& link, DeviceSide side);
    void addTextLabel(const QString& label, DeviceSide side = NoneDeviceSide);
    void addRecalibrationLabel();
    void addDefectMarker(eDeviceSide side, CID channel, int gateIndex, int centrCoord, int packetWidth, unsigned char stDelay, unsigned char edDelay);
    void setMrfPost(const TMRussian* trackMarks);
    void setCaPost(const TMMetric1KM* trackMarks);
    void addChangingSensBySpeed(int difference);
    void addMenuMode();
    void addHandScanMode();
    void addHeadScanMode();
    void addBScanMode();
    void addEvaluationMode();
    void addMScanMode();
    void addStartSwitch();
    void addEndSwitch();
    void addMinBscanThresholdRec(int value);
    void addVideo(const QString& filename);
    void addAudioComment(const QString& filename);
    void addImage(const QString& filename);
    void addAScanScreen(DeviceSide side, CID channelId, int gateIndex, int gain, int sens, float tvg, float timeDelay, int surface, const QPixmap& pixmap, int startGate, int endGate);
    void addQualityCalibrationRec(eDeviceSide Side, const tBadChannelList& BadChannelList, tQualityCalibrationRecType RecType);
    void changeOperator(const QString& operatorName);
    void setCurrentChannelType(eChannelType channelType);
    void plugTheBscanData(bool isPlug);
    void addTestSensLevelsForChannel(eDeviceSide side, CID channel, int gateIndex, int value);
    void addSensAllowedRanges(const tSensValidRangesList& ranges);
    void addMode(ModeSetted mode);
    void addMode(ModeSetted mode, bool isAddedChannelInfo, eDeviceSide side, CID channel, int gateIndex);
    void addOperatorRemindLabel(int rail, int type, const QString& label, int length, int depth);
    void AddRailHeadScanerData(eDeviceSide Side,
                               char WorkSide,
                               char ThresholdUnit,
                               char ThresholdValue,
                               int OperatingTime,
                               const QString& CodeDefect,
                               const QString& Sizes,
                               const QString& Comments,
                               void* Cross_Cross_Section_Image_Ptr,
                               unsigned int Cross_Cross_Section_Image_Size,
                               void* Vertically_Cross_Section_Image_Ptr,
                               unsigned int Vertically_Cross_Section_Image_Size,
                               void* Horizontal_Cross_Section_Image_Ptr,
                               unsigned int Horizontal_Cross_Section_Image_Size,
                               void* Zero_Probe_Section_Image_Ptr,           // Донный сигнал - указатель на изображение в формате PNG
                               unsigned int Zero_Probe_Section_Image_Size);  // Донный сигнал - размер изображения в формате PNG

    int GetMaxDisCoord(void);
    int GetMaxFileCoord();
    int GetCurSysCoord(void);
    bool waitForAllProcessing();

    inline bool getSpan(int StartDisCrd, int EndDisCrd, BScanDisplaySpan& span)
    {
        if (_dataContainer != nullptr) {
            return _dataContainer->getSpan(StartDisCrd, EndDisCrd, span);
        }
        else {
            return false;
        }
    }

    inline bool getEventSpan(eventSpan& span)
    {
        if (_dataContainer != nullptr) {
            return _dataContainer->getEventSpan(span);
        }
        else {
            return false;
        }
    }

    void tempRegistrationCleanup();
    bool getIsTemp() const;

    QString getCurrentFileName() const;
    QString getCurrentFilePath() const;

    QGeoCoordinate getCurrentGeoCoord() const;

    void updateStateAcousticContact();

    void setSatelliteInUseCount(int countSatellites);
    void setAntennaStatus(GeoPosition::AntennaStatus status);
    void setGeoPositionInfo(const QGeoPositionInfo& info);
    void modifyRegistrationFileHeader(const QString& operatorName, const QString& pathSectionName, const QString& trackNumber, TrackMarks* trackMarks, bool isReverseSides, bool isDirectionIncrease, int directionCode);
    int openFile(const QString& fileName);
    void closeFile();
    void distanceCalculate(int &km, int &pk, int &m, int disCoord, int sysCoord, int& direct);
    void changeLastSpeed(float speed);
    void delegateSensToModel(int disCoord
                             , std::vector<std::vector<unsigned char>> &kuSens
                             , std::vector<std::vector<unsigned char>> &stStrSens
                             , std::vector<std::vector<unsigned char>> &endStrSens);
    int CIDToChannel(int side, int cid);
    void addStartSwitchControlLabel(const QString& number, SwitchType type, SwitchDirectionType directionType, SwitchFixedDirection fixedDirection, DeviceDirection deviceDirection, eDeviceSide side, QString& switchVariantText);
    void addEndSwitchControlLabel(const QString& number, QString& switchVariantText);

signals:
    void doStopRegistration(QString fileName);

public slots:
    void recordSpeed();
    void onStartHandScanRegistration();
    void onStopHandScanRegistration(DeviceSide side, int surface, CID channel, int gain, int sens, float tvg, float timeDelay);
    void onAddLocalAndSatteliteTime();
    void onAddLocalTime();

private slots:
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onBScan2Data(QSharedPointer<tDEV_BScan2Head> head);
    void onStartBoltJoint();
    void onStopBoltJoint();
    void onRailTypeResult(tCalibrationToRailTypeResult* result);
    void satellitePositionUpdated(const QGeoPositionInfo& info);
    void satelliteAntennaStatusChanged(GeoPosition::AntennaStatus antennaStatus);
    void satelliteInUseChanged(const QList<QGeoSatelliteInfo>& satellites);
    void onTemperatureChanged(qreal value);

private:
    QString makeRegistrationFileName(bool isService = false);
    QString makeTestRecordFileName();
    QDate getServiceDate();
    void addParamsForAllScanChannels();
    bool addMedia(const QString& filename, int type);

private:
    cDataContainer* _dataContainer;
    Defcore* _defcore;
    QAbstractItemModel* _channelsModel;
    bool _isPause;
    bool _isCheckAcousticContact;
    bool _isCloseHeader;
    bool _isTemp;
    bool _regarStatus;

#if defined TARGET_AVICON31 || defined TARGET_AVICON15
    QPointer<GeoPosition> _geoPosition;
#endif

    TrackMarks* _tempTrackMarks;
    double _latitude;
    double _longitude;
    QTime _timeFromSatellite;
    QTimer _updateTimer;
    QTimer _recordSpeedTimer;
    QElapsedTimer* _modesElapsedTimer;
    double _lastSpeed;
    double _maxSpeed;
    QGeoCoordinate _lastGeoCoordinate;
    QGeoCoordinate _currentGeoCoord;

    eChannelType _lastChannelType;
    bool _isGpsAntennaConnect;
    int _countSatellites;
    QString _currentFileName;
    QString _currentFilePath;
    DeviceType _deviceType;
    eventSpan _eventSpan;
};

#endif  // REGISTRATION_H
