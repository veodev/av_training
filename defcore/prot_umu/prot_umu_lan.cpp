#include "platforms.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "prot_umu_lan.h"

namespace LANProtDebug
{
Add_Log onAddLog;

#ifdef DbgLog
bool useLog = true;
#endif

#ifdef Dbg
unsigned int rcvbytescnt;

#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
CRITICAL_SECTION cr;
#endif
#endif

//
void addbytcnt(unsigned int val)
{
#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    EnterCriticalSection(&cr);
#endif
#endif
    rcvbytescnt += val;

#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    LeaveCriticalSection(&cr);
#endif
#endif
}
//
unsigned int rdbytcnt()
{
    unsigned int res;
#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    EnterCriticalSection(&cr);
#endif
#endif
    res = rcvbytescnt;
    rcvbytescnt = 0;
#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    LeaveCriticalSection(&cr);
#endif
#endif
    return res;
}
//
void initbytcnt()
{
#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    InitializeCriticalSection(&cr);
#endif
#endif
    rcvbytescnt = 0;
}
//
void deinitbytcnt()
{
#if defined(DEFCORE_DEBUG)
#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    DeleteCriticalSection(&cr);
#endif
#endif
}
#endif
}  // namespace LANProtDebug

using namespace LANProtDebug;

#ifdef SIMULATION_CONTROLER_AND_MOTORS
DWORD SimulationStartTime = GetTickCount();
#endif

UMU_LAN::UMU_LAN()
{
    _save_event_id = edBScan2Data;
    memset(&_special_state, 0, sizeof(cSpecialState));
    _special_state._last_message_count = -1;
    _protocol = eProtLAN;
    _bscan_data_type = bstWithForm;

    Delay_frac_to_Int[0] = 0;
    Delay_frac_to_Int[1] = 0;
    Delay_frac_to_Int[2] = 0;
    Delay_frac_to_Int[3] = 1;
    Delay_frac_to_Int[4] = 1;
    Delay_frac_to_Int[5] = 1;
    Delay_frac_to_Int[6] = 1;
    Delay_frac_to_Int[7] = 2;
    Delay_frac_to_Int[8] = 2;
    Delay_frac_to_Int[9] = 2;

    for (unsigned char i = 0; i < MaxSideCount; ++i) {
        for (unsigned char j = 0; j < MaxLineCount; ++j) {
            for (unsigned char k = 0; k < MaxStrokeCount; ++k) {
                BScanDelayFactor[i][j][k] = 1;
            }
        }
    }

#ifdef UMULog
    if (!Log) Log = new LogFileObj(this);
    Log->AddIntParam("Create Object UMU:%d", _inner_state._umu_id);
#endif

#ifdef Dbg
    initbytcnt();
#endif
}

UMU_LAN::~UMU_LAN()
{
    _common_state._bscan_session_id.clear();  // Очистка данных сесий В-развертки
#ifdef UMULog
    Log->AddIntParam("Delete Object UMU:%d", _inner_state._umu_id);
    //	if ((!Log) && (Log->Owner == this)) delete Log;
    if ((Log != NULL) && (Log->Owner == this)) delete Log;
#endif

#ifdef Dbg
    deinitbytcnt();
#endif
}

int UMU_LAN::getFpgaProjVerId1() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._fpga_proj_ver_id1;
    _common_state._critical_section->Release();
    return res;
}

int UMU_LAN::getFpgaProjVerId2() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._fpga_proj_ver_id2;
    _common_state._critical_section->Release();
    return res;
}

int UMU_LAN::getFpgaProjVerId3() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._fpga_proj_ver_id3;
    _common_state._critical_section->Release();
    return res;
}

int UMU_LAN::getFpgaProjVerId4() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._fpga_proj_ver_id4;
    _common_state._critical_section->Release();
    return res;
}

int UMU_LAN::getSkipMessageCount() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._skip_message_count;
    _common_state._critical_section->Release();
    return res;
}

int UMU_LAN::getBuildNumberControllerFirmware() const
{
    _common_state._critical_section->Enter();
    int res = _special_state._build_number_controller_firmware;
    _common_state._critical_section->Release();
    return res;
}

eProtocol UMU_LAN::getProtocolType() const
{
    return eProtLAN;
}

void UMU_LAN::AddToOutBuff() {}

void UMU_LAN::AddToOutBuffSync(tLAN_Message* _out_block)
{
    _common_state._critical_section->Enter();
    _out_buffer.push(*_out_block);
    _common_state._critical_section->Release();
}

void UMU_LAN::AddToOutBuffNoSync(tLAN_Message* _out_block)
{
    _out_buffer.push(*_out_block);
}

void UMU_LAN::Unload()
{
    _common_state._critical_section->Enter();
    while (!_out_buffer.empty()) {
        const tLAN_Message& _currentMessage = _out_buffer.front();
        if (_common_state._data_transfer->write(_common_state._connection_id, reinterpret_cast<const unsigned char*>(&_currentMessage), _currentMessage.Size + LAN_MESSAGE_HEADER_SIZE)) {
            if ((_common_state._expectation_setting_coord) && (_currentMessage.Id == 0x43) && (_currentMessage.Data[0] == 0x47)) {
                _common_state._expectation_setting_coord_2 = true;
            }
#ifdef DbgLog
            if (useLog) onAddLog(_common_state._connection_id, (unsigned char*) &_out_buffer.front(), 2);
#endif
            _out_buffer.pop();
            if (_common_state._expectation_setting_coord_2) break;
        }
        else {
            _common_state._write_error_count++;
            break;
#ifdef UMUOutDataLog
            Log->AddText(" - WriteToLAN - Error");
#endif
        }
    }
    _common_state._critical_section->Release();
}

void UMU_LAN::TickSend()
{
    switch (_wstate) {
    case bsReady: {
        if (!_common_state._expectation_setting_coord_2) Unload();
        // Запрос данных о прогах АК, выполняется через 200 мс после передачи БУМ команды о выполнения расчета порогов АК
        if (_common_state._ac_th_request && (GetTickCount_() - _common_state._ac_th_request_time > 200)) {
            _common_state._ac_th_request = false;
            GetACTresholds();
        }
        break;
    }
    case bsOff:
        break;
    case bsInit: {
        _wstate = bsPowerOn;
        break;
    }
    case bsPowerOn: {
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d Power Switch OK", _inner_state._umu_id);
#endif
        _wstate = bsRequest;
        _read_state = rsHead;
        break;
    }
    case bsRequest: {
        _common_state._request_time = GetTickCount_();
        _wstate = bsWaitAnswer;
        GetDeviceID();
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d Send Request", _inner_state._umu_id);
#endif
        break;
    }
    case bsWaitAnswer: {
        Unload();
        if (GetTickCount_() - _common_state._request_time > 500) {
            _wstate = bsRequest;
        }
        if (_common_state._event_count[cSerialNumber] > 0) {
            _wstate = bsReady;
#ifdef UMUEventLog
            Log->AddIntParam("UMU:%d Get Request OK", _inner_state._umu_id);
#endif
        }
        break;
    }
    case bsStop:
        break;
    }
}

void UMU_LAN::readMessageHead(unsigned int& res, bool& fRepeat)
{
    _currentMessage.resetMessage();
    res = _common_state._data_transfer->read(_common_state._connection_id, reinterpret_cast<unsigned char*>(&_currentMessage), 6);

    if (res == LAN_MESSAGE_HEADER_SIZE) {
        DEFCORE_ASSERT(_currentMessage.NotUse == 0);
        fRepeat = true;
        _read_state = rsBody;
        _common_state._read_bytes_count = _currentMessage.Size;
        _common_state._message_read_bytes_count += _currentMessage.Size;

        if (_special_state._last_message_count != -1) {
            if (!((_currentMessage.MessageCount - _special_state._last_message_count == 0x01) || ((_currentMessage.MessageCount == 0x00) && (_special_state._last_message_count == 0xFF)))) {
                if (_currentMessage.MessageCount > _special_state._last_message_count) {
                    _special_state._skip_message_count += _currentMessage.MessageCount - _special_state._last_message_count - 1;
                }
                else if (_currentMessage.MessageCount < _special_state._last_message_count) {
                    _special_state._skip_message_count += _currentMessage.MessageCount + 255 - _special_state._last_message_count;
                }
                else if (_currentMessage.MessageCount == _special_state._last_message_count) {
                    _special_state._skip_message_count += 255;
                }
            }
        }

        _special_state._last_message_count = _currentMessage.MessageCount;
        ++_common_state._message_count;
    }
    else if (res != 0) {
        _common_state._error_message_count++;
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, NULL, 3);  // Ошибка приема данных
#endif
    }
}

