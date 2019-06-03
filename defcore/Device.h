#ifndef DEVICE_H
#define DEVICE_H

#include "ChannelsIds.h"
#include "DeviceConfig.h"

#include "platforms.h"
#include "Device_definitions.h"

#include <map>

//#define DeviceLog        // Разрешает запись лога класса дефектоскопа
//#define DenisDebug

class cThreadClassList;
class cDataTransferLan;
class cDataTransferCanRaw;
class cIUMU;
class cEventManager;
class cDeviceConfig;
class cDeviceCalibration;
class cChannelsTable;
class cCriticalSection;
class cDataTransferUsbCanWin;
class cDataTransferUsbCanAnd;


class cDevice
{
    bool EndWorkFlag;
    bool MarkFlag;
    bool FCalibrationMode;  // Флаг указывающий что идет настройка на тип рельса или включается режим работы "Болтовой стык"
    int LastSysCoord;
    bool FirstPathEncoderSimState1;
    bool FirstPathEncoderSimState2;
    int FirstPathEncoderSimNumber;
    bool LastPathEncoderSimState;

    char SetPathEncoderData_last_path_encoder_index;
    bool SetPathEncoderData_last_Simulator;
    int SetPathEncoderData_last_new_value;
    bool SetPathEncoderData_last_UseAsMain;


    bool SetChannelParamsFlag;
    bool SetStrokeTableFlag;
    bool OnlyOneStrokeFlag;
    bool SetAScanFlag;
    bool ChangeMode;
    bool ChangeChannel;
    bool ChangeSide;
    bool ChangeGroupIndex;
    bool ChangeCalMode;
    bool isFirstCall;


#ifndef DenisDebug
    int CurChannelUMUIndex;
    int CurChannelUMUSide;
    int CurChannelUMUGenLine;
    int CurChannelUMUResLine;
    int CurChannelUMUStroke;
    int CurChannelUMUGen;
    int CurChannelUMURes;
    int CurChannelUMUAScanDuration;
    int CurChannelUMUStrokeDuration;
    std::vector<sDeviceUnitInfo> UnitsData;  // Информация о блоках прибора
    std::vector<cIUMU*> UMUList;
    std::vector<GenResList> UMUGenResList;
    tUMUtoDEVList UMUtoDEVList[MaxModeCount];  // Список связывающий DeviceSide, CID и СЛТ БУМа для каждого режима
    bool SkipSetStrokeTableByChannelSelect;
    FiltrationData _filtrationData[MaxSideCount][CHANNELS_TOTAL];
    std::vector<PtUMU_BScanData> UMUBScan2Data_ptrs[2];  // Данные В-развертки (для накопления с двух БУМ)
    int last_speedZone; // Последняя определенная зона скорости
    int speedZone;
    float last_speed; // Последняя определенная зона скорости
    tRailTypeTuningDataList RailTypeTuningDataList;  // Массив хранение данных для настройки на тип рельса
#endif

    std::vector<tStrobeMode> StrobeModeList_;
    BottomSignalSpacingData _bottomSignalSpacingData[MaxSideCount][spclevelTotal];
    tUmuParamsStorage fUMUParamsStorage;  // Сохранеие параметров БУМ

    cDataTransferLan* dt_lan;
    cDataTransferCanRaw* dt_padcan;

#ifdef WIN32
    cDataTransferUsbCanWin* dt_usbcan;
#endif
#ifdef ANDROID
    cDataTransferUsbCanAnd* dt_usbcanand;
#endif

    int _fTailCoord;
    eDeviceMode FNewMode;          // Новый режим
    CID FNewChannel;               // Новый канал
    eDeviceSide FNewSide;          // Новая сторона
    int FNewGroupIndex;            // Новая группа
    int FNewGateIdx;               // Новый номер строба
    eCalibrationMode FNewCalMode;  // Режим настройки канала

    eDeviceMode FCurMode;          // Текущий режим
    CID FCurChannel;               // Текущий канал
    eDeviceSide FCurSide;          // Текущая сторона
    int FCurrentGateIdx;           // Номер строба текущего канала = 1, 2 (ближняя / дальняя зона)
    int FCurGroupIndex;            // Текущая группа
    eCalibrationMode FCurCalMode;  // Режим настройки канала
    tDEV_AScanMax AScanMax;        // Маркер максимума для режима настройки
    tDEV_AScanMax CalAScanMax;     // Маркер максимума для выполнения настройки канала на Ку
    int DeviceThreadIndex;
    int AcousticContactState;  // Состояние АК: 0 - выкл; 1 - сырые данные; 2 - состояние АК
    int MarkWidth;

