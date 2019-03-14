#include "Definitions.h"
#include "ChannelsTable.h"
#include "DeviceConfig_HeadScan_Test.h"
#include "datatransfers/datatransfer_lan.h"

cDeviceConfig_HeadScan_Test::cDeviceConfig_HeadScan_Test(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold): cDeviceConfig(channelsTable)
{
//    DeviceID = dcHeadScaner_Test;
    UMUCount = 1;
    DeviceName = "HeadScan Test";
    MaxControlSpeed = 0;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crSingle;                   // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 2;                          // Минимально допустимое время в призме - 2 мкс
    PrismDelayMax = 10;                         // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/bScanThreshold;        // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/aScanThreshold;   // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тип рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = false;                 // фильтрация В-развертки - ВЫКЛЮЧЕНА
    UMUConnectors = toScaner;                    // Используемые разъемы БУМ - сканера контроля головки
    MainPathEncoderIndex = 1;
    MirrorChannelTuningMethod = mctmByReceiver; // Метод настройки зеркальных каналов

    sUmuDescription umuDescription;
    umuDescription.protocol = eProtLAN;
    cDataTransferLan::cLanConnectionParams * connectionParams = new (std::nothrow) cDataTransferLan::cLanConnectionParams;
    ::strcpy(connectionParams->_remote_ip, "192.168.100.100");
    ::strcpy(connectionParams->_local_ip, "192.168.100.1");
    connectionParams->_port_in = 43001;
    connectionParams->_port_out = 43000;
    connectionParams->_port_in_tcp = false;
    connectionParams->_port_out_tcp = true;
    umuDescription.connectionParams = connectionParams;
    umuDescriptions.push_back(umuDescription);

    // === Список каналов ручного контроля ===
    const int handChannelsCount = 0;
/*    HandChannels.reserve(handChannelsCount);
    sHandChannelDescription HScanCh;
    HScanCh.UMUIndex = 0;
    HScanCh.ReceiverLine = ulRU2;
    HScanCh.GeneratorLine = ulRU2;
    HScanCh.Generator = 7;
    HScanCh.Receiver = 7;
    HScanCh.PulseAmpl = 7;
    HScanCh.Side = usLeft;

    HScanCh.Id = H0MS;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0E;
    HandChannels.push_back(HScanCh);

    HScanCh.Side = usRight;
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
*/

    // === Список каналов сплошного контроля ===
    const int scanChannelsCount = 6;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    ScanCh.ColorDescr = cdNotSet; // Идентификатор цвета не используется
    ScanCh.UMUIndex = 0;
    ScanCh.PulseAmpl = 6;      // Амплитуда зондирующего импульса

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1; // Номер группы в которую входит данный канал (Схема прозвучивания №...)
    ScanCh.Side = usLeft;      // Сторона БУМ
    ScanCh.DeviceSide = dsNone;// Сторона дефектоскопа

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;   // Номер такта

//    ScanCh.BScanTape = 0;      // Номер полосы В-развертки
    ScanCh.Id = 0xB2;         // Идентификатор канала - [Отъезжающий; 0 град; Теневой]
    ScanCh.GeneratorLine = ulRU1;
    ScanCh.ReceiverLine = ulRU1;
    ScanCh.Generator = 5;      // Номер генератора
    ScanCh.Receiver = 5;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

//    ScanCh.BScanTape = 0;      // Номер полосы В-развертки
    ScanCh.Id = 0xB3;          // Идентификатор канала - [Наезжающий; 45 град; Эхо]
    ScanCh.GeneratorLine = ulRU2;
    ScanCh.ReceiverLine = ulRU2;
    ScanCh.Generator = 5;      // Номер генератора
    ScanCh.Receiver = 5;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;   // Номер такта

    ScanCh.Id = 0xB4;          // Идентификатор канала - [Отъезжающий; 45 град; Эхо]
    ScanCh.GeneratorLine = ulRU1;
    ScanCh.ReceiverLine = ulRU1;
    ScanCh.Generator = 4;      // Номер генератора
    ScanCh.Receiver = 4;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;      // Номер полосы В-развертки
    ScanCh.Id = 0xB5;         // Идентификатор канала - [Наезжающий; 65 град; Эхо; Нерабочая грань]
    ScanCh.GeneratorLine = ulRU2;
    ScanCh.ReceiverLine = ulRU2;
    ScanCh.Generator = 4;      // Номер генератора
    ScanCh.Receiver = 4;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;   // Номер такта

    ScanCh.BScanTape = 2;      // Номер полосы В-развертки
    ScanCh.Id = 0xB6;         // Идентификатор канала - [Отъезжающий; 65 град; Эхо; Нерабочая грань]
    ScanCh.GeneratorLine = ulRU1;
    ScanCh.ReceiverLine = ulRU1;
    ScanCh.Generator = 6;      // Номер генератора
    ScanCh.Receiver = 6;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 0;      // Номер полосы В-развертки
    ScanCh.Id = 0xB7;         // Идентификатор канала - [Наезжающий; 65 град; Эхо; Рабочая грань]
    ScanCh.GeneratorLine = ulRU2;
    ScanCh.ReceiverLine = ulRU2;
    ScanCh.Generator = 6;      // Номер генератора
    ScanCh.Receiver = 6;       // Номер приемника
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    // Список каналов
    ChannelsList.reserve(ScanChannels.size() + HandChannels.size());
    for (size_t i = 0; i < ScanChannels.size(); i++) {
        ChannelsList.push_back(ScanChannels[i].Id);
    }
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
            if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] == imMirrorShadow))      // Прямой ввод ЗТМ
            {
                par.SensTuningGate[1].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 178);
                par.SensTuningGate[1].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 186);
            }
            else
            if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] != imMirrorShadow))      // Прямой ввод ЭХО
            {
                par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 41 + 7);
            }
            else
            if (chdesc.cdEnterAngle < 65)     // Наклонный но не 65, 70-ти градусные
            {
                par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 42 + 4);
                par.SensTuningGate[1] = par.SensTuningGate[0];
            }
            else                // 65, 70-ти градусные
            {
                par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 14 + 4);
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

            else if (chdesc.cdEnterAngle == 0) { // Прямой ввод

                par.PrismTuningGate[0].gStart = 20 - 10;
                par.PrismTuningGate[0].gEnd = 20 + 10;
                par.PrismTuningGate[1].gStart = 60 - 10;
                par.PrismTuningGate[1].gEnd = 60 + 10;
            }

            SensTuningParams.push_back(par);

        }
    }

    // --------------------------------

