#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <utility>

#include "Definitions.h"
#include "ChannelsTable.h"
#include "DeviceCalibration.h"

#include "sockets/socket_can.h"
#include "sockets/socket_lan.h"

/*enum eDeviceConfigID // Идентификатор конфигураци (тип прибора)
{
    dcAutocon = 0, // АВТОКОН
    dcAvicon31 = 1, // Авикон-31
    dcAvicon15 = 2, // Авикон-15
    dcFilusX111W_multichannel = 3, // FilusX111W сплошной контроль
    dcFilusX111W_singlechannel = 4, // FilusX111W ручной контроль
    dcHeadScaner = 5, // Сканер головки Авикона-31
    dcHeadScaner_Test = 6, // Сканер головки Авикона-31 - проверка каналов
    dcBUMTest = 7, // БУМ ТЕСТ
    dcNotSet = 0xFF // Не задано
};*/


typedef struct
{
    unsigned char UMUIndex;   // Номер БУМ
    unsigned char UMUSide;    // Сторона БУМ
    unsigned char UMULine;    // Линия БУМ
    unsigned char UMUStroke;  // Такт БУМ
    eDeviceSide DEVSide;      // Сторона дефектоскопа
    CID DEVChannel;           // Канал дефектоскопа
} tUMUtoDEVitem;

typedef std::vector<tUMUtoDEVitem> tUMUtoDEVList;

typedef struct
{
    unsigned char UMUIndex;  // Номер БУМ
    unsigned char GenLine;   // Номер линии генератора БУМ
    unsigned char ResLine;   // Номер линии приемника БУМ
    unsigned char Stroke;    // Номер такта
    eUMUSide Side;           // Сторона БУМ
                             //	   int DelayScale;        // Масштаб А-развертки
                             //     int Duration;          // Длительность развертки
                             //	   sGate BScan;
} tChannelData;

typedef struct
{
    int ScanChannelIndex;
    int GeneratorLine[2];
    int ReceiverLine[2];
    int Generator[2];
    int Receiver[2];
    CID cid[2];
    eDeviceSide DeviceSide;

} tStrokeGenRecFaceDataItem;  // Генераторы / приемники для каналов контроля рабочей/нерабочей грани

struct StrokeGroupSearchFunctor
{
    explicit StrokeGroupSearchFunctor(int group)
        : _group(group)
    {
    }
    inline bool operator()(const sScanChannelDescription& m) const
    {
        return m.StrokeGroupIdx == _group;
    }

private:
    int _group;
};

struct StrokeGroupCounterFunctor
{
    explicit StrokeGroupCounterFunctor(int group, int count)
        : _group(group)
        , _count(count)
    {
    }
    inline bool operator()(const sScanChannelDescription& m)
    {
        return m.StrokeGroupIdx == _group && (_count-- == 0);
    }

private:
    int _group;
    int _count;
};


class cDeviceConfig  // Конфигуратор Дефектоскопа
{
public:
    bool GeneratorOff;  // DEBUG - флаг выключение генераторов

    explicit cDeviceConfig(cChannelsTable* tbl_);  // Конструктор
    virtual ~cDeviceConfig();                      // Деструктор

protected:
    bool UseNotch;               // Использование "полочек" ВРЧ
    bool BScanDataFiltration;    // Фильтрация В-развертки и получение АСД из В-развертки
    bool UseLineSwitch;          // Использовать реле для подключения линий ручного контроля
    bool FilteringBottomSignal;  // Фильтрация донного сигнала
    bool SendBottomSignal;
    unsigned int UMUCount;

    int MaxControlSpeed;             // Максимальная допустимая скорость контроля [км/ч]
    eControlledRail ControlledRail;  // Контролируемые нити
    eCoordSys CoordSys;              // Система отсчета путейской координаты
    eTuningGate TuningGate;          // Стробы, используемые при настройке каналов контроля

