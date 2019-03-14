/* ТЕСТ
 * DeviceConfigurator.cpp
 *
 *  Created on: 07.05.2012
 *      Author: Денис
 */

#include <cassert>
#include <stdexcept>
#include "DeviceConfig.h"
#include <algorithm>
#include "ChannelsIds.h"
// #define DEBUG

#ifdef DEBUG
#include <Classes.hpp>
TStringList* log_;
#endif


cDeviceConfig::cDeviceConfig(cChannelsTable* tbl_)
{
    GeneratorOff = false;
    UMUCount = 0;
    DeviceName = NULL;              // Название прибора
    MaxControlSpeed = 0;            // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crBoth;        // Контролируемые нити
    TuningGate = tgFixed;           // Стробы, используемые при настройке каналов контроля
    CoordSys = csMetricRF;          // Система отсчета путейской координаты - Российская
    UseNotch = false;               // "Полочеки" ВРЧ не используются
    GainBase = 0;                   // Минимальное загружаемое значение аттенюатора
    GainMax = 0;                    // Максимальное значение аттенюатора
    PrismDelayMin = 0;              // Минимально допустимое время в призме
    PrismDelayMax = 0;              // Максимальное допустимое время в призме
    GainStep = 0;                   // Шаг аттенюатора в децибелах
    BScanGateLevel = 0;             // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = 0;        // Уровень строба А-развертки [отсчетов]
    MainPathEncoderIndex = 0;       // Индекс главного датчика пути
    ACStartDelay = 0;               // Момент начала подсчета суммы сигналов А-развертки, [мкс]
    BScanDataFiltration = false;    // Фильтрация В-развертки и получение АСД из В-развертки
    UseLineSwitch = false;          // Использовать реле для подключения линий ручного контроля
    FilteringBottomSignal = false;  // Фильтрация донного сигнала
    SendBottomSignal = false;
    Params = NULL;                             // Иные параметры
    UMUConnectors = toCompleteControl;         // Используемые разъемы БУМ
    SensAutoCalibration.CalibrationDelay = 0;  // Данные автонастройки Ку для теневого метода контроля
    SensAutoCalibration.MinSensChange = 0;     // Интервал между выполнением настроект, мс
    RailTypeTuningGate.gStart = 0;             // Строб поиска донного сигнала при настройке на тип рельса
    RailTypeTuningGate.gEnd = 0;
    RailTypeTuningGate_forSwitch.gStart = 0;   // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 0;
    FUMUIndex = 0;
    FGroupIndex = 0;
    CalibrationCopyData.clear();

    tbl = tbl_;
    ACStartDelay = 26;
    BScanDataFiltration = false;        // Фильтрация - выключенна
    UseLineSwitch = false;              // Реле для подключения линий ручного контроля - ВЫКЛ
    UMUConnectors = toCompleteControl;  // Используемые разъемы БУМ - сплошного контроля
    Params = NULL;
	SaveChannelGroup = -1;
	Tag = -1;

#ifdef DEBUG
    log_ = new TStringList();
#endif
}

cDeviceConfig::~cDeviceConfig()
{
    DeviceName = NULL;
    for (size_t i = 0; i < umuDescriptions.size(); ++i) {  // Удаление connectionParams
        delete umuDescriptions[i].connectionParams;
        umuDescriptions[i].connectionParams = NULL;
    }

#ifdef DEBUG
    log_->SaveToFile("log_.txt");
    delete log_;
#endif
}

bool cDeviceConfig::getSendBottomSignal() const
{
    return SendBottomSignal;
}

std::vector<CID> cDeviceConfig::getCIDsForCurrentGroup()
{
    std::vector<CID> res;

    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& currentChannel = it.operator*();
        if (currentChannel.StrokeGroupIdx == FGroupIndex) {
            res.push_back(currentChannel.Id);
        }
    }

    return res;
}

tChannelCalibrationCopyData cDeviceConfig::getCalibrationCopyData() const
{
    return CalibrationCopyData;
}

int cDeviceConfig::getSaveChannelGroup() const
{
    return SaveChannelGroup;
}

void cDeviceConfig::setSaveChannelGroup(int value)
{
    SaveChannelGroup = value;
}

int cDeviceConfig::getMaxControlSpeed() const
{
    return MaxControlSpeed;
}

eCoordSys cDeviceConfig::getCoordSys() const
{
    return CoordSys;
}

int cDeviceConfig::getACStartDelay() const
{
    return ACStartDelay;
}

void cDeviceConfig::setACStartDelay(int value)
{
    ACStartDelay = value;
}

int cDeviceConfig::getBScanGateLevel() const
{
    return BScanGateLevel;
}

bool cDeviceConfig::getUseLineSwitch() const
{
    return UseLineSwitch;
}

bool cDeviceConfig::getBScanDataFiltration() const
{
    return BScanDataFiltration;
}

int cDeviceConfig::getEvaluationGateLevel() const
{
    return EvaluationGateLevel;
}

bool cDeviceConfig::getFilteringBottomSignal() const
{
    return FilteringBottomSignal;
}

float cDeviceConfig::getPrismDelayMax() const
{
    return PrismDelayMax;
}

float cDeviceConfig::getPrismDelayMin() const
{
    return PrismDelayMin;
}

double cDeviceConfig::getGainStep() const
{
    return GainStep;
}

int cDeviceConfig::getGainMax() const
{
    return GainMax;
}

int cDeviceConfig::getGainBase() const
{
    return GainBase;
}

UMULineSwitching cDeviceConfig::getUMUConnectors() const
{
    return UMUConnectors;
}

int cDeviceConfig::getMainPathEncoderIndex() const
{
    return MainPathEncoderIndex;
}

eControlledRail cDeviceConfig::getControlledRail() const
{
    return ControlledRail;
}

