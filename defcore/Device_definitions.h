#ifndef DEVICE_DEFINITIONS_H
#define DEVICE_DEFINITIONS_H

#include "Definitions.h"
#include "prot_umu/prot_umu_lan_constants.h"
#include <deque>
#include <numeric>

static const unsigned char MASKS_MAX_COUNT = 64;

#define Delete_SignalsM6dB

//---------------------------------------------------------------------------
// --- Датчики пути ---
//---------------------------------------------------------------------------

#pragma pack(push, 1)

struct tDEV_PathStepData  // Данные от датчиков пути
{
    bool Simulator[MaxPathEncoders];   // Тип - датчика пути / имитатор
    char PathEncodersIndex;            // Индекс датчика пути по которому было сформированно данное событие
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    int XDisCrd[MaxPathEncoders];      // Дисплейная координата
    unsigned char UMUIdx;              // Номер БУМ
};

typedef tDEV_PathStepData* PtDEV_PathStepData;

#pragma pack(pop)

//---------------------------------------------------------------------------
// --- Датчики пути сканера Авикон-15 ---
//---------------------------------------------------------------------------

#pragma pack(push, 1)

struct tDEV_A15ScanerPathStepData
{
    unsigned short XCrd;
    char YCrd;
    char Angle;
//    unsigned char UMUIdx;              // Номер БУМ
};

typedef tDEV_A15ScanerPathStepData* PtDEV_A15ScanerPathStepData;

#pragma pack(pop)


//---------------------------------------------------------------------------
// --- Датчик металла ---
//---------------------------------------------------------------------------

typedef struct  // Данные от датчиков металла -  болтового стыка и стрелочного перевода
{
    unsigned char UMUIdx;    // номер БУМ
    eDeviceSide Side;        // Сторона дефекстоскопа
    unsigned char SensorId;  // Идентификатор датчика: 0 – Датчик болтового стыка; 1 - датчик стрелочного перевода
    unsigned char State;     // Событие: 2 -  переход из 0 в 1 (срабатывание); 1 - переход из 1 в 0

} tDEV_MetalSensorData;

typedef tDEV_MetalSensorData* PtDEV_MetalSensorData;


//---------------------------------------------------------------------------
// --- А - развертка ---
//---------------------------------------------------------------------------

// Данные измерений A-развертки

struct tDEV_AScanMeasure
{
    eDeviceSide Side;  // Сторона дефекстоскопа
    CID Channel;       // Канал
    float ParamM;      // Положение максимального сигнала, в стробе АСД. [мкс]
    int ParamA;        // Амплитуда максимального сигнала, в стробе АСД. [отсчетов]
    int ParamH;        // Глубина залегания отражателя [H, мм]
    int ParamR;        // Расстояние до отражателя, по лучу [R, мм]
    int ParamL;        // Расстояние от точки выхода луча до отражателя [L, мм]
    int ParamN;        // Превышение сигналом порогового уровня [N, дБ]
    int ParamKd;       // Коэффициент выявляемости [Кд, дБ]
};

typedef tDEV_AScanMeasure* PtDEV_AScanMeasure;

// Максимум сигнала A-развертки (маркер для режима настройки)

typedef struct
{
    float Delay;  // Положение максимального сигнала, в стробе АСД. [мкс]
    int Ampl;     // Амплитуда максимального сигнала, в стробе АСД. [отсчетов]

} tDEV_AScanMax;

// Данные А-развертки и ВРЧ

struct tDEV_AScanHead  // Данные A-развертки
{
    eDeviceSide Side;  // Сторона дефекстоскопа
    CID Channel;       // Канал
};

typedef tDEV_AScanHead* PtDEV_AScanHead;

// Скорость из БУМ

typedef unsigned int tDev_DeviceSpeedInStepsPerSecond;

// Амплитуда донного сигнала

typedef int tDEV_BottomSignalAmpl;

typedef tDEV_BottomSignalAmpl* PtDEV_BottomSignalAmpl;

//---------------------------------------------------------------------------
// --- В - Развертка (ТИП 1) ---
//---------------------------------------------------------------------------

struct tDEV_BScanHeadItem  // Событие: Данные В-развертки, тип 1
{
    eDeviceSide Side;  // Сторона дефектоскопа
    CID Channel;
    // Канал
    unsigned char Count;  // Количество сигналов
    tUMU_OldBScanSignalList BScanDataPtr;
};

typedef std::vector<tDEV_BScanHeadItem> tBScanItems;

struct tDEV_BScanHead  // Событие: Данные В-развертки, тип 1
{
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    int XDisCrd[MaxPathEncoders];      // Дисплейная координата
    tBScanItems Items;                 // Сигналы
};

typedef tDEV_BScanHead* PtDEV_BScanHead;


//---------------------------------------------------------------------------
// --- В - Развертка (ТИП 2) ---
//---------------------------------------------------------------------------

struct tDEV_BScan2HeadItem  // Событие: Данные В-развертки, тип 2
{
    eDeviceSide Side;  // Сторона дефектоскопа
    CID Channel;       // Канал
    std::vector<tUMU_BScanSignal> Signals;
};

