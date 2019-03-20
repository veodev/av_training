#include <QTcpServer>
#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QGeoPositionInfo>
#include <QtEndian>

#include "remotecontrol.h"
#include "settings.h"

const int PING_INTERVAL_MS = 500;
const int WATCHDOG_INTERVAL_MS = 3000;

RemoteControl::RemoteControl(QObject* parent)
    : QObject(parent)
    , _rcTcpServer(nullptr)
    , _rcTcpSocket(nullptr)
    , _trainingPcTcpSocket(nullptr)
    , _rcPingTimer(new QTimer(this))
    , _trainingPcPingTimer(new QTimer(this))
    , _rcWatchdog(new QTimer(this))
    , _trainingPcWatchdog(new QTimer(this))
{
    _rcPort = restoreRcTcpServerPort();
    _trainingPcIpAddress = restoreTrainingPcTcpServerIpAddress();
    _trainingPcPort = restoreTrainingPcTcpServerPort();

    connect(_rcWatchdog, &QTimer::timeout, this, &RemoteControl::rcWatchdogTimeout);
    _rcWatchdog->setInterval(WATCHDOG_INTERVAL_MS);

    connect(_rcPingTimer, &QTimer::timeout, this, &RemoteControl::rcPingTimerTimeout);
    _rcPingTimer->setInterval(PING_INTERVAL_MS);

    connect(_trainingPcWatchdog, &QTimer::timeout, this, &RemoteControl::trainingPcWatchdogTimeout);
    _trainingPcWatchdog->setInterval(WATCHDOG_INTERVAL_MS);

    connect(_trainingPcPingTimer, &QTimer::timeout, this, &RemoteControl::trainingPcPingTimerTimeout);
    _trainingPcPingTimer->setInterval(PING_INTERVAL_MS);
}

void RemoteControl::start()
{
    _rcTcpServer = new QTcpServer(this);
    connect(_rcTcpServer, &QTcpServer::newConnection, this, &RemoteControl::rcTcpServerNewConnection);
}

void RemoteControl::listen()
{
    qDebug() << "Remote control server started: " << _rcTcpServer->listen(QHostAddress::Any, _rcPort);
    connectTrainingPc();
}

void RemoteControl::rcTcpServerNewConnection()
{
    if (_rcTcpSocket != nullptr) {
        disconnect(_rcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::rcTcpSocketReadyRead);
        disconnect(_rcTcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::rcTcpSocketDisconnected);
        _rcTcpSocket->disconnectFromHost();
        _rcTcpSocket->deleteLater();
        _rcTcpSocket = nullptr;
    }
    _rcTcpSocket = _rcTcpServer->nextPendingConnection();
    connect(_rcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::rcTcpSocketReadyRead);
    connect(_rcTcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::rcTcpSocketDisconnected);
    emit doRcConnected();
    _rcPingTimer->start();
}

void RemoteControl::rcTcpSocketDisconnected()
{
    _rcPingTimer->stop();
    _rcTcpSocket->deleteLater();
    _rcTcpSocket = nullptr;
    emit doRcDisconnected();
}

void RemoteControl::connectTrainingPc()
{
    if (_trainingPcTcpSocket == nullptr) {
        _trainingPcTcpSocket = new QTcpSocket(this);
        connect(_trainingPcTcpSocket, &QTcpSocket::stateChanged, this, &RemoteControl::trainingPcTcpSocketStateChanged);
        connect(_trainingPcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::trainingPcTcpSocketReadyRead);
        _trainingPcTcpSocket->connectToHost(_trainingPcIpAddress, _trainingPcPort);
    }
}

void RemoteControl::disconnectTrainingPc()
{
    if (_trainingPcTcpSocket != nullptr) {
        _trainingPcTcpSocket->disconnectFromHost();
        disconnect(_trainingPcTcpSocket, &QTcpSocket::stateChanged, this, &RemoteControl::trainingPcTcpSocketStateChanged);
        disconnect(_trainingPcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::trainingPcTcpSocketReadyRead);
        _trainingPcTcpSocket->deleteLater();
        _trainingPcTcpSocket = nullptr;
    }
}