bool cDeviceConfig::ScanChannelExists(CID id)
{
    DEFCORE_ASSERT(id < 255);
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        if (id == it.operator*().Id) {
            return true;
        }
    }
    return false;
}

bool cDeviceConfig::HandChannelExists(CID id)
{
    DEFCORE_ASSERT(id < 255);
    for (tHandChannelsList::const_iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
        if (id == it.operator*().Id) {
            return true;
        }
    }
    return false;
}

bool cDeviceConfig::getSChannelbyIdx(int Idx, sScanChannelDescription* val)
{
    DEFCORE_ASSERT(val != NULL);
    if (static_cast<size_t>(Idx) < ScanChannels.size()) {
        *val = ScanChannels[Idx];
        return true;
    }

    return false;
}

int cDeviceConfig::getFirstSChannelbyID(CID id, sScanChannelDescription* val)
{
    DEFCORE_ASSERT(val != NULL);
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& cur = it.operator*();
        if (id == cur.Id) {
            *val = cur;
            return it - ScanChannels.begin();
        }
    }
    return -1;
}

int cDeviceConfig::getNextSChannelbyID(int StartIdx, CID id, sScanChannelDescription* val)
{
    DEFCORE_ASSERT(StartIdx >= 0);
    DEFCORE_ASSERT(val != NULL);
    DEFCORE_ASSERT(id < 255);
    size_t size = ScanChannels.size();
    for (size_t i = StartIdx + 1; i < size; ++i) {
        if (id == ScanChannels[i].Id) {
            *val = ScanChannels[i];
            return i;
        }
    }
    return -1;
}

bool cDeviceConfig::getHChannelbyIdx(int Idx, sHandChannelDescription* val)
{
    DEFCORE_ASSERT(val != NULL);

    if (HandChannels.size() > 0) {
        if ((size_t) Idx < HandChannels.size()) {
            *val = HandChannels[Idx];
            return true;
        }
    }

    return false;
}

bool cDeviceConfig::getHChannelbyID(CID id, sHandChannelDescription* val)
{
    DEFCORE_ASSERT(val != NULL);
    DEFCORE_ASSERT(id < 255);
    for (tHandChannelsList::const_iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
        if (id == it.operator*().Id) {
            *val = it.operator*();
            return true;
        }
    }
    return false;
}

tSensValidRangesList cDeviceConfig::getSensValidRangesList() const
{
    return SensValidRangesList;
}

bool cDeviceConfig::LoadFromFile(const char* filename)
{
    UNUSED(filename);
    return false;
}

void cDeviceConfig::SetUMUIndex(int UMUIndex)  // Выбор БУМ для которого будут выбераться данные
{
    FUMUIndex = UMUIndex;
}
/*
int cDeviceConfig::GetGroupCount()            // Количество групп каналов
{
    int GroupIndex = 0;
    for (size_t i = 0; i < ScanChannels.size(); i++)
    {
        GroupIndex = std::max(GroupIndex, ScanChannels[i].StrokeGroupIdx);
    }
    return GroupIndex + 1;
}
*/
int cDeviceConfig::GetChannelGroupCount() const  // Количество групп каналов
{
    return GroupIndexList.size();
}

int cDeviceConfig::GetChannelGroupCount(int* MinGroupIdx, int* MaxGroupIdx)  // Получить количество групп
{
    unsigned char MinGroupIdx_ = 100;
    unsigned char MaxGroupIdx_ = 0;
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& cur = it.operator*();
        MinGroupIdx_ = std::min(MinGroupIdx_, cur.StrokeGroupIdx);
        MaxGroupIdx_ = std::max(MaxGroupIdx_, cur.StrokeGroupIdx);
    }
    if (MinGroupIdx) *MinGroupIdx = MinGroupIdx_;
    if (MaxGroupIdx) *MaxGroupIdx = MaxGroupIdx_;
    return MaxGroupIdx_ - MinGroupIdx_ + 1;
}

int cDeviceConfig::GetChannelGroup(int Index) const  // Номер группы каналов
{
    if (GroupIndexList.size() > 0 && Index >= 0 && Index < (int) GroupIndexList.size()) {
        return GroupIndexList[Index];
    }

    return -1;
}

int cDeviceConfig::GetCurrentChannelGroup() const
{
    return FGroupIndex;
}

int cDeviceConfig::GetCurrentChannelGroupIndex() const
{
    int size = GroupIndexList.size();
    for (int i = 0; i < size; ++i) {
        if (GroupIndexList[i] == FGroupIndex) {
            return i;
        }
    }
    return -1;
}

sGate cDeviceConfig::GetRailTypeTuningGate() const
{
    return RailTypeTuningGate;
}

sGate cDeviceConfig::GetRailTypeTuningGate_forSwitch() const
{
    return RailTypeTuningGate_forSwitch;
}

void cDeviceConfig::SetChannelGroup(int GroupIndex)  // Выбор группы каналов для которой будут выбераться данные
{
    FGroupIndex = GroupIndex;
}

// Все данные получаемые ниже даются для ранее выбранного БУМ и группы каналов (см. выше)