void UMU_LAN::readMessageBody(unsigned int& res, bool& fRepeat)
{
    DEFCORE_ASSERT(_common_state._read_bytes_count <= tLAN_Message::LanDataMaxSize);
    res = _common_state._data_transfer->read(_common_state._connection_id, reinterpret_cast<unsigned char*>(&_currentMessage.Data), _common_state._read_bytes_count);

    if (res == _common_state._read_bytes_count) {
        fRepeat = true;
        _read_state = rsHead;
        UnPack(_currentMessage);
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, (unsigned char*) &_currentMessage, 1);
#endif
    }
    else if (res != 0) {
        ++_common_state._error_message_count;
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, (unsigned char*) &_currentMessage, 3);  // Ошибка приема данных
#endif
    }
}

void UMU_LAN::parsePathStep(const tLAN_Message& buff, bool containsDisCoord)
{
    // 1. Установка значений датчиков пути
    const unsigned char EncoderIndex = buff.Data[0] & 0x03;
    const unsigned char EncoderType = (buff.Data[0] >> 3) & 0x01;

    // Проверяем что пришли данные от выбранного датчика пути или от ДВ сканера - вертикаль
    // if ((_common_state._main_path_encoder_index != EncoderIndex) && (EncoderIndex != 2)) break;

    _special_state._pathstep_buf.UMUIdx = static_cast<unsigned char>(_common_state._umu_id);
    _special_state._pathstep_buf.PathEncodersIndex = EncoderIndex;  // Индекс датчика пути по которому было сформированно данное событие
    _special_state._pathstep_buf.Simulator[EncoderIndex] = (EncoderType == 1);
    _special_state._pathstep_buf.Dir[EncoderIndex] = static_cast<signed char>(buff.Data[1]);  // Сдвиг датчика пути
    _special_state._pathstep_buf.XSysCrd[EncoderIndex] = buff.Data[2] | (buff.Data[3] << 8) | (buff.Data[4] << 16) | buff.Data[5] << 24;


    if ((EncoderType == 0) || (_common_state._main_path_encoder_index == EncoderIndex)) {
        if (containsDisCoord) {
            // Дисплейную координату присылает БУМ
            _special_state._pathstep_buf.XDisCrd[EncoderIndex] = buff.Data[6] | (buff.Data[7] << 8) | (buff.Data[8] << 16) | buff.Data[9] << 24;  // Дисплейная координата
        }
        else {
            // Расчет дисплейной координаты
            _special_state._pathstep_buf.XDisCrd[EncoderIndex] = _special_state._pathstep_buf.XDisCrd[EncoderIndex] + abs(_special_state._pathstep_buf.XSysCrd[EncoderIndex] - _special_state.LastSysCrd[EncoderIndex]);  // Дисплейная координата
        }

        _special_state.LastSysCrd[EncoderIndex] = _special_state._pathstep_buf.XSysCrd[EncoderIndex];  // Последняя координата - для расчета дисплейной координаты
    }

    if (_common_state._main_path_encoder_index == EncoderIndex)  // Сработал основной ДП
    {
        // 2. Если есть ранее подготовленные данные то выгружаем В-развертку
        if (_special_state._bscan_buf != NULL) {
            _common_state._data_sender->WriteHeadAndPost(_save_event_id, &_special_state._bscan_buf, SIZE_OF_TYPE(&_special_state._bscan_buf));
            ++_common_state._post_count;
        }

        // 3. Сохраняем тип события
        if (buff.Data[1] != 0) {
            _save_event_id = edBScan2Data;
        }
        else {
            _save_event_id = edMScan2Data;
        }

        // 4. Подготавливаем структура для сбора новых данных В-развертки
        _special_state._bscan_buf = new (tUMU_BScanData);
        // 4.1. Очистка структур
        _special_state._bscan_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);
        memset(&_special_state._bscan_buf->Simulator, 0, MaxPathEncoders);
        _special_state._bscan_buf->PathEncodersIndex = 0;
        memset(&_special_state._bscan_buf->Dir, 0, MaxPathEncoders);
        memset(&_special_state._bscan_buf->XSysCrd, 0, MaxPathEncoders * sizeof(int));
        memset(&_special_state._bscan_buf->XDisCrd, 0, MaxPathEncoders * sizeof(int));
        _special_state._bscan_buf->BScanSession = 0;
        memset(&_special_state._bscan_buf->Signals, 0x00, sizeof(tUMU_BScanSignals));
        memset(&_special_state._bscan_buf->AC, 0x00, sizeof(tUMU_AcousticContact));
        _special_state._bscan_buf->AC.DataExists = false;

        // 4.2. Установка значений датчиков пути
        memcpy(&_special_state._bscan_buf->Simulator, &_special_state._pathstep_buf.Simulator, sizeof(tUMU_PathStepData));

        // 4.3. Определение сесии данных В-развертки
        if (!_common_state._bscan_session_id.empty()) {
            _special_state._bscan_buf->BScanSession = _common_state._bscan_session_id.back();
        }
        else {
            _special_state._bscan_buf->BScanSession = -1;
        }
    }
    else {  // Для не основного датчика пути - сразу формируем событие
        PtUMU_PathStepData pathstep_buf = new (tUMU_PathStepData);
        memset(pathstep_buf, 0x00, sizeof(tUMU_PathStepData));

        // Установка значений датчиков пути
        memcpy(&pathstep_buf->Simulator, &_special_state._pathstep_buf.Simulator, sizeof(tUMU_PathStepData));
        _common_state._data_sender->WriteHeadAndPost(edPathStepData, &pathstep_buf, SIZE_OF_TYPE(&pathstep_buf));
        ++_common_state._post_count;
    }
}

void UMU_LAN::parseA15ScanerPathStep(const tLAN_Message& buff)
{
    PtUMU_A15ScanerPathStepData pathstep_buf = new (tUMU_A15ScanerPathStepData);
    pathstep_buf->XCrd = buff.Data[0] | (buff.Data[1] << 8);
    pathstep_buf->YCrd = buff.Data[2];
    pathstep_buf->Angle = buff.Data[3];

    _common_state._data_sender->WriteHeadAndPost(edA15ScanerPathStepData, &pathstep_buf, SIZE_OF_TYPE(&pathstep_buf));
    ++_common_state._post_count;
}

void UMU_LAN::TickReceive()
{
    bool fRepeat = false;
    unsigned int res = 0;

    do {
        fRepeat = false;
        switch (_read_state) {
        case rsHead: {
            readMessageHead(res, fRepeat);
            break;
        }
        case rsBody: {
            readMessageBody(res, fRepeat);
            break;
        }
        case rsOff:
            break;
        }
    } while (fRepeat);
}

void UMU_LAN::parseFirmwareVersion(const tLAN_Message& buff)
{
    _common_state._firmware_info_time = GetTickCount_();
    _special_state._build_number_controller_firmware = buff.Data[0];  // Номер сборки версии ПО контроллера
    _common_state._firmware_version_lo = buff.Data[1];                // Дробная часть номера версии ПО контроллера
    _common_state._firmware_version_hi = buff.Data[2];                // Целая часть номера версии ПО контроллера
    _special_state._fpga_proj_ver_id1 = buff.Data[3];                 // Идентификатор 1 версии проекта ПЛИС
    _special_state._fpga_proj_ver_id2 = buff.Data[4];                 // Идентификатор 2 версии проекта ПЛИС
    _special_state._fpga_proj_ver_id3 = buff.Data[5];                 // Идентификатор 3 версии проекта ПЛИС
    _special_state._fpga_proj_ver_id4 = buff.Data[6];                 // Идентификатор 4 версии проекта ПЛИС
}

void UMU_LAN::parseSerialNumber(const tLAN_Message& buff)
{
    _common_state._serial_number_time = GetTickCount_();
    _common_state._serial_number = static_cast<unsigned int>(buff.Data[0] + (buff.Data[1] << 8));
#ifdef UMUEventLog  //
    Log->AddTwoIntParam("UMU:%d SerialNumber", _inner_state._umu_id, _inner_state._serial_number);
#endif
}