    tDEV_AScanMeasure LastAScanMeasure;     // Последнее пришедшее измерение
    unsigned long LastAScanMeasureGetTime;  // Время получения последнего измерения

    int CurrentBScanSessionID;         // Номер текущей сесии В-развертки
    int BScanSessionID[MaxModeCount];  // Номер сесии В-развертки для разных типов (ручной / сплошной) каналов

    //    int ActualSetStrokeIndex; // Номер такта установленный как такт ноль в режиме настройки

    std::vector<tSensAutoCalibrationDataItem> SensAutoCalibrationData;  // Масив значенией автонастройки Ку для теневого метода контроля

    unsigned long ShadowMethodChannelSensAutoCalibrationLastTime;  // Время последней автонастройки Ку теневых каналов

    // Канал связи (АКП-USB, U2C-USB, LAN или др.)
    // cDataTransferLan * dt;

    // Указатель на объект конфигурации прибора
    cDeviceConfig* cfg;

    // Указатель на объект с настройками прибора
    cDeviceCalibration* cal;

    // Таблица каналов
    cChannelsTable* tbl;

    cEventManager* UMU_Event;
    cEventManager* DEV_Event;

    cCriticalSection* cs;
    cCriticalSection* _externalCriticalSection;

    // Текущее состояние дефектоскопа
    eDeviceState State;

    cThreadClassList* thlist;

    // Время начала работы прибора, отсчитывается от вызова StartWork
    unsigned long StartTime;

    // Текущий режим работы каналов прибора (ПОИСК, НАСТРОЙКА и т.д.)

    // Текущий канал
    eChannelType CurChannelType;

    // Проверка связи с БУМами
    bool UMUOnLineStatus[16];
    int UMUOnLineStatusCount[16];
    bool FirstUMUOnLineTest;
    unsigned long LastOnLineTestTime;

    unsigned int UsedUMUIdx;  // Номер активного БУМа

    CallBackProcPtr CallBackFunction;  // Callback функция

    eDeviceControlMode FCurrentControlMode;
    eDeviceControlMode FNewCurrentControlMode;
    std::vector<int> ControlModeStack[3];
    std::vector<sModeChannelData> ControlModeRestore[3];

    tCalibrationToRailTypeResult CalibrationToRailTypeResult;
    bool RailTypeTrackingMode; // Режим отслеживания (высоты) типа рельса [ВКЛ/ВЫКЛ], для зон стрелочных переводов
    bool RailTypeTrackingModeChanging;
    unsigned long RailTypeTrackingModeTime;

    bool _enableSMChSensAutoCalibration;  // Состояние автонастройки Ку теневых каналов

    signed char _fPrevDir;           // Предыдущее направление +1 в сторону увеличения системной координаты -1 в сторону уменьшения
    signed char _fCurrentDir;        // Текущее направление +1 в сторону увеличения системной координаты -1 в сторону уменьшения
    bool _fFirstCoord;               // Флаг ожидания первой координаты
    bool _enableFiltration;          // Разрешение фильтрации
    int _fPrevSysCoord;              // Предыдущая координата
    int _sysCoord;                   // Текущая системная координата
    int _displayCoord;               // Текущая дисплейная координата
    int _fTailDisplayCoord;          // Дисплейная координата выгружаемая из фильтра
    signed char _currentDeltaCoord;  // Текущее смещение координаты

    bool _pathEncoderResetComandSent;  // Флаг события отправки команды сброса системной координаты
                                       // для своевременного сброса дисплейной координаты.
                                       // Когда от БУМ придёт первая нулевая системная координата.


    int fltrPrmMinPacketSize;
    int fltrPrmMaxCoordSpacing;
    int fltrPrmMaxDelayDelta;
    unsigned int fltrPrmMaxSameDelayConsecutive;
    unsigned int fltrPrmMinPacketSizeForEvent;
    unsigned int fltrPrmMinSignals0dBInNormModeForEvent;
    unsigned int fltrPrmMinSignalsMinus6dBInBoltTestModeForEvent;
    unsigned int fltrPrmMinSpacing0dBInNormModeForEvent;
    unsigned int fltrPrmMinSpacingMinus6dBInBoltTestModeForEvent;

