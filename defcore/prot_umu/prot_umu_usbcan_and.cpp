#include "prot_umu/prot_umu_usbcan_and.h"

//
void UMU_USBCANAND::TickReceive()
{
    if (!isPointerInitialized(_common_state._data_transfer)) {
        return;
    }
    bool fRepeat;
    unsigned short res = 0;
    do {
        fRepeat = false;
        switch (_read_state) {
        case rsHead: {
            res = _common_state._data_transfer->read(_common_state._connection_id, &(_message._msg[0]), CAN_HEADER_SIZE);
            if (res == CAN_HEADER_SIZE) {
                fRepeat = true;
                _bodySize = GetDataSize(_message);
                if (_bodySize <= _message.CAN_MAX_DATA_SIZE) {
                    _read_state = rsBody;
                    _common_state._read_bytes_count += CAN_HEADER_SIZE;
                }
                else {
                    ++_common_state._error_message_count;
                }
            }
            else if (res != 0) {
                ++_common_state._error_message_count;
            }
            break;
        }
        case rsBody: {
            if (_bodySize != 0) {
                res = _common_state._data_transfer->read(_common_state._connection_id, &(_message._msg[CAN_HEADER_SIZE]), _bodySize);
                if (res == _bodySize) {
                    _common_state._read_bytes_count += res + CAN_HEADER_SIZE;
                }
                else if (res != 0) {
                    ++_common_state._error_message_count;
                    break;
                }
                else {
                    break;
                }
            }
            fRepeat = true;
            UnPack(_message);
#ifdef DbgLog
            if ((CANProtDebug::useLog) && (CANProtDebug::onAddLog)) CANProtDebug::onAddLog(_common_state._umu_id, (unsigned char*) &_message, 1);
#endif
            memset(&_message, 0, sizeof(_message));
            _read_state = rsHead;
            break;
        }
        case rsOff: {
            break;
        }
        }
    } while (fRepeat == true);
}
//
void UMU_USBCANAND::UnPack(const cMessageCan& buff)
{
    // Получаем id текущего сообщения
    unsigned char command_id = GetCommandId(buff);
    ++_common_state._event_count[command_id];

    // и вызываем соответствующую функцию обработчик для сообщения с данным id
    // из _response_map <command_id, функция_обработчик>
    if (_response_map.count(command_id)) {
        Parser parser = _response_map[command_id];
        if (parser != 0) {
            (this->*parser)((unsigned char*) GetData(buff), GetDataSize(buff));
        }
    }
}
//
UMU_USBCANAND::UMU_USBCANAND()
    : CANProtDebug::UMU_RAWCAN()
{
    _protocol = eProtUSBCANAND;
    _bodySize = 0;
}
//
void UMU_USBCANAND::sendCommandWithData(const unsigned char command_id, const unsigned char* data, const unsigned char size)
{
    cMessageCan _out_block;
    memset(&_out_block, 0, sizeof(cMessageCan));

    SetCommandId(_out_block, command_id);
    SetDeviceId(_out_block, can_utils::UMU);
    SetDataAndSize(_out_block, data, size);
    AddToOutBuff_(&_out_block);
}

bool UMU_USBCANAND::parseResponse_BScanStopped(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    UNUSED(data);
    _common_state._critical_section->Enter();
    if (_common_state._bscan_session_id.size() != 0) {
        _common_state._bscan_session_id.erase(_common_state._bscan_session_id.begin());
    }
    _common_state._critical_section->Release();

    return true;
}

void UMU_USBCANAND::SetMScanFormat(MScanFormat mScanVar)
{
    const unsigned short argc = 3;
    unsigned char argv[argc];
    argv[0] = 0x11;
    argv[1] = 0xF0;
    argv[2] = mScanVar;
    sendCommandWithData(can_utils::wmSetMScanFormat, argv, argc);
}
