#ifndef LANProtUMU
#define LANProtUMU

#include "datatransfers/datatransfer_lan.h"
#include "prot_umu_lan_constants.h"
#include "iumu.h"

namespace LANProtDebug
{
#ifdef DEFCORE_OS_LINUX
typedef void (*Add_Log)(unsigned char* msg, int length, int way);
typedef void (*Add_Log2)(unsigned int* a, unsigned int b);
typedef void (*Add_Log3)(unsigned int);
#elif defined(DEFCORE_CC_BOR)
typedef void __fastcall(__closure* Add_Log)(int UMUIdx, unsigned char* msg, int way);
#else
typedef void (*Add_Log)(unsigned char* msg, int way);
#endif

#ifdef _AUTOCON
#define DbgLog
#endif

extern Add_Log onAddLog;
extern bool useLog;

#ifdef Dbg
void addbytcnt(unsigned int val);
unsigned int rdbytcnt();
void initbytcnt();
void deinitbytcnt();
#endif
}  // namespace LANProtDebug

#define LAN_MESSAGE_HEADER_SIZE 6
#define ASCAN_MESSAGE_BODY_SIZE 233
#define ASCAN_MEASURE_MESSAGE_BODY_SIZE 4


//---------------------------------------------------------------------------
// ---- LAN сообщение ----
//---------------------------------------------------------------------------

#pragma pack(push, 1)
struct tLAN_Message
{
    enum eCONSTANTS
    {
        LanDataMaxSize = 1019,
        LANBufferSize = 1024
    };
    unsigned char Id;
    unsigned char Source;
    unsigned short Size;
    unsigned char MessageCount;
    unsigned char NotUse;
    unsigned char Data[LanDataMaxSize];
    tLAN_Message()
        : Id(0)
        , Source(0)
        , Size(0)
        , MessageCount(0)
        , NotUse(0)
    {
        memset(Data, 0, LanDataMaxSize);
    }
    tLAN_Message(unsigned char id, unsigned short size)
        : Id(id)
        , Source(CDU)
        , Size(size)
        , MessageCount(0)
        , NotUse(0)
    {
        memset(Data, 0, LanDataMaxSize);
    }
    void resetMessage()
    {
        Id = 0;
        Source = 0;
        Size = 0;
        MessageCount = 0;
        NotUse = 0;
        memset(Data, 0, LanDataMaxSize);
    }
};
#pragma pack(pop)

class UMU_LAN : public cIUMU
{
private:
    void waitForUpload();
    // Special for UMU_3204:
    std::queue<tLAN_Message> _out_buffer;  // Буфер выгрузки данных
    tLAN_Message _currentMessage;

    struct cSpecialState
    {
        PtUMU_MetalSensorData _metal_sensor_buf;         // Данные датчика металла
        unsigned int _build_number_controller_firmware;  // Номер сборки версии ПО контроллера
        unsigned int _fpga_proj_ver_id1;                 // Идентификатор 1 версии проекта ПЛИС
        unsigned int _fpga_proj_ver_id2;                 // Идентификатор 2 версии проекта ПЛИС
        unsigned int _fpga_proj_ver_id3;                 // Идентификатор 3 версии проекта ПЛИС
        unsigned int _fpga_proj_ver_id4;                 // Идентификатор 4 версии проекта ПЛИС

        unsigned int _skip_message_count;  // Количество потерянных сообщений
        int _last_message_count;           // Номер последнего принятого сообщния (для контроля корректности поступающих данных)

        PtUMU_BScanData _bscan_buf;       // Данные В-развертки
                                          //        PtUMU_BScanData _bscan_buf2;                     // Данные В-развертки2
        tUMU_PathStepData _pathstep_buf;  // Данные датчика пути
        int LastSysCrd[MaxPathEncoders];  // Последняя координата - для расчета дисплейной координаты
    };
    cSpecialState _special_state;

    // ----------------------- INNER SERVICE MEMBERS ---------------------------
    // Special for UMU_3204:
    void UnPack(const tLAN_Message& buff);
    virtual bool EndWorkTest();

    // VIRTUAL OVERRIDE DECLARATIONS:
    virtual void AddToOutBuff();
    void AddToOutBuffSync(tLAN_Message* _out_block);
    void AddToOutBuffNoSync(tLAN_Message* _out_block);
    virtual void Unload();
    virtual void TickSend();
    virtual void TickReceive();

    // Сохранение множителей задержки В-развертки
    unsigned char BScanDelayFactor[MaxSideCount][MaxLineCount][MaxStrokeCount];  // Сторона, Линия, Такт
    unsigned char Delay_frac_to_Int[10];

    void readMessageHead(unsigned int& res, bool& fRepeat);
    void readMessageBody(unsigned int& res, bool& fRepeat);