void UMU_LAN::parseAScanMeasure(const tLAN_Message& buff)
{
    DEFCORE_ASSERT(buff.Size == ASCAN_MEASURE_MESSAGE_BODY_SIZE);
    _common_state._ascan_measure_buf = new (tUMU_AScanMeasure);
    _common_state._ascan_measure_buf->UMUIdx = _common_state._umu_id;
    _common_state._ascan_measure_buf->Stroke = buff.Data[0] & 0x3F;
    switch (buff.Data[0] & 0x80) {
    case 0x00: {
        _common_state._ascan_measure_buf->Side = usRight;
        break;
    }
    case 0x80: {
        _common_state._ascan_measure_buf->Side = usLeft;
        break;
    }
    default: {
        DEFCORE_ASSERT(false);
        break;
    }
    }
    _common_state._ascan_measure_buf->Line = ((buff.Data[0] & 0x40) != 0) ? 0x01 : 0x00;
    _common_state._ascan_measure_buf->ParamA = static_cast<int>(buff.Data[1]);
    _common_state._ascan_measure_buf->ParamM = static_cast<float>(buff.Data[2]) + static_cast<float>(buff.Data[3] * 0.1f);
    _common_state._data_sender->WriteHeadAndPost(edAScanMeas, &_common_state._ascan_measure_buf, SIZE_OF_TYPE(&_common_state._ascan_measure_buf));
    ++_common_state._post_count;
}

void UMU_LAN::parseAScanData(const tLAN_Message& buff)
{
    DEFCORE_ASSERT(buff.Size == ASCAN_MESSAGE_BODY_SIZE);
    _common_state._ascan_data_buf = new (tUMU_AScanData);
    _common_state._ascan_data_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);
    _common_state._ascan_data_buf->Stroke = buff.Data[0] & 0x3F;
    _common_state._ascan_data_buf->Time = GetTickCount_();
    _common_state._ascan_data_buf->Time2 = _common_state._ascan_data_buf->Time - _common_state._last_time;
    _common_state._last_time = _common_state._ascan_data_buf->Time;
    switch (buff.Data[0] & 0x80) {
    case 0x00: {
        _common_state._ascan_data_buf->Side = usRight;
        break;
    }
    case 0x80: {
        _common_state._ascan_data_buf->Side = usLeft;
        break;
    }
    default: {
        DEFCORE_ASSERT(false);
        break;
    }
    }
    _common_state._ascan_data_buf->Line = static_cast<unsigned char>((buff.Data[0] & 0x40) != 0);

    memcpy(&_common_state._ascan_data_buf->Data[0], &buff.Data[1], buff.Size - 1);

    _common_state._data_sender->WriteHeadAndPost(edAScanData, &_common_state._ascan_data_buf, SIZE_OF_TYPE(&_common_state._ascan_data_buf));
    ++_common_state._post_count;
}

void UMU_LAN::parseBScanData(const tLAN_Message& buff)
{
    unsigned char currentStroke = buff.Data[0] & 0x3F;
    DEFCORE_ASSERT(currentStroke < MaxStrokeCount);
    unsigned char currentLine = static_cast<unsigned char>((buff.Data[0] & 0x40) != 0);
    DEFCORE_ASSERT(currentLine < 2);
    DEFCORE_ASSERT(((buff.Data[0] >> 7) & 0x01) == 0x00);
    tUMU_BScanSignals& currentSignals = _special_state._bscan_buf->Signals;

    unsigned char rightSideBlocks = (buff.Data[1]) & 0x0F;
    unsigned char leftSideBlocks = (buff.Data[1] >> 4) & 0x0F;

    currentSignals.Count[usRight][currentLine][currentStroke] = rightSideBlocks;
    currentSignals.Count[usLeft][currentLine][currentStroke] = leftSideBlocks;

    unsigned char blockCount = std::max(leftSideBlocks, rightSideBlocks);
    DEFCORE_ASSERT((2 + blockCount * 7 * 2) == buff.Size);

    tUMU_BScanSignal* leftBlock = currentSignals.Data[usLeft][currentLine][currentStroke];
    tUMU_BScanSignal* rightBlock = currentSignals.Data[usRight][currentLine][currentStroke];

    unsigned char rightDelayFactor = BScanDelayFactor[usRight][currentLine][currentStroke];
    unsigned char leftDelayFactor = BScanDelayFactor[usLeft][currentLine][currentStroke];

    unsigned short dataIndex = 2;
    for (unsigned char blockIndex = 0; blockIndex < blockCount; ++blockIndex) {
        tUMU_BScanSignal& rightSig = rightBlock[blockIndex];
        tUMU_BScanSignal& leftSig = leftBlock[blockIndex];
        // Амплитуда в начале сигнала
        rightSig.Ampl[StartAmpl] = buff.Data[dataIndex];
        dataIndex++;
        leftSig.Ampl[StartAmpl] = buff.Data[dataIndex];
        dataIndex++;

        // Максимальное значение М амплитуды сигнала
        rightSig.Ampl[MaxAmpl] = buff.Data[dataIndex];
        dataIndex++;
        leftSig.Ampl[MaxAmpl] = buff.Data[dataIndex];
        dataIndex++;

        // Задержка начала сигнала, мкс
        rightSig.Ampl[StartDelay_int] = buff.Data[dataIndex];
        dataIndex++;
        leftSig.Ampl[StartDelay_int] = buff.Data[dataIndex];
        dataIndex++;

        // Задержка максимума, мкс
        rightSig.Ampl[MaxDelay_int] = buff.Data[dataIndex];
        dataIndex++;
        leftSig.Ampl[MaxDelay_int] = buff.Data[dataIndex];
        dataIndex++;

        // Задержка конца сигнала**, мкс
        rightSig.Ampl[EndDelay_int] = buff.Data[dataIndex];
        dataIndex++;
        leftSig.Ampl[EndDelay_int] = buff.Data[dataIndex];
        dataIndex++;

        // Задержка максимума М, дес.мкс
        // Задержка начала сигнала, дес.мкс
        rightSig.Ampl[StartDelay_frac] = buff.Data[dataIndex] & 0x0F;
        rightSig.Ampl[MaxDelay_frac] = (buff.Data[dataIndex] >> 4) & 0x0F;
        dataIndex++;
        leftSig.Ampl[StartDelay_frac] = buff.Data[dataIndex] & 0x0F;
        leftSig.Ampl[MaxDelay_frac] = (buff.Data[dataIndex] >> 4) & 0x0F;
        dataIndex++;

        // Задержка конца сигнала**, дес.мкс
        rightSig.Ampl[EndDelay_frac] = buff.Data[dataIndex] & 0x0F;
        dataIndex++;
        DEFCORE_ASSERT(dataIndex < tLAN_Message::LanDataMaxSize);
        DEFCORE_ASSERT(dataIndex < buff.Size);

        leftSig.Ampl[EndDelay_frac] = buff.Data[dataIndex] & 0x0F;
        dataIndex++;

        // Сохранение данных для старого режима отображения
        // Перевод задержки из формата БУМ XXX.Y мкс в XXX (byte) с учетом множителя задержки В-развертки Работает только для значений 1 и 3
        if (rightDelayFactor == 1) {
            rightSig.Delay = rightSig.Ampl[MaxDelay_int];
        }
        else {
            rightSig.Delay = rightSig.Ampl[MaxDelay_int] * rightDelayFactor + Delay_frac_to_Int[rightSig.Ampl[MaxDelay_frac]];
        }

        if (leftDelayFactor == 1) {
            leftSig.Delay = leftSig.Ampl[MaxDelay_int];
        }
        else {
            leftSig.Delay = leftSig.Ampl[MaxDelay_int] * leftDelayFactor + Delay_frac_to_Int[leftSig.Ampl[MaxDelay_frac]];
        }

        rightSig.Ampl[AmplDBIdx_int] = TableDB[rightSig.Ampl[MaxAmpl]] & 0x0F;
        leftSig.Ampl[AmplDBIdx_int] = TableDB[leftSig.Ampl[MaxAmpl]] & 0x0F;

        // --------------------------------------------------------------------------------------
    }
}

