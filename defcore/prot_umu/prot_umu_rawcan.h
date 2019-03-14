#ifndef PROT_UMU_RAWCAN_H
#define PROT_UMU_RAWCAN_H

#include "iumu.h"
#include "utils_can.h"
#include "prot_umu/prot_umu_lan.h"

#include <map>

namespace CANProtDebug
{
//#define DbgLog

extern	LANProtDebug::Add_Log onAddLog;
extern	bool useLog;

const int cLine0 = 0;
static const int AmplDBIdx_int = 8;

class UMU_RAWCAN : public cIUMU
{
public:
	typedef can_utils::cMessageCan cMessageCan;

protected:
enum ePrismDelayTimeType
{
	ePrismDelayShort = 0,
	ePrismDelayLong = 1
};
typedef union _PrismDelayTime
{
	unsigned char  _shortType;
    unsigned short _longType;
} tPrismDelayTime;
//
	// Special for UMU_1002:
	std::queue <cMessageCan>  _out_buffer; // Буфер выгрузки данных
//	cMessageCan              *_out_block;  // Указатель на свободный блок в буфере выгрузки данных
	cMessageCan               _message;

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

		unsigned long _akk_voltage_time;  // Время прихода последнего сообщения от БУМа о напряжении аккумулятора

		// Полученный заряд аккумулятора
		unsigned char _akk_voltage;

		// Текущий полученный пакет A-развертки
		unsigned char _ascan_curr_num;

		// Номер такта, для которого сейчас приходят пакеты, полученный из заголовка А развертки
		unsigned char _ascan_curr_stroke;

		// Буфер под полученные B и M развертки
		PtUMU_BScanData _bscan_buf;

		int _bscanMessageTacktMem;
//
		EventDataType  _save_event_id;   // Для сохранения id данных используется для корректного перехода с В Развертики на М Развертку
		int _XSysCrd[2];
		signed char _Dir;
		int _XDisCrd[2];
        int _XSysCrd_Last[2];
//
		ePrismDelayTimeType _prismDelayTimeType;
		tPrismDelayTime _prismDelayTime[2][MaxStrokeCount]; // [Сторона][Такт]
	};
	cSpecialState _special_state;

	typedef bool (UMU_RAWCAN::*Parser)(unsigned char *pBuf, unsigned char dataSize);
  // Карта соотносящая ид сообщения и парсер для него
	std::map<unsigned char, Parser> _response_map;

    // ----------------------- INNER SERVICE MEMBERS ---------------------------
    // Special for UMU_1002:
    virtual bool EndWorkTest() { return (_common_state._end_work_flag) && _out_buffer.empty(); }

    // VIRTUAL OVERRIDE DECLARATIONS:
    virtual void AddToOutBuff();
    virtual void AddToOutBuff_(cMessageCan *_out_block);
    virtual void Unload();
	virtual void TickSend();
	virtual unsigned char GetMessageSize(const cMessageCan &message)=0;

public:
    // ------------------------- C-TORS and D-TORS -----------------------------
    UMU_RAWCAN();
    ~UMU_RAWCAN() {}

    // ------------------------------ SETTERS ----------------------------------

    virtual void sendCommandWithData(const unsigned char  command_id,
                                    const unsigned char *data,
                                    const unsigned char  size) = 0;

    // Non virtual helpers:
    inline void setStrokeParameter(const unsigned char stroke_id,
                                   const unsigned char offset,
                                   const unsigned char param);

    void sendEmptyCommand(const unsigned char command_id);

    inline void sendCommandWithOneByte(const unsigned char command_id,
                                       const unsigned char byte);


	// Non virtual commands:
// выполнение команды с "большим" временем
	void SetPrismDelay(unsigned char  stroke,
					   unsigned short left_delay,
					   unsigned short right_delay);
// выполнение команды с "маленьким" временем
	void SetPrismDelay(unsigned char  stroke,
					   unsigned char left_delay,
					   unsigned char right_delay);

	// VIRTUAL OVERRIDE DECLARATIONS:
	virtual void SetPrismDelay(eUMUSide       side,
							   unsigned char  line,
							   unsigned char  stroke,
							   unsigned short delay);
	// -------------------------------------------------------------------------
	// Members that can not be implemented in this protocol
	// -------------------------------------------------------------------------

   // Do not exists in this protocol
	virtual bool SetLeftSideSwitching(UMULineSwitching state) {UNUSED(state); return false; }

    virtual void SetLineSwitching(UMULineSwitching state);

	virtual void SetPathEncoderData(char path_encoder_index,
									int  new_value,
									bool UseAsMain);
	virtual int SetPathEncoderData(char path_encoder_index,
								   bool Simulator,
								   int  new_value,
								   bool UseAsMain);

    virtual void SetLastSysCrd(int EncoderIndex, int NewValue);

	// Do not exists in this p rotocol
    virtual void DisableAlarm(bool Force)                      { /*assert(false);*/ }

	// Do not exists in this p rotocol
    //virtual void CalcACThreshold(unsigned char PercentageIncrease) {(void)PercentageIncrease;}

	virtual void CalcACThreshold_(unsigned char PercentageIncrease, unsigned char ThRecalcCount) {(void)PercentageIncrease; (void)ThRecalcCount;}

	// Do not exists in this p rotocol
    //virtual void SetACThreshold(void) { }

