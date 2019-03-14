#include "socket_can.h"

void cSocketCanRaw::waitForCan()
{
    SLEEP(1);
}

void cSocketCanRaw::resetWait()
{

}

bool cSocketCanRaw::connect(const cConnectionParams *socket_params)
{
    if (!cISocket::connect(socket_params))
    {
        return false;
    }

    const cSocketCanRawParams *params =
            static_cast<const cSocketCanRawParams*>(socket_params);

    ifreq        _ifr;
    sockaddr_can _addr;

    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0)
    {
        return false;
    }

    memset(&_ifr  , 0, sizeof(_ifr));
    memset(&_addr , 0, sizeof(_addr));

    _ifr.ifr_name[0] = 'c';
    _ifr.ifr_name[1] = 'a';
    _ifr.ifr_name[2] = 'n';

    // Warning! Works only for interfaces in range [0-9]
    if (params->_can_interface_num > 9)
    {
        disconnect();
        return false;
    }

    _ifr.ifr_name[3] = '0' + params->_can_interface_num;
    _ifr.ifr_name[4] = '\0';

    {
        int ioctl_res = ioctl(_socket, SIOCGIFINDEX, &_ifr);
        if (ioctl_res < 0)
        {
            disconnect();
            return false;
        }
    }

    _addr.can_ifindex = _ifr.ifr_ifindex;
    _addr.can_family  = AF_CAN;

    /* bind socket to the canN interface */
    {
        int bind_res = bind(_socket, (sockaddr *)&_addr, sizeof(_addr));
        if (bind_res < 0)
        {
            disconnect();
            return false;
        }
    }

    /*{
        int connect_res = ::connect(_socket, reinterpret_cast<sockaddr*>(&_addr), sizeof(_addr));
        if (connect_res < 0)
        {
            connect_res = errno;
            disconnect();
            return false;
        }
    }//*/

    return true;
}

void cSocketCanRaw::disconnect()
{
    if (_socket >= 0)
    {
        _socket = -1;
    }
}

bool cSocketCanRaw::sendData(const unsigned char *msg, const int length)
{
    if (_socket < 0)
    {
        return false;
    }
    waitForCan();

    int write_res = send(_socket, msg, length, MSG_NOSIGNAL);

    resetWait();


    if (write_res < 0)
    {
        char *p = strerror(errno);

        return p;
    }

    return true;
}

int cSocketCanRaw::reciveData(unsigned char *msg, const int length)
{
    if (_socket < 0)
    {
        return -1;
    }

    int read_res = -1;

#ifdef DEFCORE_OS_WIN
    read_res = recv(_socket, reinterpret_cast<char*>(msg), length, 0);
    if (read_res == SOCKET_ERROR)  read_res = 0; //FDV

#else
    read_res = recv(_socket, msg, length, MSG_DONTWAIT); // MSG_DONTWAIT подтверждена работоспособность с CAN под Linux, в случае нарушения работы иных протоколов заменить на 0
#endif

    if (read_res < 0)
    {
        read_res = -1;
        if (errno == EAGAIN)
        {
            read_res = 0;
        }
    }

    return read_res;
}
