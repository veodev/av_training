#include <linux/sockios.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "LANLinSocket.h"

#include <asm-generic/errno.h>
#include "logger.h"

const SOCKET SOCKT::InvalidSocket = SOCKET(-1);

SOCKT::SOCKT()
{
}

SOCKT::~SOCKT()
{
}

UDP_SOCKET::UDP_SOCKET()
{
}

UDP_SOCKET::~UDP_SOCKET()
{
}

bool UDP_SOCKET::Connect(char* localIPAddress, int PortNo, char* remouteIPAddress)
{
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(PortNo);
    if (localIPAddress) {
        target.sin_addr.s_addr = inet_addr(localIPAddress);
    }
    else {
        target.sin_addr.s_addr = htonl (INADDR_ANY);
    }
    _socket =  socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket < 0) {
        return false;
    }
    int i = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));
    u_long p = 1;
    if (ioctl(_socket, FIONBIO, &p) < 0) {
        Disconnect();
        return false;
    }
    if (bind(_socket, reinterpret_cast<struct sockaddr *>(&target), sizeof(target)) < 0) {
        Disconnect();
        return false;
    }
    target.sin_port = htons(PortNo);
    target.sin_addr.s_addr = inet_addr(remouteIPAddress);
    if (connect(_socket, reinterpret_cast<struct sockaddr *>(&target), sizeof(target)) < 0) {
        Disconnect();
        return false;
    }
    return true;
}

bool UDP_SOCKET::sendData(char* msg, int length)
{
    (void)msg;
    (void)length;
    return false;
}

unsigned int UDP_SOCKET::ReciveData(unsigned char* msg, int length)
{
    int res = recv(_socket, msg, length, 0);
    if (res == -1) {
        res = 0;
    }
    return res;
}

SOCKET SOCKT::getSocket()
{
    return _socket;
}

void SOCKT::Disconnect()
{
    if (_socket >= 0) {
        close(_socket);
        _socket = -1;
    }
}

TCP_SOCKET::TCP_SOCKET()
{
}

TCP_SOCKET::~TCP_SOCKET()
{
    Disconnect();
}

bool TCP_SOCKET::Connect(char* localIPAddress, int remoutePortNo, char* remouteIPAddress)
{
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket < 0) {
        return false;
    }

    int i = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = 0;
    if (localIPAddress) {
        target.sin_addr.s_addr = inet_addr(localIPAddress);
    }
    else {
        target.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    if (bind(_socket, reinterpret_cast<struct sockaddr *>(&target), sizeof(target)) < 0) {
        return false;
    }
    target.sin_addr.s_addr = inet_addr(remouteIPAddress);
    target.sin_port =  htons(remoutePortNo);
    if (connect(_socket, reinterpret_cast<struct sockaddr *>(&target), sizeof(target)) < 0) {
        return false;
    }
    return true;
}

unsigned int TCP_SOCKET::ReciveData(unsigned char* msg, int length)
{
    u_long count;
    ioctl(_socket, FIONREAD, &count) ;
    if (!count) {
        return 0;
    }
    int res = recv(_socket, msg, length, 0);
    return res;
}

int TCP_SOCKET::getDataCountInTCPBuffer()
{
    u_long count;
    ioctl( _socket, FIONREAD, &count) ;
    return count;
}

bool TCP_SOCKET::sendData(char* msg, int length)
{
    if (_socket != -1) {
        int errorCode = 0;
        socklen_t len = sizeof(errorCode);
        int retval = getsockopt(_socket, SOL_SOCKET, SO_ERROR, &errorCode, &len);
        if (retval == 0 && errorCode == 0) {
            if (send(_socket, msg, length, MSG_NOSIGNAL) != -1) {
                return true;
            }
        }

        QString errorMessage;
        if (errorCode == ECONNRESET) {
            _socket = -1;
            errorMessage = "'Connection reset by peer'";
        }

        Logger::message(QString("TCP_SOCKET::sendData failed - retval:%1 error code:%2 error message:%3").arg(retval).arg(errorCode).arg(errorMessage), QStringList() << "UMU" << "network");
    }

    return false;
}
