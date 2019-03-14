#include "prot_umu/prot_umu_can.h"

// +
void UMU_CAN::UnPack(cMessageCan buff)
{
    // Получаем id текущего сообщения
    unsigned char command_id = can_utils::GetCommandId(buff);
    ++_common_state._event_count[command_id];

    // Обрабатываем мета-логику отсылки M развертки
    // Если вновь присланный пакет это не М развертка, и у нас уже записана
    // пришедшая до этого М развертка в буфер, значит пора ее отсылать
    if ((command_id != can_utils::dMScanData) && (_special_state._mscan_buf != 0))
    {
        _common_state._critical_section->Enter();
        try
        {
            _special_state._mscan_buf->XSysCrd[0] = 0;
            _special_state._mscan_buf->Dir[0] = 0;

            _common_state._data_sender->CreateNewEvent(edMScanData);
            _common_state._data_sender->WriteEventData(&_special_state._mscan_buf,
                                                       SIZE_OF_TYPE(&_special_state._mscan_buf));
            ++_common_state._post_count;
            _common_state._data_sender->Post();
        }
        catch (...) { /* Exception will stop there! */ }

        _common_state._critical_section->Release();
        _special_state._mscan_buf = 0;
    }

    // и вызываем соответствующую функцию обработчик для сообщения с данным id
    // из _response_map <command_id, функция_обработчик>
    if (_response_map.count(command_id))
    {
        Parser parser = _response_map[command_id];
        if (parser != 0)
        {
            (this->*parser)(buff);
        }
    }
}
// +
void UMU_CAN::AddToOutBuff()
{
    if (!isPointerInitialized(_common_state._critical_section))
        return;

    _common_state._critical_section->Enter();
    try
    {
        _out_buffer.push(*_out_block);
        memset(_out_block, 0, sizeof(cMessageCan));
    }
    catch (...) { /* Exception will stop there! */ }
    _common_state._critical_section->Release();
}
// +
void UMU_CAN::Unload()
{
    if (!isPointerInitialized(_common_state._critical_section)
            ||
            !isPointerInitialized(_common_state._data_transfer)) {
        return;
    }

    _common_state._critical_section->Enter();
    try
    {
        while (!_out_buffer.empty())
        {
            bool result = _common_state._data_transfer->write(_common_state._connection_id,
                                                              &(_out_buffer.front()._msg[0]),
                                                              can_utils::cMessageCan::CAN_MAX_FRAME_SIZE);
            if (result)
            {
                _out_buffer.pop();
            }
            else
            {
                ++_common_state._write_error_count;
                break;
            }
        }
    }
    catch (...) { /* Exception will stop there! */ }
    _common_state._critical_section->Release();
}
// +
void UMU_CAN::TickSend()
{
    switch (_wstate)
    {
    case bsOff:
    {
        break;
    }
    case bsInit:
    {
        _wstate = bsPowerOn;
        break;
    }
    case bsPowerOn:
    {
        _wstate     = bsRequest;
        _read_state = rsHead;
        break;
    }
    case bsRequest:
    {
        _common_state._request_time = GetTickCount_();
        _wstate = bsWaitAnswer;
        break;
    }
    case bsWaitAnswer:
    {
        Unload();

        if (GetTickCount_() - _common_state._request_time > 500)
            _wstate = bsRequest;

        if (_common_state._event_count[can_utils::cSerialNumber] > 0)
        {
            _wstate = bsReady;
        }

        break;
    }
    case bsReady:
    {
        Unload();
        break;
    }
    case bsStop:
    {
        break;
    }
    }
}
// +
void UMU_CAN::TickReceive()
{
    if (!isPointerInitialized(_common_state._data_transfer))
    {
        return;
    }

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
}
// +
UMU_CAN::UMU_CAN()
{
    try
    {
        // Инициализация нулями для всех переменных из _special_state
        memset(&_special_state, 0, sizeof(cSpecialState));

        _out_block = new cMessageCan;
        memset(_out_block, 0, sizeof(cMessageCan));
        _protocol = eProtCAN;
        _common_state._bscan_session_id.push_back(0);     // Сессия данных В-развертки

        // Fill the response map
        // А-развертка +
        _response_map[can_utils::dAScanDataHead]      = (&UMU_CAN::parseResponse_dAScanDataHead);
        _response_map[can_utils::dAScanMeasForCurr]   = (&UMU_CAN::parseResponse_dAScanMeasForCurr);
        _response_map[can_utils::dAScanMeasForZero]   = (&UMU_CAN::parseResponse_dAScanMeasForZero);
        _response_map[can_utils::dAScanMeasFirstPack] = (&UMU_CAN::parseResponse_dAScanMeasFirstPack);
        _response_map[can_utils::dAScanMeasOtherPack] = (&UMU_CAN::parseResponse_dAScanMeasOtherPack);
        // B и M-развертки +
        _response_map[can_utils::dBScanData]          = (&UMU_CAN::parseResponse_dBScanData);
        _response_map[can_utils::dMScanData]          = (&UMU_CAN::parseResponse_dMScanData);
        // Управление БУМ +
        _response_map[can_utils::dAlarmData]          = (&UMU_CAN::parseResponse_dAlarmData);
        _response_map[can_utils::dPathStep]           = (&UMU_CAN::parseResponse_dPathStep);
        _response_map[can_utils::dPathStepAddit]      = (&UMU_CAN::parseResponse_dPathStepAddit);
        _response_map[can_utils::dVoltage]            = (&UMU_CAN::parseResponse_dVoltage);
        _response_map[can_utils::dWorkTime]           = (&UMU_CAN::parseResponse_dWorkTime);
        // Общие команды +
        _response_map[can_utils::cDevice]             = (&UMU_CAN::parseResponse_cDevice);
        _response_map[can_utils::cFWver]              = (&UMU_CAN::parseResponse_cFWver);
        _response_map[can_utils::cSerialNumber]       = (&UMU_CAN::parseResponse_cSerialNumber);
        _common_state._created = true;
    }
    catch (...)
    {
        /* Exception will stop there! */
        _common_state._created = false;
    }
}
// +
void UMU_CAN::sendEmptyCommand(const unsigned char command_id)
{
    sendCommandWithData(command_id, 0, 0);
}
// +
void UMU_CAN::sendCommandWithOneByte(const unsigned char command_id, const unsigned char byte)
{
    const unsigned short argc = 1;
    unsigned char argv[argc];
    argv[0] = byte;

    sendCommandWithData(command_id, argv, argc);
}
// +
void UMU_CAN::sendCommandWithData(const unsigned char command_id, const unsigned char *data, const unsigned char size)
{
    memset(_out_block, 0, sizeof((*_out_block)));

    can_utils::SetCommandId  (*_out_block, command_id    );
    can_utils::SetDeviceId   (*_out_block, can_utils::UMU);
    can_utils::SetDataAndSize(*_out_block, data, size    );

    AddToOutBuff();
}
// +
void UMU_CAN::SetStrokeCount(unsigned char count)
{
    sendCommandWithOneByte(can_utils::uStrokeCount, count);
    _common_state._stroke_count = count;
}
// +
void UMU_CAN::SetGainSegment(eUMUSide      side,
                              unsigned char line,
                              unsigned char stroke,
                              unsigned char st_delay,
                              unsigned char st_val,
                              unsigned char ed_delay,
                              unsigned char ed_val)
{
    assert(stroke <= _common_state._stroke_count);

    const unsigned short argc = 7; // count of args
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x40 : 0 );

    argv[0] = (uc_side & stroke);
    argv[1] = st_delay; // low byte
    argv[2] = 0;        // hight byte, unused
    argv[3] = st_val;
    argv[4] = ed_delay; // low byte
    argv[5] = 0;        // hight byte, unused
    argv[6] = ed_val;

    sendCommandWithData(can_utils::uGainSeg, argv, argc);

    if (st_delay != ed_delay)
    {
        for (int delay = st_delay; delay <= ed_delay; ++delay)
        {
            _common_state._tvg_curve[(bool)uc_side][line][stroke][delay] = (unsigned char)(
                        (float)st_val
                        + (float)((float)ed_val   - (float)st_val)
                        * (float)((float)delay    - (float)st_delay)
                        / (float)((float)ed_delay - (float)st_delay));
        }
    }
    else
    {
        _common_state._tvg_curve[(bool)uc_side][line][stroke][st_delay] = st_val;
    }

    if ((_common_state._save_ascan_zoom != 0)
            && (stroke == _common_state._save_ascan_stroke)
            && (side == _save_ascan_side)
            && (line == _common_state._save_ascan_line))
    {
        RequestTVGCurve();
    }
}
// +
void UMU_CAN::SetGate(eUMUSide        side,
                       unsigned char   line,
                       unsigned char   stroke,
                       unsigned char   index,
                       unsigned char   st_delay,
                       unsigned char   ed_delay,
                       unsigned char   level,
                       eStrobeMode     mode,
                       eAlarmAlgorithm asd_mode)
{
    (void)line;
    assert(stroke <= _common_state._stroke_count);

    const unsigned short argc = 8; // count of args
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x40 : 0 );

    argv[0] = (uc_side & stroke);
    argv[1] = index;
    argv[2] = st_delay; // low byte
    argv[3] = 0;        // hight byte, unused
    argv[4] = ed_delay; // low byte
    argv[5] = 0;        // hight byte, unused
    argv[6] = ((level & 0xFE) + mode);
    argv[7] = asd_mode;

    sendCommandWithData(can_utils::uGate, argv, argc);
}
// +
void UMU_CAN::SetPrismDelay(unsigned char  stroke,
                             unsigned short left_delay,
                             unsigned short right_delay)
{
    assert(stroke <= _common_state._stroke_count);

    const unsigned short argc = 5; // count of args
    unsigned char argv[argc];

    argv[0] = stroke;
    argv[1] = (left_delay  & 0x00FF); // low byte
    argv[2] = (right_delay & 0x00FF); // low byte
    argv[3] = (left_delay >> 8);      // hight byte
    argv[4] = (right_delay >> 8);     // hight byte

    sendCommandWithData(can_utils::uPrismDelay, argv, argc);
}
// +
void UMU_CAN::EnableAScan(eUMUSide       side,
                           unsigned char  line,
                           unsigned char  stroke,
                           unsigned short st_delay,
                           unsigned char  zoom,
                           unsigned char  mode,
                           unsigned char  strob)
{
    (void)line;

    assert(stroke <= _common_state._stroke_count);

    const unsigned short argc = 6;
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x40 : 0 );
    argv[0] = (uc_side & stroke);
    argv[1] = st_delay;
    argv[2] = 0;
    argv[3] = zoom;
    argv[4] = mode;
    argv[5] = strob;

    sendCommandWithData(can_utils::uEnableAScan, argv, argc);
}
// +
void UMU_CAN::PathSimulator(unsigned char number, bool state)
{
    (void)number;

    // number is unused
    sendCommandWithOneByte(can_utils::uPathSimulator, state);
}
// +
void UMU_CAN::setStrokeParameter(const unsigned char stroke_id,
                                  const unsigned char offset,
                                  const unsigned char param)
{
    const unsigned short argc = 3;
    unsigned char argv[argc];

    argv[0] = stroke_id;
    argv[1] = offset;
    argv[2] = param;

    sendCommandWithData(can_utils::uParameter, argv, argc);
}
// +
void UMU_CAN::SetStrokeParameters(const tStrokeParams *stroke_params)
{
    assert(stroke_params != 0);

    const unsigned char id = stroke_params->StrokeIdx;

    //setStrokeParameter(id, 0x00, 0x28); // Всегда 0x28
    setStrokeParameter(id, 0x01, stroke_params->GenResLeftLine1);       // Генератор/приемник для левой нити
    setStrokeParameter(id, 0x02, stroke_params->GenResRightLine1);      // Генератор/приемник для правой нити
    // 0x03 и 0x04 reserved
    const unsigned short addr = 0x3000 + id * 0x1000;                   // Раcчет рабочего адреса такта
    setStrokeParameter(id, 0x05, 0);    // Всегда 0                     // Рабочий адрес такта, младший байт
    setStrokeParameter(id, 0x06, (addr >> 8));                          // Рабочий адрес такта, старший байт
    setStrokeParameter(id, 0x07, stroke_params->Duration);              // Длительность развекртки t, мкс
    setStrokeParameter(id, 0x08, stroke_params->BLevelLeftLine1);       // Уровень строба B-развертки левой нити
    setStrokeParameter(id, 0x09, stroke_params->Gate1LevelLeftLine1);   // Уровень строба 1 левой нити
    setStrokeParameter(id, 0x0A, stroke_params->Gate2LevelLeftLine1);   // Уровень строба 2 левой нити
    setStrokeParameter(id, 0x0B, stroke_params->Gate3LevelLeftLine1);   // Уровень строба 3 левой нити
    setStrokeParameter(id, 0x0C, stroke_params->BLevelRightLine1);      // Уровень строба B-развертки левой нити
    setStrokeParameter(id, 0x0D, stroke_params->Gate1LevelRightLine1);  // Уровень строба 1 левой нити
    setStrokeParameter(id, 0x0E, stroke_params->Gate2LevelRightLine1);  // Уровень строба 2 левой нити
    setStrokeParameter(id, 0x0F, stroke_params->Gate3LevelRightLine1);  // Уровень строба 3 левой нити
    setStrokeParameter(id, 0x10, stroke_params->ZondAmplLeftLine1);     // Амплитуда зондирующего импульса для левой нити
    setStrokeParameter(id, 0x11, stroke_params->ZondAmplRightLine1);    // Амплитуда зондирующего импульса для правой нити
    setStrokeParameter(id, 0x12, stroke_params->DelayCommon);           // Множитель задержек сигналов (общий)
}
// +
void UMU_CAN::DisableAScan()
{
    sendCommandWithOneByte(can_utils::wmDisableAScan, 0);
}
// +
int UMU_CAN::EnableBScan()
{
    sendEmptyCommand(can_utils::wmEnableBScan);

                                                  // Сессия данных В-развертки
    _common_state._bscan_session_id.push_back(_common_state._bscan_session_id.back() + 1); // Увеличиваем номер сесии данных В-развертки
    return _common_state._bscan_session_id.back();
}
// +
void UMU_CAN::DisableBScan()
{
    sendEmptyCommand(can_utils::wmDisableBScan);
}
// +
void UMU_CAN::EnableAlarm()
{
    sendEmptyCommand(can_utils::wmEnableAlarm);
}
// +
void UMU_CAN::Enable()
{
    sendCommandWithOneByte(can_utils::wmEnable, 1);
}
// +
void UMU_CAN::Disable()
{
    sendCommandWithOneByte(can_utils::wmDisable, 1);
}
// +
void UMU_CAN::Reboot()
{
    sendEmptyCommand(can_utils::wmReboot);
}
// +
void UMU_CAN::GetWorkTime()
{
    sendCommandWithOneByte(can_utils::dWorkTime, 0x03);
}
// +
void UMU_CAN::GetDeviceID()
{
    sendEmptyCommand(can_utils::wmDeviceID);
}
// +
void UMU_CAN::GetUAKK()
{
    sendCommandWithOneByte(can_utils::wmGetVoltage, 0x03);
}
// +
void UMU_CAN::RequestTVGCurve()
{
    if (!isPointerInitialized(_common_state._critical_section)
            ||
            !isPointerInitialized(_common_state._data_sender))
    {
        return;
    }

    _common_state._critical_section->Enter();

    try
    {
        CalcTVGCurve();

        if (_common_state._tvg_data_buf == 0)
        {
            _common_state._tvg_data_buf = new (std::nothrow) tUMU_AScanData;
            if (_common_state._tvg_data_buf == 0)
            {
                _common_state._critical_section->Release();
                return;
            }
        }

        _common_state._tvg_data_buf->UMUIdx = _common_state._umu_id;

        memcpy(_common_state._tvg_data_buf,
               &_common_state._tvg_data,
               sizeof(_common_state._tvg_data));

        _common_state._data_sender->CreateNewEvent(edTVGData);
        _common_state._data_sender->WriteEventData(&_common_state._tvg_data_buf,
                                                   SIZE_OF_TYPE(&_common_state._tvg_data_buf));
        ++_common_state._post_count;
        _common_state._data_sender->Post();
    }
    catch (...) { /* Exception will stop there! */ }

    //delete _common_state._tvg_data_buf;
    _common_state._tvg_data_buf = 0;
    _common_state._critical_section->Release();
}
// +
void UMU_CAN::CalcTVGCurve()
{
    if (_common_state._save_ascan_zoom == 0)
    {
        return;
    }

    int delay_id1 = 0;
    int delay_id2 = 0;
    int delay     = 0;

    _common_state._tvg_data.Side   = _save_ascan_side;
    _common_state._tvg_data.Line   = _common_state._save_ascan_line;
    _common_state._tvg_data.Stroke = _common_state._save_ascan_stroke;

    // Число отсчетов в кадре А развертки
    const unsigned char cAScanCountsInFrame = 232 ;
    while (true)
    {
        delay_id1 = (int)((10 * delay) / _common_state._save_ascan_zoom);

        for (int i = delay_id2; i <= delay_id1; i++)
        {
            if (i < cAScanCountsInFrame)
            {
                _common_state._tvg_data.Data[i] = _common_state._tvg_curve
                        [!_save_ascan_side]
                        [_common_state._save_ascan_line]
                        [_common_state._save_ascan_stroke]
                        [delay];
            }
        }
        if (delay_id1 > cAScanCountsInFrame)
        {
            break;
        }

        delay_id2 = delay_id1 + 1;
        ++delay;
    }
}

