#ifndef DEVICECALIBRATIONDEFINITIONS_H
#define DEVICECALIBRATIONDEFINITIONS_H

#include <cstdlib>
#include <cstdint>

#define TBD
#define currentFileFormatVersion 6

#pragma pack(push, 1)

enum PROCRESULT
{
    ERROR_NO = 0,
    ERROR_FILE_NOTFOUND = -1,
    ERROR_BAD_FILEVERSION = -2,
    ERROR_FILE_READ_FAILED = -3,
    ERROR_FILE_WRITE_FAILED = -4,
    ERROR_ACTION_FAILED = -5,
    ERROR_ARCH_MISMATCH = -6,
    ERROR_BAD_SUM = -7,
    ERROR_BAD_DATA = -8,
    ERROR_MEMALLOC_FAILED = -9
};

struct tCreationTime
{
    uint8_t values[10];
};

struct tSChannelCalibrationVer2  // Настройка канала контроля - в файлах версий 1,2
{
    int32_t ID;              // ID
    int32_t TVG;             // ВРЧ [мкс]
    int32_t PrismDelay;      // Время в призме, 2TP [10 * мкс]
    int32_t Sens[2];         // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t Gain[2];         // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t StGate[4];       // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала, 3 - Настройка времени в призме) [мкс]
    int32_t EdGate[4];       // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала, 3 - Настройка времени в призме) [мкс]
    int32_t Calibrated[2];   // Канал настроен (0 - ближняя зона, 1 - дальняя зона) // 0/1 - не настроен/jнастроен
    int32_t Mark;            // Метка (центр) [мкс] ( < 0 Выключенна )
    tCreationTime DataTime;  // Дата время выполнения настройки (TDataTime - Borland)
};

struct tSChannelCalibrationVer3  // Настройка канала контроля - в файлах версий 3
{
    int32_t ID;          // ID
    int32_t TVG;         // ВРЧ [мкс]
    int32_t PrismDelay;  // Время в призме, 2TP [10 * мкс]
    int32_t Sens[2];     // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t Gain[2];     // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t StGate[5];  // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме) [мкс]
    int32_t EdGate[5];  // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме) [мкс]
    int32_t Calibrated[2];   // Канал настроен (0 - ближняя зона, 1 - дальняя зона) // 0/1 - не настроен/jнастроен
    int32_t Mark;            // Метка (центр) [мкс] ( < 0 Выключенна )
    tCreationTime DataTime;  // Дата время выполнения настройки (TDataTime - Borland)
};

struct tSChannelCalibrationVer4  // Настройка канала контроля - в файлах версий 4
{
    int32_t ID;          // ID
    int32_t TVG;         // ВРЧ [мкс]
    int32_t PrismDelay;  // Время в призме, 2TP [10 * мкс]
    int32_t Sens[2];     // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t Gain[2];     // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t StGate[6];  // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t EdGate[6];  // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t Calibrated[2];   // Канал настроен (0 - ближняя зона, 1 - дальняя зона) // 0/1 - не настроен/jнастроен
    int32_t Mark;            // Метка (центр) [мкс] ( < 0 Выключенна )
    tCreationTime DataTime;  // Дата время выполнения настройки (TDataTime - Borland)
};

typedef int32_t tSnapshotFileID;

struct tSChannelCalibrationVer5  // Настройка канала контроля - в файлах версии 5
{
    int32_t ID;          // ID
    int32_t TVG;         // ВРЧ [мкс]
    int32_t PrismDelay;  // Время в призме, 2TP [10 * мкс]
    int32_t Sens[2];     // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t Gain[2];     // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t StGate[6];  // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t EdGate[6];  // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t Calibrated[2];                // Канал настроен (0 - ближняя зона, 1 - дальняя зона) // 0/1 - не настроен/jнастроен
    int32_t Mark;                         // Метка (центр) [мкс] ( < 0 Выключенна )
    tCreationTime DataTime;               // Дата время выполнения настройки (TDataTime - Borland)
    int32_t CalibrationTemperatureValue;  // в 1/10 долях градуса Цельсия
    tSnapshotFileID SnapshotFileID[2];
};

#define maxGateLowIndex 1
#define maxGateHighIndex 5

#define VERIFY_GATE_IDX_LOWMAX (assert((GateIdx >= 0) && (GateIdx <= maxGateLowIndex)))
#define VERIFY_GATE_IDX_HIGHMAX (assert((GateIdx >= 0) && (GateIdx <= maxGateHighIndex)))

#define VERIFY_CID_IDX (DEFCORE_ASSERT(Channel >= 0))
#define VERIFY_NEW_VALUE_SENS (DEFCORE_ASSERT(NewVal < 128 && NewVal > -128))
#define VERIFY_NEW_VALUE_GAIN (DEFCORE_ASSERT(NewVal < 128 && NewVal > -128))