void UMU_LAN::parseAlarmData(const tLAN_Message& buff)
{
    _common_state._alarm_buf = new (tUMU_AlarmItem);
    memset(_common_state._alarm_buf, 0x00, sizeof(tUMU_AlarmItem));
    _common_state._alarm_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);
    unsigned char alarmSize = static_cast<unsigned char>(buff.Size >> 1);
    DEFCORE_ASSERT(alarmSize < MaxStrokeCount);
    _common_state._alarm_buf->AlarmsCount = alarmSize;

    unsigned char st = 0;
    for (unsigned char currentStroke = 0; currentStroke < alarmSize; ++currentStroke) {
        DEFCORE_ASSERT(st < buff.Size);
        _common_state._alarm_buf->State[0][0][currentStroke][0] = static_cast<bool>(buff.Data[st] & 0x01);
        _common_state._alarm_buf->State[0][0][currentStroke][1] = static_cast<bool>(buff.Data[st] & 0x02);
        _common_state._alarm_buf->State[0][0][currentStroke][2] = static_cast<bool>(buff.Data[st] & 0x04);
        _common_state._alarm_buf->State[0][0][currentStroke][3] = static_cast<bool>(buff.Data[st] & 0x08);

        _common_state._alarm_buf->State[0][1][currentStroke][0] = static_cast<bool>(buff.Data[st] & 0x10);
        _common_state._alarm_buf->State[0][1][currentStroke][1] = static_cast<bool>(buff.Data[st] & 0x20);
        _common_state._alarm_buf->State[0][1][currentStroke][2] = static_cast<bool>(buff.Data[st] & 0x40);
        _common_state._alarm_buf->State[0][1][currentStroke][3] = static_cast<bool>(buff.Data[st] & 0x80);
        st++;

        DEFCORE_ASSERT(st < buff.Size);
        _common_state._alarm_buf->State[1][0][currentStroke][0] = static_cast<bool>(buff.Data[st] & 0x01);
        _common_state._alarm_buf->State[1][0][currentStroke][1] = static_cast<bool>(buff.Data[st] & 0x02);
        _common_state._alarm_buf->State[1][0][currentStroke][2] = static_cast<bool>(buff.Data[st] & 0x04);
        _common_state._alarm_buf->State[1][0][currentStroke][3] = static_cast<bool>(buff.Data[st] & 0x08);

        _common_state._alarm_buf->State[1][1][currentStroke][0] = static_cast<bool>(buff.Data[st] & 0x10);
        _common_state._alarm_buf->State[1][1][currentStroke][1] = static_cast<bool>(buff.Data[st] & 0x20);
        _common_state._alarm_buf->State[1][1][currentStroke][2] = static_cast<bool>(buff.Data[st] & 0x40);
        _common_state._alarm_buf->State[1][1][currentStroke][3] = static_cast<bool>(buff.Data[st] & 0x80);
        st++;
    }

    _common_state._data_sender->WriteHeadAndPost(edAlarmData, &_common_state._alarm_buf, SIZE_OF_TYPE(&_common_state._alarm_buf));
    ++_common_state._post_count;
}

void UMU_LAN::parseWorkTime(const tLAN_Message& buff)
{
    _common_state._work_time = 0;
    _common_state._work_time = buff.Data[0] | (buff.Data[1] << 8) | (buff.Data[2] << 16) | buff.Data[3] << 24;  // Buff[2] | Buff[5] << 8;
#ifdef UMUEventLog                                                                                              //
    Log->AddTwoIntParam("UMU:%d WorkTime: %d", _inner_state._umu_id, WorkTime);
#endif
}

void UMU_LAN::parseDeviceSpeed(const tLAN_Message& buff)
{
    unsigned int deviceSpeedInStepsPerHalfSecond = (static_cast<unsigned int>(buff.Data[0] | (buff.Data[1] << 8) | (buff.Data[2] << 16) | buff.Data[3] << 24)) * 2;
    _common_state._device_speed = deviceSpeedInStepsPerHalfSecond;
    _common_state._data_sender->WriteHeadAndPost(edDeviceSpeed, &deviceSpeedInStepsPerHalfSecond, sizeof(unsigned int));
}

void UMU_LAN::parseMetalSensor(const tLAN_Message& buff)
{
    _special_state._metal_sensor_buf = new (tUMU_MetalSensorData);
    _special_state._metal_sensor_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);
    switch (buff.Data[0] & 0x80) {
    case 0x00: {
        _special_state._metal_sensor_buf->Side = usRight;
        break;
    }
    case 0x80: {
        _special_state._metal_sensor_buf->Side = usLeft;
        break;
    }
    }
    _special_state._metal_sensor_buf->SensorId = buff.Data[1];
    _special_state._metal_sensor_buf->State = buff.Data[2];
    _common_state._data_sender->WriteHeadAndPost(edMSensor, &_special_state._metal_sensor_buf, SIZE_OF_TYPE(&_special_state._metal_sensor_buf));
    ++_common_state._post_count;
}

void UMU_LAN::parseConfirmStopBScan(const tLAN_Message& buff)
{
    UNUSED(buff);
    // Установка новой сессии данных В-развертки, после подтверждения остановки В-развертки
    if (!_common_state._bscan_session_id.empty()) {
        _common_state._bscan_session_id.erase(_common_state._bscan_session_id.begin());
    }
}

void UMU_LAN::parseConfirmPathEncoderSetup(const tLAN_Message& buff)
{
    unsigned char EncoderIndex = buff.Data[0] & 0x03;
    unsigned char EncoderType = (buff.Data[0] >> 3) & 0x01;
    if (buff.Data[1] == 0 && buff.Data[2] == 0 && buff.Data[3] == 0 && buff.Data[4] == 0x80) {
        // qDebug() << "=============== SYNC ERROR!!! ===============";
    }
    if (EncoderType == 0) {
        _special_state._pathstep_buf.XSysCrd[EncoderIndex] = buff.Data[1] | (buff.Data[2] << 8) | (buff.Data[3] << 16) | buff.Data[4] << 24;  // Дисплейная координата
        _special_state.LastSysCrd[EncoderIndex] = buff.Data[1] | (buff.Data[2] << 8) | (buff.Data[3] << 16) | buff.Data[4] << 24;             // Последняя координата - для расчета дисплейной координаты
    }
    _common_state._expectation_setting_coord = false;
    _common_state._expectation_setting_coord_2 = false;
}

void UMU_LAN::parseAcousticContactSumm(const tLAN_Message& buff)
{
    int st = 0;
    int Idx = 0;
    int Size = buff.Size;

    _special_state._bscan_buf->AC.DataExists = true;
    while (Size != 0) {
        DEFCORE_ASSERT(st < MaxStrokeCount);
        _special_state._bscan_buf->AC.Summ[0][0][st] = buff.Data[Idx + 0] | (buff.Data[Idx + 2] << 8) | (buff.Data[Idx + 4] << 16);
        _special_state._bscan_buf->AC.Summ[1][0][st] = buff.Data[Idx + 1] | (buff.Data[Idx + 3] << 8) | (buff.Data[Idx + 5] << 16);
        Idx = Idx + 6;
        _special_state._bscan_buf->AC.Summ[0][1][st] = buff.Data[Idx + 0] | (buff.Data[Idx + 2] << 8) | (buff.Data[Idx + 4] << 16);
        _special_state._bscan_buf->AC.Summ[1][1][st] = buff.Data[Idx + 1] | (buff.Data[Idx + 3] << 8) | (buff.Data[Idx + 5] << 16);
        Idx = Idx + 6;
        _special_state._bscan_buf->AC.Th[0][0][st] = buff.Data[Idx + 0] | (buff.Data[Idx + 2] << 8) | (buff.Data[Idx + 4] << 16);
        _special_state._bscan_buf->AC.Th[1][0][st] = buff.Data[Idx + 1] | (buff.Data[Idx + 3] << 8) | (buff.Data[Idx + 5] << 16);
        Idx = Idx + 6;
        _special_state._bscan_buf->AC.Th[0][1][st] = buff.Data[Idx + 0] | (buff.Data[Idx + 2] << 8) | (buff.Data[Idx + 4] << 16);
        _special_state._bscan_buf->AC.Th[1][1][st] = buff.Data[Idx + 1] | (buff.Data[Idx + 3] << 8) | (buff.Data[Idx + 5] << 16);
        Idx = Idx + 6;
        st++;
        Size = Size - 24;
    }
}

void UMU_LAN::parseAcousticContact(const tLAN_Message& buff)
{
    int st = 0;
    int Idx = 0;
    int Size = buff.Size;

    _special_state._bscan_buf->AC.DataExists = true;
    while (Size != 0) {
        DEFCORE_ASSERT(st < MaxStrokeCount);
        _special_state._bscan_buf->AC.Data[0][0][st] = ((buff.Data[Idx] & 0x01) != 0);
        _special_state._bscan_buf->AC.Data[0][1][st] = (((buff.Data[Idx] >> 4) & 0x01) != 0);
        _special_state._bscan_buf->AC.Data[1][0][st] = ((buff.Data[Idx + 1] & 0x01) != 0);
        _special_state._bscan_buf->AC.Data[1][1][st] = (((buff.Data[Idx + 1] >> 4) & 0x01) != 0);
        Idx = Idx + 2;
        st++;
        Size = Size - 2;
    }
}

