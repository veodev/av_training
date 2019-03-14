#ifndef SOCKET_LAN_H
#define SOCKET_LAN_H

#include "isocket.h"

class cSocketLan : public cISocket
{
public:
    struct cSocketLanParams : public cConnectionParams
    {
        char *_local_ip_address;
        int   _remoute_port_num;
        char *_remoute_ip_address;
    };
};

class cSocketLanUdp : public cSocketLan
{
public:
    virtual bool connect(const cConnectionParams *socket_params);
    virtual bool sendData(const unsigned char *msg, const int length);
};

class cSocketLanTcp : public cSocketLan
{
public:
    virtual bool connect(const cConnectionParams *socket_params);
    virtual bool sendData(const unsigned char *msg, const int length);
    virtual int  reciveData(unsigned char     *msg, const int length);

    int getDataCountInTcpBuffer();
};

#endif // SOCKET_LAN_H