/*
int cDeviceConfig::GetHandStrokeCount()
{
    return 1;
}
*/
void cDeviceConfig::GetHandStrokeParams(CID Channel, tStrokeParams* Params, int* UMUIndex)
{
    Params->StrokeIdx = 0;
    // Изначально все генераторы выключенны
    Params->GenResLeftLine1 = 0x00;   // Генератор/приемник для левой стороны, линия 1
    Params->GenResLeftLine2 = 0x00;   // Генератор/приемник для левой стороны, линия 2
    Params->GenResRightLine1 = 0x00;  // Генератор/приемник для правой стороны, линия 1
    Params->GenResRightLine2 = 0x00;  // Генератор/приемник для правой стороны, линия 2

    Params->ZondAmplRightLine1 = 0;
    Params->ZondAmplRightLine2 = 0;
    Params->ZondAmplLeftLine1 = 0;
    Params->ZondAmplLeftLine2 = 0;
    Params->ParamsRightLine1 = 0;
    Params->ParamsRightLine2 = 0;
    Params->ParamsLeftLine1 = 0;
    Params->ParamsLeftLine2 = 0;

    Params->DelayFactorLeftLine1 = 1;
    Params->DelayFactorLeftLine2 = 1;
    Params->DelayFactorRightLine1 = 1;
    Params->DelayFactorRightLine2 = 1;
    Params->DelayCommon = 1;

    Params->BLevelLeftLine1 = BScanGateLevel;
    Params->Gate1LevelLeftLine1 = EvaluationGateLevel;
    Params->Gate2LevelLeftLine1 = EvaluationGateLevel;
    Params->Gate3LevelLeftLine1 = EvaluationGateLevel;
    Params->BLevelLeftLine2 = BScanGateLevel;
    Params->Gate1LevelLeftLine2 = EvaluationGateLevel;
    Params->Gate2LevelLeftLine2 = EvaluationGateLevel;
    Params->Gate3LevelLeftLine2 = EvaluationGateLevel;
    Params->BLevelRightLine1 = BScanGateLevel;
    Params->Gate1LevelRightLine1 = EvaluationGateLevel;
    Params->Gate2LevelRightLine1 = EvaluationGateLevel;
    Params->Gate3LevelRightLine1 = EvaluationGateLevel;
    Params->BLevelRightLine2 = BScanGateLevel;
    Params->Gate1LevelRightLine2 = EvaluationGateLevel;
    Params->Gate2LevelRightLine2 = EvaluationGateLevel;
    Params->Gate3LevelRightLine2 = EvaluationGateLevel;
    Params->Duration = 0;

    Params->ACStartDelayRightLine1 = ACStartDelay;  // Старт подсчета суммы А-развертки для правой стороны, линия 1, мкс
    Params->ACStartDelayRightLine2 = ACStartDelay;  // Старт подсчета суммы А-развертки для правой стороны, линия 2, мкс
    Params->ACStartDelayLeftLine1 = ACStartDelay;   // Старт подсчета суммы А-развертки для левой стороны, линия 1, мкс
    Params->ACStartDelayLeftLine2 = ACStartDelay;   // Старт подсчета суммы А-развертки для левой стороны, линия 2, мкс

    memset(&Params->Reserv[0], 0xFF, 12);

    bool UseGenLeftLine1 = false;
    bool UseGenLeftLine2 = false;
    bool UseGenRightLine1 = false;
    bool UseGenRightLine2 = false;

    for (tHandChannelsList::iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
        sHandChannelDescription& currentHandChannel = it.operator*();
        if (currentHandChannel.Id == Channel) {
            int idx = UMUtoDEVList.size();
            UMUtoDEVList.resize(idx + 1);
            UMUtoDEVList[idx].UMUIndex = currentHandChannel.UMUIndex;
            UMUtoDEVList[idx].DEVSide = dsNone;
            UMUtoDEVList[idx].DEVChannel = currentHandChannel.Id;
            UMUtoDEVList[idx].UMUSide = currentHandChannel.Side;
            UMUtoDEVList[idx].UMULine = currentHandChannel.ReceiverLine;
            UMUtoDEVList[idx].UMUStroke = 0;

            bool BSFilterFlag = ((currentHandChannel.Id == H0E) || (currentHandChannel.Id == H0MS)) && FilteringBottomSignal;

            sChannelDescription ChanDesc;
            tbl->ItemByCID(Channel, &ChanDesc);

            if (ChanDesc.WorkFrequency == wf5MHz) {
                Params->WorkFrequencyL = 0x11;  // Рабочая частота (идентификатор) левой стороны
                Params->WorkFrequencyR = 0x11;  // Рабочая частота (идентификатор) правой стороны
            }
            else {
                Params->WorkFrequencyL = 0x00;  // Рабочая частота (идентификатор) левой стороны
                Params->WorkFrequencyR = 0x00;  // Рабочая частота (идентификатор) правой стороны
            }

            Params->Duration = ChanDesc.cdStrokeDuration;
            *UMUIndex = currentHandChannel.UMUIndex;
            switch (currentHandChannel.Side) {
            case usLeft: {
                switch (currentHandChannel.ReceiverLine) {
                case 0: {
                    Params->GenResLeftLine1 = currentHandChannel.Receiver;
                    Params->ZondAmplLeftLine1 = currentHandChannel.PulseAmpl;
                    Params->DelayFactorLeftLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine1;
                    Params->ParamsLeftLine1 = BSFilterFlag;
                    break;
                }
                case 1: {
                    Params->GenResLeftLine2 = currentHandChannel.Receiver;
                    Params->ZondAmplLeftLine2 = currentHandChannel.PulseAmpl;
                    Params->ParamsLeftLine2 = BSFilterFlag;
                    Params->DelayFactorLeftLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine2;
                    break;
                }
                }
                switch (currentHandChannel.GeneratorLine) {
                case 0: {
                    Params->GenResLeftLine1 = Params->GenResLeftLine1 | (currentHandChannel.Generator << 4);
                    Params->ZondAmplLeftLine1 = currentHandChannel.PulseAmpl;
                    Params->ParamsLeftLine1 = BSFilterFlag;
                    UseGenLeftLine1 = true;
                    Params->DelayFactorLeftLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine1;
                    break;
                }
                case 1: {
                    Params->GenResLeftLine2 = Params->GenResLeftLine2 | (currentHandChannel.Generator << 4);
                    Params->ZondAmplLeftLine2 = currentHandChannel.PulseAmpl;
                    Params->ParamsLeftLine2 = BSFilterFlag;
                    UseGenLeftLine2 = true;
                    Params->DelayFactorLeftLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine2;
                    break;
                }
                }
                break;
            }
            case usRight: {
                switch (currentHandChannel.ReceiverLine) {
                case 0: {
                    Params->GenResRightLine1 = currentHandChannel.Receiver;
                    Params->ZondAmplRightLine1 = currentHandChannel.PulseAmpl;
                    Params->ParamsRightLine1 = BSFilterFlag;
                    Params->DelayFactorRightLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine1;
                    break;
                }
                case 1: {
                    Params->GenResRightLine2 = currentHandChannel.Receiver;
                    Params->ZondAmplRightLine2 = currentHandChannel.PulseAmpl;
                    Params->ParamsRightLine2 = BSFilterFlag;
                    Params->DelayFactorRightLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine2;
                    break;
                }
                }
                switch (currentHandChannel.GeneratorLine) {
                case 0: {
                    Params->GenResRightLine1 = Params->GenResRightLine1 | (currentHandChannel.Generator << 4);
                    Params->ZondAmplRightLine1 = currentHandChannel.PulseAmpl;
                    Params->ParamsRightLine1 = BSFilterFlag;
                    UseGenRightLine1 = true;
                    Params->DelayFactorRightLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine1;
                    break;
                }
                case 1: {
                    Params->GenResRightLine2 = Params->GenResRightLine2 | (currentHandChannel.Generator << 4);
                    Params->ZondAmplRightLine2 = currentHandChannel.PulseAmpl;
                    Params->ParamsRightLine2 = BSFilterFlag;
                    UseGenRightLine2 = true;
                    Params->DelayFactorRightLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine2;
                    break;
                }
                }
                break;
            }
            }
            currentHandChannel.Active = true;
            //  break;
        }
        else {
            currentHandChannel.Active = false;
        }
    }

    if (!UseGenLeftLine1) Params->GenResLeftLine1 = Params->GenResLeftLine1 | 0xF0;
    if (!UseGenLeftLine2) Params->GenResLeftLine2 = Params->GenResLeftLine2 | 0xF0;
    if (!UseGenRightLine1) Params->GenResRightLine1 = Params->GenResRightLine1 | 0xF0;
    if (!UseGenRightLine2) Params->GenResRightLine2 = Params->GenResRightLine2 | 0xF0;
}

