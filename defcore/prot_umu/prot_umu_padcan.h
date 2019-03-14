#ifndef PROT_UMU_PADCAN_H
#define PROT_UMU_PADCAN_H

#include "datatransfers/datatransfer_can.h"
#include "prot_umu_rawcan.h"

class UMU_PADCAN : public CANProtDebug::UMU_RAWCAN
{

private:


// ----------------------- INNER SERVICE MEMBERS ---------------------------
	// Special for UMU_1002:
	void UnPack(cMessageCan buff);
    virtual bool EndWorkTest() { return (_common_state._end_work_flag) && _out_buffer.empty(); }

	// VIRTUAL OVERRIDE DECLARATIONS:
	virtual void TickReceive();
	virtual void sendCommandWithData(const unsigned char command_id, const unsigned char *data, const unsigned char size);
    int EnableBScan();
    virtual unsigned char GetMessageSize(const cMessageCan &message)
	{
        UNUSED(message);
		return _message.CAN_MAX_FRAME_SIZE;
	}
// Переключение линии ручного контроля
// (раздельный или совмещенный) - БУМ 0101
    void SetLineSwitching(UMULineSwitching state);
//
public:
	// ------------------------- C-TORS and D-TORS -----------------------------
    UMU_PADCAN();
    ~UMU_PADCAN() { /*delete _out_block; _out_block = 0;*/ }

    eProtocol getProtocolType() const { return eProtPADCAN; }

    // ------------------------------ SETTERS ----------------------------------
};

#endif // PROT_UMU_PADCAN_H