	// Do not exists in this p rotocol
	virtual void SetBSFilterStDelay(eUMUSide        side,
									unsigned char   line,
									unsigned char   stroke,
									unsigned char   delay) {(void)side;(void)line;(void)stroke;(void)delay; } // Установка момента начала фильтрации сигналов B-развертки
	// Do not exists in this p rotocol
	virtual void PathSimulatorEx(unsigned char SimulatorNumber,
								 unsigned short Interval_ms,
								 bool state) { (void)SimulatorNumber, (void)Interval_ms; (void) state; }

	virtual void ScanerPathSimulator(bool state,
									 unsigned short Interval_ms,
									 unsigned char Param_A,
									 unsigned char Param_B)
									 { (void) state; (void)Interval_ms; (void)Param_A; (void)Param_B;}
	// Do not exists in this p rotocol
	virtual void EnableACSumm(void) {};
    // Do not exists in this p rotocol
	virtual void DisableACSumm(bool Force){} ;

    // Do not exists in this p rotocol
    virtual void EnableAC(void) {}; // Разрешить передачу данных акустического контакта (АК)

    // Do not exists in this p rotocol
    virtual void DisableAC(bool Force) {}; // Запретить передачу данных АК


    // -------------------------------------------------------------------------

    // Valid overrided commands:
    virtual void SetStrokeCount(unsigned char count);
/*
    virtual void SetGainSegment(eUMUSide      side,
                                unsigned char line,
                                unsigned char stroke,
                                unsigned char st_delay,
                                unsigned char st_val,
                                unsigned char ed_delay,
                                unsigned char ed_val,
                                unsigned char ascan_scale);
*/
    virtual void SetGainSegmentPoint(eUMUSide      side,
                                      unsigned char line,
                                      unsigned char stroke,
                                      unsigned char delay,
                                      unsigned char gain,
                                      unsigned char ascan_scale,
                                      eGainPointType point);


    virtual void SetGate(eUMUSide        side,
                         unsigned char   line,
                         unsigned char   stroke,
                         unsigned char   index,
                         unsigned char   st_delay,
                         unsigned char   ed_delay,
                         unsigned char   level,
                         eStrobeMode     mode,
						 eAlarmAlgorithm asd_mode);

	virtual void EnableAScan(eUMUSide       side,
							 unsigned char  line,
							 unsigned char  stroke,
							 unsigned short st_delay,
							 unsigned char  zoom,
							 unsigned char  mode,
							 unsigned char  strob);

	virtual void PathSimulator(unsigned char number,
							   bool          state);

	virtual void SetStrokeParameters(const tStrokeParams *stroke_params);
	virtual void DisableAScan(bool Force);
	virtual int EnableBScan();
	virtual void DisableBScan(bool Force);
	virtual void EnableAlarm();
	virtual void Enable();
	virtual void Disable();
	virtual void Reboot();
	virtual void GetWorkTime();
	virtual void GetDeviceID();
    virtual void GetACTresholds() { };
	virtual void RequestTVGCurve();
	virtual void CalcTVGCurve();
// чтение полученного от БУМ полученного напряжения
    unsigned char GetUAkkValue() { return _special_state._akk_voltage; }

private:
//
	virtual void GetUAkkQuery();
    // Каждый парсер возвращает true если он
    // полностью отработал и false, если возникла ошибка по ходу выполнения
    // buf - указатель на данные в CAN-сообщении
    // ------------------------- RESPONSE PARSERS ------------------------------
    // А-развертка
    bool parseResponse_dAScanDataHead     (unsigned char *data, unsigned char size);
    bool parseResponse_dAScanMeasForCurr  (unsigned char *data, unsigned char size);
    bool parseResponse_dAScanMeasForZero  (unsigned char *data, unsigned char size);
    bool parseResponse_dAScanMeasFirstPack(unsigned char *data, unsigned char size);
    bool parseResponse_dAScanMeasOtherPack(unsigned char *data, unsigned char size);
    // B и M-развертки
	bool parseResponse_dBScanData         (unsigned char *data, unsigned char size);
// сообщения с кодом M-развертки не обрабатываются
// М-развертка представляется сообщением датчика пути со смещением 0 и данными с
// кодом B-развертки
// В случае использования 1-канального БУМ сообщения с кодом М-развертки игнорируются
    bool parseResponse_dMScanData         (unsigned char *data, unsigned char size) {UNUSED(data); UNUSED(size); return true;}
    // Управление БУМ
    bool parseResponse_dAlarmData         (unsigned char *data, unsigned char size);
    bool parseResponse_dPathStep          (unsigned char *data, unsigned char size);
    bool parseResponse_dPathStepAddit     (unsigned char *data, unsigned char size);
    bool parseResponse_dVoltage           (unsigned char *data, unsigned char size);
    bool parseResponse_dWorkTime          (unsigned char *data, unsigned char size);
    // Общие команды
    bool parseResponse_cDevice            (unsigned char *data, unsigned char size);
    bool parseResponse_cFWver             (unsigned char *data, unsigned char size);
	bool parseResponse_cSerialNumber      (unsigned char *data, unsigned char size);
    virtual bool parseResponse_BScanStopped (unsigned char *data, unsigned char size); // {UNUSED(data); UNUSED(size); return true;}
};

}
#endif // PROT_UMU_RAWCAN_H
