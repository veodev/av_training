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

void RemoteControl::registrationOn(QString operatorName, QString railroadPathName, int pathNumber, TrainingEnums::Direction direction, int km, int pk, int m)
{
    QByteArray data;
    data.append(char(operatorName.length() * 2));
    data.append(convertQStringToUtf16ByteArray(operatorName));
    data.append(char(railroadPathName.length() * 2));
    data.append(convertQStringToUtf16ByteArray(railroadPathName));
    data.append(char(pathNumber));
    data.append(char(direction));
    data.append(char(km));
    data.append(char(pk));
    data.append(char(m));
    sendMessageToTrainingPc(TrainingEnums::MessageId::RegistrationOnId, data);
}

void RemoteControl::registrationOff()
{
    sendMessageToTrainingPc(TrainingEnums::MessageId::RegistrationOffId);
}

void RemoteControl::setCurrentTrackMarks(int km, int pk)
{
    QByteArray data;
    data.append(char(km));
    data.append(char(pk));
    sendMessageToTrainingPc(TrainingEnums::MessageId::OperatorTrackCoordinateId, data);
}

void RemoteControl::setRailroadSwitch(int number)
{
    QByteArray data;
    data.append(char(number));
    sendMessageToTrainingPc(TrainingEnums::MessageId::RailroadSwitchMarkId, data);
}

void RemoteControl::setDefect(QString defectCode, TrainingEnums::RailroadSide side)
{
    QByteArray data;
    data.append(char(defectCode.length() * 2));
    data.append(convertQStringToUtf16ByteArray(defectCode));
    data.append(char(side));
    sendMessageToTrainingPc(TrainingEnums::MessageId::DefectMarkId, data);
}

void RemoteControl::boltJointOn()
{
    sendMessageToTrainingPc(TrainingEnums::MessageId::BoltJointOnId);
}

void RemoteControl::boltJointOff()
{
    sendMessageToTrainingPc(TrainingEnums::MessageId::BoltJointOffId);
}

void RemoteControl::setCduMode(TrainingEnums::CduMode mode)
{
    QByteArray data;
    data.append(char(mode));
    sendMessageToTrainingPc(TrainingEnums::MessageId::ChangeCduModeId, data);
}

void RemoteControl::netSettingsChanged()
{
    disconnectRcTcpSocket();
    disconnectTrainingPc();
    _rcTcpServer->close();
    qDebug() << "Remote control server stopped";

    _rcPort = restoreRcTcpServerPort();
    _trainingPcIpAddress = restoreTrainingPcTcpServerIpAddress();
    _trainingPcPort = restoreTrainingPcTcpServerPort();

    listen();
}

QByteArray RemoteControl::convertQStringToUtf16ByteArray(QString str)
{
    QByteArray utf16ByteArray;
    for (auto ch : str) {
        ushort code = ch.unicode();
        utf16ByteArray.append(reinterpret_cast<char*>(&code), sizeof(ushort));
    }
    return utf16ByteArray;
}

void RemoteControl::rcTcpServerNewConnection()
{
    disconnectRcTcpSocket();
    _rcTcpSocket = _rcTcpServer->nextPendingConnection();
    connect(_rcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::rcTcpSocketReadyRead);
    connect(_rcTcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::rcTcpSocketDisconnected);
    emit doRcConnected();
    _rcPingTimer->start();
}

void RemoteControl::disconnectRcTcpSocket()
{
    if (_rcTcpSocket != nullptr) {
        disconnect(_rcTcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::rcTcpSocketReadyRead);
        disconnect(_rcTcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::rcTcpSocketDisconnected);
        _rcTcpSocket->disconnectFromHost();
        _rcTcpSocket->deleteLater();
        _rcTcpSocket = nullptr;
    }
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
        emit doTrainingPcDisconnected();
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
                case TrainingEnums::MessageId::BoltJointOnId:
                    emit doBoltJointButtonPressed();
                    break;
                case TrainingEnums::MessageId::BoltJointOffId:
                    emit doBoltJointButtonReleased();
                    break;
                case TrainingEnums::MessageId::ChangeCduModeId:
                    changeCduMode(static_cast<TrainingEnums::CduMode>(_rcMessagesBuffer.at(0)));
                    _rcMessagesBuffer.remove(0, 1);
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

void RemoteControl::changeCduMode(TrainingEnums::CduMode mode)
{
    switch (mode) {
    case TrainingEnums::CduMode::TrackMarksMode:
        emit doTrackMarksButtonReleased();
        break;
    case TrainingEnums::CduMode::ServiceMarksMode:
        emit doServiceMarksButtonReleased();
        break;
    default:
        break;
    }
}

void RemoteControl::rcTcpSocketReadyRead()
{
    _rcMessagesBuffer.append(_rcTcpSocket->readAll());
    parseRcMessages();
}