    unsigned long _alarmSendTick;         // Таймер отправки АСД
    unsigned long _bottomSignalSendTick;  // Таймер отправки амплитуды донного сигнала

    std::deque<int> _bottomSignalAmplFilter;

    bool FixBadSensState;
    std::vector<tKuValidRanges> ValidRangesArr;  // Массив слежения за Ку
    int TimeToFixBadSens;                        // Время до возврата Ку к рекомендованному
    int ValidRangesTestPeriod_ms;                // Частота проверки, мс
    unsigned long ValidRangesTestTime_ms;

    bool PathEncoderGisState;  // Гистограмма прирощений ДП - вкл / выкл
    int PathEncoderGis[11];    // Гистограмма прирощений ДП - для отладки

    bool SensBySpeedState;
    float SensBySpeed_pathStep;
    int SensBySpeed_summDelta; // Текущее отклонение от начального Ку

    void ManagePathEncoder_();

public:
#ifdef DenisDebug
    int CurChannelUMUIndex;
    int CurChannelUMUSide;
    int CurChannelUMUGenLine;
    int CurChannelUMUResLine;
    int CurChannelUMUStroke;
    int CurChannelUMUGen;
    int CurChannelUMURes;
    int CurChannelUMUAScanDuration;
    int CurChannelUMUStrokeDuration;
    std::vector<sDeviceUnitInfo> UnitsData;  // Информация о блоках прибора
    std::vector<cIUMU*> UMUList;
    std::vector<GenResList> UMUGenResList;
    tUMUtoDEVList UMUtoDEVList[MaxModeCount];  // Список связывающий DeviceSide, CID и СЛТ БУМа для каждого режима
    bool SkipSetStrokeTableByChannelSelect;
    FiltrationData _filtrationData[MaxSideCount][CHANNELS_TOTAL];
    std::vector<PtUMU_BScanData> UMUBScan2Data_ptrs[2];  // Данные В-развертки (накопление с двух БУМ и последующее соединение)
    int last_speedZone; // Последняя определенная зона скорости
    int speedZone;
    float last_speed; // Последняя определенная зона скорости
    tRailTypeTuningDataList RailTypeTuningDataList;  // Массив хранение данных для настройки на тип рельса
#endif

    tPathEncoderDebug PathEncoderDebug[2][3];

    cDevice(cThreadClassList* ThreadClassList, cDeviceConfig* Config, cDeviceCalibration* Calibration, cChannelsTable* Table, cEventManager* Device_EM, cEventManager* UMU_EM, cCriticalSection* CritSec, cCriticalSection* ExtCritSec);  // Конструктор

    ~cDevice();  // Деструктор

    ////////////////////////////////////////////////////////

    sChannelDescription WorkChannelDescription;

    void SetBScanGateLevel(int BScanGateLevel_);                                         // Установка уровеня строба В-развертки [отсчетов]
    void SetMode(eDeviceMode Mode);                                                      // Установить режим работы дефектоскопа
    void SetCalibrationMode(eCalibrationMode Mode);                                      // Установить режим настройки канала - Ку или 2Тп
    bool SetChannel(CID id);                                                             // Выбор канала
    bool SetSide(eDeviceSide Side);                                                      // Выбор стороны
    void SetChannelGroup(int GroupIndex);                                                // Установить рабочую групу каналов
    bool SetFaceData(tTrolleySide Side);                                                 // Установка соответствия рабочих/нерабочих граней
    void SetControlMode(eDeviceControlMode Mode, void* Params);                          // Установить режим контроля
    eDeviceControlMode GetControlMode() const;                                           // Получить режим контроля
    void SetAlarmMode(eDeviceSide Side, CID Channel, int GateIdx, eGateAlarmMode Mode);  // Установить режим работы АСД
    eGateAlarmMode GetAlarmMode(eDeviceSide Side, CID Channel, int GateIdx);             // Получить режим работы АСД - GateIdx строб: 1 (ближняя зона), 2 (дальняя зона)
    eBScanDataType GetBScanDataType() const;

    int GetChannelGroup() const;  //