// ---------------------------- RESPONSE PARSERS -------------------------------
// + // Правильно ли я не использую _save_ascan_stroke ?
bool UMU_CAN::parseResponse_dAScanDataHead(const cMessageCan &buf)
{
    const unsigned char *data = can_utils::GetData(buf);
    switch (data[0] & 0x80)
    {
    case 0x00: { _save_ascan_side = usLeft;   break; }
    case 0x80: { _save_ascan_side = usRight;  break; }
    default:
        return false;
    }

    _special_state._ascan_curr_num = 0;
    _special_state._ascan_curr_stroke = data[0] & 0x7F; //_common_state._save_ascan_stroke

    return true;
}
// +
bool UMU_CAN::parseResponse_dAScanMeasForCurr(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    _common_state._critical_section->Enter();

    try
    {
        if (_common_state._ascan_measure_buf == 0)
        {
            _common_state._ascan_measure_buf = new (std::nothrow) tUMU_AScanMeasure;
            if (_common_state._ascan_measure_buf == 0)
            {
                _common_state._critical_section->Release();
                return false;
            }
        }

        const unsigned char *data = can_utils::GetData(buf);
        _common_state._ascan_measure_buf->UMUIdx = _common_state._umu_id;
        _common_state._ascan_measure_buf->Stroke = data[3] & 0x7F;

        switch (data[3] & 0x80)
        {
        case 0x00: { _common_state._ascan_measure_buf->Side = usLeft;   break; }
        case 0x80: { _common_state._ascan_measure_buf->Side = usRight;  break; }
        default:
            _common_state._critical_section->Release();
            return false;
        }

        _common_state._ascan_measure_buf->Line = 0;
        _common_state._ascan_measure_buf->ParamA = data[0];
        _common_state._ascan_measure_buf->ParamM = data[1] + (float)(data[2] * 0.1);

        _common_state._data_sender->CreateNewEvent(edAScanMeas);
        _common_state._data_sender->WriteEventData(&_common_state._ascan_measure_buf,
                                                   SIZE_OF_TYPE(&_common_state._ascan_measure_buf));
        ++_common_state._post_count;
        _common_state._data_sender->Post();
    }
    catch (...) { /* Exception will stop there! */ }

    //delete _common_state._ascan_measure_buf;
    _common_state._ascan_measure_buf = 0;
    _common_state._critical_section->Release();
    return true;
}
// + Empty body
bool UMU_CAN::parseResponse_dAScanMeasForZero(const cMessageCan &buf)
{
    (void)buf;

    return false;
}
// +
bool UMU_CAN::parseResponse_dAScanMeasFirstPack(const cMessageCan &buf)
{
    // Первая посылка в данном протоколе аналогична последующим
    return parseResponse_dAScanMeasOtherPack(buf);
}
// +/- unknown_const
bool UMU_CAN::parseResponse_dAScanMeasOtherPack(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    _common_state._critical_section->Enter();

    try
    {
        if (_common_state._ascan_data_buf == 0)
        {
            _common_state._ascan_data_buf = new (std::nothrow) tUMU_AScanData;
            if (_common_state._ascan_data_buf == 0)
            {
                _common_state._critical_section->Release();
                return false;
            }
        }

        const unsigned char *data = can_utils::GetData(buf);
        const unsigned char  size = can_utils::GetDataSize(buf);
        unsigned long t = GetTickCount_();
        const int unknown_const = 4 * 19;

        memcpy(&(_common_state._last_d[1]),
                &(_common_state._last_d[0]),
                unknown_const);

        _common_state._last_d[0] = abs(t - _common_state._last_t);
        _common_state._last_t = t;

        _common_state._ascan_data_buf->UMUIdx = _common_state._umu_id;
        _common_state._ascan_data_buf->Stroke = _special_state._ascan_curr_stroke;
        _common_state._ascan_data_buf->Time   = GetTickCount_();
        _common_state._ascan_data_buf->Time2  = _common_state._ascan_data_buf->Time - _common_state._last_time;
        _common_state._last_time = _common_state._ascan_data_buf->Time;
        _common_state._ascan_data_buf->Side   = _save_ascan_side;
        _common_state._ascan_data_buf->Line   = 0;

        t = GetTickCount_();
        memcpy(&(_common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][1]),
                &(_common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][0]),
                unknown_const);

        _common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][0] =
                abs(t - _common_state._last_t2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line]);

        _common_state._last_t2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line] = t;
        _common_state._ascan_debug_arr[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line] = GetTickCount_();

        memcpy(&_common_state._ascan_data_buf->Data[_special_state._ascan_curr_num * 8],
                &data[0],
                size); // 8 число байт отсчетов в пакете


        ++_special_state._ascan_curr_num;

        if (_special_state._ascan_curr_num == 29) // 29 - число пакетов по 8 отсчетов для передачи всех 232 отсчетов
        {
            _special_state._ascan_curr_num = 0;
            _common_state._data_sender->CreateNewEvent(edAScanData);
            _common_state._data_sender->WriteEventData(&_common_state._ascan_data_buf,
                                                       SIZE_OF_TYPE(&_common_state._ascan_data_buf));

            ++_common_state._post_count;
            _common_state._data_sender->Post();
            //delete _common_state._ascan_data_buf;
            _common_state._ascan_data_buf = 0;
        }
    }
    catch (...) { /* Exception will stop there! */ }
    _common_state._critical_section->Release();

    return true;
}
// +
bool UMU_CAN::parseResponse_dBScanData(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    if (_special_state._bscan_buf == 0)
    {
        return false;
    }

    const unsigned char *data = can_utils::GetData(buf);
    const unsigned char  size = can_utils::GetDataSize(buf);
    bool side   = !(data[0] & 0x80);
    int  stroke = (data[0] & 0x7F);
    int  id     = 0;

    switch (size)
    {
    case 0x03:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[2] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];
        break;
    }
    case 0x04:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[3] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[3] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];
        break;
    }
    case 0x06:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[4] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[4] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[3];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        break;
    }
    case 0x07:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[3];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[6] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Delay = data[4];
        _special_state._bscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[6] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][stroke];

        break;
    }
    }

    return true;
}
// +
bool UMU_CAN::parseResponse_dMScanData(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    // Если пришла М развертка, а буффер под нее не выделен
    if (_special_state._mscan_buf == 0)
    {
        _special_state._mscan_buf = new (std::nothrow) tUMU_OldBScanData;

        // Если все-таки не удалось выделить память
        if (_special_state._mscan_buf == 0)
        {
            return false;
        }

        _special_state._mscan_buf->UMUIdx = _common_state._umu_id;
        memset(&_special_state._mscan_buf->Signals,
               0,
               sizeof(tUMU_OldBScanSignals));

        _special_state._mscan_buf->Dir[0] = 0;
        _special_state._mscan_buf->XSysCrd[0] = 0;
    }

    const unsigned char *data = can_utils::GetData(buf);
    const unsigned char  size = can_utils::GetDataSize(buf);
    bool side   = !(data[0] & 0x80);
    int  stroke = (data[0] & 0x7F);
    int  id     = 0;

    switch (size)
    {
    case 0x03:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[2] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];
        break;
    }
    case 0x04:
    {
        id = _special_state._bscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[3] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[3] & 0x0F);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];
        break;
    }
    case 0x06:
    {
        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[4] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[4] & 0x0F);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[3];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        break;
    }
    case 0x07:
    {
        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[1];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[2];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[5] & 0x0F);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[3];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[6] >> 4);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        id = _special_state._mscan_buf->Signals.Count[side][stroke];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Delay = data[4];
        _special_state._mscan_buf->Signals.Data[side][stroke][id].Ampl  = (data[6] & 0x0F);
        ++_special_state._mscan_buf->Signals.Count[side][stroke];

        break;
    }
    }

    return true;
}
// +/- if (stroke == _common_state._stroke_count)
bool UMU_CAN::parseResponse_dAlarmData(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    if (_common_state._alarm_buf == 0)
    {
        _common_state._alarm_buf = new (std::nothrow) tUMU_AlarmItem;
        if (_common_state._alarm_buf == 0)
        {
            return false;
        }
    }

    const unsigned char *data = can_utils::GetData(buf);

    _common_state._alarm_buf->UMUIdx = _common_state._umu_id;
    int stroke = (data[1] & 0x7F);
    _common_state._alarm_buf->Count = stroke; // TODO: Протестировать, будет ли так работать

    _common_state._alarm_buf->State[0][0][stroke][0] = ((data[0] & BIN8(00000001)) != 0);
    _common_state._alarm_buf->State[0][0][stroke][1] = ((data[0] & BIN8(00000010)) != 0);
    _common_state._alarm_buf->State[0][0][stroke][2] = ((data[0] & BIN8(00000100)) != 0);
    _common_state._alarm_buf->State[0][0][stroke][3] = ((data[0] & BIN8(00001000)) != 0);
    _common_state._alarm_buf->State[1][0][stroke][0] = ((data[0] & BIN8(00010000)) != 0);
    _common_state._alarm_buf->State[1][0][stroke][1] = ((data[0] & BIN8(00100000)) != 0);
    _common_state._alarm_buf->State[1][0][stroke][2] = ((data[0] & BIN8(01000000)) != 0);
    _common_state._alarm_buf->State[1][0][stroke][3] = ((data[0] & BIN8(10000000)) != 0);

    if (stroke == _common_state._stroke_count)
    {
        _common_state._critical_section->Enter();

        try
        {
            _common_state._data_sender->CreateNewEvent(edAlarmData);
            _common_state._data_sender->WriteEventData(&_common_state._alarm_buf,
                                                       SIZE_OF_TYPE(&_common_state._alarm_buf));

            ++_common_state._post_count;
            _common_state._data_sender->Post();
        }
        catch (...) { /*Exception will stop there*/ }

        //delete _common_state._alarm_buf;
        _common_state._alarm_buf = 0;
        _common_state._critical_section->Release();
    }

    return true;
}
// +
bool UMU_CAN::parseResponse_dPathStep(const cMessageCan &buf)
{
    if (!isPointerInitialized(_common_state._critical_section)
            || (!isPointerInitialized(_common_state._data_sender)))
    {
        return false;
    }

    _common_state._critical_section->Enter();

    try
    {
        int old_XSysCrd = 0;
        if (_special_state._bscan_buf != 0)
        {
            old_XSysCrd =_special_state._bscan_buf->XSysCrd[0];
            _common_state._data_sender->CreateNewEvent(edBScanData);
            _common_state._data_sender->WriteEventData(&_special_state._bscan_buf,
                                                       SIZE_OF_TYPE(&_special_state._bscan_buf));
            ++_common_state._post_count;
            _common_state._data_sender->Post();

            //delete _special_state._bscan_buf;
            _special_state._bscan_buf = 0;
        }

        // no else!
        if (_special_state._bscan_buf == 0)
        {
            _special_state._bscan_buf = new (std::nothrow) tUMU_OldBScanData;
            if (_special_state._bscan_buf == 0)
            {
                _common_state._critical_section->Release();
                return false;
            }
        }

        _special_state._bscan_buf->UMUIdx = _common_state._umu_id;
        memset(&_special_state._bscan_buf->Signals,
               0,
               sizeof(tUMU_OldBScanSignals));

        const unsigned char *data = can_utils::GetData(buf);
        _special_state._bscan_buf->Dir[0] = ((data[0] <= 127) ? data[0] : data[0] - 256);
        _special_state._bscan_buf->XSysCrd[0] = old_XSysCrd + _special_state._bscan_buf->Dir[0];

    }
    catch(...) { /* Exception will stop there */ }

    _common_state._critical_section->Release();
    return true;
}
// + Empty body
bool UMU_CAN::parseResponse_dPathStepAddit(const cMessageCan &buf)
{
    (void)buf;

    return false;
}
// + Empty body
bool UMU_CAN::parseResponse_dVoltage(const cMessageCan &buf)
{
    (void)buf;

    return false;
}
// +
bool UMU_CAN::parseResponse_dWorkTime(const cMessageCan &buf)
{
    const unsigned char *data = can_utils::GetData(buf);
    _common_state._work_time =
            ((unsigned)data[0] | ((unsigned)data[1] << 8) | ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24));
    return true;
}
// + Empty body
bool UMU_CAN::parseResponse_cDevice(const cMessageCan &buf)
{
    (void)buf;

    return false;
}
// +
bool UMU_CAN::parseResponse_cFWver(const cMessageCan &buf)
{
    _common_state._firmware_info_time = GetTickCount_();
    const unsigned char *data = can_utils::GetData(buf);

    _common_state._firmware_version_lo = data[0] & 0x0F; // Дробная часть номера версии ПО контроллера
    _common_state._firmware_version_hi = data[1];	 // Целая часть номера версии ПО контроллера

    _special_state._ver_year = data[2];
    _special_state._ver_mon  = data[3];
    _special_state._ver_day  = data[4];
    _special_state._ver_hour = data[5];
    _special_state._ver_min  = data[6];
    _special_state._ver_sec  = data[7];

    return true;
}
// +
bool UMU_CAN::parseResponse_cSerialNumber(const cMessageCan &buf)
{
    _common_state._serial_number_time = GetTickCount_();
    const unsigned char *data = can_utils::GetData(buf);
    _common_state._serial_number = (data[0] + (data[1] << 8));

    return true;
}

