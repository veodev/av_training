#include "prot_umu/prot_umu_rawcan.h"
#include <cmath>
//#include <debug.h>

namespace CANProtDebug
{
LANProtDebug::Add_Log onAddLog = NULL;
bool useLog = true;

// +
void UMU_RAWCAN::AddToOutBuff()
{
    /*    if (!isPointerInitialized(_common_state._critical_section)) {
            return;
        }

        _common_state._critical_section->Enter();
        _out_buffer.push(*_out_block);
        memset(_out_block, 0, sizeof(cMessageCan));

        _common_state._critical_section->Release();  */
}

void UMU_RAWCAN::AddToOutBuff_(cMessageCan* _out_block)
{
    if (!isPointerInitialized(_common_state._critical_section)) {
        return;
    }

    _common_state._critical_section->Enter();

    _out_buffer.push(*_out_block);

    _common_state._critical_section->Release();
}

// +
void UMU_RAWCAN::Unload()
{
    if (!isPointerInitialized(_common_state._critical_section) || !isPointerInitialized(_common_state._data_transfer)) {
        return;
    }

    _common_state._critical_section->Enter();

    while (!_out_buffer.empty()) {
        bool result = _common_state._data_transfer->write(_common_state._connection_id, &(_out_buffer.front()._msg[0]), GetMessageSize(_out_buffer.front()));
        if (result) {
#ifdef DbgLog
            if ((useLog) && (onAddLog)) onAddLog(_common_state._connection_id, (unsigned char*) &_out_buffer.front(), 2);
#endif
            _out_buffer.pop();
        }
        else {
            ++_common_state._write_error_count;
            break;
        }
    }

    _common_state._critical_section->Release();
}
// +
void UMU_RAWCAN::TickSend()
{
    switch (_wstate) {
    case bsOff: {
        break;
    }
    case bsInit: {
        _wstate = bsPowerOn;
        break;
    }
    case bsPowerOn: {
        _wstate = bsRequest;
        _read_state = rsHead;
        break;
    }
    case bsRequest: {
        _common_state._request_time = GetTickCount_();
        _special_state._akk_voltage_time = _common_state._request_time;
        _common_state._event_count[can_utils::wmGetVoltage] = 0;
        GetUAkkQuery();
        _wstate = bsWaitAnswer;
        break;
    }
    case bsWaitAnswer: {
        Unload();
        if (GetTickCount_() - _common_state._request_time > 500) _wstate = bsRequest;

        if (_common_state._event_count[can_utils::wmGetVoltage] > 0) {
            _wstate = bsReady;
        }

        break;
    }
    case bsReady: {
        Unload();
        if (GetTickCount_() - _common_state._request_time > 2000) {
            _wstate = bsRequest;
        }
        break;
    }
    case bsStop: {
        break;
    }
    }
}
//
UMU_RAWCAN::UMU_RAWCAN()
    : cIUMU()
{
    // Инициализация нулями для всех переменных из _special_state
    memset(&_special_state, 0, sizeof(cSpecialState));
    //_out_block = new cMessageCan;
    // memset(_out_block, 0, sizeof(cMessageCan));

    // Fill the response map
    // А-развертка +
    _response_map[can_utils::dAScanDataHead] = (&UMU_RAWCAN::parseResponse_dAScanDataHead);
    _response_map[can_utils::dAScanMeasForCurr] = (&UMU_RAWCAN::parseResponse_dAScanMeasForCurr);
    _response_map[can_utils::dAScanMeasForZero] = (&UMU_RAWCAN::parseResponse_dAScanMeasForZero);
    _response_map[can_utils::dAScanMeasFirstPack] = (&UMU_RAWCAN::parseResponse_dAScanMeasFirstPack);
    _response_map[can_utils::dAScanMeasOtherPack] = (&UMU_RAWCAN::parseResponse_dAScanMeasOtherPack);
    // B и M-развертки +
    _response_map[can_utils::dBScanData] = (&UMU_RAWCAN::parseResponse_dBScanData);
    _response_map[can_utils::dMScanData] = (&UMU_RAWCAN::parseResponse_dMScanData);
    // Управление БУМ +
    _response_map[can_utils::dAlarmData] = (&UMU_RAWCAN::parseResponse_dAlarmData);
    _response_map[can_utils::dPathStep] = (&UMU_RAWCAN::parseResponse_dPathStep);
    _response_map[can_utils::dPathStepAddit] = (&UMU_RAWCAN::parseResponse_dPathStepAddit);
    _response_map[can_utils::dVoltage] = (&UMU_RAWCAN::parseResponse_dVoltage);
    _response_map[can_utils::dWorkTime] = (&UMU_RAWCAN::parseResponse_dWorkTime);
    // Общие команды +
    _response_map[can_utils::cDevice] = (&UMU_RAWCAN::parseResponse_cDevice);
    _response_map[can_utils::cFWver] = (&UMU_RAWCAN::parseResponse_cFWver);
    _response_map[can_utils::cSerialNumber] = (&UMU_RAWCAN::parseResponse_cSerialNumber);
    _response_map[can_utils::wmDisableBScan] = (&UMU_RAWCAN::parseResponse_BScanStopped);
    _common_state._created = true;
    _bscan_data_type = bstSimple;
}
// +
void UMU_RAWCAN::sendEmptyCommand(const unsigned char command_id)
{
    sendCommandWithData(command_id, 0, 0);
}
// +
void UMU_RAWCAN::sendCommandWithOneByte(const unsigned char command_id, const unsigned char byte)
{
    const unsigned short argc = 1;
    unsigned char argv[argc];
    argv[0] = byte;

    sendCommandWithData(command_id, argv, argc);
}

// +
void UMU_RAWCAN::SetStrokeCount(unsigned char count)
{
    _common_state.BSFilterStrokeData.clear();

    sendCommandWithOneByte(can_utils::uStrokeCount, count);
    _common_state._stroke_count = count;
}
// +
/*
void UMU_RAWCAN::SetGainSegment(eUMUSide      side,
                              unsigned char line,
                              unsigned char stroke,
                              unsigned char st_delay,
                              unsigned char st_val,
                              unsigned char ed_delay,
                              unsigned char ed_val,
                              unsigned char ascan_scale)
{
    assert(stroke < _common_state._stroke_count);

    const unsigned short argc = 7; // count of args
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x80 : 0 );

    argv[0] = (uc_side | stroke);
    argv[1] = st_delay; // low byte
    argv[2] = 0;        // hight byte, unused
    argv[3] = st_val;
    argv[4] = ed_delay; // low byte
    argv[5] = 0;        // hight byte, unused
    argv[6] = ed_val;

    sendCommandWithData(can_utils::uGainSeg, argv, argc);

    unsigned char stPixel = static_cast<unsigned char>((10.0f * st_delay) / static_cast<float>(ascan_scale));
    unsigned char edPixel = static_cast<unsigned char>((10.0f * ed_delay) / static_cast<float>(ascan_scale));

    if (stPixel != edPixel) {
        for (unsigned char Pixel = stPixel; Pixel <= edPixel; Pixel++)
        {
            _common_state._tvg_curve[side][line][stroke][Pixel] =   static_cast<unsigned char>(st_val +
                                                                                               static_cast<float>((ed_val - st_val) * (Pixel - stPixel)) /
                                                                                               static_cast<float>(edPixel - stPixel));
        }
    } else _common_state._tvg_curve[side][line][stroke][stPixel] = st_val;


    if ((_common_state._save_ascan_zoom != 0)
            && (stroke == _common_state._save_ascan_stroke)
            && (side == _common_state._save_ascan_side)
            && (line == _common_state._save_ascan_line))
    {
        RequestTVGCurve();
    }
}
*/

//---------------------------------------------------------------------------
void UMU_RAWCAN::SetGainSegmentPoint(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay, unsigned char gain, unsigned char ascan_scale,
                                     eGainPointType point)  //
{
    if (point == gptStart) {
        _common_state.TVG_point_Gain = gain;
        _common_state.TVG_point_Delay = delay;
        _common_state.TVG_point_Pixel = std::min(static_cast<unsigned char>(231), static_cast<unsigned char>((10.0f * delay) / static_cast<float>(ascan_scale)));
    }
    else if ((point == gptCentr) || (point == gptEnd)) {
        assert(stroke < _common_state._stroke_count);

        const unsigned short argc = 7;  // count of args
        unsigned char argv[argc];

        unsigned char uc_side = (side == usRight ? 0x80 : 0);

        argv[0] = (uc_side | stroke);
        argv[1] = _common_state.TVG_point_Delay;  // low byte
        argv[2] = 0;                              // hight byte, unused
        argv[3] = _common_state.TVG_point_Gain;
        argv[4] = delay;  // low byte
        argv[5] = 0;      // hight byte, unused
        argv[6] = gain;

        sendCommandWithData(can_utils::uGainSeg, argv, argc);


        unsigned char edPixel = std::min(static_cast<unsigned char>(231), static_cast<unsigned char>((10.0f * delay) / static_cast<float>(ascan_scale)));

        unsigned char* currentTVG = _common_state._tvg_curve[side][line][stroke];
        if (currentTVG == NULL) {
            _common_state._tvg_curve[side][line][stroke] = new unsigned char[232];
            currentTVG = _common_state._tvg_curve[side][line][stroke];
            memset(currentTVG, 0, 232);
        }

        for (unsigned char Pixel = _common_state.TVG_point_Pixel; Pixel <= edPixel; Pixel++) {
            if (edPixel - _common_state.TVG_point_Pixel != 0)
                currentTVG[Pixel] = static_cast<unsigned char>(_common_state.TVG_point_Gain + static_cast<float>((gain - _common_state.TVG_point_Gain) * (Pixel - _common_state.TVG_point_Pixel)) / static_cast<float>(edPixel - _common_state.TVG_point_Pixel));
            else
                currentTVG[Pixel] = gain;
        }
        _common_state.TVG_point_Gain = gain;
        _common_state.TVG_point_Delay = delay;
        _common_state.TVG_point_Pixel = edPixel;
    };

    if (point == gptEnd)
        if ((_common_state._save_ascan_zoom != 0) && (stroke == _common_state._save_ascan_stroke) && (side == _common_state._save_ascan_side) && (line == _common_state._save_ascan_line)) {
            RequestTVGCurve();
        }
}

// +
void UMU_RAWCAN::SetGate(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char index, unsigned char st_delay, unsigned char ed_delay, unsigned char level, eStrobeMode mode, eAlarmAlgorithm asd_mode)
{
    (void) line;
    assert(stroke < _common_state._stroke_count);

    const unsigned short argc = 8;  // count of args
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x80 : 0);