int cDeviceConfig::GetHandChannelsCount() const
{
    return HandChannels.size();
}

int cDeviceConfig::GetScanStrokeCount() const
{
    int MaxStrokeIndex = -1;
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& cur = it.operator*();
        if ((cur.UMUIndex == FUMUIndex) && (cur.StrokeGroupIdx == FGroupIndex)) {
            DEFCORE_ASSERT(cur.StrokeNumber < MaxStrokeCount);
            MaxStrokeIndex = std::max(MaxStrokeIndex, static_cast<int>(cur.StrokeNumber));
        }
    }
    return MaxStrokeIndex + 1;
}

void cDeviceConfig::ResetUsedList()
{
    for (tScanChannelsList::iterator it = ScanChannels.begin(); it != ScanChannels.end(); ++it) {
        it.operator*().Used = false;
    }
}

void cDeviceConfig::resetUMUtoDEVList()
{
    UMUtoDEVList.clear();
}

void cDeviceConfig::GetScanChannelsStrokeParams(int StrokeIndex, tStrokeParams* Params, CID OneChannelId)
{
    DEFCORE_ASSERT(StrokeIndex < MaxStrokeCount);
    DEFCORE_ASSERT(Params != NULL);
    DEFCORE_ASSERT(OneChannelId < 255);
    Params->StrokeIdx = StrokeIndex;
    // Изначально все генераторы выключенны
    Params->GenResLeftLine1 = 0x0;   // Генератор/приемник для левой стороны, линия 1
    Params->GenResLeftLine2 = 0x0;   // Генератор/приемник для левой стороны, линия 2
    Params->GenResRightLine1 = 0x0;  // Генератор/приемник для правой стороны, линия 1
    Params->GenResRightLine2 = 0x0;  // Генератор/приемник для правой стороны, линия 2

    bool GenResLeftLine1Flag = false;
    bool GenResLeftLine2Flag = false;
    bool GenResRightLine1Flag = false;
    bool GenResRightLine2Flag = false;

    Params->ZondAmplRightLine1 = 0;
    Params->ZondAmplRightLine2 = 0;
    Params->ZondAmplLeftLine1 = 0;
    Params->ZondAmplLeftLine2 = 0;
    Params->ParamsRightLine1 = 0;
    Params->ParamsRightLine2 = 0;
    Params->ParamsLeftLine1 = 0;
    Params->ParamsLeftLine2 = 0;

    Params->DelayFactorLeftLine1 = 1;
    Params->DelayFactorLeftLine2 = 1;
    Params->DelayFactorRightLine1 = 1;
    Params->DelayFactorRightLine2 = 1;
    Params->DelayCommon = 1;

    Params->BLevelLeftLine1 = BScanGateLevel;
    Params->Gate1LevelLeftLine1 = EvaluationGateLevel;
    Params->Gate2LevelLeftLine1 = EvaluationGateLevel;
    Params->Gate3LevelLeftLine1 = EvaluationGateLevel;
    Params->BLevelLeftLine2 = BScanGateLevel;
    Params->Gate1LevelLeftLine2 = EvaluationGateLevel;
    Params->Gate2LevelLeftLine2 = EvaluationGateLevel;
    Params->Gate3LevelLeftLine2 = EvaluationGateLevel;
    Params->BLevelRightLine1 = BScanGateLevel;
    Params->Gate1LevelRightLine1 = EvaluationGateLevel;
    Params->Gate2LevelRightLine1 = EvaluationGateLevel;
    Params->Gate3LevelRightLine1 = EvaluationGateLevel;
    Params->BLevelRightLine2 = BScanGateLevel;
    Params->Gate1LevelRightLine2 = EvaluationGateLevel;
    Params->Gate2LevelRightLine2 = EvaluationGateLevel;
    Params->Gate3LevelRightLine2 = EvaluationGateLevel;
    Params->Duration = 0;

    Params->ACStartDelayRightLine1 = ACStartDelay;  // Старт подсчета суммы А-развертки для правой стороны, линия 1, мкс
    Params->ACStartDelayRightLine2 = ACStartDelay;  // Старт подсчета суммы А-развертки для правой стороны, линия 2, мкс
    Params->ACStartDelayLeftLine1 = ACStartDelay;   // Старт подсчета суммы А-развертки для левой стороны, линия 1, мкс
    Params->ACStartDelayLeftLine2 = ACStartDelay;   // Старт подсчета суммы А-развертки для левой стороны, линия 2, мкс

    memset(&Params->Reserv[0], 0xFF, 12);

#ifdef DEBUG
    log_->Add(Format("StrokeIndex - %d", ARRAYOFCONST((StrokeIndex))));
#endif

    bool flg2 = false;

    for (tScanChannelsList::iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        sScanChannelDescription& cur = it.operator*();
        if ((cur.UMUIndex == FUMUIndex) && (cur.StrokeGroupIdx == FGroupIndex) && (cur.StrokeNumber == StrokeIndex)) {
            cur.Used = true;
            unsigned char SaveGenerator = cur.Generator;
            unsigned char SaveReceiver = cur.Receiver;
            unsigned char SaveGeneratorLine = cur.GeneratorLine;
            unsigned char SaveReceiverLine = cur.ReceiverLine;

            unsigned int idx = UMUtoDEVList.size();
            UMUtoDEVList.resize(idx + 1);
            UMUtoDEVList[idx].UMUIndex = cur.UMUIndex;
            UMUtoDEVList[idx].DEVSide = cur.DeviceSide;
            UMUtoDEVList[idx].DEVChannel = cur.Id;
            UMUtoDEVList[idx].UMUSide = cur.Side;
            UMUtoDEVList[idx].UMULine = cur.ReceiverLine;
            UMUtoDEVList[idx].UMUStroke = StrokeIndex;

            bool BSFilterFlag = (cur.Id == N0EMS_WP) || (cur.Id == N0EMS) || (cur.Id == N0EMS2);

            sChannelDescription ChanDesc;
            tbl->ItemByCID(cur.Id, &ChanDesc);

            if (ChanDesc.WorkFrequency == wf5MHz) {
                Params->WorkFrequencyL = 0x11;  // Рабочая частота (идентификатор) левой стороны
                Params->WorkFrequencyR = 0x11;  // Рабочая частота (идентификатор) правой стороны
            }
            else {
                Params->WorkFrequencyL = 0x00;  // Рабочая частота (идентификатор) левой стороны
                Params->WorkFrequencyR = 0x00;  // Рабочая частота (идентификатор) правой стороны
            }

            if ((OneChannelId != -1) && (cur.Id == OneChannelId) && (ChanDesc.cdMethod[0] == imMirror)) {
                switch (MirrorChannelTuningMethod) {
                case mctmByGenerator:
                    cur.Receiver = cur.Generator;
                    break;  // Если зеркальный метод контроля то Приемник = Генератору
                case mctmByReceiver:
                    cur.Generator = cur.Receiver;
                    break;  // Если зеркальный метод контроля то Генератор = Приемнику
                case mctmNotSet:
                    DEFCORE_ASSERT(false);
                    break;
                }
            }

            Params->Duration = std::max(Params->Duration, ChanDesc.cdStrokeDuration);
            switch (cur.Side) {
            case usLeft: {
                switch (cur.ReceiverLine)  // ReceiverLine 1
                {
                case 0: {
                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))  // TODO: redundant condition
                        Params->GenResLeftLine1 = Params->GenResLeftLine1 | cur.Receiver;
                    else
                        Params->GenResLeftLine1 = (0xF0 | cur.Receiver);
                    Params->ZondAmplLeftLine1 = cur.PulseAmpl;
                    Params->ParamsLeftLine1 = BSFilterFlag;
                    Params->DelayFactorLeftLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine1;

#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Left, Line: 1, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                case 1: {                                                                            // ReceiverLine 2
                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))  // TODO: redundant condition
                        Params->GenResLeftLine2 = Params->GenResLeftLine2 | cur.Receiver;
                    else
                        Params->GenResLeftLine2 = (0xF0 | cur.Receiver);

                    Params->ZondAmplLeftLine2 = cur.PulseAmpl;
                    Params->ParamsLeftLine2 = BSFilterFlag;
                    Params->DelayFactorLeftLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine2;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Left, Line: 2, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                }
                switch (cur.GeneratorLine)  // GeneratorLine 1
                {
                case 0: {
                    unsigned char tmp = cur.Generator;  // Если зеркальный метод контроля то Генератор = Приемнику
                                                        //  if ((OneChannelId != -1) && (ChanDesc.cdMethod[0] == imMirror)) tmp = cur.Receiver;

                    if (GeneratorOff) {
                        tmp = 0x0F;
                    }
                    GenResLeftLine1Flag = true;

                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResLeftLine1 = Params->GenResLeftLine1 | (tmp << 4);
                    else
                        Params->GenResLeftLine1 = (0xF0 | cur.Receiver);


                    Params->ZondAmplLeftLine1 = cur.PulseAmpl;
                    Params->ParamsLeftLine1 = BSFilterFlag;
                    Params->DelayFactorLeftLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine1;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Left, Line: 1, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                case 1: {                               // Line 2
                    unsigned char tmp = cur.Generator;  // Если зеркальный метод контроля то Генератор = Приемнику
                                                        //   if ((OneChannelId != -1) && (ChanDesc.cdMethod[0] == imMirror)) tmp = cur.Receiver;

                    if (GeneratorOff) {
                        tmp = 0x0F;
                    }
                    GenResLeftLine2Flag = true;

                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResLeftLine2 = Params->GenResLeftLine2 | (tmp << 4);
                    else
                        Params->GenResLeftLine2 = (0xF0 | cur.Receiver);

                    Params->ZondAmplLeftLine2 = cur.PulseAmpl;
                    Params->ParamsLeftLine2 = BSFilterFlag;
                    Params->DelayFactorLeftLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorLeftLine2;

#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Left, Line: 2, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                }
                break;
            }
            case usRight: {
                switch (cur.ReceiverLine) {
                case 0: {
                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResRightLine1 = Params->GenResRightLine1 | cur.Receiver;
                    else
                        Params->GenResRightLine1 = (0xF0 | cur.Receiver);
                    Params->ZondAmplRightLine1 = cur.PulseAmpl;
                    Params->ParamsRightLine1 = BSFilterFlag;
                    Params->DelayFactorRightLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine1;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Right, Line: 1, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                case 1: {
                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResRightLine2 = Params->GenResRightLine2 | cur.Receiver;
                    else
                        Params->GenResRightLine2 = (0xF0 | cur.Receiver);
                    Params->ZondAmplRightLine2 = cur.PulseAmpl;
                    Params->ParamsRightLine2 = BSFilterFlag;
                    Params->DelayFactorRightLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine2;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Right, Line: 2, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                }
                switch (cur.GeneratorLine) {
                case 0: {
                    unsigned char tmp = cur.Generator;  // Если зеркальный метод контроля то Генератор = Приемнику
                                                        //   if ((OneChannelId != -1) && (ChanDesc.cdMethod[0] == imMirror)) tmp = cur.Receiver;

                    if (GeneratorOff) {
                        tmp = 0x0F;
                    }
                    GenResRightLine1Flag = true;

                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResRightLine1 = Params->GenResRightLine1 | (tmp << 4);
                    else
                        Params->GenResRightLine1 = (0xF0 | cur.Receiver);
                    Params->ZondAmplRightLine1 = cur.PulseAmpl;
                    Params->ParamsRightLine1 = BSFilterFlag;
                    Params->DelayFactorRightLine1 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine1;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Right, Line: 1, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                case 1: {
                    unsigned char tmp = cur.Generator;  // Если зеркальный метод контроля то Генератор = Приемнику
                                                        //   if ((OneChannelId != -1) && (ChanDesc.cdMethod[0] == imMirror)) tmp = cur.Receiver;

                    if (GeneratorOff) {
                        tmp = 0x0F;
                    }
                    GenResRightLine2Flag = true;

                    if ((OneChannelId == -1) || ((OneChannelId != -1) && (cur.Id == OneChannelId)))
                        Params->GenResRightLine2 = Params->GenResRightLine2 | (tmp << 4);
                    else
                        Params->GenResRightLine2 = (0xF0 | cur.Receiver);
                    Params->ZondAmplRightLine2 = cur.PulseAmpl;
                    Params->ParamsRightLine2 = BSFilterFlag;
                    Params->DelayFactorRightLine2 = ChanDesc.cdBScanDelayMultiply;
                    Params->DelayCommon = Params->DelayFactorRightLine2;
#ifdef DEBUG
                    log_->Add(Format("CID: %x, Side: Right, Line: 2, Gen: %d; Rec: %d", ARRAYOFCONST((cur.Id, cur.Generator, cur.Receiver))));
#endif
                    break;
                }
                }
                break;
            }
            }

            cur.Generator = SaveGenerator;
            cur.Receiver = SaveReceiver;
            cur.GeneratorLine = SaveGeneratorLine;
            cur.ReceiverLine = SaveReceiverLine;

            if (flg2) {
                break;
            }
        }
    }

    if (OneChannelId == -1) {
        if (!GenResLeftLine1Flag) {
            Params->GenResLeftLine1 = Params->GenResLeftLine1 | 0xF0;
        }  // Генератор/приемник для левой стороны, линия 1
        if (!GenResLeftLine2Flag) {
            Params->GenResLeftLine2 = Params->GenResLeftLine2 | 0xF0;
        }  // Генератор/приемник для левой стороны, линия 2
        if (!GenResRightLine1Flag) {
            Params->GenResRightLine1 = Params->GenResRightLine1 | 0xF0;
        }  // Генератор/приемник для правой стороны, линия 1
        if (!GenResRightLine2Flag) {
            Params->GenResRightLine2 = Params->GenResRightLine2 | 0xF0;
        }  // Генератор/приемник для правой стороны, линия 2
    }
}


size_t cDeviceConfig::GetAllScanChannelsCount() const
{
    return ScanChannels.size();
}

unsigned int cDeviceConfig::GetUMUCount() const
{
    return UMUCount;
}

unsigned int cDeviceConfig::GetScanChannelsCount_() const
{
    return std::count_if(ScanChannels.begin(), ScanChannels.end(), StrokeGroupSearchFunctor(FGroupIndex));
}

int cDeviceConfig::GetScanChannelIndex_(int Idx)
{
    tScanChannelsList::iterator it = find_if(ScanChannels.begin(), ScanChannels.end(), StrokeGroupCounterFunctor(FGroupIndex, Idx));
    if (it != ScanChannels.end()) {
        return it - ScanChannels.begin();
    }

    return -1;
}

CID cDeviceConfig::GetScanChannel(int Index) const
{
    return ScanChannels[Index].Id;
}

bool cDeviceConfig::GetChannelData(eDeviceSide Side, CID Channel, tChannelData* Data)
{
    if (tbl->isHandScan(Channel))  // Для каналов ручного контроля
    {
        for (tHandChannelsList::const_iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
            const sHandChannelDescription& curHandChannel = it.operator*();
            if                                 /*((HandChannels[i].UMUIndex == FUMUIndex) ||
                                               (HandChannels[i].StrokeGroupIdx == FGroupIndex) || */
                (curHandChannel.Id == Channel) /*)*/
            {
                Data->UMUIndex = curHandChannel.UMUIndex;      // Номер БУМ
                Data->Side = curHandChannel.Side;              // Сторона БУМ
                Data->ResLine = curHandChannel.ReceiverLine;   // Номер линии приемника БУМ
                Data->GenLine = curHandChannel.GeneratorLine;  // Номер линии генератора БУМ
                Data->Stroke = 0;                              // Номер такта
                return true;
            }
        }
    }
    else
        for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
            const sScanChannelDescription& cur = it.operator*();
            if ((cur.DeviceSide == Side) &&
                //    (cur.UMUIndex == FUMUIndex) &&
                (cur.StrokeGroupIdx == FGroupIndex) && (cur.Id == Channel)) {
                Data->UMUIndex = cur.UMUIndex;      // Номер БУМ
                Data->Side = cur.Side;              // Сторона БУМ
                Data->ResLine = cur.ReceiverLine;   // Номер линии приемника БУМ
                Data->GenLine = cur.GeneratorLine;  // Номер линии генератора БУМ
                Data->Stroke = cur.StrokeNumber;    // Номер такта
                return true;
            }
        }
    return false;
}

int cDeviceConfig::GetStrokeIndexByCID(eDeviceSide Side, CID Channel)
{
    if (tbl->isHandScan(Channel))  // Для каналов ручного контроля
    {
        return 0;
    }
    else
        for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
            const sScanChannelDescription& cur = it.operator*();
            if ((cur.DeviceSide == Side) && (cur.UMUIndex == FUMUIndex) && (cur.StrokeGroupIdx == FGroupIndex) && (cur.Id == Channel)) return cur.StrokeNumber;  // Номер такта
        }
    return -1;
}

bool cDeviceConfig::GetChannelBySLS(eChannelType ChType, int UMUIdx, eUMUSide Side, int ReceiverLine, int StrokeIndex, eDeviceSide* Side_, CID* Channel) const
{
    if (ChType == ctHandScan)  // Для каналов ручного контроля
    {
        for (tHandChannelsList::const_iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
            const sHandChannelDescription& curHandChannel = it.operator*();
            if ((curHandChannel.UMUIndex == UMUIdx) && (curHandChannel.Side == Side) && (curHandChannel.ReceiverLine == ReceiverLine) && (curHandChannel.Active)) {
                *Side_ = dsNone;               // Сторона дефектоскопа
                *Channel = curHandChannel.Id;  // id канала дефектоскопа
                return true;
            }
        }
    }
    else {
        for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
            const sScanChannelDescription& curScanChannel = it.operator*();
            if ((curScanChannel.UMUIndex == UMUIdx) && ((curScanChannel.StrokeNumber == StrokeIndex)) && (curScanChannel.StrokeGroupIdx == FGroupIndex) && (curScanChannel.Side == Side) && (curScanChannel.ReceiverLine == ReceiverLine)) {
                *Side_ = curScanChannel.DeviceSide;  // Сторона дефектоскопа
                *Channel = curScanChannel.Id;        // id канала дефектоскопа
                return true;
            }
        }
    }
    return false;
}