void UMU_LAN::parseACTresholds(const tLAN_Message& buff)
{
    memcpy(&(_common_state._ac_th_data[0]), &(buff.Data[0]), 48 * 2);
    _common_state._ac_th_data_exists = true;
}

void UMU_LAN::waitForUpload()
{
    while (true) {
        _common_state._critical_section->Enter();
        if (_out_buffer.empty()) {
            _common_state._last_OK_upload_time = GetTickCount_();
            _common_state._critical_section->Release();
            break;
        }
        _common_state._critical_section->Release();
        SLEEP(1);
        if (GetTickCount_() - _common_state._last_OK_upload_time > 2000) break;
    }
}

void UMU_LAN::UnPack(const tLAN_Message& buff)
{
    _common_state._critical_section->Enter();
#ifdef SIMULATION_CONTROLER_AND_MOTORS
#ifdef SIMULATE_PATH_ENCODER_IN_REAL_UMU
    if (buff.Id == dBScan2Data) {
        _inner_state._event_count[dPathStep]++;

        if (BScanBuff_ != NULL) {
            _inner_state._critical_section->Enter();
            _inner_state._data_sender->CreateNewEvent(_inner_state._save_event_id);
            SaveEventID = edBScan2Data;
            // if (Buff.Data[1] != 0) SaveEventID = edBScan2Data;
            //				  else SaveEventID = edMScan2Data;
            _inner_state._data_sender->WriteEventData(&BScanBuff_, SIZE_OF_TYPE(&_special_state._bscan_buf));
            _inner_state._data_sender->Post();
            _inner_state._critical_section->Release();
        }

        BScanBuff_ = new (tUMU_BScanData);
        memset(&BScanBuff_->Signals.Count[0][0][0], 0, 2 * 2 * MaxStrokeCount);
        BScanBuff_->UMUIdx = UMUIdx;
        BScanBuff_->Dir[0] = 1;
        BScanBuff_->Dir[1] = 0;
        BScanBuff_->XSysCrd[0] = ((GetTickCount_() - SimulationStartTime) / 50) % ScanLen;
        BScanBuff_->XSysCrd[1] = 0;
    }

#endif
#endif
    DEFCORE_ASSERT(buff.Size < tLAN_Message::LanDataMaxSize);
    _common_state._event_count[buff.Id]++;
    bool GoodEvent = true;
    switch (buff.Id) {
    case dBScan2Data:  // БУМ - Данные В-развертки
    {
        if (!_special_state._bscan_buf) {
            break;
        }
        parseBScanData(buff);
        break;
    }
    case dPathStep:  // БУМ - Срабатывание датчика пути
    {
        if (_common_state._expectation_setting_coord) {
            break;
        }
        parsePathStep(buff, false);
        break;
    }
    case dPathStepEx:  // БУМ - Срабатывание датчика пути
    {
        if (_common_state._expectation_setting_coord) {
            break;
        }
        parsePathStep(buff, true);
        break;
    }

    case dA15ScanerPathStep:  // БУМ - Координата сканера Ав-15
    {
        parseA15ScanerPathStep(buff);
        break;
    }

    case cFWver:  // ОБЩ - Версия и дата прошивки блока
    {
        parseFirmwareVersion(buff);
        break;
    }
    case cSerialNumber:  // ОБЩ - Серийный номер устройства
    {
        parseSerialNumber(buff);
        break;
    }
    case dAScanMeas:  // А-развертка, точное значение амплитуды и задержки
    {
        parseAScanMeasure(buff);
        break;
    }
    case dAScanDataHead:  // А-развертка
    {
        parseAScanData(buff);
        break;
    }
    case dAlarmData:  // БУМ - Данные АСД
    {
        parseAlarmData(buff);
        break;
    }
    case dWorkTime:  // БУМ - Время работы БУМ
    {
        parseWorkTime(buff);
        break;
    }
    case dDeviceSpeed:  // БУМ - Скорость системы
    {
        parseDeviceSpeed(buff);
        break;
    }
    case dMetalSensor:  // Срабатывание датчика маталла (болтового стыка и стрелочного перевода)
    {
        parseMetalSensor(buff);
        break;
    }
    case dConfirmStopBScan:  // БУМ - Подтверждение выполнения команды «Запретить передачу В-развертки с идентификатором 0x72»
    {
        parseConfirmStopBScan(buff);
        break;
    }
    case dConfirmPathEncoderSetup:  // Подтверждение установки значения датчика пути
    {
        parseConfirmPathEncoderSetup(buff);
        break;
    }
    case dAcousticContactSumm:  // БУМ - Сырые данные акустического контакта
    {
        if (!_special_state._bscan_buf) {
            break;
        }
        parseAcousticContactSumm(buff);
        break;
    }
    case dAcousticContact_:  // БУМ - Данные акустического контакта
    {
        if (!_special_state._bscan_buf) {
            break;
        }
        parseAcousticContact(buff);
        break;
    }
    case dACTresholds:  // БУМ - Данные акустического контакта
    {
        parseACTresholds(buff);
        break;
    }
    default: {
        GoodEvent = false;
    }
    }
    if (GoodEvent) {
        _common_state._last_event_time = GetTickCount_();
    }
    _common_state._critical_section->Release();
}

bool UMU_LAN::EndWorkTest()
{
    _common_state._critical_section->Enter();
    if (_common_state._save_write_error_count == 0) {
        _common_state._save_write_error_count = _common_state._write_error_count;
    }

    bool res = false;
    if (GetTickCount_() - _common_state._end_work_time < 2000)
        res = (_common_state._end_work_flag) && (_out_buffer.empty() || (_common_state._save_write_error_count != _common_state._write_error_count));
    else
        res = (_common_state._end_work_flag);
    _common_state._critical_section->Release();
    return res;
}

void UMU_LAN::SetStrokeCount(unsigned char count)
{
    // qDebug() << "SetStrokeCount" << static_cast<int>(count);
    DEFCORE_ASSERT(count <= MaxStrokeCount);
    DEFCORE_ASSERT(count > 0);

    _common_state.BSFilterStrokeData.clear();

    tLAN_Message packet(uStrokeCount, 0x01);
    packet.Data[0] = count;
    _common_state._critical_section->Enter();
    AddToOutBuffNoSync(&packet);

#ifdef UMUEventLog
    Log->AddTwoIntParam("UMU:%d SetStrokeCount: %d", _inner_state._umu_id, count);
#endif

    _common_state._stroke_count = count;
    _common_state._ac_state = false;
    _common_state._critical_section->Release();

    waitForUpload();
}

void UMU_LAN::SetGainSegmentPoint(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay, unsigned char gain, unsigned char ascan_scale,
                                  eGainPointType point)  //
{
    DEFCORE_ASSERT(stroke < MaxStrokeCount);
    _common_state._critical_section->Enter();
    if (point == gptStart) {
        _common_state.TVG_point_Gain = gain;
        _common_state.TVG_point_Delay = delay;
        _common_state.TVG_point_Pixel = static_cast<unsigned char>(std::min(static_cast<float>(231), (10.0f * static_cast<float>(delay)) / static_cast<float>(ascan_scale)));
    }
    else if ((point == gptCentr) || (point == gptEnd)) {
        tLAN_Message packet(uGainSeg, 0x05);

        packet.Data[0] = static_cast<unsigned char>(((((side) << 1) | line) << 6) | stroke);
        packet.Data[1] = _common_state.TVG_point_Delay;
        packet.Data[2] = _common_state.TVG_point_Gain;
        packet.Data[3] = delay;
        packet.Data[4] = gain;

        AddToOutBuffNoSync(&packet);

        const unsigned char edPixel = static_cast<unsigned char>(std::min(static_cast<float>(231), 10.0f * static_cast<float>(delay) / static_cast<float>(ascan_scale)));
        DEFCORE_ASSERT(edPixel < 232);
        const unsigned char divisor = edPixel - _common_state.TVG_point_Pixel;
        unsigned char* currentCurve = _common_state._tvg_curve[side][line][stroke];
        if (currentCurve == NULL) {
            _common_state._tvg_curve[side][line][stroke] = new unsigned char[232];
            currentCurve = _common_state._tvg_curve[side][line][stroke];
            memset(currentCurve, 0, 232);
        }

        for (unsigned char Pixel = _common_state.TVG_point_Pixel; Pixel <= edPixel; ++Pixel) {
            DEFCORE_ASSERT(Pixel < 232);
            if (divisor != 0) {
                currentCurve[Pixel] = static_cast<unsigned char>(_common_state.TVG_point_Gain + static_cast<float>((gain - _common_state.TVG_point_Gain) * (Pixel - _common_state.TVG_point_Pixel)) / static_cast<float>(divisor));
            }
            else {
                currentCurve[Pixel] = gain;
            }
        }
        _common_state.TVG_point_Gain = gain;
        _common_state.TVG_point_Delay = delay;
        _common_state.TVG_point_Pixel = edPixel;

#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d SetGainStatement", _inner_state._umu_id);
#endif
    };

    if (point == gptEnd) {
        if ((_common_state._save_ascan_zoom != 0) && (stroke == _common_state._save_ascan_stroke) && (side == _common_state._save_ascan_side) && (line == _common_state._save_ascan_line)) {
            CalcTVGCurve();
            _common_state._tvg_data_buf = new (tUMU_AScanData);
            memset(_common_state._tvg_data_buf, 0x00, sizeof(tUMU_AScanData));

            memcpy(_common_state._tvg_data_buf, &_common_state._tvg_data, sizeof(tUMU_AScanData));
            _common_state._tvg_data_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);

            _common_state._data_sender->WriteHeadAndPost(edTVGData, &_common_state._tvg_data_buf, SIZE_OF_TYPE(&_common_state._tvg_data_buf));
            ++_common_state._post_count;
        }
    }
    _common_state._critical_section->Release();

    //    waitForUpload();
}

