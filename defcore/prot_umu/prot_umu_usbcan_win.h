#ifndef PROT_UMU_USBCAN_H
#define PROT_UMU_USBCAN_H

//#include "datatransfers/datatransfer_usbcan_win.h"
#include "prot_umu_rawcan.h"

class UMU_USBCAN : public CANProtDebug::UMU_RAWCAN
{

private:

	static const unsigned int CAN_HEADER_SIZE = 2;
    unsigned int _bodySize;
//
    void UnPack(cMessageCan buff);

    // VIRTUAL OVERRIDE DECLARATIONS:
    virtual void TickReceive();
    virtual void sendCommandWithData(const unsigned char command_id, const unsigned char *data, const unsigned char size);
//
    virtual void SetCommandId(cMessageCan &message, const unsigned char command_id)
    {
		message._msg[0] |= command_id;
    }
//
    virtual unsigned char GetCommandId(const cMessageCan &message)
    {
        return message._msg[0];
    }
//
    virtual void SetDeviceId(cMessageCan &message, const unsigned char device_id)
    {
        message._msg[1] &= 0x0F;
        message._msg[1] |= (device_id & 0x7) << 4;
    }
    virtual unsigned char GetDeviceId(const cMessageCan &message)
    {
        return (message._msg[1] & 0xF0) >> 4;
    }
//
    virtual void SetSizeOfData(cMessageCan &message, const unsigned char data_size)
    {
        message._msg[1] &= 0xF0;
        message._msg[1] |= data_size & 0xF;
    }
//
    virtual unsigned char GetDataSize(const cMessageCan &message)
    {
		return message._msg[1] & 0xF;
	}
//
	virtual unsigned char GetMessageSize(const cMessageCan &message)
	{
		return (message._msg[1] & 0xF) + CAN_HEADER_SIZE;
	}
//
    virtual void SetDataAndSize(cMessageCan &message,
                        const unsigned char *bytes,
                        const unsigned char size = 1)
    {
        for (size_t i = 0; i < cMessageCan::CAN_MAX_DATA_SIZE; ++i)
        {
            if (i < size)
            {
                message._msg[CAN_HEADER_SIZE + i] = bytes[i];
            }
            else
            {
                message._msg[CAN_HEADER_SIZE + i] = 0;
            }
        }

        SetSizeOfData(message, size);
    }
//
    virtual const unsigned char *GetData(const cMessageCan &message)
    {
        return &(message._msg[CAN_HEADER_SIZE]);
    }

	virtual bool parseResponse_BScanStopped(unsigned char *data, unsigned char size);
//
// Выбор формата М-развертки - Ав15
	virtual void SetMScanFormat(MScanFormat mScanVar);

public:
    // ------------------------- C-TORS and D-TORS -----------------------------
    UMU_USBCAN();
    ~UMU_USBCAN() { /*delete _out_block; _out_block = 0;*/ }

    eProtocol getProtocolType() const { return eProtUSBCAN; }
};

#endif // PROT_UMU_USBCAN_H