void cDeviceConfig::GetSLSByChannel(eChannelType ChType, eDeviceSide Side_, CID Channel, int* UMUIdx, eUMUSide* Side, int* Line, int* StrokeIndex) const
{
    if (ChType == ctHandScan)  // Для каналов ручного контроля
    {
        for (tHandChannelsList::const_iterator it = HandChannels.begin(), total = HandChannels.end(); it != total; ++it) {
            const sHandChannelDescription& cur = it.operator*();
            if ((Side_ == dsNone) && (Channel == cur.Id) && (cur.Active)) {
                *UMUIdx = cur.UMUIndex;
                *StrokeIndex = 0;
                *Side = cur.Side;
                *Line = cur.ReceiverLine;
                return;
            }
        }
    }
    else {  // Для каналов сплошного контроля
        for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
            const sScanChannelDescription& cur = it.operator*();
            if ((Side_ == cur.DeviceSide) && (Channel == cur.Id)) {
                *UMUIdx = cur.UMUIndex;
                *StrokeIndex = cur.StrokeNumber;
                *Side = cur.Side;
                *Line = cur.ReceiverLine;
                return;
            }
        }
    }
}

void cDeviceConfig::SetBScanGateLevel(int BScanGateLevel_)  // Установка уровеня строба В-развертки [отсчетов]
{
    BScanGateLevel = BScanGateLevel_;
}

