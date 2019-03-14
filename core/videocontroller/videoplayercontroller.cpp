#include "videoplayercontroller.h"
#include <QDebug>
#include "debug.h"
#include <array>

VideoPlayerController::VideoPlayerController(QObject* parent)
    : QObject(parent)
    , _tcpSocket(nullptr)
    , _connectTimer(new QTimer(this))
    , _lastCoord(0)
{
    _status = eStatusDisconnectd;
    _connectTimer->setInterval(2000);
    ASSERT(connect(_connectTimer, &QTimer::timeout, this, &VideoPlayerController::onTryConnect));

    qDebug() << "VideoPlayerController: created...";
}

VideoPlayerController::~VideoPlayerController()
{
    qDebug() << "VideoPlayerController: deleting...";
}

void VideoPlayerController::sendSimpleCommand(unsigned char id)
{
    std::array<unsigned char, 2> data;
    data[0] = id;
    data[1] = 0x00;
    if (_tcpSocket->isWritable()) {
        _tcpSocket->write(reinterpret_cast<char*>(data.data()), data.size());
        _tcpSocket->waitForBytesWritten(10000);
    }
    else {
        qDebug() << "VideoPlayerController: cannot send command - device not writeable!";
    }
}

void VideoPlayerController::sendCommandWithString(unsigned char id, const QString& string)
{
    QByteArray stringArray(string.toUtf8());
    std::array<unsigned char, 2> data;
    data[0] = id;
    data[1] = stringArray.size();
    if (_tcpSocket->isWritable()) {
        _tcpSocket->write(reinterpret_cast<char*>(data.data()), data.size());
        _tcpSocket->write(stringArray);
        _tcpSocket->waitForBytesWritten(10000);
    }
    else {
        qDebug() << "VideoPlayerController: cannot send command - device not writeable!";
    }
}

void VideoPlayerController::onTryConnect()
{
    qDebug() << "VideoPlayerController: try connect...";
    onConnect();
}

void VideoPlayerController::onInit()
{
    qDebug() << "VideoPlayerController: Initializing...";
    _tcpSocket = new QTcpSocket(this);
    _connectTimer->start();
}

void VideoPlayerController::onStop()
{
    qDebug() << "VideoPlayerController: stopped!";
    _connectTimer->stop();
    onDisconnect();
}

void VideoPlayerController::onConnect()
{
    qDebug() << "VideoPlayerController: connect...";
    if (_status == eStatusDisconnectd) {
        if (_tcpSocket->state() == QAbstractSocket::SocketState::UnconnectedState) {
            _tcpSocket->connectToHost("172.168.100.1", 49005, QIODevice::ReadWrite, QAbstractSocket::NetworkLayerProtocol::IPv4Protocol);
            if (_tcpSocket->waitForConnected(15000)) {
                _status = eStatusConnected;
                qDebug() << "VideoPlayerController: connected!";
            }
            else {
                _tcpSocket->disconnectFromHost();
                _tcpSocket->waitForDisconnected(5000);
            }
        }
    }
}

void VideoPlayerController::onDisconnect()
{
    qDebug() << "VideoPlayerController: disconnect...";
    if (_status == eStatusConnected) {
        _status = eStatusDisconnectd;
        if (_tcpSocket != nullptr) {
            if (_tcpSocket->state() == QAbstractSocket::SocketState::ConnectedState) {
                _tcpSocket->disconnectFromHost();
                _tcpSocket->waitForDisconnected(5000);
                qDebug() << "VideoPlayerController: disconnected...";
            }
        }
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}

void VideoPlayerController::onRegistrationOn(QString name)
{
    qDebug() << "VideoPlayerController: registration on...";
    if (_status == eStatusConnected) {
        sendCommandWithString(VIDEO_PROTOCOL::MESSAGE_TYPE_START_REG, name);
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}

void VideoPlayerController::onRegistrationOff()
{
    qDebug() << "VideoPlayerController: registration off...";
    if (_status == eStatusConnected) {
        sendSimpleCommand(VIDEO_PROTOCOL::MESSAGE_TYPE_STOP_REG);
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}

void VideoPlayerController::onViewMode()
{
    qDebug() << "VideoPlayerController: view mode...";
    if (_status == eStatusConnected) {
        sendSimpleCommand(VIDEO_PROTOCOL::MESSAGE_TYPE_VIEW_MODE);
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}

void VideoPlayerController::onRealtimeMode()
{
    qDebug() << "VideoPlayerController: realtime...";
    if (_status == eStatusConnected) {
        sendSimpleCommand(VIDEO_PROTOCOL::MESSAGE_TYPE_REALTIME_MODE);
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}

void VideoPlayerController::onSetCoord(unsigned int value)
{
    qDebug() << "VideoPlayerController: set coord:" << value;
    if (_status == eStatusConnected) {
        if (value != _lastCoord) {
            std::array<unsigned char, 6> data;
            data[0] = VIDEO_PROTOCOL::MESSAGE_TYPE_SHOW_COORD;
            data[1] = 0x04;
            data[2] = (value) &0x000000ff;
            data[3] = (value >> 8) & 0x000000ff;
            data[4] = (value >> 16) & 0x000000ff;
            data[5] = (value >> 24) & 0x000000ff;
            if (_tcpSocket->isWritable()) {
                _lastCoord = value;
                _tcpSocket->write(reinterpret_cast<char*>(data.data()), data.size());
                _tcpSocket->waitForBytesWritten(10000);
            }
            else {
                qDebug() << "VideoPlayerController: cannot send command - device not writeable!";
            }
        }
    }
    else {
        qDebug() << "VideoPlayerController: Not connected!";
    }
}