void UMU_LAN::SetGate(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char index, unsigned char st_delay, unsigned char ed_delay, unsigned char level, eStrobeMode mode,
                      eAlarmAlgorithm asd_mode)  //
{
    DEFCORE_ASSERT(stroke < MaxStrokeCount);
    tLAN_Message packet(uGate, 0x07);

    packet.Data[0] = static_cast<unsigned char>(((((side) << 1) | line) << 6) | stroke);
    packet.Data[1] = index;
    packet.Data[2] = st_delay;
    packet.Data[3] = ed_delay;
    packet.Data[4] = level;
    packet.Data[5] = mode;
    packet.Data[6] = asd_mode;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d SetGate", _inner_state._umu_id);
#endif

    //    waitForUpload();
}

void UMU_LAN::SetPrismDelay(eUMUSide Side, unsigned char line, unsigned char stroke, unsigned short delay)
{
    DEFCORE_ASSERT(stroke < MaxStrokeCount);
    tLAN_Message packet(uPrismDelay, 0x03);

    packet.Data[0] = static_cast<unsigned char>(((((Side) << 1) | line) << 6) | stroke);
    packet.Data[1] = static_cast<unsigned char>(delay & 0xFF);
    packet.Data[2] = static_cast<unsigned char>(delay >> 8);
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d SetPrismDelay", _inner_state._umu_id);
#endif

    //    waitForUpload();
}

bool UMU_LAN::SetLeftSideSwitching(UMULineSwitching state)  // Переключение линий с сплошного контроля на сканер
{
    tLAN_Message packet(wmLeftSideSwitching, 0x01);

    if (state == toScaner) {
        packet.Data[0] = 1;
    }
    else {
        packet.Data[0] = 0;
    }
    AddToOutBuffSync(&packet);

    //    waitForUpload();
    return true;
}

void UMU_LAN::EnableAScan(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short st_delay, unsigned char zoom, unsigned char mode, unsigned char strob)
{
    // qDebug() << "UMU" << this->_id << " ============================== before if EnableAScan";
    if ((!_common_state._ascan_state) || (_common_state._save_ascan_side != side) || (_common_state._save_ascan_line != line) || (_common_state._save_ascan_stroke != stroke) || (_common_state._save_ascan_st_delay != static_cast<unsigned char>(st_delay)) || (_common_state._save_ascan_zoom != zoom)
        || (_common_state._save_ascan_mode != mode) || (_common_state._save_ascan_strob != strob)) {
        // qDebug() << "UMU" << this->_id << "============================== into of EnableAScan";
        _common_state._ascan_state = true;

        DEFCORE_ASSERT(st_delay <= 255);
        tLAN_Message packet(uEnableAScan, 0x05);
        packet.Data[0] = static_cast<unsigned char>(((((side) << 1) | line) << 6) | stroke);
        packet.Data[1] = static_cast<unsigned char>(st_delay);
        packet.Data[2] = zoom;
        packet.Data[3] = mode;
        packet.Data[4] = strob;
        _common_state._critical_section->Enter();
        AddToOutBuffNoSync(&packet);

#ifdef UMUEventLog  //
        Log->AddTwoIntParam("UMU:%d EnableAScan for Stroke: %d", _inner_state._umu_id, stroke);
#endif

        _common_state._save_ascan_side = side;
        _common_state._save_ascan_line = line;
        _common_state._save_ascan_stroke = stroke;
        _common_state._save_ascan_st_delay = static_cast<unsigned char>(st_delay);
        _common_state._save_ascan_zoom = zoom;
        _common_state._save_ascan_mode = mode;
        _common_state._save_ascan_strob = strob;

        CalcTVGCurve();
        _common_state._tvg_data_buf = new (tUMU_AScanData);

        memcpy(_common_state._tvg_data_buf, &_common_state._tvg_data, sizeof(tUMU_AScanData));
        _common_state._tvg_data_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);

        _common_state._data_sender->WriteHeadAndPost(edTVGData, &_common_state._tvg_data_buf, SIZE_OF_TYPE(&_common_state._tvg_data_buf));
        ++_common_state._post_count;
        _common_state._critical_section->Release();

        //       waitForUpload();
    }
}

void UMU_LAN::RequestTVGCurve()
{
    _common_state._critical_section->Enter();
    CalcTVGCurve();
    _common_state._tvg_data_buf = new (tUMU_AScanData);

    memcpy(_common_state._tvg_data_buf, &_common_state._tvg_data, sizeof(tUMU_AScanData));
    _common_state._tvg_data_buf->UMUIdx = static_cast<unsigned char>(_common_state._umu_id);

    _common_state._data_sender->WriteHeadAndPost(edTVGData, &_common_state._tvg_data_buf, SIZE_OF_TYPE(&_common_state._tvg_data_buf));
    ++_common_state._post_count;
    _common_state._critical_section->Release();

    //    waitForUpload();
}

void UMU_LAN::CalcTVGCurve()
{
    if (_common_state._save_ascan_zoom != 0) {
        _common_state._tvg_data.Side = _common_state._save_ascan_side;
        _common_state._tvg_data.Line = _common_state._save_ascan_line;
        _common_state._tvg_data.Stroke = _common_state._save_ascan_stroke;

        DEFCORE_ASSERT(static_cast<unsigned char>(_common_state._save_ascan_side) < 2);
        DEFCORE_ASSERT(_common_state._save_ascan_line < 2);
        DEFCORE_ASSERT(_common_state._save_ascan_stroke < MaxStrokeCount);

        unsigned char* currentTVG = _common_state._tvg_curve[_common_state._save_ascan_side][_common_state._save_ascan_line][_common_state._save_ascan_stroke];
        if (currentTVG == NULL) {
            _common_state._tvg_curve[_common_state._save_ascan_side][_common_state._save_ascan_line][_common_state._save_ascan_stroke] = new unsigned char[232];
            currentTVG = _common_state._tvg_curve[_common_state._save_ascan_side][_common_state._save_ascan_line][_common_state._save_ascan_stroke];
            memset(currentTVG, 0, 232);
        }

        memcpy(&_common_state._tvg_data.Data[0], currentTVG, 232);
    }
}

void UMU_LAN::DisableAScan(bool Force)
{
    // qDebug() << "UMU" << this->_id << "============================== before if DisableAScan";
    if (_common_state._ascan_state || Force) {
        // qDebug() << "UMU" << this->_id << "============================== into if DisableAScan";
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmDisableAScan;
        packet.Data[1] = 0;
        AddToOutBuffSync(&packet);
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d DisableAScan", _inner_state._umu_id);
#endif
        _common_state._ascan_state = false;
    }

    waitForUpload();
}