bool cDeviceConfig::ReceiverStateByCID(CID id, bool state)
{
    for (tScanChannelsList::iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        sScanChannelDescription& cur = it.operator*();
        if (id == cur.Id) {
            cur.ReceiverState = state;
            return true;
        }
    }
    return false;
}

bool cDeviceConfig::GetReceiverStateByCID(CID id) const
{
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& cur = it.operator*();
        if (id == cur.Id) {
            return cur.ReceiverState;
        }
    }
    return false;
}

bool cDeviceConfig::SetPulseAmplByCID(CID id, int Val)
{
    for (tScanChannelsList::iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        sScanChannelDescription& cur = it.operator*();
        if (id == cur.Id) {
            cur.PulseAmpl = Val;
            return true;
        }
    }
    return false;
}

int cDeviceConfig::GetPulseAmplByCID(CID id) const
{
    for (tScanChannelsList::const_iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        const sScanChannelDescription& cur = it.operator*();
        if (id == cur.Id) {
            return cur.PulseAmpl;
        }
    }
    return false;
}

int cDeviceConfig::GetControlModeCount() const
{
    return ModeList.size();
}

sDeviceModeData cDeviceConfig::GetControlModeItem(int Idx) const
{
    assert(Idx < GetControlModeCount());

    if (Idx < GetControlModeCount()) {
        return ModeList[Idx];
    };

    return sDeviceModeData();
}