    bool ChannelInCurrentGroup(CID Channel);                    // Проверка на вхождение канала в текущую группу
    void Update(bool ResetStrokes, bool SetAScanFlag = false);  // Установка изменений параметров

    bool ChangeDeviceConfig(cDeviceConfig* Config);  // Смена конфигурации
    void setUmuToCompleteControl(cDeviceConfig* Config, UMULineSwitching state);

    int GetBScanEchoCount(eChannelType ChType, PtUMU_BScanData DataPtr, eDeviceSide Side, CID id);
    PtUMU_BScanSignal GetBScanEchoData(eChannelType ChType, PtUMU_BScanData DataPtr, eDeviceSide Side, CID id, int Index);
    bool GetAlarmData(eChannelType ChType, PtUMU_AlarmItem DataPtr, eDeviceSide Side, CID id, int GateIdx, bool* State);

    CID GetChannel() const;
    eChannelType GetChannelType() const;
    eDeviceSide GetSide() const;
    eDeviceMode GetMode() const;                  // Режим работы дефектоскопа
    eCalibrationMode GetCalibrationMode() const;  // Режим настройки канала - Ку или 2Тп

    //	eDeviceState GetState(); // Состояние дефектоскопа
    eDeviceMode GetDeviceMode() const;  // Получить режим работы каналов дефектоскопа
    unsigned long GetTime() const;      // Время с начала работы класса в мкс
    int GetModeIndex() const;           // Порядковый номер установленного режима

    pCalibrationToRailTypeResult CalibrationToRailType();   // Настройка на тип рельса
    void SetRailTypeTrackingMode(bool State);               // Режим отслеживания (высоты) типа рельса, для зон стрелочных переводов

    bool PrismDelayAutoCalibration(float* DeltaPrismDelay = NULL);  // Автоматическая настройка времени в призме

    int GetGateCount() const;      // Количество стробов
    int GetGateIndex() const;      // Получить строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetGateIndex(int Index);  // Установить строб: 1 (ближняя зона), 2 (дальняя зона)

    int GetTVG() const;           // Получить значение ВРЧ [мкс]
    bool SetTVG(int New);         // Установить значение ВРЧ [мкс]
    int GetPrismDelay() const;    // Получить значение 2TP [10 * мкс]
    bool SetPrismDelay(int New);  // Установить значение 2TP [10 * мкс]

    int GetSens() const;                   // Получить значение Ky [дБ]
    bool SetSens(int New);                 // Установить значение Ky [дБ]
    int GetSens(int GateIndex) const;      // Получить значение Ky [дБ], строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetSens(int GateIndex, int New);  // Установить значение Ky [дБ] , строб: 1 (ближняя зона), 2 (дальняя зона)

    int GetRecommendedSens() const;               // Получить рекомендованное значение Ky [дБ]
    int GetRecommendedSens(int GateIndex) const;  // Получить рекомендованное значение Ky [дБ]

    int GetGain();                         // Получить значение Aтт [дБ]
    bool SetGain(int New);                 // Установить значение Aтт [дБ]
    int GetGain(int GateIndex);            // Получить значение Aтт [дБ], строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetGain(int GateIndex, int New);  // Установить значение Aтт [дБ], строб: 1 (ближняя зона), 2 (дальняя зона)

    GateType GetStGate(/*bool forBScan = false*/);                              // Получить значение начала строба [мкс]
    bool SetStGate(GateType New, bool SkipTestGateLen = false);                 // Установить значение начала строба [мкс]
    GateType GetStGate(int GateIndex /*, bool forBScan = false*/);              // Получить значение начала строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetStGate(int GateIndex, GateType New, bool SkipTestGateLen = false);  // Установить значение начала строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)

    GateType GetEdGate(/*bool forBScan = false*/);                              // Получить значение конца строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetEdGate(GateType New, bool SkipTestGateLen = false);                 // Установить значение конца строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)
    GateType GetEdGate(int GateIndex /*, bool forBScan = false*/);              // Получить значение конца строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)
    bool SetEdGate(int GateIndex, GateType New, bool SkipTestGateLen = false);  // Установить значение конца строба [мкс], строб: 1 (ближняя зона), 2 (дальняя зона)