struct tSChannelCalibration  // Настройка канала контроля - в файлах версии 6
{
    int32_t ID;                            // ID
    int32_t TVG;                           // ВРЧ [мкс]
    int32_t PrismDelay;                    // Время в призме, 2TP [10 * мкс]
    int32_t Sens[maxGateLowIndex + 1];     // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t Gain[maxGateLowIndex + 1];     // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int32_t StGate[maxGateHighIndex + 1];  // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t EdGate[maxGateHighIndex + 1];  // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int32_t Calibrated[maxGateLowIndex + 1];  // Канал настроен (0 - ближняя зона, 1 - дальняя зона) // 0/1 - не настроен/jнастроен
    int32_t Mark;                             // Метка (центр) [мкс] ( < 0 Выключенна )
    tCreationTime DataTime;                   // Дата время выполнения настройки (TDataTime - Borland)
    int32_t CalibrationTemperatureValue;      // в 1/10 долях градуса Цельсия
    tSnapshotFileID SnapshotFileID[maxGateLowIndex + 1];
    int32_t NotchStart[maxGateLowIndex + 1];  // Начало полки (0 - ближняя зона, 1 - дальняя зона) [мкс]
    int32_t NotchDur[maxGateLowIndex + 1];    // Длительность полки (0 - ближняя зона, 1 - дальняя зона) [мкс]
    int32_t NotchLevel[maxGateLowIndex + 1];  // Уровень полки (ослабления) (0 - ближняя зона, 1 - дальняя зона) [мкс]
};

struct tCalibrationsProperty
{
    char Name[256];
    uint32_t ReadOnly;
};

struct tCalibrationsProperty2
{
    char Name[256];
    uint32_t ReadOnly;
    uint32_t scheme;  // схема прозвучивания
};

struct datFileHeader
{
    int32_t fileVersion;
    int32_t sizeOfIntegerType;
    int32_t sizeOfDoubleType;
    double creatingTime;
#ifndef TBD
    TDataTime - Borland -
#endif
};

struct ChannelCalibrationPatch
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t side;
    tSChannelCalibration cannelCalibration;
};

struct ChannelCalibrationPatchVer2
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t side;
    tSChannelCalibrationVer2 cannelCalibration;
};

struct ChannelCalibrationPatchVer3
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t side;
    tSChannelCalibrationVer3 cannelCalibration;
};

struct ChannelCalibrationPatchVer4
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t side;
    tSChannelCalibrationVer4 cannelCalibration;
};

struct NamePatch  //   при вызове метода переименования настройки
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    char newCalibrationName[256];  // неиспользуемые символы заполнены нулями
};

struct CreateNewPatch  //    при вызове метода создания CreateNew
{
    uint8_t creationTime[10];
};

struct CreateFromPatch  //   при вызове метода создания  CreateFrom
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
};

struct DeletePatch  //  при вызове метода Delete(int Idx)
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
};

struct Delete2Patch  //  при вызове метода Delete(int Idx,bool toCreateTheOne)
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t toCreateTheOne;
};

struct ReadOnlyPatch  //  при вызове методов   SetReadOnly, ResetReadOnly
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t state;  // 0/1     -    ReadOnly сброшен/установлен
};

struct SetCurrentPatch  //  при вызове методов   SetCurrent
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;  // индекс текущей настройки
};

struct SetSchemePatch  //  при вызове метода   SetScheme
{
    uint8_t creationTime[10];
    int32_t calibrationIndex;
    int32_t scheme;
};

struct PATCH
{
    uint32_t sign;
    union
    {
        uint8_t patch[512];
        struct NamePatch namePatch;
        struct CreateFromPatch createFromPatch;
        struct CreateNewPatch createNewPatch;
        struct DeletePatch deletePatch;
        struct Delete2Patch delete2Patch;
        struct ReadOnlyPatch readOnlyPatch;
        struct SetCurrentPatch setCurrentPatch;
        struct SetSchemePatch setSchemePatch;
    };
};

#pragma pack(pop)
#define calibrationNameLength 254
#define mainDataSignature (-1)
#define channelCalibrationPatchSignVer2 1
#define namePatchSign 2
#define createNewPatchSign 3
#define createFromPatchSign 4
#define deletePatchSign 5
#define readOnlyPatchSign 6
#define setCurrentPatchSign 7
#define setSchemePatchSign 8
#define channelCalibrationPatchSignVer3 9
#define channelCalibrationPatchSignVer4 10
#define channelCalibrationPatchSignVer5 11
#define delete2PatchSign 12

#endif  // DEVICECALIBRATIONDEFINITIONS_H