size_t cDeviceConfig::GetRailTypeTuningCount() const
{
    return RailTypeTuning.size();
}

sRailTypeTuningGroup cDeviceConfig::GetRailTypeTuningItem(int Idx) const
{
    assert(Idx < static_cast<int>(GetRailTypeTuningCount()));

    if (Idx < static_cast<int>(GetRailTypeTuningCount())) {
        return RailTypeTuning[Idx];
    }
    else {
        sRailTypeTuningGroup ret;
        ret.MasterId = ChannelNotSet;
        ret.Rails = crBoth;
        ret.ChType = ctCompInsp;
        return ret;
    }
}

eProtocol cDeviceConfig::getProtocolForUmuNum(unsigned int umu_id) const
{
    if (static_cast<size_t>(umu_id) >= umuDescriptions.size()) {
        return eProtUNDEF;
    }

    return umuDescriptions.at(umu_id).protocol;
}

cISocket::cConnectionParams* cDeviceConfig::getConnectionParamsForUmuNum(int umu_id) const
{
    if (static_cast<size_t>(umu_id) >= umuDescriptions.size()) {
        return (cISocket::cConnectionParams*) 0;
    }

    return umuDescriptions[umu_id].connectionParams;
}

const sBScanTape& cDeviceConfig::getTapeByIndex(int index) const
{
    static sBScanTape bScanTapeDefault;
    int groupIndex = GetCurrentChannelGroupIndex();
    if (groupIndex >= 0) {
        const tBScanTapesList& BScanTapeList = BScanTapesGroupList.at(groupIndex);
        if (index >= 0 && index < static_cast<int>(BScanTapeList.size())) {
            return BScanTapeList.at(index);
        }
    }
    return bScanTapeDefault;
}