    int GetCalibrationTemperature();             // получить температуру (град.Цельсия) в момент настройки канала и строба, являющихся текущими
    bool SetCalibrationTemperature(int NewVal);  // Записать температуру (град.Цельсия) в момент настройки

    bool GetSnapShotFileID(sSnapshotFileID* pFileID);              // получить ID-файла со скриншотом экрана в момент настройки
    bool SetSnapShotFileID(int GateIdx, sSnapshotFileID pFileID);  // записать ID-файла со скриншотом экрана в момент настройки канала: 1 (ближняя зона), 2 (дальняя зона)

    void StartUseMark();  // Включение метки
    void StopUseMark();   // Выключение метки
    bool GetMarkState();
    int GetMark();               // Получить значение метки [мкс]
    bool SetMark(int New);       // Установить значение метки [мкс]
    int GetMarkWidth();          // Получить ширину метки [мкс]
    void SetMarkWidth(int New);  // Установить ширину метки [мкс]
    int GetStMark();             // Получить начало метки [мкс]
    int GetEdMark();             // Получить конец метки [мкс]

    bool GetNotchState() const;      // Есть ли в текущем канале полка
    int GetStNotch();                // Получить начало полки [мкс]
    int GetNotchLen();               // Получить длительность полки [мкс]
    int GetNotchLevel();             // Получить уровень ослабления в пределах полки [дБ]
    bool SetStNotch(int NewVal);     // Установить начало полки [мкс]
    bool SetNotchLen(int NewVal);    // Установить длительность полки [мкс]
    void SetNotchLevel(int NewVal);  // Установить уровень ослабления в пределах полки [дБ]

    int GetStrokeLen() const;                             // Длительность такта [мкс]
    int GetAScanLen() const;                              // Длительность развертки [мкс]
    int GetChannelBScanDelayMultiply(CID Channel) const;  // Получить множитель задержек В развертки канала

    int GetScanZeroProbeAmpl() const;                       // Получить амплитуду донного сигнала канала сплошного контроля
    int GetCurrentBScanSessionID() const;                   // Получить текущую сессию В-развертки
    int GetBScanSessionID(eChannelType ChannelType) const;  // Получить сессию В-развертки для заданного типа канала

    bool Tick();                  // Метод жизнедеятельности класса (вызывать в цикле)
                                  //	void Stop();
    void DisableAll();            // Выключить БУМы
    bool TestDisableAll();        // Проверка были ли выключены БУМы
    void EnableAll();             // Включить БУМы
    void ResetPathEncoder();      // Сбросить ДП на 0
    bool TestResetPathEncoder();  // Проверка выполнения сброса ДП на 0

    //    bool SetAcousticContactThreshold(unsigned char PercentageIncrease_); // Команда - Выполнить установку порога АК
    bool SetAcousticContactThreshold_(unsigned char PercentageIncrease_, unsigned char ThRecalcCount);  // Команда - Выполнить установку порога АК
    void SetACControlATT(int ATT);                                                                      // Установка значения АТТ для зоны контроля АК

    void InitTuningGateList();                // Инициализация стробов для настройки каналов
    bool ChannelSensCalibration();            // Настройка чувствительности выбранного канала по пороговому значению усиления результат: true - настроен; false - ошибка, канал не настроен
    bool ChannelSensCalibration_ResetSens();  // Настройка чувствительности - сброс чуствительности канала
    bool ChannelSensCalibration_Type2();      // Настройка чувствительности выбранного канала, тип 2

    void SetFixBadSensState(bool State);              // Управление функцией возвращения Ку к рекрмендованному
    void ResetValidRangesArr();                       // Сброс массива слежения за Ку
    void SetTimeToFixBadSens(int TimeToFixBadSens_);  // Время до возврата Ку к рекомендованному

    //    bool TestSensValidRanges(eDeviceSide Side, CID Channel, int GateIndex); // Проверка значения условной чувствительности на отклонени от нормативного значения
    int TestSensValidRanges(eDeviceSide Side, CID Channel, int GateIndex);  // Проверка значения условной чувствительности на отклонени от нормативного значения: 0 - соответствует; != 0 - отклонение, не соответствует

    tDEV_AScanMax GetAScanMax();  // Получение максимума сигнала A-развертки (маркер для режима настройки)

