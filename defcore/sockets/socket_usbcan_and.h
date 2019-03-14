#ifndef SOCKET_USBCAN_AND_H
#define SOCKET_USBCAN_AND_H

#include "isocket.h"

class cSocketUSBCanAnd : public cISocket {
public:
	cSocketUSBCanAnd();

private:
	virtual bool connect(const cConnectionParams *) override;
	virtual void disconnect() override;
	virtual int  reciveData(unsigned char * msg, const int length) override;
	virtual bool sendData(const unsigned char * msg, const int length) override;
};

#endif // SOCKET_USBCAN_AND_H
