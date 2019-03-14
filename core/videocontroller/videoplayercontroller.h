#ifndef VIDEOPLAYERCONTROLLER_HPP
#define VIDEOPLAYERCONTROLLER_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "videocontrolprotocol.h"
#include <QTimer>

class VideoPlayerController : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayerController(QObject* parent = nullptr);
    ~VideoPlayerController();
    void sendSimpleCommand(unsigned char id);
    void sendCommandWithString(unsigned char id, const QString& string);
signals:

private slots:
    void onTryConnect();

public slots:
    void onInit();
    void onStop();
    void onConnect();
    void onDisconnect();
    void onRegistrationOn(QString name);
    void onRegistrationOff();
    void onViewMode();
    void onRealtimeMode();
    void onSetCoord(unsigned int value);

private:
    enum ControllerStatus
    {
        eStatusDisconnectd = 0,
        eStatusConnected = 1
    };
    ControllerStatus _status;
    QTcpSocket* _tcpSocket;
    QByteArray _currentArray;
    QTimer* _connectTimer;
    unsigned int _lastCoord;
};

#endif  // VIDEOPLAYERCONTROLLER_HPP