bool cDeviceConfig::SetGenerator(eUMUSide Side, int GeneratorLine, int NewGenerator)
{
    for (tScanChannelsList::iterator it = ScanChannels.begin(), total = ScanChannels.end(); it != total; ++it) {
        sScanChannelDescription& cur = it.operator*();
        if ((cur.Side == Side) && (cur.GeneratorLine == GeneratorLine)) {
            cur.Generator = NewGenerator;
            return true;
        }
    }
    return false;
}

bool cDeviceConfig::SetReceiver(eUMUSide Side, int ReceiverLine, int NewReceiver)
{
    for (tScanChannelsList::iterator it = ScanChannels.begin(); it != ScanChannels.end(); ++it) {
        sScanChannelDescription& cur = it.operator*();
        if ((cur.Side == Side) && (cur.ReceiverLine == ReceiverLine)) {
            cur.Receiver = NewReceiver;
            return true;
        }
    }
    return false;
}

bool cDeviceConfig::SetFaceData(tTrolleySide Side, cDeviceCalibration* Cal)  // Установка соответствия рабочих/нерабочих граней
{
    if (StrokeGenRecFaceData.size() == 0) return false;

    size_t faceDataSize = StrokeGenRecFaceData.size();
    for (size_t i = 0; i < faceDataSize; ++i) {
        const tStrokeGenRecFaceDataItem& cur = StrokeGenRecFaceData[i];
        int tmp = 0;
        CID id1 = cur.cid[LeftSideTrolley_WorkSide];
        CID id2 = cur.cid[LeftSideTrolley_UnWorkSide];
        eDeviceSide side = cur.DeviceSide;

        tmp = Cal->GetSens(side, id1, 0);  // Условная чувствительность [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        Cal->SetSens(side, id1, 0, Cal->GetSens(side, id2, 0));
        Cal->SetSens(side, id2, 0, tmp);

        tmp = Cal->GetGain(side, id1, 0);  // Аттенюатор [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        Cal->SetGain(side, id1, 0, Cal->GetGain(side, id2, 0));
        Cal->SetGain(side, id2, 0, tmp);

        tmp = Cal->GetStGate(side, id1, 0);  // Начало строб АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        Cal->SetStGate(side, id1, 0, Cal->GetStGate(side, id2, 0));
        Cal->SetStGate(side, id2, 0, tmp);

        tmp = Cal->GetEdGate(side, id1, 0);  // Конец строба АСД [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        Cal->SetEdGate(side, id1, 0, Cal->GetStGate(side, id2, 0));
        Cal->SetEdGate(side, id2, 0, tmp);

        tmp = Cal->GetPrismDelay(side, id1);  // Время в призме [10 * мкс]
        Cal->SetPrismDelay(side, id1, Cal->GetStGate(side, id2, 0));
        Cal->SetPrismDelay(side, id2, tmp);

        tmp = Cal->GetTVG(side, id1);  // ВРЧ [мкс]
        Cal->SetTVG(side, id1, Cal->GetStGate(side, id2, 0));
        Cal->SetTVG(side, id2, tmp);

        sScanChannelDescription& desc = ScanChannels[cur.ScanChannelIndex];
        desc.GeneratorLine = cur.GeneratorLine[Side];
        desc.ReceiverLine = cur.ReceiverLine[Side];
        desc.Generator = cur.Generator[Side];
        desc.Receiver = cur.Receiver[Side];
    }
    return true;
}

bool cDeviceConfig::IsCopySlaveChannel(eDeviceSide Side, CID Channel)
{
	for (tChannelCalibrationCopyData::iterator it = CalibrationCopyData.begin(), total = CalibrationCopyData.end(); it != total; ++it) {
		if ((it->DeviceSide == Side) && (it->Slave == Channel)) {
			return true;
		}
	}
	return false;
}

char* cDeviceConfig::GetDeviceName(void) {

	return DeviceName;
}