typedef std::vector<tDEV_BScan2HeadItem> tBScan2Items;

struct tDEV_ACHeadItem  // Событие: АК
{
    eDeviceSide Side;  // Сторона дефектоскопа
    CID Channel;       // Канал
    bool ACValue;
    unsigned int ACSumm;
    unsigned int ACTh;
};

typedef std::vector<tDEV_ACHeadItem> tACHeadItems;

struct tDEV_BScan2Head  // Событие: Данные В-развертки, тип 2
{
    //    eDeviceConfigID DeviceID;                 // Идентификатор конфигураци
    eChannelType ChannelType;          // Тип данных: ручного контроля или сплошного контроля
    int BScanSessionID;                // Номер текущей сесии В-развертки
    char PathEncodersIndex;            // Индекс датчика пути по которому было сформированно данное событие
    bool Simulator[MaxPathEncoders];   // Тип датчика пути: False - датчик пути / True - имитатор
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    int XDisCrd[MaxPathEncoders];      // Дисплейная координата
    tBScan2Items Items;                // Сигналы
    tACHeadItems ACItems;              // AC
};

typedef tDEV_BScan2Head* PtDEV_BScan2Head;

//---------------------------------------------------------------------------
// --- АСД ---
//---------------------------------------------------------------------------

typedef struct  //
{
    eDeviceSide Side;  // Сторона дефектоскопа
    CID Channel;       // Канал
    bool State[4];     // Номер строба: 0 - не используется, 1 - ближняя зона, 2 - дальняя зона, 3 - не используется
} tDEV_AlarmHeadItem;

struct tDEV_AlarmHead
{
    std::vector<tDEV_AlarmHeadItem> Items;  // Сигналы АСД
};

typedef tDEV_AlarmHead* PtDEV_AlarmHead;

//////////////////////////////////////////////////////////////

// typedef int TDataChannelIdList[3];

//////////////////////////////////////////////////////////////

typedef struct
{
    unsigned long Time;    // Время получение данных
    eDeviceSide Side;      // Сторона дефектоскопа
    CID Channel;           // Канал контроля (0 град) - для которого определена высота рельса
    int DelayHeight;       // Высота рельса (мкс) -1 нет данных для выполнения измерения
    unsigned char MaxAmp;  // Амплитуда ДС
    int SignalIdx;         // Номер сигнала ДС
} sRailTypeTuningData;
// Данные для выполнения настройки на тип рельса
typedef std::vector<sRailTypeTuningData> tRailTypeTuningDataList;


//////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------
// Фильтр сигналов

typedef struct
{
    // unsigned char UMUIdx;      // Номер БУМ
    signed char Dir;          // Сдвиг датчика пути
    tUMU_BScanSignal Signal;  // Сигнал
    // eChannelType ChannelType;  // Тип данных: ручного контроля или сплошного контроля
    int SysCoord;  // Системная координата
    int DisCoord;  // Диспейная координата
    // long BScanSession;         // Сессия данных В-развертки
} sMaskSignalItem;

struct sMaskItem
{
    bool Busy;            // Ячейка занята
    bool OK;              // Идет прямое сохранение сигналов
    bool Aprox;           // Маска в режиме апроксимации для поиска дефекта 53.1
    bool Defect53_1Flag;  // Отметка о предполагаемом дефекте 53.1

    unsigned char Upper53SignalCount;  // Количество сигналов подпадающих под параметры дефекта 53.1 сверху.
    unsigned char Lower53SignalCount;  // Количество сигналов подпадающих под параметры дефекта 53.1 снизу.
    unsigned char StartDelay;
    unsigned char LastDelay;  // Задержка последнего добавленного сигнала


    signed char MaxAmpl;
    signed char SameDelay;  // Оставшееся (разрешенное) количество идущих подряд сигналов с одинаковой задержкой

    unsigned int Count_0dB;
    unsigned int Count_6dB;
    int StartSysCoord;      // Начальная координата
    int LastSysCoord;       // Координата последнего добавленного сигнала
    int StartDisplayCoord;  // Начальная начальна координата
    int LastDisplayCoord;   // Координата последнего добавленного сигнала

    std::vector<sMaskSignalItem> Signals_;  // Все сигналы
#ifndef Delete_SignalsM6dB
    std::deque<sMaskSignalItem> SignalsM6dB;  // Сигналы -6 dB
#endif
    sMaskItem()
        : Busy(false)
        , OK(false)
        , Aprox(false)
        , Defect53_1Flag(false)
        , Upper53SignalCount(0)
        , Lower53SignalCount(0)
        , StartDelay(0)
        , LastDelay(0)
        , MaxAmpl(0)
        , SameDelay(0)
        , Count_0dB(0)
        , Count_6dB(0)
        , StartSysCoord(0)
        , LastSysCoord(0)
        , StartDisplayCoord(0)
        , LastDisplayCoord(0)
    {
    }
    ~sMaskItem()
    {
        Signals_.clear();
#ifndef Delete_SignalsM6dB
        SignalsM6dB.clear();
#endif
    }
    void reset()
    {
        Busy = false;
        OK = false;
        Aprox = false;
        Defect53_1Flag = false;
        Upper53SignalCount = 0;
        Lower53SignalCount = 0;
        StartDelay = 0;
        LastDelay = 0;
        MaxAmpl = 0;
        SameDelay = 0;
        Count_0dB = 0;
        Count_6dB = 0;
        StartSysCoord = 0;
        LastSysCoord = 0;
        StartDisplayCoord = 0;
        LastDisplayCoord = 0;
        Signals_.clear();
#ifndef Delete_SignalsM6dB
        SignalsM6dB.clear();
#endif
    }
};

