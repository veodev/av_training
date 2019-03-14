#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>

#include "coredefinitions.h"

class QTcpServer;
class QTcpSocket;
class QGeoPositionInfo;

class RemoteControl : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl(QObject *parent = nullptr);
    void start();

signals:
    void doRemoteControlConnected();
    void doRemoteControlDisconnected();
    void doRemoteControlPingFail();
    void doStartRegistration();
    void doStopRegistration();
    void doUpdateRemoteState();
    void doCurrentTrackMark(int km, int pk);
    void doSatellitesInUse(int countSatellites);
    void doSatellitesInfo(const QGeoPositionInfo& info);
    void doTextLabel(QString& textlabel);
    void doStartSwitchLabel();
    void doEndSwitchLabel();

public slots:
    void sendMeter(int m);
    void updateRemoteState(bool isRegOn, ViewCoordinate viewType, Direction direction, int km, int pk, int m);
    void updateTrackMarks(int km, int pk, int m);
    void updateRemoteMarks();
    void changeSpeed(double value);
    void listen();

private slots:
    void onNewConnection();
    void onCloseConnection();
    void onReadyRead();
    void onPingSendTimerTimeout();
    void onWatchdogTimeout();

private:    
    void sendStringList(RemoteControlHeaders header, QStringList& list);
    void sendMessage(QByteArray& message);
    void readMessageFromBuffer();

private:
    QTcpServer* _tcpServer;
    QTcpSocket* _tcpSocket;
    QStringList _bridgesList;
    QStringList _platformsList;
    QStringList _miscList;
    QByteArray _messagesBuffer;
    bool _isRegistrationStarted;
    QTimer* _pingTimer;
    QTimer* _watchdog;
};

#endif // REMOTECONTROL_H