    float getDelayToDepthFactor() const;
    float getPixelToDepthFactor() const;
    float DelayToDepth(float Delay) const;  // Глубина по лучу
    float DepthToDelay(float Depth) const;
    float DelayToDepthByHeight(float Delay);  // Глубина по высоте рельса
    int DelayToPixel(float Delay);
    float getPixelToDelayFactor() const;
    float getDelayToPixelFactor() const;
    int PixelToDelay(int Pixel);
    float PixelToDelayFloat(int Pixel) const;


    void SetChannelParams(eDeviceSide Side, CID Channel, bool Gain_Flag, bool PrismDelay_Flag, bool Gate_Flag, bool BScanGate_Flag);
    void PathEncoderSim(bool State);
    void PathEncoderSim(int Number, bool State);
    void PathEncoderSimEx(unsigned char Number, unsigned short Interval_ms, bool state);                              // БУИ - Имитатор датчика пути (расширенный вариант)
    void PathEncoderSimScaner(bool state, unsigned short Interval_ms, unsigned char Param_A, unsigned char Param_B);  // БУИ - Имитатор датчика пути сканера
    void SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain);  // Установка значения датчика пути. Если UseAsMain установлен, полная координата для датчика (имитатора) не изменяется
    void SetAcousticContact(int Mode, bool State);                                                    // Вкл / выкл контроля акустического контакта
    void SetLeftSideSwitching(UMULineSwitching state);                                                // Переключение линий с сплошного контроля на сканер
    unsigned int GetUMUCount() const;
    bool GetUMUOnLineStatus(unsigned int Idx) const;
    void GetUAkkQuery();
    int CheckSetBadSens(eDeviceSide Side, CID Channel, int GateIdx);  // Для функция возвращения Ку к рекомендованному через TimeToFixBadSens сек
    int GetUMUSkipMessageCount(unsigned int Idx);
    int GetUMUErrorMessageCount(unsigned int Idx);
    unsigned int GetUMUDownloadSpeed(unsigned int Idx);
    unsigned int GetUMUUploadSpeed(unsigned int Idx);

    int connectToDevice();

    void StartWork();  // Начало работы
    void EndWork();    // Конец работы

    void RequestTVGCurve();
    unsigned int GetUsedUMUIdx() const;


    void ResetPathEncoderGis();
    void SetPathEncoderGisState(bool State);
    void SetCallBack(CallBackProcPtr function);  // Устанавливает callback функцию

    bool ManagePathEncoderSimulation;                // Переменна позволяет Device-у включать / выключать имитатор ДП для работы в ручных каналах
    bool UsePathEncoderSimulationinSearchMode;       // Переменна указывае Device-у включать имитатор ДП в режиме поиск
    bool SkipBScanDataInCalibrationMode;             // Переменна для включения игнарирования В-разветки в режиме настройка

    void EnableFiltration(bool isEnable);  // Фильтрация данных В-развертки

    void setFilterParams(tBScan2FilterParamId prmId, unsigned int value);  // Параметры фильтра B развёртки

    void EnableSMChSensAutoCalibration(bool isEnable);  // Автонастройка Ку теневых каналов
    void MakeTestSensCallBackForAllChannel();           // Формирование TestSens CallBack -ов для всех каналов контроля

    bool GetUAkkByVolt(double* pVoltageByVolt);           // получение напряжения аккумулятора
                                                          // в Вольтах  	из UMUList[0] c протоколами
                                                          // eProtUSBCAN и 	eProtUSBCANAND
    bool GetUAkkByPercent(unsigned int* pVoltageByVolt);  // получение напряжения аккумулятора
                                                          // в процентах от заряда

    bool IsCopySlaveChannel(eDeviceSide Side, CID Channel);
    int getSerialNumberForUMU(unsigned int UMUIdx);
    std::string getFirmwareVersionForUMU(unsigned int UMUIdx);
    int getBuildNumberForUMU(unsigned int UMUIdx);
    std::string getFPGAVersionForUMU(unsigned int UMUIdx);

    void setUsePathEncoderSimulationinSearchMode(bool value);
    void ManageSensBySpeed(bool State, float pathStep_mm);

protected:
    void CleanupUMUEventMgr();
    void CallBackFunction_(int GroupIndex, eDeviceSide Side, CID Channel, int GateIndex, eValueID ID, int Value);

