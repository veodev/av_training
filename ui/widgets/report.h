#ifndef REPORT_H
#define REPORT_H

#include <QWidget>
#include <QTime>
#include <QMap>

namespace Ui
{
class Report;
}

class Report : public QWidget
{
    struct CoordinateMarker
    {
        CoordinateMarker()
            : km(0)
            , pk(0)
            , m(0)
            , systemCoordinate(0)
        {
        }
        int km;
        int pk;
        int m;
        int systemCoordinate;
        bool operator==(const CoordinateMarker& marker)
        {
            return (km == marker.km && pk == marker.pk && m == marker.m && systemCoordinate == marker.systemCoordinate);
        }
        CoordinateMarker& operator=(const CoordinateMarker& marker)
        {
            km = marker.km;
            pk = marker.pk;
            m = marker.m;
            systemCoordinate = marker.systemCoordinate;
            return *this;
        }
    };

    struct AcousticContactArea
    {
        AcousticContactArea()
            : startSysCoordinate(0)
            , stopSysCoordinate(0)
            , isBackWard(false)
            , isComplete(false)
        {
        }
        int startSysCoordinate;
        int stopSysCoordinate;
        bool isBackWard;
        bool isComplete;

        void operator=(const AcousticContactArea& other)
        {
            startSysCoordinate = other.startSysCoordinate;
            stopSysCoordinate = other.stopSysCoordinate;
            isBackWard = other.isBackWard;
            isComplete = other.isComplete;
        }
    };

    Q_OBJECT

public:
    explicit Report(QWidget* parent = 0);
    ~Report();

    void makeReport(const QString& fileName);

protected:
    void mousePressEvent(QMouseEvent* e);
    bool event(QEvent* e);

private:
    double meanSpeed();
    QTime travelTime();
    double percentOfSatisfactoryBoltJoints();
    double percentOfDistanceWithOverSpeed();
    double percentOfDistanceWithWrongSens();
    double percentOfBadAcousticContact();

    void calculateRemainingMeters();
    int calculateDistanceInMeters();
    void resetVariables();
    void clearReportWidget();
    void checkAcousticContact();
    void checkSystemCoordinate(int prevCoord, int nextCoord);
    void completeZone();

    int keyFromChannelIdAndRail(int channelId, int rail);
    int calculateFullAcousticContactArea();

    void finalizeBadKyZone();

signals:
    void visible(bool isVisible);

private:
    Ui::Report* ui;

    double _scanStep;
    int _direction;

    int _forwardSystemCoordinate;

    CoordinateMarker _startCoordinateMarker;
    CoordinateMarker _currentCoordinateMarker;
    int _startSystemCoordinate;
    int _currentSystemCoordinate;

    QDate _startDate;
    QTime _startTime;
    QTime _currentTime;

    int _startOverSpeedSystemCoordinate;
    int _stopOverSpeedSystemCoordinate;
    QVector<quint32> _vectorOfSpeedValues;
    double _maxSpeed;
    int _overSpeedDistance;

    int _countBoltJoints;
    int _countOfGoodBoltJoints;

    QString _lineName;

    QString _firstOperator;
    QString _secondOperator;
    QString _thirdOperator;

    int _countChannelsWithWrongSens;
    int _distanceWithWrongSens;
    int _startWrongSensSystemCoordinate;
    int _stopWrongSensSystemCoordinate;

    bool _isBadKy;
    bool _isBackWard;
    bool _acousticContactState;
    bool _isZoneForwardStart;
    bool _isZoneBackwardStart;

    QVector<AcousticContactArea> _vectorAcSections;
    QMap<int, int> _listOfBadAcChannels;

    QMap<int, int> _listOfBadKyChannels;

    int _fullDistanceWithoutBackZone;
    int _fullDistanceWithBackZone;

    bool _isAcousticContactReg;
    bool _isNeedRecalibrate;
};

#endif  // REPORT_H
