#include "iumu.h"
//#include <QDebug>

bool cIUMU::isPointerInitialized(const void* ptr) const
{
    if (ptr == NULL) {
        return false;
    }
    return true;
}

cIUMU::cIUMU()
{
    // Инициализируем нулем все переменные _inner_state
    _common_state._created = false;
    _common_state._critical_section = NULL;
    _common_state._data_transfer = NULL;
    _common_state._data_sender = NULL;
    _common_state._ascan_measure_buf = NULL;
    _common_state._ascan_data_buf = NULL;
    // memset(&_common_state._tvg_curve, 0, sizeof(_common_state._tvg_curve));
    for (int side = 0; side < MaxSideCount; ++side) {
        for (int line = 0; line < MaxLineCount; ++line) {
            for (int stroke = 0; stroke < MaxStrokeCount; ++stroke) {
                _common_state._tvg_curve[side][line][stroke] = NULL;
            }
        }
    }
    _common_state._tvg_data_buf = NULL;
    memset(&_common_state._tvg_data, 0, sizeof(tUMU_AScanData));
    _common_state._save_ascan_stroke = 0;
    _common_state._save_ascan_zoom = 10;
    _common_state._save_ascan_line = 0;
    _common_state._save_ascan_side = usLeft;
    _common_state._save_ascan_st_delay = 0;
    _common_state._save_ascan_mode = 0;
    _common_state._save_ascan_strob = 0;
    _common_state._alarm_state = false;
    _common_state._calc_DisCrd_state = false;
    _common_state._ascan_state = false;
    _common_state._bscan_state = false;
    _common_state._ac_state = false;
    _common_state._bscan_session_id.clear();
    _common_state._bscan_last_session_id = -1;
    _common_state._alarm_buf = NULL;
    _common_state._stroke_count = 0;
    _common_state._umu_id = 0;
    _common_state._connection_id = 0;
    _common_state._end_work_flag = false;
    _common_state._end_work_time = 0;
    _common_state._last_OK_upload_time = 0;
    _common_state._send_tick_time = 0;
    _common_state._request_time = 0;
    _common_state._last_time = 0;
    _common_state._firmware_info_time = 0;
    _common_state._last_event_time = 0;
    _common_state._serial_number_time = 0;
    _common_state._work_time = 0;
    _common_state._device_speed = 0;
    _common_state._serial_number = 0;
    _common_state._firmware_version_hi = 0;
    _common_state._firmware_version_lo = 0;
    _common_state._post_count = 0;
    _common_state._message_count = 0;
    _common_state._error_message_count = 0;
    _common_state._write_error_count = 0;
    _common_state._save_write_error_count = 0;
    _common_state._message_read_bytes_count = 0;
    _common_state._read_bytes_count = 0;
    memset(&_common_state._event_count, 0, sizeof(_common_state._event_count));
    _common_state._ac_th_request = false;
    _common_state._ac_th_request_time = 0;
    _common_state._ac_th_data_exists = false;
    memset(&_common_state._ac_th_data, 0, sizeof(_common_state._ac_th_data));
    _common_state._expectation_setting_coord = false;
    _common_state._expectation_setting_coord_2 = false;
    _common_state._main_path_encoder_index = 0;
    _common_state.BSFilterStrokeData.clear();
    _common_state.BScanFiltrationModeState = false;

    _bscan_data_type = bstSimple;
    // Инициализируем частные случаи (их 5 супротив ~50 тех что были ранее)
    _wstate = bsOff;
    _read_state = rsOff;
    _save_event_id = edMScan2Data;
    _protocol = eProtUNDEF;
}

void cIUMU::StartWork()
{
#ifdef UMUEventLog  //
    Log->AddIntParam("UMU:%d StartWork", _inner_state._umu_id);
#endif
//    qDebug() << "UMU" << this->_id << "============================== StartWork";
    if (_wstate == bsOff) {
        _wstate = bsPowerOn;
    }
}

void cIUMU::EndWork()
{
//    qDebug() << "UMU" << this->_id << "============================== EndWork";
    DisableAScan();
    DisableBScan();
    DisableAlarm();
    DisableACSumm();
    _common_state._end_work_flag = true;
    _common_state._end_work_time = GetTickCount_();


#ifdef UMUEventLog
    Log->AddIntParam("UMU:%d EndWork", _inner_state._umu_id);
#endif
}

bool cIUMU::Tick()
{
    this->TickSend();
    this->TickReceive();
    SLEEP(1);
    return !EndWorkTest();
}

void cIUMU::SetData(int umu_id, cIDataTransfer* data_transfer, cEventManager* data_sender, cCriticalSection* cs)
{
    _common_state._umu_id = umu_id;
    _common_state._data_sender = data_sender;
    _common_state._data_transfer = data_transfer;
    _common_state._critical_section = cs;
}

// ------------------------------ GETTERS ----------------------------------
// COMMON DEFENITIONS:
unsigned int cIUMU::GetDownloadSpeed() const
{
    if (!isPointerInitialized(_common_state._data_transfer)) {
        return 0;
    }

    return _common_state._data_transfer->getDownloadSpeed(_common_state._connection_id);
}

unsigned int cIUMU::GetUploadSpeed() const
{
    if (!isPointerInitialized(_common_state._data_transfer)) {
        return 0;
    }

    return _common_state._data_transfer->getUploadSpeed(_common_state._connection_id);
}