    int GainBase;         // Минимальное загружаемое значение аттенюатора
    int GainMax;          // Максимальное значение аттенюатора
    float PrismDelayMin;  // Минимально допустимое время в призме
    float PrismDelayMax;  // Максимальное допустимое время в призме

    int BScanGateLevel;                                    // Уровень строба В-развертки [отсчетов]
    int EvaluationGateLevel;                               // Уровень строба А-развертки [отсчетов]
    int MainPathEncoderIndex;                              // Индекс главного датчика пути
    eMirrorChannelTuningMethod MirrorChannelTuningMethod;  // Метод настройки зеркальных каналов
    int SaveChannelGroup;                                  //

    int ACStartDelay;  // Момент начала подсчета суммы сигналов А-развертки, [мкс]
    double GainStep;   // Шаг аттенюатора в децибелах

    void* Params;            // Иные параметры
	char* DeviceName;  // Название прибора

    UMULineSwitching UMUConnectors;  // Используемые разъемы БУМ

    tChannelCalibrationCopyData CalibrationCopyData;  // Список каналов не подлежащих настройки

    tUmuDescriptionsList umuDescriptions;

    tScanChannelsList ScanChannels;      // Список каналов сплошного контроля
    tHandChannelsList HandChannels;      // Список каналов ручного контроля
    tDeviceModeDataList ModeList;        // Режимы работы каналов сплошного контроля
    sGate RailTypeTuningGate;            // Строб поиска донного сигнала при настройке на тип рельса
    sGate RailTypeTuningGate_forSwitch;  // Строб поиска донного сигнала при настройке на тип рельса в зоне стрелочного перевода
    tRailTypeTuningList RailTypeTuning;  // Данные для выполнения настроки на тип рельса
                                         //	tRecommendParamsList RecParams;           // Список рекомендованных значений
    cChannelsTable* tbl;                 // Таблица каналов
    tBScanTapesGroupList BScanTapesGroupList;

    std::vector<int> GroupIndexList;                              // Список групп каналов
    std::vector<tStrokeGenRecFaceDataItem> StrokeGenRecFaceData;  // Список генераторов/приемников для каналов рабнчей/нерабочей грани

private:
    int FUMUIndex;
    int FGroupIndex;

public:
	bool virtual LoadFromFile(const char* filename);
	char* GetDeviceName(void); // Название прибора
	unsigned char virtual dBGain_to_UMUGain(char dBGain) = 0;
    size_t GetAllScanChannelsCount() const;

    // Каналы ручного сканирования

    // ???

    // Каналы сканирования

    unsigned int GetUMUCount() const;  // Количество БУМ
    void SetUMUIndex(int UMUIndex);    // Выбор БУМ для которого будут выбераться данные

    int GetChannelGroupCount() const;                              // Количество групп каналов
    int GetChannelGroupCount(int* MinGroupIdx, int* MaxGroupIdx);  // Получить количество групп
    int GetChannelGroup(int Index) const;                          // Номер группы каналов
    void SetChannelGroup(int GroupIndex);                          // Выбор группы каналов для которой будут выбераться данные
    int GetCurrentChannelGroup() const;
    int GetCurrentChannelGroupIndex() const;
    sGate GetRailTypeTuningGate() const;  // Получить строб для поиска максимума при настройке на тит рельса
    sGate GetRailTypeTuningGate_forSwitch() const; // Получить строб для поиска максимума при настройке на тит рельса в зоне стрлочного перевода

    // Все данные получаемые ниже даются для ранее выбранного БУМ и группы каналов (см. выше)

    int GetScanStrokeCount() const;
    void ResetUsedList();
    void resetUMUtoDEVList();
    void GetScanChannelsStrokeParams(int StrokeIndex, tStrokeParams* Params, CID OneChannelId = -1);

    void GetHandStrokeParams(CID Channel, tStrokeParams* Params, int* UMUIndex);

    int GetHandChannelsCount() const;

    unsigned int GetScanChannelsCount_() const;
    int GetScanChannelIndex_(int Idx);
    CID GetScanChannel(int Index) const;
    bool GetChannelData(eDeviceSide Side, CID Channel, tChannelData* Data);
    int GetStrokeIndexByCID(eDeviceSide Side, CID Channel);