int UMU_LAN::EnableBScan()
{
    _common_state._critical_section->Enter();
    int ret = 0;
    // qDebug() << "UMU" << this->_id << "============================== before if EnableBScan";
    if ((!_common_state._bscan_state) || (_common_state._bscan_session_id.empty())) {
        tLAN_Message packet(uEnable, 0x02);

        packet.Data[0] = wmEnableBScan;
        packet.Data[1] = 0;
        AddToOutBuffNoSync(&packet);
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d EnableBScan", _inner_state._umu_id);
#endif
        _common_state._bscan_state = true;

        // Сессия данных В-развертки
        if (_common_state._bscan_session_id.empty()) {
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

        ret = _common_state._bscan_session_id.back();
    }
    else {
        if (_common_state._bscan_session_id.empty()) {
            ret = -1;
        }
        else {
            ret = _common_state._bscan_session_id.back();
        }
    }
    _common_state._critical_section->Release();

    waitForUpload();
    return ret;
}

void UMU_LAN::DisableBScan(bool Force)
{
    // qDebug() << "UMU" << this->_id << "============================== before if DisableBScan";
    _common_state._critical_section->Enter();
    if (_common_state._bscan_state || Force) {
        // qDebug() << "UMU" << this->_id << "============================== into if DisableBScan";
        tLAN_Message packet(uEnable, 0x02);

        packet.Data[0] = wmDisableBScan;
        packet.Data[1] = 0;
        AddToOutBuffNoSync(&packet);

        _common_state._bscan_state = false;

#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d DisableBScan", _inner_state._umu_id);
#endif
    }
    _common_state._critical_section->Release();
    waitForUpload();
}

void UMU_LAN::EnableACSumm()  // БУИ - Разрешить передачу сырых данных АК
{
    // qDebug() << "UMU" << this->_id << "============================== before if EnableACSumm";
    _common_state._critical_section->Enter();
    if (!_common_state._ac_state) {
        // qDebug() << "UMU" << this->_id << "============================== into if EnableACSumm";
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmEnableACSumm;
        packet.Data[1] = 0;
        AddToOutBuffNoSync(&packet);
        _common_state._ac_state = true;
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d EnableACSumm", _inner_state._umu_id);
#endif
    }
    _common_state._critical_section->Release();
    //    waitForUpload();
}

void UMU_LAN::DisableACSumm(bool Force)  //   БУИ - Запретить передачу сырых данных АК
{
    // qDebug() << "UMU" << this->_id << "============================== before if DisableACSumm";
    _common_state._critical_section->Enter();
    if (_common_state._ac_state || Force) {
        // qDebug() << "UMU" << this->_id << "============================== into if DisableACSumm";
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmDisableACSumm;
        packet.Data[1] = 0;
        AddToOutBuffNoSync(&packet);
        _common_state._ac_state = false;
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d DisableACSumm", _inner_state._umu_id);
#endif
    }
    _common_state._critical_section->Release();
    //    waitForUpload();
}

void UMU_LAN::EnableAC()  // БУИ - Разрешить передачу данных АК
{
    // qDebug() << "UMU" << this->_id << "============================== before if EnableAC";
    _common_state._critical_section->Enter();
    if (!_common_state._ac_state) {
        // qDebug() << "UMU" << this->_id << "============================== into if EnableAC";
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmEnableAC;
        packet.Data[1] = 0;
        AddToOutBuffNoSync(&packet);
        _common_state._ac_state = true;
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d EnableAC", _inner_state._umu_id);
#endif
    }
    _common_state._critical_section->Release();
    //    waitForUpload();
}

void UMU_LAN::EnableAlarm()
{
    // qDebug() << "UMU" << this->_id << "============================== before if EnableAlarm";
    if (!_common_state._alarm_state) {
        // qDebug() << "UMU" << this->_id << "============================== into if EnableAlarm";

        _common_state._alarm_state = true;
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmEnableAlarm;
        packet.Data[1] = 0;
        AddToOutBuffSync(&packet);
#ifdef UMUEventLog
        Log->AddIntParam("UMU:%d EnableAlarm", _inner_state._umu_id);
#endif
    }
    //    waitForUpload();
}

void UMU_LAN::DisableAlarm(bool Force)
{
    // qDebug() << "UMU" << this->_id << "============================== before if DisableAlarm";
    if (_common_state._alarm_state || Force) {
        // qDebug() << "UMU" << this->_id << "============================== into if DisableAlarm";
        _common_state._alarm_state = false;
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmDisableAlarm;
        packet.Data[1] = 0;
        AddToOutBuffSync(&packet);
#ifdef UMUEventLog  //
        Log->AddIntParam("UMU:%d DisableAlarm", _inner_state._umu_id);
#endif
    }
    //    waitForUpload();
}

void UMU_LAN::EnableCalcDisCoord()  // БУИ - включить передачу данных о срабатывании основного датчика пути в формате п. 4.31***** вместо формата п. 4.13.
{
    // qDebug() << "UMU" << this->_id << "============================== before if EnableCalcDisCoord";
    if (!_common_state._calc_DisCrd_state) {
        // qDebug() << "UMU" << this->_id << "============================== into if EnableCalcDisCoord";
        _common_state._calc_DisCrd_state = false;
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmEnableCalcDisCoord;
        packet.Data[1] = 0;
        AddToOutBuffSync(&packet);
#ifdef UMUEventLog  //
        Log->AddIntParam("UMU:%d DisableAlarm", _inner_state._umu_id);
#endif
    }
    waitForUpload();
}

void UMU_LAN::DisableCalcDisCoord(bool Force)  // БУИ - отключить передачу данных о срабатывании основного датчика пути в формате п. 4.31
{
    // qDebug() << "UMU" << this->_id << "============================== before if DisableCalcDisCoord";
    if (_common_state._calc_DisCrd_state || Force) {
        // qDebug() << "UMU" << this->_id << "============================== into if DisableCalcDisCoord";
        _common_state._calc_DisCrd_state = false;
        tLAN_Message packet(uEnable, 0x02);
        packet.Data[0] = wmDisableCalcDisCoord;
        packet.Data[1] = 0;
        AddToOutBuffSync(&packet);
#ifdef UMUEventLog  //
        Log->AddIntParam("UMU:%d DisableAlarm", _inner_state._umu_id);
#endif
    }
    waitForUpload();
}


void UMU_LAN::GetDeviceID()
{
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmDeviceID;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d GetDeviceID", _inner_state._umu_id);
#endif
}

void UMU_LAN::GetACTresholds()
{
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmGetACTresholds;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d GetDeviceID", _inner_state._umu_id);
#endif
}

void UMU_LAN::PathSimulator(unsigned char SimulatorNumber, bool state)
{
    tLAN_Message packet(uPathSimulator, 0x02);
    packet.Data[0] = SimulatorNumber;
    packet.Data[1] = state ? 0x01 : 0x00;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddTwoIntParam("UMU:%d SetPathSimulator: %d", _inner_state._umu_id, state);
#endif
}

void UMU_LAN::PathSimulatorEx(unsigned char SimulatorNumber, unsigned short Interval_ms, bool state)  // БУИ - Имитатор датчика пути (расширенный вариант)
{
    tLAN_Message packet(uPathSimulatorEx, 0x04);
    packet.Data[0] = SimulatorNumber;
    packet.Data[1] = state ? 0x01 : 0x00;
    packet.Data[2] = static_cast<unsigned char>(Interval_ms & 0x00FF);
    packet.Data[3] = static_cast<unsigned char>((Interval_ms >> 8) & 0x00FF);
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddTwoIntParam("UMU:%d SetPathSimulator: %d", _inner_state._umu_id, state);
#endif
}

void UMU_LAN::ScanerPathSimulator(bool state, unsigned short Interval_ms, unsigned char Param_A, unsigned char Param_B)  // БУИ - Имитатор датчика пути сканера
{
    tLAN_Message packet(uPathSimulatorEx2, 0x04);
    packet.Data[0] = state ? 0x01 : 0x00;
    packet.Data[1] = static_cast<unsigned char>(Interval_ms & 0x00FF);
    packet.Data[2] = static_cast<unsigned char>((Interval_ms >> 8) & 0x00FF);
    packet.Data[3] = Param_A;
    packet.Data[4] = Param_B;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddTwoIntParam("UMU:%d SetPathSimulator: %d", _inner_state._umu_id, state);
#endif
}

void UMU_LAN::Enable()
{
    // qDebug() << "UMU" << this->_id << "============================== Enable";
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmEnable;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d Enable work", _inner_state._umu_id);
#endif
    waitForUpload();
}

void UMU_LAN::Disable()
{
    // qDebug() << "UMU" << this->_id << "============================== Disable";
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmDisable;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d Disable work", _inner_state._umu_id);
#endif
    waitForUpload();
}

void UMU_LAN::Reboot()
{
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmReboot;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d Reboot", _inner_state._umu_id);
#endif
}

void UMU_LAN::GetWorkTime()
{
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmGetWorkTime;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d GetWorkTime", _inner_state._umu_id);
#endif
}

int UMU_LAN::SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain)  // Установка значения датчика пути
{
    // qDebug() << "UMU" << this->_id << "============================== SetPathEncoderData";
    tLAN_Message packet(dPathEncoderSetup, 0x05);
    unsigned char UseAsMain_mask = 0;
    if (UseAsMain) {
        UseAsMain_mask = 0x80;
    }
    unsigned char Simulator_mask = 0;
    if (Simulator) {
        Simulator_mask = 0x08;
    }
    packet.Data[0] = UseAsMain_mask | Simulator_mask | (path_encoder_index & 0x03);
    packet.Data[1] = new_value & 0xFF;
    packet.Data[2] = (new_value >> 8) & 0xFF;
    packet.Data[3] = (new_value >> 16) & 0xFF;
    packet.Data[4] = (new_value >> 24) & 0xFF;
    _common_state._critical_section->Enter();
    int ret = 0;
    AddToOutBuffNoSync(&packet);

    if (UseAsMain) {
        _common_state._main_path_encoder_index = path_encoder_index;
        ret = -1;
    }
    else {
        _common_state._expectation_setting_coord = true;  // До прихода подтверждения установки значения координаты события В-развертки выбрасываются
                                                          /*                                             // Сессия формирования координаты
                                                             if (_common_state._coord_session_id.size() == 0) {
                                                                 _common_state._coord_session_id.push_back(0);  // Сессия данных В-развертки
                                                             }   else _common_state._coord_session_id.push_back(_common_state._coord_session_id.back() + 1); // Увеличиваем номер сесии данных В-развертки
                                                             return _common_state._coord_session_id.back(); */
        ret = -1;
    }

    _common_state._critical_section->Release();

#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d SetPathEncoderData", _inner_state._umu_id);
#endif
#ifdef SIMULATION_CONTROLER_AND_MOTORS
    SimulationStartTime = GetTickCount();
#endif

    waitForUpload();
    return ret;
}