    argv[0] = (uc_side | stroke);
    argv[1] = index;
    argv[2] = st_delay;  // low byte
    argv[3] = 0;         // hight byte, unused
    argv[4] = ed_delay;  // low byte
    argv[5] = 0;         // hight byte, unused
    argv[6] = ((level & 0xFE) + mode);
    argv[7] = asd_mode;

    sendCommandWithData(can_utils::uGate, argv, argc);
}
void UMU_RAWCAN::SetPrismDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short delay)
{
    (void) line;
    unsigned int sideIdx = (side == static_cast<eUMUSide>(dsRight)) ? 0 : 1;

    assert(stroke < MaxStrokeCount);
    _special_state._prismDelayTime[sideIdx][stroke]._longType = delay;
    if (_special_state._prismDelayTimeType == ePrismDelayShort) {
        assert(delay <= 255);
        SetPrismDelay(stroke, (_special_state._prismDelayTime[1][stroke])._shortType, (_special_state._prismDelayTime[0][stroke])._shortType);
    }
    else {
        SetPrismDelay(stroke, (_special_state._prismDelayTime[1][stroke])._longType, (_special_state._prismDelayTime[0][stroke])._longType);
    }
}
// +
void UMU_RAWCAN::SetPrismDelay(unsigned char stroke, unsigned char left_delay, unsigned char right_delay)
{
    assert(stroke < _common_state._stroke_count);

    const unsigned short argc = 3;  // count of args
    unsigned char argv[argc];

    argv[0] = stroke;
    argv[1] = left_delay;
    argv[2] = right_delay;
    sendCommandWithData(can_utils::uPrismDelay, argv, argc);
}
//
void UMU_RAWCAN::SetPrismDelay(unsigned char stroke, unsigned short left_delay, unsigned short right_delay)
{
    assert(stroke < _common_state._stroke_count);

    const unsigned short argc = 5;  // count of args
    unsigned char argv[argc];

    argv[0] = stroke;
    argv[1] = (left_delay & 0x00FF);   // low byte
    argv[2] = (right_delay & 0x00FF);  // low byte
    argv[3] = (left_delay >> 8);       // hight byte
    argv[4] = (right_delay >> 8);      // hight byte

    sendCommandWithData(can_utils::uPrismDelay, argv, argc);
}
// +
void UMU_RAWCAN::EnableAScan(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short st_delay, unsigned char zoom, unsigned char mode, unsigned char strob)
{
    (void) line;

    assert(stroke < _common_state._stroke_count);

    const unsigned short argc = 6;
    unsigned char argv[argc];

    unsigned char uc_side = (side == usRight ? 0x80 : 0);
    argv[0] = (uc_side | stroke);
    argv[1] = st_delay;
    argv[2] = 0;
    argv[3] = zoom;
    argv[4] = mode;
    argv[5] = strob;

    _common_state._save_ascan_side = side;
    _common_state._save_ascan_line = line;
    _common_state._save_ascan_stroke = stroke;
    _common_state._save_ascan_st_delay = st_delay;
    _common_state._save_ascan_zoom = zoom;
    _common_state._save_ascan_mode = mode;
    _common_state._save_ascan_strob = strob;


    sendCommandWithData(can_utils::uEnableAScan, argv, argc);

    RequestTVGCurve();
}
// +
void UMU_RAWCAN::PathSimulator(unsigned char number, bool state)
{
    (void) number;  // number is unused
    sendCommandWithOneByte(can_utils::uPathSimulator, state);
}
// +
void UMU_RAWCAN::setStrokeParameter(const unsigned char stroke_id, const unsigned char offset, const unsigned char param)
{
    const unsigned short argc = 3;
    unsigned char argv[argc];

    argv[0] = stroke_id;
    argv[1] = offset;
    argv[2] = param;

    sendCommandWithData(can_utils::uParameter, argv, argc);
}
// +
void UMU_RAWCAN::SetStrokeParameters(const tStrokeParams* stroke_params)
{
    assert(stroke_params != 0);

    if ((stroke_params->ParamsLeftLine1 & 0x01 == 0x01) ||
        (stroke_params->ParamsLeftLine2 & 0x01 == 0x01) ||
        (stroke_params->ParamsRightLine1 & 0x01 == 0x01) ||
        (stroke_params->ParamsRightLine2 & 0x01 == 0x01)) _common_state.BSFilterStrokeData.push_back(*stroke_params);

    const unsigned char id = stroke_params->StrokeIdx;

    // setStrokeParameter(id, 0x00, 0x28); // Всегда 0x28
    setStrokeParameter(id, 0x01, stroke_params->GenResLeftLine1);   // Генератор/приемник для левой нити
    setStrokeParameter(id, 0x02, stroke_params->GenResRightLine1);  // Генератор/приемник для правой нити
    // 0x03 и 0x04 reserved
    const unsigned short addr = 0x3000 + id * 0x1000;                   // Раcчет рабочего адреса такта
    setStrokeParameter(id, 0x05, 0);                                    // Всегда 0                     // Рабочий адрес такта, младший байт
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
void UMU_RAWCAN::DisableAScan(bool Force)
{
    (void)Force;
    sendCommandWithOneByte(can_utils::wmDisableAScan, 0);
}
// +
int UMU_RAWCAN::EnableBScan()
{
    if (!_common_state._bscan_state) {
        int res;
        SetMScanFormat(eMScanNew);
        sendEmptyCommand(can_utils::wmEnableBScan);
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
        else {
            _common_state._bscan_last_session_id = _common_state._bscan_session_id.back() + 1;
            _common_state._bscan_session_id.push_back(_common_state._bscan_last_session_id);  // Увеличиваем номер сесии данных В-развертки
        }

        res = _common_state._bscan_session_id.back();
        _common_state._critical_section->Release();
        return res;
    }
    else {
        if (_common_state._bscan_session_id.size() == 0) return -1;
        return _common_state._bscan_session_id.back();
    }
}
// +
void UMU_RAWCAN::DisableBScan(bool Force)
{
    if (_common_state._bscan_state || Force) {
        sendEmptyCommand(can_utils::wmDisableBScan);
        _common_state._bscan_state = false;
    }
}
// +
void UMU_RAWCAN::EnableAlarm()
{
    sendEmptyCommand(can_utils::wmEnableAlarm);
}
// +
void UMU_RAWCAN::Enable()
{
    sendCommandWithOneByte(can_utils::wmEnable, 1);
}
// +
void UMU_RAWCAN::Disable()
{
    sendCommandWithOneByte(can_utils::wmDisable, 1);
}
// +
void UMU_RAWCAN::Reboot()
{
    sendEmptyCommand(can_utils::wmReboot);
}
// +
void UMU_RAWCAN::GetWorkTime()
{
    sendCommandWithOneByte(can_utils::dWorkTime, 0x03);
}
// +
void UMU_RAWCAN::GetDeviceID()
{
    sendEmptyCommand(can_utils::wmDeviceID);
}
// +
void UMU_RAWCAN::GetUAkkQuery()
{
    sendCommandWithOneByte(can_utils::wmGetVoltage, 0x03);
}
// +
void UMU_RAWCAN::RequestTVGCurve()
{
    if (!isPointerInitialized(_common_state._critical_section) || !isPointerInitialized(_common_state._data_sender)) {
        return;
    }

    _common_state._critical_section->Enter();


    CalcTVGCurve();

    if (_common_state._tvg_data_buf == 0) {
        _common_state._tvg_data_buf = new (std::nothrow) tUMU_AScanData;
        if (_common_state._tvg_data_buf == 0) {
            _common_state._critical_section->Release();
            return;
        }
    }

    _common_state._tvg_data_buf->UMUIdx = _common_state._umu_id;

    memcpy(_common_state._tvg_data_buf, &_common_state._tvg_data, sizeof(_common_state._tvg_data));

    _common_state._data_sender->CreateNewEvent(edTVGData);
    _common_state._data_sender->WriteEventData(&_common_state._tvg_data_buf, SIZE_OF_TYPE(&_common_state._tvg_data_buf));
    ++_common_state._post_count;
    _common_state._data_sender->Post();


    // delete _common_state._tvg_data_buf;
    _common_state._tvg_data_buf = 0;
    _common_state._critical_section->Release();
}
// +
void UMU_RAWCAN::CalcTVGCurve()
{
    if (_common_state._save_ascan_zoom == 0) {
        return;
    }

    _common_state._tvg_data.Side = _common_state._save_ascan_side;
    _common_state._tvg_data.Line = _common_state._save_ascan_line;
    _common_state._tvg_data.Stroke = _common_state._save_ascan_stroke;

    if (_common_state._tvg_curve[_common_state._save_ascan_side][_common_state._save_ascan_line][_common_state._save_ascan_stroke])
        memcpy(&_common_state._tvg_data.Data[0], &_common_state._tvg_curve[_common_state._save_ascan_side][_common_state._save_ascan_line][_common_state._save_ascan_stroke][0], 232);
}
//
// Переключение линий со сплошного контроля на ручной контроль
// (сплошной или совмещенный) - Ав15
void UMU_RAWCAN::SetLineSwitching(UMULineSwitching state)
{
    assert((state == toCompleteControl) || (state == toCombineSensorHandControl) || (state == toSeparateSensorHandControl));
    const unsigned short argc = 2;
    unsigned char argv[argc];
    if (state == toCompleteControl) {
        argv[0] = 0;
        argv[1] = 1;
        sendCommandWithData(can_utils::wmLineSwitching, argv, argc);
    }
    else {
        argv[0] = 0;
        argv[1] = 0;
        sendCommandWithData(can_utils::wmLineSwitching, argv, argc);

        argv[0] = 1;
        if (state == toCombineSensorHandControl) {
            argv[1] = 1;  // совмещенный режим
        }
        else {
            argv[1] = 0;  // раздельный режим
        }
        sendCommandWithData(can_utils::wmLineSwitching, argv, argc);
    }
}
//
// ---------------------------- RESPONSE PARSERS -------------------------------
// + // Правильно ли я не использую _save_ascan_stroke ?
bool UMU_RAWCAN::parseResponse_dAScanDataHead(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    switch (data[0] & 0x80) {
    case 0x00: {
        _common_state._save_ascan_side = usLeft;
        break;
    }
    case 0x80: {
        _common_state._save_ascan_side = usRight;
        break;
    }
    default:
        return false;
    }

    _special_state._ascan_curr_num = 0;
    _special_state._ascan_curr_stroke = data[0] & 0x7F;  //_common_state._save_ascan_stroke

    return true;
}
// +
bool UMU_RAWCAN::parseResponse_dAScanMeasForCurr(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    if (!isPointerInitialized(_common_state._critical_section) || (!isPointerInitialized(_common_state._data_sender))) {
        return false;
    }

    _common_state._critical_section->Enter();

    if (_common_state._ascan_measure_buf == 0) {
        _common_state._ascan_measure_buf = new (std::nothrow) tUMU_AScanMeasure;
        if (_common_state._ascan_measure_buf == 0) {
            _common_state._critical_section->Release();
            return false;
        }
    }
    _common_state._ascan_measure_buf->UMUIdx = _common_state._umu_id;
    _common_state._ascan_measure_buf->Stroke = data[3] & 0x7F;

    switch (data[3] & 0x80) {
    case 0x00: {
        _common_state._ascan_measure_buf->Side = usLeft;
        break;
    }
    case 0x80: {
        _common_state._ascan_measure_buf->Side = usRight;
        break;
    }
    default:
        _common_state._critical_section->Release();
        return false;
    }

    _common_state._ascan_measure_buf->Line = 0;
    _common_state._ascan_measure_buf->ParamA = data[0];
    _common_state._ascan_measure_buf->ParamM = data[1] + (float) (data[2] * 0.1);

    _common_state._data_sender->CreateNewEvent(edAScanMeas);
    _common_state._data_sender->WriteEventData(&_common_state._ascan_measure_buf, SIZE_OF_TYPE(&_common_state._ascan_measure_buf));
    ++_common_state._post_count;
    _common_state._data_sender->Post();

    // delete _common_state._ascan_measure_buf;
    _common_state._ascan_measure_buf = 0;
    _common_state._critical_section->Release();

    return true;
}
// + Empty body
bool UMU_RAWCAN::parseResponse_dAScanMeasForZero(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    (void) data;

    return false;
}
// +
bool UMU_RAWCAN::parseResponse_dAScanMeasFirstPack(unsigned char* data, unsigned char size)
{
    // Первая посылка в данном протоколе аналогична последующим
    return parseResponse_dAScanMeasOtherPack(data, size);
}
// +/- unknown_const
bool UMU_RAWCAN::parseResponse_dAScanMeasOtherPack(unsigned char* data, unsigned char size)
{
    if (!isPointerInitialized(_common_state._critical_section) || (!isPointerInitialized(_common_state._data_sender))) {
        return false;
    }

    _common_state._critical_section->Enter();

    if (_common_state._ascan_data_buf == 0) {
        _common_state._ascan_data_buf = new (std::nothrow) tUMU_AScanData;
        if (_common_state._ascan_data_buf == 0) {
            _common_state._critical_section->Release();
            return false;
        }
    }

    //        unsigned long t = GetTickCount_();
    //        const int unknown_const = 4 * 19;

    //        memcpy(&(_common_state._last_d[1]),
    //                &(_common_state._last_d[0]),
    //                unknown_const);

    //		_common_state._last_d[0] = abs ((long)(t - _common_state._last_t));
    //		_common_state._last_t = t;

    _common_state._ascan_data_buf->UMUIdx = _common_state._umu_id;
    _common_state._ascan_data_buf->Stroke = _special_state._ascan_curr_stroke;
    _common_state._ascan_data_buf->Time = GetTickCount_();
    _common_state._ascan_data_buf->Time2 = _common_state._ascan_data_buf->Time - _common_state._last_time;
    _common_state._last_time = _common_state._ascan_data_buf->Time;
    _common_state._ascan_data_buf->Side = _common_state._save_ascan_side;
    _common_state._ascan_data_buf->Line = 0;

    //        t = GetTickCount_();
    //        memcpy(&(_common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][1]),
    //                &(_common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][0]),
    //                unknown_const);

    //        _common_state._last_d2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line][0] =
    //				abs ((long)(t - _common_state._last_t2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line]));

    //        _common_state._last_t2[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line] = t;
    //        _common_state._ascan_debug_arr[_common_state._ascan_data_buf->Side][_common_state._ascan_data_buf->Line] = GetTickCount_();

    memcpy(&_common_state._ascan_data_buf->Data[_special_state._ascan_curr_num * 8], &data[0],
           size);  // 8 число байт отсчетов в пакете


    ++_special_state._ascan_curr_num;

    if (_special_state._ascan_curr_num == 29)  // 29 - число пакетов по 8 отсчетов для передачи всех 232 отсчетов
    {
        _special_state._ascan_curr_num = 0;
        _common_state._data_sender->CreateNewEvent(edAScanData);
        _common_state._data_sender->WriteEventData(&_common_state._ascan_data_buf, SIZE_OF_TYPE(&_common_state._ascan_data_buf));

        ++_common_state._post_count;
        _common_state._data_sender->Post();
        // delete _common_state._ascan_data_buf;
        _common_state._ascan_data_buf = 0;
    }
    _common_state._critical_section->Release();

    return true;
}
// +
bool UMU_RAWCAN::parseResponse_dBScanData(unsigned char* data, unsigned char size)
{
    if (!isPointerInitialized(_common_state._critical_section) || (!isPointerInitialized(_common_state._data_sender))) {
        return false;
    }

    if (_special_state._bscan_buf == 0) {
        return false;
    }
    bool side = !(data[0] & 0x80);
    int stroke = (data[0] & 0x7F);
    int id;

    if (_special_state._bscanMessageTacktMem != stroke) {
        id = 0;
    }
    else {
        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
    }
    _special_state._bscanMessageTacktMem = stroke;
    //
    switch (size) {
    case 0x03: {
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[2] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        break;
    }
    case 0x04: {
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[3] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[3] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        break;
    }
    case 0x06: {
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[4] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[4] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[3];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[5] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        break;
    }
    case 0x07: {
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[1];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[5] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[2];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[5] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[3];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[6] >> 4);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];

        id = _special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Delay = data[4];
        _special_state._bscan_buf->Signals.Data[side][cLine0][stroke][id].Ampl[AmplDBIdx_int] = (data[6] & 0x0F);
        ++_special_state._bscan_buf->Signals.Count[side][cLine0][stroke];
        break;
    }
    }
    return true;
}
//
bool UMU_RAWCAN::parseResponse_dAlarmData(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    if (!isPointerInitialized(_common_state._critical_section) || (!isPointerInitialized(_common_state._data_sender))) {
        return false;
    }

    if (_common_state._alarm_buf == 0) {
        _common_state._alarm_buf = new (std::nothrow) tUMU_AlarmItem;
        if (_common_state._alarm_buf == 0) {
            return false;
        }
    }

    _common_state._alarm_buf->UMUIdx = _common_state._umu_id;
    int stroke = (data[1] & 0x7F);
    _common_state._alarm_buf->AlarmsCount = stroke;  // TODO: Протестировать, будет ли так работать

    _common_state._alarm_buf->State[1][cLine0][stroke][0] = static_cast<bool>((data[0] & BIN8(00000001)) != 0);
    _common_state._alarm_buf->State[1][cLine0][stroke][1] = static_cast<bool>((data[0] & BIN8(00000010)) != 0);
    _common_state._alarm_buf->State[1][cLine0][stroke][2] = static_cast<bool>((data[0] & BIN8(00000100)) != 0);
    _common_state._alarm_buf->State[1][cLine0][stroke][3] = static_cast<bool>((data[0] & BIN8(00001000)) != 0);

    _common_state._alarm_buf->State[0][cLine0][stroke][0] = static_cast<bool>((data[0] & BIN8(00000001)) != 0);  // Перенос АСД с левой нити на праваю
    _common_state._alarm_buf->State[0][cLine0][stroke][1] = static_cast<bool>((data[0] & BIN8(00000010)) != 0);
    _common_state._alarm_buf->State[0][cLine0][stroke][2] = static_cast<bool>((data[0] & BIN8(00000100)) != 0);
    _common_state._alarm_buf->State[0][cLine0][stroke][3] = static_cast<bool>((data[0] & BIN8(00001000)) != 0);

    /*
        _common_state._alarm_buf->State[0][cLine0][stroke][0] = ((data[0] & BIN8(00010000)) != 0);
        _common_state._alarm_buf->State[0][cLine0][stroke][1] = ((data[0] & BIN8(00100000)) != 0);
        _common_state._alarm_buf->State[0][cLine0][stroke][2] = ((data[0] & BIN8(01000000)) != 0);
        _common_state._alarm_buf->State[0][cLine0][stroke][3] = ((data[0] & BIN8(10000000)) != 0);
    */
    if (stroke == _common_state._stroke_count - 1) {
        _common_state._critical_section->Enter();

        _common_state._data_sender->CreateNewEvent(edAlarmData);
        _common_state._data_sender->WriteEventData(&_common_state._alarm_buf, SIZE_OF_TYPE(&_common_state._alarm_buf));

        ++_common_state._post_count;
        _common_state._data_sender->Post();

        // delete _common_state._alarm_buf;
        _common_state._alarm_buf = 0;
        _common_state._critical_section->Release();
    }

    return true;
}
// +
bool UMU_RAWCAN::parseResponse_dPathStep(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    if (!isPointerInitialized(_common_state._critical_section) || (!isPointerInitialized(_common_state._data_sender))) {
        return false;
    }

    if (_special_state._bscan_buf != 0) {
        if (size == 2) {
            _special_state._bscan_buf->Simulator[0] = !(data[1] == 0);
        }
        else {
            _special_state._bscan_buf->Simulator[0] = true;
        }

        // определение номера сессии
        _common_state._critical_section->Enter();
        if (_common_state._bscan_session_id.size() != 0) {
            _special_state._bscan_buf->BScanSession = _common_state._bscan_session_id.front();
        }
        else {
            _special_state._bscan_buf->BScanSession = -1;
        }
        _common_state._critical_section->Release();

        _special_state._bscan_buf->XSysCrd[0] = _special_state._XSysCrd[(int) _special_state._bscan_buf->Simulator[0]];
        _special_state._bscan_buf->XDisCrd[0] = _special_state._XDisCrd[(int) _special_state._bscan_buf->Simulator[0]];


        _special_state._bscan_buf->Dir[0] = _special_state._Dir;

        if (/*(*/ (_common_state._bscan_state) /* && (edBScan2Data == _save_event_id)) || (edMScan2Data == _save_event_id)*/) {
            _common_state._critical_section->Enter();
            _common_state._data_sender->CreateNewEvent(_save_event_id);
            _common_state._data_sender->WriteEventData(&_special_state._bscan_buf, SIZE_OF_TYPE(&_special_state._bscan_buf));
            ++_common_state._post_count;
            _common_state._data_sender->Post();
            _common_state._critical_section->Release();
        }
        _special_state._bscan_buf = 0;
    }
    if (data[0] != 0) {
        _save_event_id = edBScan2Data;
        _special_state._Dir = data[0];  //((data[0] <= 127) ? data[0] : data[0] - 256);
        if (size == 2) {
            _special_state._XSysCrd[(int) (!(data[1] == 0))] += _special_state._Dir;
            _special_state._XDisCrd[(int) (!(data[1] == 0))] = _special_state._XDisCrd[(int) (!(data[1] == 0))] + abs(_special_state._XSysCrd[(int) (!(data[1] == 0))] - _special_state._XSysCrd_Last[(int) (!(data[1] == 0))]);  // Дисплейная координата
            _special_state._XSysCrd_Last[(int) (!(data[1] == 0))] = _special_state._XSysCrd[(int) (!(data[1] == 0))];
        }
        else {
            _special_state._XSysCrd[1] += _special_state._Dir;                                                                                                                                                                    // всегда имитатор
            _special_state._XDisCrd[(int) (!(data[1] == 0))] = _special_state._XDisCrd[(int) (!(data[1] == 0))] + abs(_special_state._XSysCrd[(int) (!(data[1] == 0))] - _special_state._XSysCrd_Last[(int) (!(data[1] == 0))]);  // Дисплейная координата
            _special_state._XSysCrd_Last[(int) (!(data[1] == 0))] = _special_state._XSysCrd[(int) (!(data[1] == 0))];
        }
    }
    else {
        _save_event_id = edMScan2Data;
        _special_state._Dir = 0;
    }
    //
    _special_state._bscanMessageTacktMem = -1;
    _special_state._bscan_buf = new (std::nothrow) tUMU_BScanData;
    if (_special_state._bscan_buf == 0) {
        return false;
    }
    memset(_special_state._bscan_buf, 0, sizeof(tUMU_BScanData));
    _special_state._bscan_buf->UMUIdx = _common_state._umu_id;

    return true;
}