void RemoteControl::trainingPcTcpSocketStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        emit doRcDisconnected();
        disconnectTrainingPc();
        QTimer::singleShot(3000, this, &RemoteControl::connectTrainingPc);
        break;
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::ConnectedState:
        _trainingPcPingTimer->start();
        emit doTrainingPcConnected();
        break;
    default:
        break;
    }
}

void RemoteControl::trainingPcTcpSocketReadyRead()
{
    _trainingPcMessagesBuffer.append(_trainingPcTcpSocket->readAll());
    parseTrainingPcMessages();
}


void RemoteControl::rcWatchdogTimeout()
{
    _rcPingTimer->stop();
    emit doRcDisconnected();
}

void RemoteControl::rcPingTimerTimeout()
{
    sendMessageToRc(TrainingEnums::MessageId::PingId);
}

void RemoteControl::trainingPcWatchdogTimeout()
{
    _trainingPcPingTimer->stop();
    emit doTrainingPcDisconnected();
}

void RemoteControl::trainingPcPingTimerTimeout()
{
    sendMessageToTrainingPc(TrainingEnums::MessageId::PingId);
}

void RemoteControl::sendMessageToTrainingPc(TrainingEnums::MessageId messageId, QByteArray data)
{
    if (_trainingPcTcpSocket != nullptr) {
        TrainingEnums::MessageHeader header;
        header.Id = static_cast<unsigned char>(messageId);
        header.Reserved1 = 0;
        header.Size = static_cast<ushort>(data.size());
        _trainingPcTcpSocket->write(reinterpret_cast<char*>(&header), sizeof(header));
        if (!data.isEmpty()) {
            _trainingPcTcpSocket->write(data);
        }
        _trainingPcTcpSocket->flush();
    }
}

void RemoteControl::sendMessageToRc(TrainingEnums::MessageId messageId, QByteArray data)
{
    if (_rcTcpSocket != nullptr) {
        TrainingEnums::MessageHeader header;
        header.Id = static_cast<unsigned char>(messageId);
        header.Reserved1 = 0;
        header.Size = static_cast<ushort>(data.size());
        _rcTcpSocket->write(reinterpret_cast<char*>(&header), sizeof(header));
        if (!data.isEmpty()) {
            _rcTcpSocket->write(data);
        }
        _rcTcpSocket->flush();
    }
}

void RemoteControl::parseTrainingPcMessages()
{
    TrainingEnums::MessageHeader header;
    while (true) {
        if (_trainingPcMessagesBuffer.size() >= static_cast<int>(sizeof(header))) {
            header.Id = static_cast<uchar>(_trainingPcMessagesBuffer.at(0));
            header.Size = qFromLittleEndian<ushort>(reinterpret_cast<const uchar*>(_trainingPcMessagesBuffer.mid(2, sizeof(ushort)).data()));
            _trainingPcMessagesBuffer.remove(0, sizeof(header));
            if (_trainingPcMessagesBuffer.size() >= header.Size) {
                switch (static_cast<TrainingEnums::MessageId>(header.Id)) {
                case TrainingEnums::MessageId::PingId:
                    _trainingPcWatchdog->start();
                    break;
                default:
                    break;
                }
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
}

void RemoteControl::parseRcMessages()
{
    TrainingEnums::MessageHeader header;
    while (true) {
        if (_rcMessagesBuffer.size() >= static_cast<int>(sizeof(header))) {
            header.Id = static_cast<uchar>(_rcMessagesBuffer.at(0));
            header.Size = qFromLittleEndian<ushort>(reinterpret_cast<const uchar*>(_rcMessagesBuffer.mid(2, sizeof(ushort)).data()));
            _rcMessagesBuffer.remove(0, sizeof(header));
            if (_rcMessagesBuffer.size() >= header.Size) {
                switch (static_cast<TrainingEnums::MessageId>(header.Id)) {
                case TrainingEnums::MessageId::PingId:
                    _rcWatchdog->start();
                    break;
                default:
                    break;
                }
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
}

void RemoteControl::rcTcpSocketReadyRead()
{
    _rcMessagesBuffer.append(_rcTcpSocket->readAll());
    parseRcMessages();
}