inline bool isMaskBusy(const sMaskItem& item)
{
    return item.Busy;
}

struct MaskTailFunctor
{
    explicit MaskTailFunctor(int tailCoord)
        : _tailCoord(tailCoord)
    {
    }
    inline bool operator()(const sMaskSignalItem& item) const
    {
        return item.SysCoord != _tailCoord;
    }

private:
    int _tailCoord;
};

inline int debugFilterGetMaskSize(int prev, const sMaskItem& mask)
{
    int maskSize = sizeof(sMaskItem);
    maskSize += (mask.Signals_.size()
#ifndef Delete_SignalsM6dB
                 + mask.SignalsM6dB.size()
#endif
                     )
                * sizeof(sMaskSignalItem);
    return prev + maskSize;
}

typedef struct
{
    bool Flag;
    unsigned char StartDelay;
    unsigned int Echo0Count;  //для определения разрывов донного при условии сигналов в канале 0 эхо
    int StartCoord;
    int StartDisplayCoord;
} tDevSignalSpacingFlag;

typedef enum
{
    spclevelMinus6 = 0,
    spclevelZero = 1,
    spclevelTotal = 2
} tSpacingLevel;

typedef struct
{
    unsigned char UMUIdx;    // Номер БУМ
    signed char Dir;         // Сдвиг датчика пути
    char PathEncodersIndex;  // Индекс датчика пути по которому было сформированно данное событие
    bool Simulator;
    int XSysCrd;               // Системная координата
    int XDisCrd;               // Дисплейная координата
    eChannelType ChannelType;  // Тип данных: ручного контроля или сплошного контроля
    int BScanSession;          // Сессия В-развертки
    tACHeadItems ACHeadItems;
} sUMUParams;

typedef std::deque<sUMUParams> tUmuParamsStorage;

typedef struct
{
    eDeviceSide Side;
    CID Channel;
    int NewGain;

} tSensAutoCalibrationDataItem;

typedef struct
{
    sAlarmInfo Modes[3][3];  // Тип для сохранения установленных режимов работы АСД одного CID - размерность [нить][строб]

} tStrobeMode;

#if defined(DEFCORE_CC_BOR)
typedef void(__closure* CallBackProcPtr)(int GroupIndex, eDeviceSide Side, CID Channel, int GateIndex, eValueID ID, int Value);
// GroupIndex и GateIndex могут принимать значение = - 1, это значит что значение отсутствует
#else
typedef void (*CallBackProcPtr)(int GroupIndex, eDeviceSide Side, CID Channel, int GateIndex, eValueID ID, int Value);
// GroupIndex и GateIndex могут принимать значение = - 1, это значит что значение отсутствует
#endif

typedef struct
{
    unsigned long Time;
    bool BadState;  // Не верное Ку
    int GoodSens;
    eDeviceSide Side;
    CID Channel;
    int GateIdx;

} tKuValidRanges;

struct GenResList
{
    unsigned char List[MaxSideCount][MaxLineCount][MaxStrokeCount];
    unsigned char StrokeCount;
    GenResList()
    {
        memset(List, 0x00, sizeof(List));
        StrokeCount = 0;
    }
};

struct FiltrationData
{
    std::vector<sMaskItem> masks;                // Масив масок поиска, разбит по правилу - занятые маски в начале
    std::vector<sMaskItem>::iterator partition;  // Итератор указывающий на первый не занятый элемент
    int debugGetSize()
    {
        int masksSize = std::accumulate(masks.begin(), masks.end(), 0, &debugFilterGetMaskSize);
        return sizeof(FiltrationData) + masksSize;
    }
};

struct BottomSignalSpacingData
{
    unsigned char lastBottomSignalDelay;
    bool noBottomSinal;
    tDevSignalSpacingFlag fSignalSpacingFlag;  // Флаг события начала разрыва донного сигнала
    void reset()
    {
        lastBottomSignalDelay = 180;
        noBottomSinal = true;
        fSignalSpacingFlag.StartCoord = 0;
        fSignalSpacingFlag.StartDisplayCoord = 0;
        fSignalSpacingFlag.Flag = false;
        fSignalSpacingFlag.StartDelay = 0;
        fSignalSpacingFlag.Echo0Count = 0;
    }
};

typedef struct
{
    bool Simulator;
    int XSysCrd;
    int XDisCrd;
} tPathEncoderDebug;


#endif  // DEVICE_DEFINITIONS_H