//    Для Романа

    for (int dBGain = 0; dBGain <= 80; dBGain++)  {
      UMUGain[dBGain] = GainBase + dBGain / GainStep;
    }

        for (int dBGain = 16; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        }

/*    for (int dBGain = 23; dBGain <= 80; dBGain++)  {
      UMUGain[dBGain] = UMUGain[dBGain] - 1;
    } */

        for (int dBGain = 31; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        }


        for (int dBGain = 40; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        }

/*        for (int dBGain = 54; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        } */

    for (int dBGain = 55; dBGain <= 80; dBGain++)  {
      UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

        for (int dBGain = 76; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        }

    for (int dBGain = 80; dBGain <= 80; dBGain++)  {
      UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    // --------------------------------

    // Список групп каналов
    GroupIndexList.push_back(1);

    // --------------------------------
    //
    sBScanTape tape;
    tBScanTapesList BScanTapesList;
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    BScanTapesGroupList.push_back(BScanTapesList);
}

cDeviceConfig_HeadScan_Test::~cDeviceConfig_HeadScan_Test(void)
{

    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_HeadScan_Test::dBGain_to_UMUGain(char dBGain)
{
    if ((dBGain >= 0) && (dBGain <= 80))
      return UMUGain[dBGain];
      else return 0;
}