void UMU_LAN::CalcACThreshold_(unsigned char PercentageIncrease, unsigned char ThRecalcCount)  // Расчитать пороги акустического контакта
{
    tLAN_Message packet(uCalcACThreshold, 0x02);
    packet.Data[0] = PercentageIncrease;
    packet.Data[1] = ThRecalcCount;
    _common_state._critical_section->Enter();
    AddToOutBuffNoSync(&packet);

    _common_state._ac_th_request = true;
    _common_state._ac_th_request_time = GetTickCount_();
    _common_state._critical_section->Release();

#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d SetACThreshold", _inner_state._umu_id);
#endif

    //    waitForUpload();
}

void UMU_LAN::SetBSFilterStDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay)  // Установка момента начала фильтрации сигналов B-развертки
{
    DEFCORE_ASSERT(stroke < MaxStrokeCount);
    tLAN_Message packet(uBSFilterStDelay, 0x03);
    packet.Data[0] = static_cast<unsigned char>(((((side) << 1) | line) << 6) | stroke);
    packet.Data[1] = delay;
    packet.Data[2] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d SetBSFilterStDelay", _inner_state._umu_id);
#endif

    //    waitForUpload();
}

void UMU_LAN::GetUAkkQuery()
{
    tLAN_Message packet(uEnable, 0x02);
    packet.Data[0] = wmGetVoltage;
    packet.Data[1] = 0;
    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d GetUAKK", _inner_state._umu_id);
#endif
}

void UMU_LAN::SetStrokeParameters(const tStrokeParams* stroke_params)
{
    // qDebug() << "SetStrokeParameters";

    if ((!_common_state.BScanFiltrationModeState) && ((stroke_params->ParamsLeftLine1 & 0x01 == 0x01) || (stroke_params->ParamsLeftLine2 & 0x01 == 0x01) || (stroke_params->ParamsRightLine1 & 0x01 == 0x01) || (stroke_params->ParamsRightLine2 & 0x01 == 0x01)))
        _common_state.BSFilterStrokeData.push_back(*stroke_params);

    tLAN_Message packet(uParameter, 0x30);
    memcpy(packet.Data, stroke_params, sizeof(tStrokeParams));

    DEFCORE_ASSERT(stroke_params->StrokeIdx < 16);
    DEFCORE_ASSERT(stroke_params->ACStartDelayRightLine1 >= 20);
    DEFCORE_ASSERT(stroke_params->ACStartDelayRightLine1 <= 180);
    DEFCORE_ASSERT(stroke_params->ACStartDelayRightLine2 >= 20);
    DEFCORE_ASSERT(stroke_params->ACStartDelayRightLine2 <= 180);

    DEFCORE_ASSERT(stroke_params->ACStartDelayLeftLine1 >= 20);
    DEFCORE_ASSERT(stroke_params->ACStartDelayLeftLine1 <= 180);
    DEFCORE_ASSERT(stroke_params->ACStartDelayLeftLine2 >= 20);
    DEFCORE_ASSERT(stroke_params->ACStartDelayLeftLine2 <= 180);

    DEFCORE_ASSERT(stroke_params->ZondAmplRightLine1 <= 0x07);
    DEFCORE_ASSERT(stroke_params->ZondAmplRightLine2 <= 0x07);
    DEFCORE_ASSERT(stroke_params->ZondAmplLeftLine1 <= 0x07);
    DEFCORE_ASSERT(stroke_params->ZondAmplLeftLine2 <= 0x07);

    DEFCORE_ASSERT((stroke_params->WorkFrequencyL & 0x0F) < 2);
    DEFCORE_ASSERT(((stroke_params->WorkFrequencyL >> 4) & 0x0F) < 2);

    DEFCORE_ASSERT((stroke_params->WorkFrequencyR & 0x0F) < 2);
    DEFCORE_ASSERT(((stroke_params->WorkFrequencyR >> 4) & 0x0F) < 2);

    BScanDelayFactor[usRight][ulRU1][stroke_params->StrokeIdx] = stroke_params->DelayFactorRightLine1;  // Множитель задержек сигналов В-развертки, для правой стороны, линия 1
    BScanDelayFactor[usRight][ulRU2][stroke_params->StrokeIdx] = stroke_params->DelayFactorRightLine2;  // Множитель задержек сигналов В-развертки, для правой стороны, линия 2
    BScanDelayFactor[usLeft][ulRU1][stroke_params->StrokeIdx] = stroke_params->DelayFactorLeftLine1;    // Множитель задержек сигналов В-развертки, для левой стороны, линия 1
    BScanDelayFactor[usLeft][ulRU2][stroke_params->StrokeIdx] = stroke_params->DelayFactorLeftLine2;    // Множитель задержек сигналов В-развертки, для левой стороны, линия 2

    AddToOutBuffSync(&packet);
#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d Set strobe params", _inner_state._umu_id);
#endif
    waitForUpload();
}

void UMU_LAN::SetBScanFiltrationMode(bool State)
{
    size_t size = _common_state.BSFilterStrokeData.size();  // Отключение фильтрации В-развертки (отсечение сигналов за вторым стробом )

    if (State) {
        _common_state.BScanFiltrationModeState = true;
        for (size_t i = 0; i < size; ++i) {
            tStrokeParams params = _common_state.BSFilterStrokeData[i];
            params.ParamsLeftLine1 = params.ParamsLeftLine1 & 0xFE;
            params.ParamsLeftLine2 = params.ParamsLeftLine2 & 0xFE;
            params.ParamsRightLine1 = params.ParamsRightLine1 & 0xFE;
            params.ParamsRightLine2 = params.ParamsRightLine2 & 0xFE;

            SetStrokeParameters(&params);
        }
    }
    else {
        for (size_t i = 0; i < size; ++i) {
            tStrokeParams params = _common_state.BSFilterStrokeData[i];
            SetStrokeParameters(&params);
        }
        _common_state.BScanFiltrationModeState = false;
    }
}

void UMU_LAN::SetLastSysCrd(int EncoderIndex, int NewValue)
{
    _special_state.LastSysCrd[EncoderIndex] = NewValue;
    _special_state._pathstep_buf.XSysCrd[EncoderIndex] = NewValue;
}
