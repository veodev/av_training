#ifndef DATATRANSFER_USBCAN_WIN_H
#define DATATRANSFER_USBCAN_WIN_H

#include "idatatransfer.h"
#include "sockets/socket_usbcan_win.h"

class cDataTransferUsbCanWin : public cIDataTransfer
{
public:

	cDataTransferUsbCanWin()  {}
	~cDataTransferUsbCanWin() {}

    virtual int addConnection(const cISocket::cConnectionParams *const connection_params);

    virtual int openConnection(const int connection_id);

    virtual bool write(const int                  connection_id,
                       const unsigned char *const data_ptr,
                       const unsigned int         data_count);



    virtual int read (const int           connection_id,
                      unsigned char      *data_ptr,
                      const unsigned int  data_count);
};

#endif // DATATRANSFER_USBCAN_WIN_H
