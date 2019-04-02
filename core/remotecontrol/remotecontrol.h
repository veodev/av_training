#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include "enums.h"

class RemoteControl : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl(QObject* parent = nullptr);
    void start();
    void listen();

    void registrationOn(QString operatorName, QString railroadPathName, int pathNumber, TrainingEnums::Direction direction, int km, int pk, int m);
    void registrationOff();
    void setCurrentTrackMarks(int km, int pk);
    void setRailroadSwitch(int number);
    void setDefect(QString defectCode, TrainingEnums::RailroadSide side);
    void boltJointOn();
    void boltJointOff();
    void setCduMode(TrainingEnums::CduMode mode);
    void netSettingsChanged();

    QByteArray convertQStringToUtf16ByteArray(QString str);

signals:
    void doRemoteControlConnected();
    void doRemoteControlDisconnected();
    void doRemoteControlPingFail();
    void doTrackMarks();

    void doRcConnected();
    void doRcDisconnected();
    void doTrainingPcConnected();
    void doTrainingPcDisconnected();

    void doBoltJointButtonPressed();
    void doBoltJointButtonReleased();
    void doTrackMarksButtonReleased();
    void doServiceMarksButtonReleased();

private:
    void rcTcpServerNewConnection();
    void disconnectRcTcpSocket();
    void rcTcpSocketReadyRead();
    void rcTcpSocketDisconnected();

    void connectTrainingPc();
    void disconnectTrainingPc();

    void trainingPcTcpSocketStateChanged(QAbstractSocket::SocketState state);
    void trainingPcTcpSocketReadyRead();

    void rcWatchdogTimeout();
    void rcPingTimerTimeout();
    void trainingPcWatchdogTimeout();
    void trainingPcPingTimerTimeout();

    void sendMessageToTrainingPc(TrainingEnums::MessageId messageId, QByteArray data = QByteArray());
    void sendMessageToRc(TrainingEnums::MessageId messageId, QByteArray data = QByteArray());

    void parseTrainingPcMessages();
    void parseRcMessages();
    void changeCduMode(TrainingEnums::CduMode mode);

private:
    QTcpServer* _rcTcpServer;
    QTcpSocket* _rcTcpSocket;
    QTcpSocket* _trainingPcTcpSocket;

    QByteArray _rcMessagesBuffer;
    QByteArray _trainingPcMessagesBuffer;

    QTimer* _rcPingTimer;
    QTimer* _trainingPcPingTimer;

    QTimer* _rcWatchdog;
    QTimer* _trainingPcWatchdog;

    QString _trainingPcIpAddress;
    quint16 _trainingPcPort;
    quint16 _rcPort;
};

#endif  // REMOTECONTROL_H
