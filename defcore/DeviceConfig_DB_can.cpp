#include "Definitions.h"
#include "ChannelsTable.h"
#include "ChannelsIds.h"
#include "DeviceConfig_DB_can.h"
#include "datatransfers/datatransfer_can.h"

cDeviceConfig_DB_can::cDeviceConfig_DB_can(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    // Заполнить _umu_by_protocols
    UMUCount = 1;
    DeviceName = "Avicon-31";
    MaxControlSpeed = 2;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crBoth;                     // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    CoordSys = csMetric1km;                      // Система отсчета путейской координаты - Metric_1km
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 0.2;                         // Минимально допустимое время в призме - 2 мкс
    PrismDelayMax = 100;                         // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/ bScanThreshold;       // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/ aScanThreshold;  // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тит рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    UseLineSwitch = true;               // Реле для подключения линий ручного контроля - ВКЛ
    UMUConnectors = toCompleteControl;  // Используемые разъемы БУМ - сплошного контроля
    MainPathEncoderIndex = 0;
    MirrorChannelTuningMethod = mctmByReceiver;  // Метод настройки зеркальных каналов

    sUmuDescription umuDescription;
    umuDescription.protocol = eProtPADCAN;
    cDataTransferCanRaw::cCanConnectionParams* connectionParams = new (std::nothrow) cDataTransferCanRaw::cCanConnectionParams;
    connectionParams->_can_interface_num = 0;
    umuDescription.connectionParams = connectionParams;
    umuDescriptions.push_back(umuDescription);

    // === Список каналов ручного контроля ===
    const int handChannelsCount = 7;
    HandChannels.reserve(handChannelsCount);
    sHandChannelDescription HScanCh;
    HScanCh.UMUIndex = 0;
    HScanCh.GeneratorLine = 0;
    HScanCh.ReceiverLine = 0;
    HScanCh.Generator = 0;
    HScanCh.Receiver = 0;
    HScanCh.PulseAmpl = 7;
    HScanCh.Side = usLeft;
    HScanCh.Id = H0E_UIC60;
    HScanCh.Active = false;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0MS_UIC60;
    HandChannels.push_back(HScanCh);

    // HScanCh.Side = usRight;
    HScanCh.Id = H45;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H50;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H58;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H65;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H70;
    HandChannels.push_back(HScanCh);

    // === Список каналов сплошного контроля ===

    // Список каналов

    ChannelsList.reserve(HandChannels.size());

    for (size_t i = 0; i < HandChannels.size(); i++) {
        ChannelsList.push_back(HandChannels[i].Id);
    }

    // Инициализация Таблицы стробов для настройки каналов
    sSensTuningParam par;
    sChannelDescription chdesc;
    for (size_t index = 0; index < ChannelsList.size(); index++) {
        tbl->ItemByCID(ChannelsList[index], &chdesc);
        for (int GateIdx = 0; GateIdx < chdesc.cdGateCount; GateIdx++) {
            par.id = ChannelsList[index];
            par.GateIndex = GateIdx;
            // Для Настройки Ку
            if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] == imMirrorShadow))  // Прямой ввод ЗТМ
            {
                par.SensTuningGate[1].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 178);
                par.SensTuningGate[1].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 186);
            }
            else if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] != imMirrorShadow))  // Прямой ввод ЭХО
            {
                par.SensTuningGate[0].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                par.SensTuningGate[0].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 41 + 7);
            }
            else if (chdesc.cdEnterAngle < 65)  // Наклонный но не 65, 70-ти градусные
            {
                par.SensTuningGate[0].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                par.SensTuningGate[0].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 42 + 4);
                par.SensTuningGate[1] = par.SensTuningGate[0];
            }
            else  // 65, 70-ти градусные
            {
                par.SensTuningGate[0].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                par.SensTuningGate[0].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 14 + 4);
                par.SensTuningGate[1] = par.SensTuningGate[0];
            }

            // Для Настройки 2ТП
            /*    if (chdesc.cdEnterAngle != 0) { // Прямой ввод
                      par.PrismTuningGate.gStart = 34 - 6;
                      par.PrismTuningGate.gEnd = 39 + 6;
                  }
                  else if (chdesc.cdEnterAngle == 0) {
                      par.PrismTuningGate.gStart = 19 - 2;
                      par.PrismTuningGate.gEnd = 21 + 2;
                  } */

            // Для Настройки 2ТП
            if (chdesc.cdEnterAngle != 0) {
                par.PrismTuningGate[0].gStart = 36 - 10;
                par.PrismTuningGate[0].gEnd = 36 + 10;
                par.PrismTuningGate[1].gStart = 36 - 10;
                par.PrismTuningGate[1].gEnd = 36 + 10;
            }
            else {  // Прямой ввод
                par.PrismTuningGate[0].gStart = 20 - 10;
                par.PrismTuningGate[0].gEnd = 20 + 10;
                par.PrismTuningGate[1].gStart = 20 - 10;
                par.PrismTuningGate[1].gEnd = 20 + 10;
            }

            SensTuningParams.push_back(par);
        }
    }

    // -----------------------------------------------------
    // Режим "Тип рельса" - для ручных
    // -----------------------------------------------------

    sRailTypeTuningGroup g1;         // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    sRailTypeTuningChannelAction a;  // Действие для одного канала при настройке на тип рельса
    g1.Rails = crSingle;             // Одна нить
    g1.MasterId = H0E;               // Эхо
    g1.ChType = ctHandScan;          // Каналы ручного контроля

    /*                         // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.id = H0MS;         // 0 град
    a.StrobeIndex = 1;   // ЗТМ
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = - 2;       // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.ValueType = rtt_mcs; // мкс
    a.SkipTestGateLen = true; // игнорировать значение минимальной протяженности строба
    g1.List.push_back(a);
                         // конец «0 ЗТМ»  = Tдс +6 мкс;
    a.id = H0MS;         // 0 град
    a.StrobeIndex = 1;   // ЗТМ
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = + 6;       // начало «0 ЗТМ»  =  Tдс +6 мкс;
    a.ValueType = rtt_mcs; // мкс
    a.SkipTestGateLen = true; // игнорировать значение минимальной протяженности строба
    g1.List.push_back(a);
*/                              // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
    a.id = H0E;                 // 0 град
    a.StrobeIndex = 1;          // ЭХО
    a.Action = maMoveEndGate;   // Изменение конца строба
    a.Delta = -2;               // Tдс – 2 мкс;
    a.ValueType = rtt_mcs;      // мкс
    a.SkipTestGateLen = false;  // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;
    g1.List.push_back(a);

    RailTypeTuning.push_back(g1);

    // -----------------------------------------------------

    sRailTypeTuningGroup g2;  // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    g2.MasterId = H0MS;       // ЗТМ
    g2.Rails = crSingle;      // Одна нить
    g2.ChType = ctHandScan;   // Каналы ручного контроля

    // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.id = H0MS;                 // 0 град
    a.StrobeIndex = 1;           // ЗТМ
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -2;                // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.ValueType = rtt_mcs;       // мкс
    a.SkipTestGateLen = true;    // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g2.List.push_back(a);
    // конец «0 ЗТМ»  = Tдс +6 мкс;
    a.id = H0MS;               // 0 град
    a.StrobeIndex = 1;         // ЗТМ
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = +6;              // начало «0 ЗТМ»  =  Tдс +6 мкс;
    a.ValueType = rtt_mcs;     // мкс
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g2.List.push_back(a);
    /*                         // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
        a.id = H0E;          // 0 град
        a.StrobeIndex = 1;   // ЭХО
        a.Action = maMoveEndGate;  // Изменение конца строба
        a.Delta = - 3;       // Tдс – 3 мкс;
        a.ValueType = rtt_mcs; // мкс
        a.SkipTestGateLen = false; // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
        a.ActiveInTuning = false;
        g2.List.push_back(a);
    */
    RailTypeTuning.push_back(g2);

    GroupIndexList.push_back(1);
    // GroupIndexList.push_back(2);
}

cDeviceConfig_DB_can::~cDeviceConfig_DB_can()
{
    for (size_t i = 0; i < ModeList.size(); ++i) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_DB_can::dBGain_to_UMUGain(char dBGain)
{
    return static_cast<unsigned char>(GainBase + dBGain / GainStep);
}
