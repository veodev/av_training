#include <QTcpServer>
#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QGeoPositionInfo>
#include <QtEndian>

#include "remotecontrol.h"
#include "settings.h"

const int PING_INTERVAL_MS = 300;
const int WATCHDOG_INTERVAL_MS = 3000;

RemoteControl::RemoteControl(QObject* parent)
    : QObject(parent)
    , _tcpServer(nullptr)
    , _tcpSocket(nullptr)
    , _isRegistrationStarted(false)
    , _pingTimer(nullptr)
    , _watchdog(nullptr)
{
    _pingTimer = new QTimer(this);
    _pingTimer->setInterval(PING_INTERVAL_MS);
    connect(_pingTimer, &QTimer::timeout, this, &RemoteControl::onPingSendTimerTimeout);

    _watchdog = new QTimer(this);
    _watchdog->setInterval(WATCHDOG_INTERVAL_MS);
    connect(_watchdog, &QTimer::timeout, this, &RemoteControl::onWatchdogTimeout);
}

void RemoteControl::sendMeter(int m)
{
    QByteArray message;
    message.append(CurrentMeter);
    message.append(reinterpret_cast<char*>(&m), sizeof(int));
    sendMessage(message);
}

void RemoteControl::updateRemoteState(bool isRegOn, ViewCoordinate viewType, Direction direction, int km, int pk, int m)
{
    QByteArray message;
    message.append(UpdateState);
    message.append(isRegOn);
    message.append(viewType);
    message.append(reinterpret_cast<char*>(&direction), sizeof(Direction));
    message.append(reinterpret_cast<char*>(&km), sizeof(int));
    message.append(static_cast<char>(pk));
    message.append(reinterpret_cast<char*>(&m), sizeof(int));
    sendMessage(message);
}

void RemoteControl::updateTrackMarks(int km, int pk, int m)
{
    QByteArray message;
    message.append(CurrentTrackMarks);
    message.append(reinterpret_cast<char*>(&km), sizeof(int));
    message.append(static_cast<char>(pk));
    message.append(reinterpret_cast<char*>(&m), sizeof(int));
    sendMessage(message);
}

void RemoteControl::updateRemoteMarks()
{
    QByteArray message;
    message.append(ClearMarksLists);
    sendMessage(message);

    _bridgesList = restoreBridgesToSettings();
    _platformsList = restorePlatformsToSettings();
    _miscList = restoreMiscToSettings();
    sendStringList(BridgesItem, _bridgesList);
    sendStringList(PlatformsItem, _platformsList);
    sendStringList(MiscItem, _miscList);

    message.clear();
    message.append(UpdateMarksLists);
    sendMessage(message);
}

void RemoteControl::changeSpeed(double value)
{
    float speed = static_cast<float>(value);
    QByteArray message;
    message.append(CurrentSpeed);
    message.append(reinterpret_cast<char*>(&speed), sizeof(float));
    sendMessage(message);
}

void RemoteControl::start()
{
    _tcpServer = new QTcpServer(this);
    connect(_tcpServer, &QTcpServer::newConnection, this, &RemoteControl::onNewConnection);
}

void RemoteControl::listen()
{
    qDebug() << "Remote control server started: " << _tcpServer->listen(QHostAddress::Any, 49001);
}

void RemoteControl::onNewConnection()
{
    if (_tcpSocket == nullptr) {
        _tcpSocket = _tcpServer->nextPendingConnection();
        connect(_tcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::onCloseConnection);
        connect(_tcpSocket, &QTcpSocket::readyRead, this, &RemoteControl::onReadyRead);
        emit doRemoteControlConnected();
        emit doUpdateRemoteState();
        updateRemoteMarks();
        if (_pingTimer->isActive() == false) {
            _pingTimer->start();
        }
    }
    else {
        _tcpServer->nextPendingConnection()->close();
    }
}

void RemoteControl::onCloseConnection()
{
    _pingTimer->stop();
    disconnect(_tcpSocket, &QTcpSocket::disconnected, this, &RemoteControl::onCloseConnection);
    _tcpSocket->deleteLater();
    _tcpSocket = nullptr;
    emit doRemoteControlDisconnected();
}

void RemoteControl::onReadyRead()
{
    while (_tcpSocket->bytesAvailable()) {
        _messagesBuffer.append(_tcpSocket->readAll());
    }
    readMessageFromBuffer();
}

