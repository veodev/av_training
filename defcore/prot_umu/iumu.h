#ifndef IUMU_H
#define IUMU_H

#include <cstdio>
#include <cstdlib>
#include <queue>
#include <vector>

#include "datatransfers/idatatransfer.h"
#include "Definitions.h"

#include "EventManager.h"
#include "CriticalSection.h"
#include "Definitions.h"
#include "platforms.h"
#include "TickCount.h"

class cIUMU
{
public:             // Outer enums and structs
    int _id;
    enum eUMUState  // Состояние БУМ
    {
        bsOff = 0,         // Выключенно
        bsInit = 1,        // Инициализация LAN адаптера
        bsPowerOn = 2,     // Включение питания БУМ
        bsRequest = 3,     // Посылка запроса в БУМ
        bsWaitAnswer = 4,  // Ожидание ответа от БУМ
        bsReady = 5,       // Готов к работе
        bsStop = 6,        // Работа приостановленна
    };

    enum eGainPointType  // Тип точки ВРЧ
    {
        gptStart = 0,  // Начальеая
        gptCentr = 1,  // Гдето посередине
        gptEnd = 2     // Конечная
    };

    // Запрет на копирование этого колосса, оперировать им только по указателю
private:
    cIUMU(const cIUMU&);
    void operator=(const cIUMU&);

public:
    // ------------------------- C-TORS and D-TORS -----------------------------
    cIUMU();
    virtual ~cIUMU()
    { /*Empty body*/
        for (int side = 0; side < MaxSideCount; ++side) {
            for (int line = 0; line < MaxLineCount; ++line) {
                for (int stroke = 0; stroke < MaxStrokeCount; ++stroke) {
                    if (_common_state._tvg_curve[side][line][stroke] != NULL) {
                        delete[] _common_state._tvg_curve[side][line][stroke];
                    }
                }
            }
        }
    }

    // ------------------------------ CONTROLS ---------------------------------
    // COMMON DEFENITIONS:
    void StartWork();  // Начало работы с БУМ
    void EndWork();    // Окончание работы с БУМ
    bool Tick();       //
    void Stop()
    {  /*Empty body*/
    }  //

    // VIRTUAL DECLARATIONS:
    virtual bool EndWorkTest() = 0;  //

    // ------------------------------ GETTERS ----------------------------------
    // COMMON DEFENITIONS:
    eUMUState getWState() const
    {
        return _wstate;
    }
    eBScanDataType getBScanDataType() const
    {
        return _bscan_data_type;
    }
    cIDataTransfer* getDataTransfer()
    {
        _common_state._critical_section->Enter();
        cIDataTransfer* res = _common_state._data_transfer;
        _common_state._critical_section->Release();
        return res;
    }
    unsigned long getFirmwareInfoTime() const
    {
        _common_state._critical_section->Enter();
        unsigned long res = _common_state._firmware_info_time;
        _common_state._critical_section->Release();
        return res;
    }
    unsigned long getLastEventTime() const
    {
        _common_state._critical_section->Enter();
        unsigned long res = _common_state._last_event_time;
        _common_state._critical_section->Release();
        return res;
    }
    int getSerialNumber() const
    {
        _common_state._critical_section->Enter();
        int res = _common_state._serial_number;
        _common_state._critical_section->Release();
        return res;
    }
    int getFirmwareVerionHi() const
    {
        _common_state._critical_section->Enter();
        int res = _common_state._firmware_version_hi;
        _common_state._critical_section->Release();
        return res;
    }
    int getFirmwareVerionLo() const
    {
        _common_state._critical_section->Enter();
        int res = _common_state._firmware_version_lo;
        _common_state._critical_section->Release();
        return res;
    }
    int getErrorMessageCount() const
    {
        _common_state._critical_section->Enter();
        int res = _common_state._error_message_count;
        _common_state._critical_section->Release();
        return res;
    }
    unsigned char GetStrokeCount() const
    {
        _common_state._critical_section->Enter();
        unsigned char res = _common_state._stroke_count;
        _common_state._critical_section->Release();
        return res;
    }
    unsigned int GetDownloadSpeed() const;
    unsigned int GetUploadSpeed() const;
    eProtocol getProtocol() const
    {
        return _protocol;
    }
    bool isCreated() const
    {
        return _common_state._created;
    }

