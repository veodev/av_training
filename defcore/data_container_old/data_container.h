#ifndef CDATACONTAINER_H
#define CDATACONTAINER_H

#include <QString>
#include <QFile>
#include "Definitions.h"
#include "ChannelsTable.h"
#include "DeviceConfig.h"

#include "data_container_constants.h"

#if defined(CreateFile)
#   undef CreateFile
#endif

// -----------------------------------------------------------------------------
typedef unsigned short tHeaderStr[65];     //
typedef tHeaderStr*    ptHeaderStr;        //
typedef unsigned short tHeaderBigStr[255]; //
typedef tHeaderBigStr* ptHeaderBigStr;     //
typedef char           tCardinalPoints[4]; // Стороны света
typedef unsigned char  tUsedItemsList[64]; // Флаги определяющие используемые записи
typedef unsigned short tLinkData[16];      //

#pragma pack(push,1)
struct tMRFPost // Столб
{
    int Km[2];
    int Pk[2];
};

struct sUnitInfo // Информация о блоке прибора
{
    unsigned char UnitType;   // Тип блока (eDeviceUnitType)
    tHeaderStr WorksNumber;   // Заводской номер блока
    tHeaderStr FirmwareVer;   // Версия программного обеспечения блока
};
typedef sUnitInfo tUnitsInfoList[10]; // Информация о блоках прибора

// Путейская координата
struct tCaCrd // Размер 8 байт
{
    int XXX;
    int YYY;
};

struct tMRFCrd // Размер 12 байт
{
    int Km;
    int Pk;
    int mm;
};

struct sCoordPostMRF // Размер: 144 Байта
{
    int Km[2];
    int Pk[2];
    int Reserv[32];
};

struct sCoordPostChainage // Размер: 136 Байта
{
    tCaCrd Val; // Координата в фармате XXX.YYY
    int Reserv[32];
};

struct sEchoBlockItem
{
    unsigned char Delay;
    unsigned char Ampl;
};
typedef sEchoBlockItem tEchoBlock[8]; // Сигналы В-развертки одного канала, на одной координате

struct sFileHeader // Заголовок файла
{
    tAviconFileID FileID; // RSPBRAFD
    tAviconFileID DeviceID; // Идентификатор прибора
    unsigned char DeviceVer; // Версия прибора
    unsigned char HeaderVer; // Версия заголовка
    int MoveDir; // Направление движения: + 1 в сторону увеличения путейской координаты; - 1 в сторону уменьшения путейской координаты
    unsigned short ScanStep; // Шаг датчика пути (мм * 100)
    unsigned char PathCoordSystem; // Система отсчета путейской координаты
    tUsedItemsList UsedItems; // Флаги определяющие используемые записи
    unsigned int CheckSumm; // Контрольная сумма (unsigned 32-bit)
    unsigned char UnitsCount; // Количество блоков прибора
    tUnitsInfoList UnitsInfo; // Информация о блоках прибора
    tHeaderStr Organization; // Название организации осуществляющей контроль
    tHeaderStr RailRoadName; // Название железной дороги
    unsigned int DirectionCode; // Код направления
    tHeaderStr PathSectionName; // Название участка дороги
    tHeaderStr OperatorName; // Имя оператора
    int RailPathNumber; // Номер ж/д пути
    int RailSection; // Звено рельса
    unsigned short Year; // Дата время контроля
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    int StartKM; // Начальная координата - Метрическая
    int StartPk;
    int StartMetre;
    unsigned char WorkRailTypeA;  // Рабочая нить (для однониточных приборов): 0 – левая, 1 – правая
    tCaCrd StartChainage;  // Начальная координата в зависимости от PathCoordSystem
    tCardinalPoints WorkRailTypeB;  // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
    tCardinalPoints TrackDirection; // Код направления:                          NB, SB, EB, WB, CT, PT, RA, TT
    tCardinalPoints TrackID;        // Track ID:                                 NR, SR, ER, WR
    unsigned char CorrespondenceSides; // Соответствие стороны тележки нитям пути
    double HCThreshold; // Уровень порога для Head Checking (Венгрия)
    tLinkData ChIdxtoCID;     // Массив связи индекса канала в файле с CID
    tLinkData ChIdxtoGateIdx; // Массив связи индекса канала в файле с номером строба
    unsigned char Reserv[2018 - 64]; // Резерв
    // + Кодировка
    unsigned int TableLink; // Ссылка на расширенный заголовок
};
#pragma pack(pop)

