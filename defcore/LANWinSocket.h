#ifndef LANWinSockH
#define LANWinSockH
//---------------------------------------------------------------------------

#include <winsock2.h>
#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <vector>

//#include <queue>
//#include <process.h>
//#include <iostream.h>


//---------------------------------------------------------------------------

#define MESSAGE_NOTIFICATION 1664

class SOCKT
{
protected:
    SOCKET sock;
    bool State;

public:
    SOCKT();
    ~SOCKT();

    const static SOCKET InvalidSocket;

    bool getState();
    void setState(bool val);
    SOCKET getSocket();
    virtual bool Connect(char* localIPAddress, int PortNo, char* remouteIPAddress) = 0;
    virtual void Disconnect();
    virtual bool sendData(char* msg, int length) = 0;
    virtual unsigned int ReciveData(unsigned char* msg, int length) = 0;
};
//
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
//

class UDP_SOCKET : public SOCKT
{
public:
    UDP_SOCKET();
    ~UDP_SOCKET();
    bool Connect(char* localIPAddress, int remoutePortNo, char* remouteIPAddress);
    bool sendData(char* msg, int length)
    {
        return false;
    }
    unsigned int ReciveData(unsigned char* msg, int length);
};


//


#endif
