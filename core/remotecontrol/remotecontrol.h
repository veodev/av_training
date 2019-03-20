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

signals:
    void doRemoteControlConnected();
    void doRemoteControlDisconnected();
    void doRemoteControlPingFail();
    void doTrackMarks();

    void doRcConnected();
    void doRcDisconnected();
    void doTrainingPcConnected();
    void doTrainingPcDisconnected();

private:
    void rcTcpServerNewConnection();
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
