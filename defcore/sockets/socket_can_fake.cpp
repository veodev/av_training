#include "socket_can.h"

bool cSocketCanRaw::connect(const cConnectionParams * socket_params)
{
    (void)socket_params;
    return false;
}

bool cSocketCanRaw::sendData(const unsigned char * msg, const int length)
{
    (void)msg;
    (void)length;
    return false;
}

void cSocketCanRaw::disconnect()
{
}

int cSocketCanRaw::reciveData(unsigned char *msg, const int length)
{
    (void)msg;
    (void)length;
    return -1;
}
