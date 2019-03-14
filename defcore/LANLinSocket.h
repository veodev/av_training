#ifndef LANLinSockH
#define LANLinSockH

#include <cstdio>
#include <cstdlib>
#include <vector>

#define MESSAGE_NOTIFICATION 1664

typedef int SOCKET;

class SOCKT
{
public:
    SOCKT();
    virtual ~SOCKT();

    const static SOCKET InvalidSocket;

    SOCKET getSocket();
    virtual bool Connect(char* localIPAddress, int PortNo, char* remouteIPAddress) = 0;
    virtual void Disconnect();
    virtual bool sendData(char* msg, int length) = 0;
    virtual unsigned int ReciveData(unsigned char* msg, int length) = 0;

protected:
    SOCKET _socket;
};

class TCP_SOCKET : public SOCKT
{
public:
    TCP_SOCKET();
    ~TCP_SOCKET();
    bool Connect(char* localIPAddress, int remoutePortNo, char* remouteIPAddress);
    bool sendData(char* msg, int length);
    unsigned int ReciveData(unsigned char* msg, int length);
    int getDataCountInTCPBuffer();
};

class UDP_SOCKET : public SOCKT
{
public:
    UDP_SOCKET();
    ~UDP_SOCKET();
    bool Connect(char* localIPAddress, int remoutePortNo, char* remouteIPAddress);
    bool sendData(char* msg, int length);
    unsigned int ReciveData(unsigned char* msg, int length);
};

#endif