    void parsePathStep(const tLAN_Message& buff, bool containsDisCoord);
    void parseA15ScanerPathStep(const tLAN_Message& buff);
    void parseFirmwareVersion(const tLAN_Message& buff);
    void parseSerialNumber(const tLAN_Message& buff);
    void parseAScanMeasure(const tLAN_Message& buff);
    void parseAScanData(const tLAN_Message& buff);
    void parseBScanData(const tLAN_Message& buff);
    void parseAlarmData(const tLAN_Message& buff);
    void parseWorkTime(const tLAN_Message& buff);
    void parseDeviceSpeed(const tLAN_Message& buff);
    void parseMetalSensor(const tLAN_Message& buff);
    void parseConfirmStopBScan(const tLAN_Message& buff);
    void parseConfirmPathEncoderSetup(const tLAN_Message& buff);
    void parseAcousticContactSumm(const tLAN_Message& buff);
    void parseAcousticContact(const tLAN_Message& buff);
    void parseACTresholds(const tLAN_Message& buff);

public:
    // ------------------------- C-TORS and D-TORS -----------------------------

    UMU_LAN();
    virtual ~UMU_LAN();

    // ------------------------------ GETTERS ----------------------------------
    int getFpgaProjVerId1() const;
    int getFpgaProjVerId2() const;
    int getFpgaProjVerId3() const;
    int getFpgaProjVerId4() const;
    int getSkipMessageCount() const;
    int getBuildNumberControllerFirmware() const;

    eProtocol getProtocolType() const;

    // ------------------------------ SETTERS ----------------------------------
    // VIRTUAL OVERRIDE DECLARATIONS:

    virtual void SetStrokeCount(unsigned char count);
    virtual void SetGainSegmentPoint(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay, unsigned char gain, unsigned char ascan_scale, eGainPointType point);
    virtual void SetGate(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char index, unsigned char st_delay, unsigned char ed_delay, unsigned char level, eStrobeMode mode, eAlarmAlgorithm asd_mode);
    virtual void SetPrismDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short delay);
    virtual bool SetLeftSideSwitching(UMULineSwitching state);                                               // Переключение линий с сплошного контроля на сканер
    virtual int SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain);  // Установка значения датчика пути
    virtual void SetLastSysCrd(int EncoderIndex, int NewValue);
    virtual void CalcACThreshold_(unsigned char PercentageIncrease, unsigned char ThRecalcCount);                   // Расчитать пороги акустического контакта
    virtual void SetBSFilterStDelay(eUMUSide side, unsigned char line, unsigned char stroke, unsigned char delay);  // Установка момента начала фильтрации сигналов B-развертки

    ////////////////////////////////////////////////

    virtual void EnableAScan(eUMUSide side, unsigned char line, unsigned char stroke, unsigned short st_delay, unsigned char zoom, unsigned char mode, unsigned char strob);
    virtual void PathSimulator(unsigned char number, bool state);
    virtual void PathSimulatorEx(unsigned char SimulatorNumber, unsigned short Interval_ms, bool state);                     // БУИ - Имитатор датчика пути (расширенный вариант)
    virtual void ScanerPathSimulator(bool state, unsigned short Interval_ms, unsigned char Param_A, unsigned char Param_B);  // БУИ - Имитатор датчика пути сканера
    virtual void SetStrokeParameters(const tStrokeParams* stroke_params);
    virtual void DisableAScan(bool Force);
    virtual int EnableBScan();
    virtual void DisableBScan(bool Force);
    virtual void EnableAlarm();
    virtual void DisableAlarm(bool Force);
    virtual void EnableACSumm();             // БУИ - Разрешить передачу сырых данных АК
    virtual void DisableACSumm(bool Force);  //   БУИ - Запретить передачу сырых данных АК
    virtual void EnableAC();                 // БУИ - Разрешить передачу данных АК
    virtual void Enable();
    virtual void Disable();
    virtual void Reboot();
    virtual void GetWorkTime();
    virtual void GetDeviceID();
    virtual void GetACTresholds();
    virtual void GetUAkkQuery();
    virtual void RequestTVGCurve();
    virtual void CalcTVGCurve();
    virtual void EnableCalcDisCoord();             // БУИ - включить передачу данных о срабатывании основного датчика пути в формате п. 4.31***** вместо формата п. 4.13.
    virtual void DisableCalcDisCoord(bool Force);  // БУИ - отключить передачу данных о срабатывании основного датчика пути в формате п. 4.31
    virtual void SetBScanFiltrationMode(bool State);  // включить/отключить - фильтрации В-развертки (отсечение сигналов за вторым стробом )
};

// BUMLog
#if defined(UMUDebug) || defined(UMUEventLog) || defined(UMUInDataLog) || defined(UMUOutDataLog)
#define UMULog
#endif

#endif
