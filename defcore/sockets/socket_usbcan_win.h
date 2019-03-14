#ifndef SOCKET_USBCAN_WIN_H
#define SOCKET_USBCAN_WIN_H

#include "isocket.h"

typedef _cdecl void (*vfuncv)(void);
typedef _cdecl int (*ifuncv)(void);
typedef _cdecl unsigned char(*ReadfuncPtr)(unsigned char*, unsigned char,unsigned char);
typedef _cdecl unsigned short (*WritefuncPtr)(unsigned char*, unsigned short);
typedef _cdecl unsigned short (*PowerSwitchPtr)(unsigned char);

class cSocketUSBCanWin : public cISocket
{
	static const int CONNECT_POLLING_PERIOD = 500;
private:
	bool _fInitCompleted;
	HINSTANCE _hdll;
	vfuncv pFinish;
	ifuncv pInit;
	ReadfuncPtr pRead;
	WritefuncPtr pWrite;
    PowerSwitchPtr pPowerSwitch;
public:
    cSocketUSBCanWin(); 
	virtual bool connect(const cConnectionParams *socket_params);
    virtual void disconnect();
    virtual int  reciveData(unsigned char     *msg, const int length);
    virtual bool sendData(const unsigned char *msg, const int length);
};

#endif // SOCKET_USBCAN_WIN_H


