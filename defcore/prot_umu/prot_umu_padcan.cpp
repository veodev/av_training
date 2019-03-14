#include "prot_umu/prot_umu_padcan.h"
//#include "debug.h"
// +
void UMU_PADCAN::UnPack(cMessageCan buff)
{
    // Получаем id текущего сообщения
    unsigned char command_id = can_utils::GetCommandId(buff);
    ++_common_state._event_count[command_id];

    // и вызываем соответствующую функцию обработчик для сообщения с данным id
    // из _response_map <command_id, функция_обработчик>
    if (_response_map.count(command_id)) {
        Parser parser = _response_map[command_id];
        if (parser != 0) {
            (this->*parser)((unsigned char*) can_utils::GetData(buff), can_utils::GetDataSize(buff));
            _common_state._last_event_time = GetTickCount_();
        }
    }
}
// +
void UMU_PADCAN::TickReceive()
{
    if (!isPointerInitialized(_common_state._data_transfer)) {
        return;
    }

    // OLD --------------------------------------------
    /*
    memset(&_message, 0, sizeof(_message));

    unsigned short res = 0;
    _read_state = rsBody;

    res = _common_state._data_transfer->read(
                _common_state._connection_id,
                &(_message._msg[0]),
            cMessageCan::CAN_MAX_FRAME_SIZE);

    if (res == cMessageCan::CAN_MAX_FRAME_SIZE)
    {
        ++_common_state._message_count;
        _common_state._message_read_bytes_count += res;
        UnPack(_message);
    }
    else if (res != 0)
    {
        ++_common_state._error_message_count;
        if (res > 0)
        {
            _common_state._read_bytes_count += res;
        }
    }
    */
    // NEW ----------------------------------

    memset(&_message, 0, sizeof(_message));
    _read_state = rsBody;

    int totalBytesRead = 0;
    while (true) {
        int res = _common_state._data_transfer->read(_common_state._connection_id, _message._msg, cMessageCan::CAN_MAX_FRAME_SIZE);

        if (res > 0) {
            totalBytesRead += res;
            _common_state._message_read_bytes_count += res;
        }
        else if (res < 0) {
            totalBytesRead = 0;
        }

        if (totalBytesRead == cMessageCan::CAN_MAX_FRAME_SIZE) {
            // qDebug() << " Read: " << QByteArray((char *)_message._msg,cMessageCan::CAN_MAX_FRAME_SIZE).toHex();
            ++_common_state._message_count;

            UnPack(_message);
            memset(&_message, 0, sizeof(_message));
            totalBytesRead = 0;
            // break;
        }

        if (res == 0) {
            break;
        }
    }
}
// +
UMU_PADCAN::UMU_PADCAN()
    : CANProtDebug::UMU_RAWCAN()
{
    _protocol = eProtPADCAN;
}
// +
void UMU_PADCAN::sendCommandWithData(const unsigned char command_id, const unsigned char* data, const unsigned char size)
{
    //    memset(_out_block, 0, sizeof((*_out_block)));
    cMessageCan _out_block;

    memset(&_out_block, 0, sizeof(cMessageCan));

    can_utils::SetCommandId(_out_block, command_id);
    can_utils::SetDeviceId(_out_block, can_utils::UMU);
    can_utils::SetDataAndSize(_out_block, data, size);

    AddToOutBuff_(&_out_block);
}

int UMU_PADCAN::EnableBScan()
{
    if (!_common_state._bscan_state) {
        unsigned long res;
        CANProtDebug::UMU_RAWCAN::sendEmptyCommand(can_utils::wmEnableBScan);
        _common_state._bscan_state = true;
        // Сессия данных В-развертки
        _common_state._critical_section->Enter();

        // Сессия данных В-развертки
        if (_common_state._bscan_session_id.size() == 0) {
            if (_common_state._bscan_last_session_id == -1) {
                _common_state._bscan_session_id.push_back(0);  // Сессия данных В-развертки
                _common_state._bscan_last_session_id = 0;
            }
            else {
                _common_state._bscan_last_session_id++;
                _common_state._bscan_session_id.push_back(_common_state._bscan_last_session_id);
            }
        }
        else
            _common_state._bscan_session_id.push_back(_common_state._bscan_session_id.back() + 1);  // Увеличиваем номер сесии данных В-развертки


        res = _common_state._bscan_session_id.back();
        _common_state._critical_section->Release();
        return res;
    }
    return 0;
}
//
// Переключение линии ручного контроля
// (раздельный или совмещенный) - БУМ 0101
void UMU_PADCAN::SetLineSwitching(UMULineSwitching state)
{
    assert((state == toCompleteControl) || (state == toCombineSensorHandControl) || (state == toSeparateSensorHandControl));

    if (state == toCompleteControl) return;
    const unsigned short argc = 2;
    unsigned char argv[argc];

    argv[0] = 1;
    if (state == toCombineSensorHandControl) {
        argv[1] = 0;  // совмещенный режим
    }
    else {
        argv[1] = 1;  // раздельный режим
    }
    sendCommandWithData(can_utils::wmLineSwitching, argv, argc);
}
//