private:
    // Internal functions
    void prepareRailTypeTuningList();

    void filterResetPacketProccess();
    void filterInit();
    bool filterUploadSignals(int coord, signed char dir, int tailCoord);  // Функция выгружающая очередную координату из фильтра в случае обнаружения каких либо ошибок возвращает false
    bool filterCheck53Defect(sMaskItem& mask, std::vector<sMaskItem>::iterator beginIt, std::vector<sMaskItem>::iterator endIt);  // Функция проверки маски на дефект 53.1
    void filterUseNewMask(sMaskItem& currentNewMask, const tUMU_BScanSignal& newSignal, signed char direction);
    void filterCreateAlarmData(tBScan2Items::const_iterator beginIt, tBScan2Items::const_iterator endIt);
    bool filterGetUploadCondition() const;

    void sendBottomSignalAmpl();

    void checkPereskok(PtUMU_BScanData data);

    void mergeChannelsTwoACValues(tDEV_BScan2Head* DEV_BScan2Head, std::map<CID, tDEV_ACHeadItem>& mapForSide, CID cid1, CID cid2) const;
    void mergeChannelsFourACValues(tDEV_BScan2Head* DEV_BScan2Head, std::map<CID, tDEV_ACHeadItem>& mapForSide, CID cid1, CID cid2, CID cid3, CID cid4) const;
    void filterAddNewSignal(const tDEV_BScan2Head* data);
    void tickAScanData();
    void tickAScanMeas();
    void tickTVGData();
    void tickAlarmData();
    void tickBScanData(unsigned long EventId);
    void tickBScanData_2UMU(unsigned long EventId);
    void merge2UMUData(unsigned long EventId);
    void uploadBscanPacket(std::vector<PtUMU_BScanData>::iterator it, unsigned long eventId);
    void tickPathStepData();
    void tickA15ScanerPathStepData();
    void tickMetalSensor();
    void tickDeviceSpeed();
    void tickCheckUMUConnection();
    void tickFixBadSensState();
    void convertBScanFormat(const PtUMU_BScanData BScan2Data_ptr, tDEV_BScan2Head* DEV_BScan2Head) const;
    void collectRailTuningData(tDEV_BScan2Head* DEV_BScan2Head);
    void RailTypeTracking();
    void shadowMethodSensAutoCalibration(tDEV_BScan2Head* DEV_BScan2Head);
    void updateShadowMethodAutoGain();
    void postBScan2Data(unsigned long EventId, tDEV_BScan2Head* DEV_BScan2Head);
    void filterPutSignalsInMask(sMaskItem& currentMask, const tUMU_BScanSignal& currentSignal, signed char direction);
    bool filterIsDelayAcceptable(const sMaskItem& currentMask, CID channel, eChannelDir channelDirType, int absDeltaCoord, unsigned int delay) const;
    void filterSendSpacing(tDevSignalSpacingFlag& currentSpacingFlag, int tailCoord, const tUMUtoDEVitem& curItem, const tUMU_BScanSignal& signalItem, int displCoord, int spacingLength, unsigned char spLevel);
    void filterSendSignalsPacket(const sMaskItem& cur, const tUMUtoDEVitem& curItem, int length);
    void filterSendDefect53_1(const sMaskItem& cur, const tUMUtoDEVitem& curItem);
    void updateChannelsInCurrentGroup();
    bool filterTryInsertSignal(CID channel, eChannelDir channelDirType, signed char direction, sMaskItem& currentMask, const tUMU_BScanSignal& currentSignal);

    int debugFilterGetDataSize(const FiltrationData& data);
    int debugGetTotalFilterSize();
    void filterAnalyzeBottomSignal(const tUMU_BScanSignal& signalItem, const tUMUtoDEVitem& curItem, const GateType bottomStartStrobe, int tailCoord, int displCoord);
    void filterInitMasksList(FiltrationData& currentFiltrationData);

    void changeChannelCalibrationMode();
    void changeGroupIndex();
    void changeDeviceMode();
    void changeChannelType();
    void changeChannel();
    void setDeviceForCalibrationMode();
    void setDeviceForEvaluationOrHandMode();
    void setDeviceForSearchModeContiniousControl();
    void setDeviceForSearchModeManualControl();
    void setSensDeltaForAllChannels(int delta);
};
#endif /* DEVICE_H */