void RemoteControl::onPingSendTimerTimeout()
{
    QByteArray message;
    message.append(PingRemoteControl);
    sendMessage(message);
}

void RemoteControl::onWatchdogTimeout()
{
    _watchdog->stop();
    onCloseConnection();
    emit doRemoteControlPingFail();
}

void RemoteControl::sendStringList(RemoteControlHeaders header, QStringList& list)
{
    for (QString& item : list) {
        QByteArray message;
        message.append(header);
        message.append(item.toUtf8());
        sendMessage(message);
    }
}

void RemoteControl::sendMessage(QByteArray& message)
{
    if (_tcpSocket != nullptr) {
        quint16 size = static_cast<quint16>(message.size());
        _tcpSocket->write(reinterpret_cast<char*>(&size), sizeof(quint16));
        _tcpSocket->write(message);
        _tcpSocket->flush();
    }
}

void RemoteControl::readMessageFromBuffer()
{
    RemoteControlHeaders header = UnknownHeader;
    while (true) {
        if (_messagesBuffer.size() >= static_cast<int>(sizeof(qint16))) {
            quint16 size = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(quint16)).data()));
            if (_messagesBuffer.size() >= size) {
                _messagesBuffer.remove(0, sizeof(quint16));
                header = static_cast<RemoteControlHeaders>(_messagesBuffer.at(0));
                _messagesBuffer.remove(0, sizeof(RemoteControlHeaders));
                switch (static_cast<RemoteControlHeaders>(header)) {
                case StartRegistration:
                    if (_isRegistrationStarted == false) {
                        _isRegistrationStarted = true;
                        emit doStartRegistration();
                    }
                    break;
                case StopRegistration:
                    _isRegistrationStarted = false;
                    emit doStopRegistration();
                    break;
                case CurrentTrackMarks: {
                    int km = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    int pk = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));
                    emit doCurrentTrackMark(km, pk);
                    break;
                }
                case SatellitesInUse: {
                    int satellitesInUse = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    emit doSatellitesInUse(satellitesInUse);
                    break;
                }
                case SatellitesInfo: {
                    float latitude = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(float)).data()));
                    _messagesBuffer.remove(0, sizeof(float));
                    float longitude = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(float)).data()));
                    _messagesBuffer.remove(0, sizeof(float));
                    float altitude = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(float)).data()));
                    _messagesBuffer.remove(0, sizeof(float));
                    float direction = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(float)).data()));
                    _messagesBuffer.remove(0, sizeof(float));
                    float speed = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(float)).data()));
                    _messagesBuffer.remove(0, sizeof(float));
                    int year = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    int month = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));
                    int day = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));
                    int hours = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));
                    int minutes = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));
                    int seconds = static_cast<int>(_messagesBuffer.at(0));
                    _messagesBuffer.remove(0, sizeof(char));

                    QGeoPositionInfo geoPositionInfo;
                    geoPositionInfo.setCoordinate(QGeoCoordinate(static_cast<double>(latitude), static_cast<double>(longitude), static_cast<double>(altitude)));
                    geoPositionInfo.setTimestamp(QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds)));
                    geoPositionInfo.setAttribute(QGeoPositionInfo::Direction, static_cast<qreal>(direction));
                    geoPositionInfo.setAttribute(QGeoPositionInfo::GroundSpeed, static_cast<qreal>(speed));
                    emit doSatellitesInfo(geoPositionInfo);
                    break;
                }
                case BridgesItem: {
                    int index = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    if (index >= 0 && index < _bridgesList.size()) {
                        QString label = _bridgesList.at(index);
                        emit doTextLabel(label);
                    }
                    break;
                }
                case PlatformsItem: {
                    int index = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    if (index >= 0 && index < _platformsList.size()) {
                        QString label = _platformsList.at(index);
                        emit doTextLabel(label);
                    }
                    break;
                }
                case MiscItem: {
                    int index = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(_messagesBuffer.left(sizeof(int)).data()));
                    _messagesBuffer.remove(0, sizeof(int));
                    if (index >= 0 && index < _miscList.size()) {
                        QString label = _miscList.at(index);
                        emit doTextLabel(label);
                    }
                    break;
                }
                case StartSwitch:
                    emit doStartSwitchLabel();
                    break;
                case EndSwitch:
                    emit doEndSwitchLabel();
                    break;
                case PingRemoteServer: {
                    _watchdog->start();
                }
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