    // VIRTUAL DECLARATIONS:
    virtual eProtocol getProtocolType() const = 0;

    // ------------------------------ SETTERS ----------------------------------
    // COMMON DEFENITIONS:
    void setWState(eUMUState state)
    {
        _wstate = state;
    }
    void setDataTransfer(cIDataTransfer* data_transfer)
    {
        _common_state._data_transfer = data_transfer;
    }
    void SetAScanScale(unsigned char zoom)
    {
        _common_state._save_ascan_zoom = zoom;
    }
    void SetDataUMUIdx(int umu_id)
    {
        _common_state._umu_id = umu_id;
    }
    void setConnectionId(int connection_id)
    {
        _common_state._connection_id = connection_id;
    }
    int getConnectionId()
    {
        return _common_state._connection_id;
    }
    void SetData(int umu_id, cIDataTransfer* data_transfer, cEventManager* data_sender, cCriticalSection* cs);

    // VIRTUAL DECLARATIONS:
    // Записать параметры строба
    virtual void SetGate(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char index, unsigned char st_delay, unsigned char ed_delay, unsigned char level, eStrobeMode mode, eAlarmAlgorithm asd_mode) = 0;
    // Записать параметры отрезка кривой ВРУ
    virtual void SetGainSegmentPoint(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay, unsigned char gain, unsigned char ascan_scale, eGainPointType point) = 0;
    // Разрешить передачу А-развертки
    virtual void EnableAScan(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short st_delay, unsigned char zoom, unsigned char mode, unsigned char strob) = 0;
    // Записать задержку в призме
    virtual void SetPrismDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short delay) = 0;
    // Переключение линий со сплошного контроля на сканер
    virtual bool SetLeftSideSwitching(UMULineSwitching state) = 0;
    // Переключение линий со сплошного контроля на ручной контроль (раздельный или совмещенный ) - Ав15
    virtual void SetLineSwitching(UMULineSwitching state)
    {
        UNUSED(state);
    }
    // Выбор формата М-развертки - Ав15
    virtual void SetMScanFormat(MScanFormat mScanVar)
    {
        UNUSED(mScanVar);
    }

    // Включение / выключение имитатора датчика пути
    virtual void PathSimulator(unsigned char number, bool state) = 0;
    virtual int SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain) = 0;  // Установка значения датчика пути
    virtual void SetLastSysCrd(int EncoderIndex, int NewValue) = 0;
    virtual void CalcACThreshold_(unsigned char PercentageIncrease, unsigned char ThRecalcCount) = 0;                            // Расчитать пороги акустического контакта
    virtual void SetBSFilterStDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay) = 0;           // Установка момента начала фильтрации сигналов B-развертки
    virtual void PathSimulatorEx(unsigned char SimulatorNumber, unsigned short Interval_ms, bool state) = 0;                     // БУИ - Имитатор датчика пути (расширенный вариант)
    virtual void ScanerPathSimulator(bool state, unsigned short Interval_ms, unsigned char Param_A, unsigned char Param_B) = 0;  // БУИ - Имитатор датчика пути сканера
    virtual void SetStrokeParameters(const tStrokeParams* stroke_params) = 0;

    virtual void SetStrokeCount(unsigned char count) = 0;
    virtual void DisableAScan(bool Force = false) = 0;   // Запретить передачу А-развертки
    virtual int EnableBScan() = 0;                       // Разрешить передачу B-развертки
    virtual void DisableBScan(bool Force = false) = 0;   // Запретить передачу B-развертки
    virtual void EnableAlarm() = 0;                      // Разрешить передачу АСД
    virtual void DisableAlarm(bool Force = false) = 0;   // Запретит передачу АСД
    virtual void EnableACSumm() = 0;                     // Разрешить передачу сырых данных АК
    virtual void DisableACSumm(bool Force = false) = 0;  // Запретить передачу сырых данных АК
    virtual void EnableAC() = 0;                         // Разрешить передачу данных АК
    virtual void Enable() = 0;                           // Разрешить работу БУМ
    virtual void Disable() = 0;                          // Запретить работу БУМ
    virtual void Reboot() = 0;                           // Перезагрузить БУМ
    virtual void GetWorkTime() = 0;                      // Запрос на получение времени работы
    virtual void GetDeviceID() = 0;                      // Запросить информацию о версии и самоидентификации устройства
    virtual void GetACTresholds() = 0;                   // Запросить пороги АК
    virtual void RequestTVGCurve() = 0;                  // Запрос ВРЧ
    virtual void CalcTVGCurve() = 0;                     // Пересчет кривой ВРЧ
    virtual void EnableCalcDisCoord()                    // включить передачу данных о срабатывании основного датчика пути в формате п. 4.31***** вместо формата п. 4.13.
    {
        return;
    }                                                     // чтение полученного от БУМ напряжения аккумулятора
    virtual void DisableCalcDisCoord(bool Force = false)  // отключить передачу данных о срабатывании основного датчика пути в формате п. 4.31
    {
        (void) Force;
        return;
    }
    virtual void SetBScanFiltrationMode(bool State)  // включить/отключить - фильтрации В-развертки (отсечение сигналов за вторым стробом )
    {
        (void) State;
        return;
    }
    virtual unsigned char GetUAkkValue() // чтение полученного от БУМ напряжения аккумулятора
    {
        return 0;
    }                                     // чтение полученного от БУМ напряжения аккумулятора
    virtual void GetUAkkQuery() = 0;      // Запрос напряжения аккумулятора
    virtual void delayedUnloadStart(){};  // включает накопление накопление данных без выгрузки в DataTransfer
    virtual void delayedUnloadStop(){};   // отключает