    bool GetChannelBySLS(eChannelType ChType, int UMUIdx, eUMUSide Side, int ReceiverLine, int StrokeIndex, eDeviceSide* Side_, CID* Channel) const;
    void GetSLSByChannel(eChannelType ChType, eDeviceSide Side_, CID Channel, int* UMUIdx, eUMUSide* Side, int* Line, int* StrokeIndex) const;
    void SetBScanGateLevel(int BScanGateLevel_);                           // Установка уровня строба В-развертки [отсчетов]
    bool SetFaceData(tTrolleySide Side, cDeviceCalibration* Calibration);  // Установка соответствия рабочех/нерабочех граней
    bool IsCopySlaveChannel(eDeviceSide Side, CID Channel);

    ///////////////////////////////////////////////////////////////////////

    bool ScanChannelExists(CID id);
    bool HandChannelExists(CID id);
    //	Считаем все с 0!!!
    bool getSChannelbyIdx(int Idx, sScanChannelDescription* val);
    int getFirstSChannelbyID(CID id, sScanChannelDescription* val);
    int getNextSChannelbyID(int StartIdx, CID id, sScanChannelDescription* val);
    bool getHChannelbyIdx(int Idx, sHandChannelDescription* val);
    bool getHChannelbyID(CID id, sHandChannelDescription* val);

    bool ReceiverStateByCID(CID id, bool state);
    bool GetReceiverStateByCID(CID id) const;
    bool SetPulseAmplByCID(CID id, int Val);
    int GetPulseAmplByCID(CID id) const;
    int GetBScanGateLevel()
    {
        return BScanGateLevel;
    };

    int GetControlModeCount() const;
    sDeviceModeData GetControlModeItem(int Idx) const;

    size_t GetRailTypeTuningCount() const;
    sRailTypeTuningGroup GetRailTypeTuningItem(int Idx) const;

    tChannelsList ChannelsList;              // Список каналов контроля
    tSensTuningParamsList SensTuningParams;  // Параметры настройки каналов контроля на условную чувствительность

    eProtocol getProtocolForUmuNum(unsigned int umu_id) const;
    cISocket::cConnectionParams* getConnectionParamsForUmuNum(int umu_id) const;

    const sBScanTape& getTapeByIndex(int index) const;
    tSensAutoCalibration SensAutoCalibration;  // Данные автонастройки Ку для теневого метода контроля
    tSensValidRangesList SensValidRangesList;  // Допустимые отклонения (от нормативных) значений условной чувствительности

    tUMUtoDEVList UMUtoDEVList;  // Список связывающий DeviceSise, CID и СЛТ БУМа

    bool SetGenerator(eUMUSide Side, int GeneratorLine, int NewGenerator);
    bool SetReceiver(eUMUSide Side, int ReceiverLine, int NewReceiver);

    tSensValidRangesList getSensValidRangesList() const;
    eControlledRail getControlledRail() const;
    int getMainPathEncoderIndex() const;
    UMULineSwitching getUMUConnectors() const;
    int getGainBase() const;
    int getGainMax() const;
    double getGainStep() const;
    float getPrismDelayMin() const;
    float getPrismDelayMax() const;
    bool getFilteringBottomSignal() const;
    int getEvaluationGateLevel() const;
    bool getBScanDataFiltration() const;
    bool getUseLineSwitch() const;
    int getBScanGateLevel() const;
    int getACStartDelay() const;
    void setACStartDelay(int value);
    eCoordSys getCoordSys() const;
    int getMaxControlSpeed() const;
    int getSaveChannelGroup() const;
    void setSaveChannelGroup(int value);
    tChannelCalibrationCopyData getCalibrationCopyData() const;
    bool getSendBottomSignal() const;
	std::vector<CID> getCIDsForCurrentGroup();

	int Tag;
};

#endif /* DEVICECONFIG_H */
