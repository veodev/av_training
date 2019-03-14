#ifndef DATATRANSFER_CAN_H
#define DATATRANSFER_CAN_H

#include "idatatransfer.h"

class cDataTransferCanRaw : public cIDataTransfer
{
public:
    typedef cSocketCanRaw::cSocketCanRawParams cCanConnectionParams;

    cDataTransferCanRaw() {}
    ~cDataTransferCanRaw() {}

    virtual int addConnection(const cISocket::cConnectionParams* const connection_params);
    virtual int openConnection(const int connection_id);
    virtual bool write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count);
    virtual int read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count);
};

#endif  // DATATRANSFER_CAN_H