// + Empty body
bool UMU_RAWCAN::parseResponse_dPathStepAddit(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    (void) data;

    return false;
}
// + Empty body
bool UMU_RAWCAN::parseResponse_dVoltage(unsigned char* data, unsigned char size)
{
    unsigned short value = data[0] | (data[1] << 8);
    UNUSED(size);
    _special_state._akk_voltage = (unsigned char) std::ceil(1.258e-2 * (long) value - 19.7);
    ++_common_state._event_count[can_utils::wmGetVoltage];

    return true;
}
// +
bool UMU_RAWCAN::parseResponse_dWorkTime(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    _common_state._work_time = ((unsigned) data[0] | ((unsigned) data[1] << 8) | ((unsigned) data[2] << 16) | ((unsigned) data[3] << 24));
    return true;
}
// + Empty body
bool UMU_RAWCAN::parseResponse_cDevice(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    (void) data;

    return false;
}
// +
bool UMU_RAWCAN::parseResponse_cFWver(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    _common_state._firmware_info_time = GetTickCount_();

    _common_state._firmware_version_lo = data[0] & 0x0F;  // Дробная часть номера версии ПО контроллера
    _common_state._firmware_version_hi = data[1];         // Целая часть номера версии ПО контроллера

    _special_state._ver_year = data[2];
    _special_state._ver_mon = data[3];
    _special_state._ver_day = data[4];
    _special_state._ver_hour = data[5];
    _special_state._ver_min = data[6];
    _special_state._ver_sec = data[7];

    return true;
}
// +
bool UMU_RAWCAN::parseResponse_cSerialNumber(unsigned char* data, unsigned char size)
{
    UNUSED(size);
    _common_state._serial_number_time = GetTickCount_();
    _common_state._serial_number = (data[0] + (data[1] << 8));
    return true;
}
// Установка значения датчика пути 0 или его имитатора
int UMU_RAWCAN::SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain)
{
    //   (void)UseAsMain;

    if (!UseAsMain) {
        if (path_encoder_index == 0) {
            _special_state._XSysCrd[(int) Simulator] = new_value;
            _special_state._XDisCrd[(int) Simulator] = new_value;
        }
    }

    return -1;
}

// Установка значения датчика пути 0
void UMU_RAWCAN::SetPathEncoderData(char path_encoder_index, int new_value, bool UseAsMain)
{
    SetPathEncoderData(path_encoder_index, false, new_value, UseAsMain);
}

bool UMU_RAWCAN::parseResponse_BScanStopped(unsigned char* data, unsigned char size)
{
    // TODO: parse response? data & size unused?
    _common_state._critical_section->Enter();
    if (_common_state._bscan_session_id.size() != 0) {
        _common_state._bscan_session_id.erase(_common_state._bscan_session_id.begin());
    }
    _common_state._critical_section->Release();

    return true;
}

void UMU_RAWCAN::SetLastSysCrd(int EncoderIndex, int NewValue)
{
    _special_state._XSysCrd_Last[EncoderIndex] = NewValue;  // Дисплейная координата
}

}  // namespace CANProtDebug