// -----------------------------------------------------------------------------
class cDataContainer
{
private:

    sFileHeader Head;
    bool HeadOpen;
    QFile *DataFile;

    int CurSysCoord; // Текущая системная координата
    int CurDisCoord; // Текущая дисплейная координата
    int MaxSysCoord; // Максимальная системная координата
    int MinSysCoord; // Минимальная системная координата
    int MaxDisCoord; // Максимальная дисплейная координата
    int SaveSysCoord; // Прошлая системная координата
    int LastOffset;   // Смещение последнего записанного события
    bool BackMotionFlag; //
    bool SearchBM;       //
    bool SearchBMEnd;    //
    int StartBMSysCoord; //
    tMRFCrd LastMRFCrd; // Последняя отметка путейской координаты
    int LastPostSysCrd; // Системная координата последней отметки путейской координаты
    //int LastSysCrdOffset;
    unsigned char id;
    int SaveScanStep;
    int SaveMoveDir;
    unsigned char CIDtoChIdx[3][256]; // Массив связи CID и номера строба с индексом канала в файле
    tLinkData ChIdxtoCID;     // Массив связи индекса канала в файле с CID
    tLinkData ChIdxtoGateIdx; // Массив связи индекса канала в файле с номером строба


    QString HeaderStrToString(tHeaderStr Text);
    void StringToHeaderStr(QString InText, ptHeaderStr OutText);
    QString HeaderBigStrToString(tHeaderBigStr Text);
    void StringToHeaderBigStr(QString InText, ptHeaderBigStr OutText);

public:
    cDataContainer(int Scheme); // Схема прозвучивания: 1, 2, 3
    ~cDataContainer();
    bool CreateFile(QString FileName);

    // Заполнение заголовка файла:

    bool AddDeviceUnitInfo(eDeviceUnitType UnitType, QString WorksNumber, QString FirmwareVer); // Добавить информацию о блоке прибора
    bool SetRailRoadName(QString Name); // Название железной дороги
    bool SetOrganizationName(QString Org); // Название организации осуществляющей контроль
    bool SetDirectionCode(unsigned int Code); // Код направления
    bool SetPathSectionName(QString Name); // Название участка дороги
    bool SetRailPathNumber(int Number); // Номер ж/д пути
    bool SetRailSection(int Number); // Звено рельса
    bool SetDateTime(unsigned short Day_, unsigned short Month_, unsigned short Year_, unsigned short Hour_, unsigned short Minute_); // Дата/время контроля
    bool SetOperatorName(QString Name); // Имя оператора
    bool SetStartMRFCrd(tMRFCrd StartCrd);
    bool SetStartCaCrd(eCoordSys CoordSys, tCaCrd Chainage);
    bool SetWorkRailTypeB(tCardinalPoints Val); // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
    bool SetTrackDirection(tCardinalPoints Val); // Код направления: NB, SB, EB, WB, CT, PT, RA, TT
    bool SetTrackID(tCardinalPoints Val); // Track ID: NR, SR, ER, WR
    bool SetCorrespondenceSides(eCorrespondenceSides Val); // Соответствие сторон тележки нитям пути
    bool CloseHeader(int MovDir, unsigned short ScanStep); // Закончить формирование заголовка (ScanStep - мм * 100)

    void CloseFile(void); // Закончить формирование файла

    // Сигналы и координата:

