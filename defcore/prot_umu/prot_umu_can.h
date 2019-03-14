#ifndef PROT_UMU_CAN_H
#define PROT_UMU_CAN_H

#include "datatransfers/datatransfer_can.h"
#include "iumu.h"
#include "utils_can.h"

#include <map>

class UMU_CAN : public cIUMU
{
public:
    typedef can_utils::cMessageCan cMessageCan;

private:
    // Special for UMU_1002:
    std::queue<cMessageCan> _out_buffer;  // Буфер выгрузки данных
    cMessageCan* _out_block;              // Указатель на свободный блок в буфере выгрузки данных
    cMessageCan _message;

    typedef bool (UMU_CAN::*Parser)(const cMessageCan&);
    // Карта соотносящая ид сообщения и парсер для него
    std::map<unsigned char, Parser> _response_map;

    // POD-класс аналогичный _common_state в базовом классе, задает внутренние
    // переменные специфичные для UMU_1002 и не используемые в других UMU
    struct cSpecialState
    {
        // Дата и время прошивки БУМА
        unsigned char _ver_sec;
        unsigned char _ver_min;
        unsigned char _ver_hour;
        unsigned char _ver_day;
        unsigned char _ver_mon;
        unsigned char _ver_year;

        // Полученный заряд аккумулятора
        unsigned char _akk_voltage;

        // Текущий полученный пакет A-развертки
        unsigned char _ascan_curr_num;

        // Номер такта, для которого сейчас приходят пакеты, полученный из заголовка А развертки
        unsigned char _ascan_curr_stroke;

        // Буферы под полученные B и M развертки
        PtUMU_OldBScanData _bscan_buf;
        PtUMU_OldBScanData _mscan_buf;
    };
    cSpecialState _special_state;

    // ----------------------- INNER SERVICE MEMBERS ---------------------------
    // Special for UMU_1002:
    void UnPack(cMessageCan buff);
    virtual bool EndWorkTest()
    {
        return (_common_state._end_work_flag) && !_out_buffer.empty();
    }

    // VIRTUAL OVERRIDE DECLARATIONS:
    virtual void AddToOutBuff();
    virtual void Unload();
    virtual void TickSend();
    virtual void TickReceive();

public:
    // ------------------------- C-TORS and D-TORS -----------------------------
    UMU_CAN();
    ~UMU_CAN()
    {
        delete _out_block;
        _out_block = 0;
    }

    eProtocol getProtocolType() const
    {
        return eProtCAN;
    }

    // ------------------------------ SETTERS ----------------------------------

    // Non virtual helpers:
    inline void setStrokeParameter(const unsigned char stroke_id, const unsigned char offset, const unsigned char param);

    inline void sendEmptyCommand(const unsigned char command_id);

    inline void sendCommandWithOneByte(const unsigned char command_id, const unsigned char byte);

    inline void sendCommandWithData(const unsigned char command_id, const unsigned char* data, const unsigned char size);

    // Non virtual commands:
    void SetPrismDelay(unsigned char stroke, unsigned short left_delay, unsigned short right_delay);

    // VIRTUAL OVERRIDE DECLARATIONS:
    // -------------------------------------------------------------------------
    // Members that can not be implemented in this protocol
    // -------------------------------------------------------------------------
    // Do not exists in this protocol
    virtual void SetPrismDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short delay)
    {
        (void) side;
        (void) line;
        (void) stroke;
        (void) delay;
    }

    virtual bool SetLeftSideSwitching(UMULineSwitching state)
    {
        UNUSED(state);
        return false;
    }

    // Do not exists in this protocol
    virtual void SetPathEncoderData(char path_encoder_index, int new_value, bool UseAsMain)
    {
        (void) path_encoder_index;
        (void) new_value;
        (void) UseAsMain;
    }
    // Do not exists in this p rotocol
    virtual void DisableAlarm()
    { /*assert(false);*/
    }

    // Do not exists in this p rotocol
    virtual void SetACThreshold(eUMUSide side, unsigned char line, unsigned char stroke, unsigned int threshold)
    {
        (void) side;
        (void) line;
        (void) stroke;
        (void) threshold;
    }  // Установка порога значений суммы А-развертки

    // Do not exists in this p rotocol
    virtual void SetBSFilterStDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay)
    {
        (void) side;
        (void) line;
        (void) stroke;
        (void) delay;
    }  // Установка момента начала фильтрации сигналов B-развертки

    // Do not exists in this p rotocol
    virtual void EnableAC(void){};  // Разрешить передачу данных акустического контакта (АК)

    // Do not exists in this p rotocol
    virtual void DisableAC(void){};  // Запретить передачу данных АК


    // -------------------------------------------------------------------------

    // Valid overrided commands:
    virtual void SetStrokeCount(unsigned char count);

    virtual void SetGainSegment(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char st_delay, unsigned char st_val, unsigned char ed_delay, unsigned char ed_val);

    virtual void SetGate(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char index, unsigned char st_delay, unsigned char ed_delay, unsigned char level, eStrobeMode mode, eAlarmAlgorithm asd_mode);

    virtual void EnableAScan(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short st_delay, unsigned char zoom, unsigned char mode, unsigned char strob);

    virtual void PathSimulator(unsigned char number, bool state);

    virtual void SetStrokeParameters(const tStrokeParams* stroke_params);
    virtual void DisableAScan();
    virtual int EnableBScan();
    virtual void DisableBScan();
    virtual void EnableAlarm();
    virtual void Enable();
    virtual void Disable();
    virtual void Reboot();
    virtual void GetWorkTime();
    virtual void GetDeviceID();
    virtual void GetUAKK();
    virtual void RequestTVGCurve();
    virtual void CalcTVGCurve();

private:
    // Парсеры для прихордящих сообщений оформлены согласно единому интерфейсу
    // и могут быть в последствие вынесены в отдельные классы для последующей
    // декомпозиции при рефекторинге. Каждый парсер возвращает true если он
    // полностью отработал и false, если возникла ошибка по ходу выполнения
    // ------------------------- RESPONSE PARSERS ------------------------------
    // А-развертка
    bool parseResponse_dAScanDataHead(const cMessageCan& buf);
    bool parseResponse_dAScanMeasForCurr(const cMessageCan& buf);
    bool parseResponse_dAScanMeasForZero(const cMessageCan& buf);
    bool parseResponse_dAScanMeasFirstPack(const cMessageCan& buf);
    bool parseResponse_dAScanMeasOtherPack(const cMessageCan& buf);
    // B и M-развертки
    bool parseResponse_dBScanData(const cMessageCan& buf);
    bool parseResponse_dMScanData(const cMessageCan& buf);
    // Управление БУМ
    bool parseResponse_dAlarmData(const cMessageCan& buf);
    bool parseResponse_dPathStep(const cMessageCan& buf);
    bool parseResponse_dPathStepAddit(const cMessageCan& buf);
    bool parseResponse_dVoltage(const cMessageCan& buf);
    bool parseResponse_dWorkTime(const cMessageCan& buf);
    // Общие команды
    bool parseResponse_cDevice(const cMessageCan& buf);
    bool parseResponse_cFWver(const cMessageCan& buf);
    bool parseResponse_cSerialNumber(const cMessageCan& buf);
};

#endif  // PROT_UMU_CAN_H
