#ifndef CHECKCALIBRATION
#define CHECKCALIBRATION

#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

#include "modeltypes.h"
#include "registration.h"
#include "Definitions.h"
#include "ChannelsIds.h"
#include "coredefinitions.h"

#define WAIT_BUFFERS \
    2  // Два буфера для сохранения поступающих сигналов
       // во время ожидания продолжения пакета

#define MAX_ATTEMTS 3  // Число попыток обнаружить продолжение пакета
#define MAX_CHEKED_CHANNEL_NUM B42E
#define RAILS 2
#define CHNLS MAX_CHEKED_CHANNEL_NUM + 1

class Defcore;
class Core;

class CheckCalibration : public QObject
{
    Q_OBJECT
public:
    explicit CheckCalibration(QObject* parent = nullptr);
    ~CheckCalibration();

    void setDefcore(Core* core, Defcore* defcore);
    bool getCheckCalibrationResult(unsigned char rail, CID channel);
    void cidToNameString(CID channel, QString* name);
    tBadChannelList getBadChannelList(eDeviceSide side);
    void setEvalCalibrMinZTMSpacingSize(int value);
    void setMinKdEchoPrm(int value);

signals:
    void doLeftSideWrongCalibration();
    void doRightSideWrongCalibration();
    void doLeftSideNo70Signal();
    void doRightSideNo70Signal();

private slots:
    void onDeviceSignalsPacket(QSharedPointer<tDEV_Packet>);
    void onStartBoltJoint();
    void onStopBoltJoint();
    void updateChannelsMap();

private:
    void readParams();
    void packetProcessInit();
    void signalBreaksProcessInit();
    void checkPacketQuality(tDEV_Packet* packet);
    void checkResult();

private:
    Defcore* _defcore;
    QAbstractItemModel* _channelsModel;
    bool _boltJointActive;
    int _sensitivity[RAILS][CHNLS];

    unsigned char _goodPacketCount[RAILS][CHNLS];
    int _kd[RAILS][CHNLS];
    unsigned int _signalGoodBreaksCnt[RAILS];
    bool _checkCalibrationResult[RAILS][CHNLS];
    tBadChannelList _leftSideBadChannelList, _rightSideBadChannelList;
    unsigned int _minSignalBreaksInterval;
    int _minKdEchoPrm;
    bool _isEnabledCheckCalibration;
    QMap<CID, QString> _channelsMap;
};

#endif  // CHECKCALIBRATION