    void AddSysCoord(int SysCoord); // Новая координата
    // Сигналы В-развертки
    bool AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, unsigned char D0, unsigned char A0, unsigned char D1, unsigned char A1, unsigned char D2, unsigned char A2, unsigned char D3, unsigned char A3, unsigned char D4, unsigned char A4, unsigned char D5, unsigned char A5, unsigned char D6, unsigned char A6, unsigned char D7, unsigned char A7);
    bool AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, tEchoBlock EchoBlock);
    void AddSensor1State(eDeviceSide Side, bool State); // Данные датчика болтового стыка

    // Настройки каналов:

    void AddSens(eDeviceSide Side, CID Channel, int GateIndex, char NewValue); // Изменение условной чувствительности
    void AddGain(eDeviceSide Side, CID Channel, unsigned char NewValue); // Изменение положения нуля аттенюатора (0 дБ условной чувствительности)
    void AddTVG(eDeviceSide Side, CID Channel, unsigned char NewValue); // Изменение ВРЧ
    void AddStGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue); // Изменение положения начала строба
    void AddEndGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue); // Изменение положения конца строба
    void AddPrismDelay(eDeviceSide Side, CID Channel, unsigned short NewValue); // Изменение 2Тп мкс * 10 (word)
    void AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime, bool AddChInfo, eDeviceSide Side, CID Channel, int GateIndex); // Информация о смене режима
    void AddSetRailType(void); // Настройка на тип рельса
    void AddHeadPh(eDeviceSide Side, CID Channel, int GateIndex, bool Enabled); // Изменение состояния наушников (вкл/выкл)

    // Отметки:

    void AddTextLabel(QString Text); // Добавление текстовой отметки
    void AddDefLabel(eDeviceSide Side, QString Text); // Добавление отметки о дефекте
    void AddStrelka(QString Text); // Добавление отметки о стрелочном переводе
    void AddStBoltStyk(void); // Отметка начало болтового стыка
    void AddEdBoltStyk(void); // Отметка конец болтового стыка
    void AddTime(unsigned short Hour_, unsigned short Minute_); // Время
    void AddHandScan();

    void AddMRFPost(tMRFPost Post); // Столб (километр / пикет )
    void AddCaPost(tCaCrd Chainage); // Путевой столб иностранный

    void AddSatelliteCoord(float Latitude, float Longitude); // Географическая координата
    void AddSCReceiverStatus(unsigned char State); // Состояние приемника GPS
    void AddMedia(void * DataPrt, tMediaType Type, unsigned int Size); // Медиа данные
    // void AddHandScanFromDataContainer(SrcData: TAviconDataContainer; Rail: TRail; Surf, HandChNum, Att, Ku, TVG: ShortInt; StartCrd, EndCrd: Integer);

    // -------------------------------------------

    tMRFCrd GetCurrectCoord(void); // Текущая путейская координата

    // Загрузка:
    /*
        procedure LoadData(StreamIdx: Integer; StartDisCoord, EndDisCoord, LoadShift: Integer; BlockOk: TDataNotifyEvent);
        function DisToFileOffset(StreamIdx: Integer; NeedDisCoord: Integer; var DisCoord: Int64; var SysCoord: Integer; var OffSet: Integer): Boolean;
        function GetParamFirst(StreamIdx: Integer; DisCoord: Integer; var Params: TEvalChannelsParams; var Sensor1: TSensor1Data): Boolean;
        function GetParamNext(StreamIdx: Integer; DisCoord: Integer; var Params: TEvalChannelsParams; var Sensor1: TSensor1Data): Boolean;
        function GetBMStateFirst(StreamIdx: Integer; DisCoord: Integer): Boolean;
        function GetBMStateNext(StreamIdx: Integer; var DisCoord: Integer; var State: Boolean): Boolean;

        function SysToDisCoord(Coord: Integer): Integer;
        function DisToSysCoord(Coord: Integer): Integer;
        procedure DisToDisCoords(Coord: Integer; var Res: TIntegerDynArray);

        function GetEventIdx(StartDisCoord, EndDisCoord: Integer; var StartIdx, EndIdx: Integer; EmptyPar: Integer): Boolean;
        procedure GetNearestEventIdx(DisCoord: Integer; var LeftIdx, RightIdx: Integer; var SameCoord: TIntegerDynArray);
        function GetLeftEventIdx(DisCoord: Integer): Integer;
        function GetRightEventIdx(DisCoord: Integer): Integer;
        procedure GetEventData(Idx: Integer; var ID: Byte; var pData: PEventData);

      // -------------< Система координат - Метрическая РФ >-----------------------------------

        function DisToMRFCrdPrm(DisCoord: Integer; var CoordParams: TMRFCrdParams): Boolean;
        function DisToMRFCrd(DisCoord: Longint): TMRFCrd;
        function MRFCrdToDisCrd(RFC: TMRFCrd; var DisCoord: Longint): Boolean;

      // -------------< Система координат - Имперская >----------------------------------------

        function DisToCaCrdPrm(DisCoord: Integer; var CoordParams: TCaCrdParams): Boolean;
        function DisToCaCrd(DisCoord: Longint): TCaCrd;
        function CaCrdToDis(CaCrd: TCaCrd; var DisCoord: Longint): Boolean;

      // --------------------------------------------------------------------------------------

        property MaxDisCoord: Integer read GetMaxDisCoord;
        property MaxSysCoord: Integer read GetMaxSysCoord;
        property EventCount: Integer read GetEventCount;
        property Event[Index: Integer]: TFileEvent read GetEvent;

        property CurSaveSysCoord: Longint read FCurSaveSysCrd;  // Системная координата записи
        property CurSaveDisCoord: Integer read FCurSaveDisCrd;  // Дисплейная координата записи

                                                                                   // Загрузка - LoadData
        property CurLoadOffset[Index: Integer]: Integer read GetCurLoadOffset;     // FCurOffset;
        property CurLoadSysCoord[Index: Integer]: Longint read GetCurLoadSysCoord; // FCurSysCoord;
        property CurLoadDisCoord[Index: Integer]: Int64 read GetCurLoadDisCoord;   // FCurDisCoord;
        property CurLoadEcho[Index: Integer]: TCurEcho read GetCurLoadEcho;        // FCurEcho
        property CurLoadBackMotion[Index: Integer]: Boolean read GetCurLoadBackMotion;
        property CurLoadParams[Index: Integer]: TEvalChannelsParams read GetCurLoadParams;
        property CurLoadSensor1[Index: Integer]: TSensor1Data read GetLoadSensor1;

        property Config: TDataFileConfig read FConfig;
        property Header: TFileHeader read FHead;

    //    property StartCoord: TRealCoord read GetStartCoord;
        property CoordSys: TCoordSys read FPathCoordSystem;
        property LastCaPost: TCaCrd read FLastCaPost;

      end;
    */
};

tMRFCrd GetPrevMRFPostCrd(tMRFCrd Post, int MoveDir); // Получить дредыдущий столб
tMRFCrd GetNextMRFPostCrd(tMRFCrd Crd, int MoveDir); // Получить следующий столб
tMRFPost GetMRFPost(tMRFCrd Crd, int MoveDir); // Получить следующий столб

// Режимы:
/*
    0	Настройка сплошного канала + Нить, номер канала
    1	Настройка ручного канала +    Нить, номер канала
    2	Оценка	Нить, номер канала
    3	Ручной	Нить, номер канала
    4	Поиск В	-
    5	Поиск М	-
    6	Меню	-
    7	Пауза	-
    8	Настройка 2Тп сплошного канала	Нить, номер канала
    9	Настройка 2Тп ручного канала	Нить, номер канала
    10	Сканирование вперед (МИГ)	-
    11	Сканирование назад (МИГ)	-
    */

#endif // CDATACONTAINER_H