protected:                                // Inner enums and structs
    enum eReadState                       // Состояние процесса чтения сообщения
    {
        rsOff = 0,   // Выключенно
        rsHead = 1,  // Ожидаем заголовок сообщения
        rsBody = 2,  // Ожидаем тело сообщения
    };

    enum eHandScanLineMode  // Режим работы канала ручного ПЭП
    {
        Separate = 0,  // Раздельный
        Combine = 1,   // Совмещенный
    };

public:
    enum eINNER_CONSTANTS
    {
        MAX_EVENT_COUNT = 256
    };

protected:  // Inner variables
    // ---------------------- INNER STATE (VARIABLES) --------------------------
    eUMUState _wstate;                // Состояние БУМ
    eBScanDataType _bscan_data_type;  // Формат данных В-развертки
    eReadState _read_state;           // Состояние чтения данных
    eProtocol _protocol;              // Протокол передачи данных (CAN, LAN, ...)
    EventDataType _save_event_id;     // Для сохранения id данных используется для корректного перехода с В Развертики на М Развертку

    // Все внутренние переменные POD-типов, общие для любых реализаций
    // протоколов объявляются внутри структуры cInnerStateBase с целью упрощения
    // отслеживания неинициализированных переменных
    // Благодяря такому подходу мы сможем проинициализировать их все в
    // конструкторе через memset(&_common_state, 0, sizeof(_common_state));
    // После чего не будет необходимости отслеживать для каждой переменной из
    // почти ~50 проинициализированна ли она индивидуально.
    // ! Только для POD-типов ! Ссылки, enum'ы, классы с констр./деструкторами и
    // полиморфные требуется объявлять по старому вне этой структуры.
    struct cInnerStateBase
    {
        bool _created;  // Флаг выставляемый в true, если конструктор отработал без исключений
        cCriticalSection* _critical_section;
        cIDataTransfer* _data_transfer;
        cEventManager* _data_sender;  // Объект передачи данных владельцу класса

        // А развертка
        bool _ascan_state;                                                      // Состояние A-развертки - вкл / выкл
        PtUMU_AScanMeasure _ascan_measure_buf;                                  // Данные измерения
        PtUMU_AScanData _ascan_data_buf;                                        // Данные А-развертки
        unsigned char* _tvg_curve[MaxSideCount][MaxLineCount][MaxStrokeCount];  // Массивы кривых ВРЧ - исходные данные [сторона][линия][такт][мкс]
        PtUMU_AScanData _tvg_data_buf;                                          // Данные ВРЧ
        tUMU_AScanData _tvg_data;                                               // Данные ВРЧ
        eUMUSide _save_ascan_side;

        unsigned char _save_ascan_line;
        unsigned char _save_ascan_stroke;
        unsigned char _save_ascan_st_delay;
        unsigned char _save_ascan_zoom;
        unsigned char _save_ascan_mode;
        unsigned char _save_ascan_strob;

        // TVG
        unsigned char TVG_point_Gain;   // Точка ВРЧ АТТ
        unsigned char TVG_point_Pixel;  // Точка ВРЧ задержка, пиксель
        unsigned char TVG_point_Delay;  // Точка ВРЧ задержка, мкс

        // B развертка
        bool _bscan_state;                   // Состояние В-развертки - вкл / выкл
        bool _ac_state;                      // Состояние Акустического контакта - вкл / выкл
        bool _expectation_setting_coord;     // Ожидание установки значения координаты
        bool _expectation_setting_coord_2;     // Ожидание установки значения координаты 2
        int _bscan_last_session_id;          // Последняя сесия данных В-развертки
        int _main_path_encoder_index;        // Номер используемого датчика пути
        std::vector<int> _bscan_session_id;  // Cесии данных В-развертки
        bool _calc_DisCrd_state;
        std::vector<tStrokeParams> BSFilterStrokeData;  // Список тактов в которых включена фильтрация В-развертки
        bool BScanFiltrationModeState;

        // АСД
        bool _alarm_state;
        PtUMU_AlarmItem _alarm_buf;   // Данные АСД
        unsigned char _stroke_count;  // Количество тактов
        bool _end_work_flag;          // Флаг окончания работы
        unsigned long _end_work_time; // Время окончания работы
        unsigned long _last_OK_upload_time; // Время последней удачной выгрузки
        int _umu_id;                  // Номер БУМ
        int _connection_id;           // Идентификатор соединения в cDatatr

        // Time
        unsigned int _send_tick_time;       // Время отсылки данных
        unsigned int _work_time;            // Время работы БУМ
        unsigned long _request_time;        //
        unsigned long _last_time;           //
        unsigned long _firmware_info_time;  // Время прихода последнего сообщения о прошивки БУМа
        unsigned long _serial_number_time;  // Время прихода последнего сообщения о серийном номере БУМа
        unsigned long _last_event_time;     // Время прихода последнего сообщения от БУМ

        // Данные железа и ПО
        unsigned int _serial_number;        // Серийный номер БУМ
        unsigned int _firmware_version_hi;  // Целая часть номера версии ПО
        unsigned int _firmware_version_lo;  // Дробная часть номера версии ПО

        // Для обработки получаемых сообщений
        unsigned int _post_count;                    // c Число отправленных событий
        unsigned int _message_count;                 // c Количество принятых заголовков сообщений
        unsigned int _error_message_count;           // c Число ошибок типа "несовпадения по размеру" у прочитанного и ожидаемого
        unsigned int _write_error_count;             // c Число операций записи, вернувших ошибку
        unsigned int _save_write_error_count;        // c Сохраненное число операций записи, вернувших ошибку
        unsigned int _message_read_bytes_count;      // c Сколько всего байт должно было быть прочитано (тк размер берется из поля размер заголовка сообщения, а не фактический)
        unsigned int _read_bytes_count;              // c Сколько байт должно было быть прочитано в последнем сообщение
        unsigned int _event_count[MAX_EVENT_COUNT];  // c Подсчет количества пришедших событий

        bool _ac_th_request;                // Запрос порогов АК
        bool _ac_th_data_exists;            // Наличие данных порогов АК
        unsigned short _ac_th_data[48];     // Значение порогов акустического контакта
        unsigned long _ac_th_request_time;  // Время запроса порогов АК

        unsigned int _device_speed;  // Скорость движения системы [ШДП/сек]
    };

public:
    cInnerStateBase _common_state;

protected:
    // ----------------------- INNER SERVICE MEMBERS ---------------------------
    // COMMON DEFENITIONS:
    bool isPointerInitialized(const void* ptr) const;
    // VIRTUAL DECLARATIONS:
    virtual void AddToOutBuff() = 0;  // Добавление данных в выходной буфер - изменяет указатель OutBlock -
    virtual void Unload() = 0;        // Выгрузка данных из буффера OutBuff
    virtual void TickSend() = 0;      // Цикл записи
    virtual void TickReceive() = 0;   // Цикл чтения
};

#endif  // IUMU_H
