#include "Definitions.h"
#include "ChannelsTable.h"
#include "ChannelsIds.h"
#include "DeviceConfig_DB_lan.h"
#include "datatransfers/datatransfer_lan.h"

cDeviceConfig_DB_lan::cDeviceConfig_DB_lan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    UMUCount = 1;
    DeviceName = "DB";
    MaxControlSpeed = 100;                       // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crSingle;                   // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    CoordSys = csMetric1km;                      // Система отсчета путейской координаты - Metric_1km
    UseNotch = true;                             // "Полочеки" ВРЧ используются
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 0.2;                         // Минимально допустимое время в призме - 2 мкс
    PrismDelayMax = 100;                         // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/ bScanThreshold;       // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/ aScanThreshold;  // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тип рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    UMUConnectors = toCompleteControl;  // Используемые разъемы БУМ - сплошного контроля
    MainPathEncoderIndex = 0;
    MirrorChannelTuningMethod = mctmByReceiver;  // Метод настройки зеркальных каналов
    SendBottomSignal = true;
    sUmuDescription umuDescription;
    umuDescription.protocol = eProtLAN;
    cDataTransferLan::cLanConnectionParams* connectionParams = new (std::nothrow) cDataTransferLan::cLanConnectionParams;
#ifndef BUM_IMITATOR_MODE
    ::strcpy(connectionParams->_remote_ip, "192.168.100.100");
    ::strcpy(connectionParams->_local_ip, "192.168.100.1");
#else
    ::strcpy(connectionParams->_remote_ip, "127.0.0.1");
    ::strcpy(connectionParams->_local_ip, "127.0.0.1");
#endif
    connectionParams->_port_in = 43001;
    connectionParams->_port_out = 43000;
    connectionParams->_port_in_tcp = false;
    connectionParams->_port_out_tcp = true;
    umuDescription.connectionParams = connectionParams;
    umuDescriptions.push_back(umuDescription);

    // === Список каналов ручного контроля ===
    const int handChannelsCount = 7;
    HandChannels.reserve(handChannelsCount);
    sHandChannelDescription HScanCh;
    HScanCh.UMUIndex = 0;
    HScanCh.GeneratorLine = 0;
    HScanCh.ReceiverLine = 1;
    HScanCh.Generator = 1;
    HScanCh.Receiver = 7;
    HScanCh.PulseAmpl = 7;
    HScanCh.Side = usLeft;
    HScanCh.Active = false;
    //    HScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2.5 МГц
    HScanCh.Id = H0MS_UIC60;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0E_UIC60;
    HandChannels.push_back(HScanCh);

    HScanCh.Side = usLeft;
    HScanCh.GeneratorLine = 0;
    HScanCh.ReceiverLine = 0;
    HScanCh.Generator = 1;
    HScanCh.Receiver = 1;
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
    const int scanChannelsCount = 11;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    tStrokeGenRecFaceDataItem FaceData;  // Генераторы приемники для каналов контроля рабочих/нерабочих граней

    //    ScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2.5 МГц
    ScanCh.UMUIndex = 0;
    ScanCh.PulseAmpl = 6;  // Амплитуда зондирующего импульса

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1;   // Номер группы в которую входит данный канал (Схема прозвучивания №...)
    ScanCh.Side = usLeft;        // Сторона БУМ
    ScanCh.DeviceSide = dsNone;  // Сторона дефектоскопа

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;  // Номер такта

    ScanCh.BScanTape = 0;  // Номер полосы В-развертки
    ScanCh.Id = B65EW_WP;  // Идентификатор канала - [Отъезжающий; 65 град; Эхо; Рабочая грань]
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 2;    // Номер генератора
    ScanCh.Receiver = 2;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 0;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 0;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 2;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 2;
    FaceData.cid[LeftSideTrolley_WorkSide] = B65EW_WP;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 3;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 3;
    FaceData.cid[LeftSideTrolley_UnWorkSide] = B65EU_WP;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------

    ScanCh.BScanTape = 4;  // Номер полосы В-развертки
    ScanCh.Id = F45E_WP;   // Идентификатор канала - [Наезжающий; 45 град; Эхо]
    ScanCh.GeneratorLine = 1;
    ScanCh.ReceiverLine = 1;
    ScanCh.Generator = 1;    // Номер генератора
    ScanCh.Receiver = 1;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;  // Номер такта

    ScanCh.Id = B45E_WP;  // Идентификатор канала - [Отъезжающий; 45 град; Эхо]
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 0;    // Номер генератора
    ScanCh.Receiver = 0;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;  // Номер полосы В-развертки
    ScanCh.Id = F65EU_WP;  // Идентификатор канала - [Наезжающий; 65 град; Эхо; Нерабочая грань]
    ScanCh.GeneratorLine = 1;
    ScanCh.ReceiverLine = 1;
    ScanCh.Generator = 0;    // Номер генератора
    ScanCh.Receiver = 0;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 0;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 0;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 2;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 2;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;  // Номер такта

    ScanCh.BScanTape = 2;  // Номер полосы В-развертки
    ScanCh.Id = B65EU_WP;  // Идентификатор канала - [Отъезжающий; 65 град; Эхо; Нерабочая грань]
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 3;    // Номер генератора
    ScanCh.Receiver = 3;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 0;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 0;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 3;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 3;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 2;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 2;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------


    ScanCh.BScanTape = 0;  // Номер полосы В-развертки
    ScanCh.Id = F65EW_WP;  // Идентификатор канала - [Наезжающий; 65 град; Эхо; Рабочая грань]
    ScanCh.GeneratorLine = 1;
    ScanCh.ReceiverLine = 1;
    ScanCh.Generator = 2;    // Номер генератора
    ScanCh.Receiver = 2;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 2;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 2;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 0;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------

    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;  // Номер такта

    ScanCh.BScanTape = 1;  // Номер полосы В-развертки
    ScanCh.Id = F65E_WP;   // Идентификатор канала - [Наезжающий; 65 град; Эхо]
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 5;    // Номер генератора
    ScanCh.Receiver = 5;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 3;  // Номер полосы В-развертки
    ScanCh.Id = N55MSLW;   // Идентификатор канала - [Нет направления; 55/90 град; ЗТМ; ?; Рабочая грань]
    ScanCh.PulseAmpl = 1;  // Амплитуда зондирующего импульса
    ScanCh.GeneratorLine = 1;
    ScanCh.ReceiverLine = 1;
    ScanCh.Generator = 4;    // Номер генератора
    ScanCh.Receiver = 4;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdDirect1;
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 4;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 4;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 3;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 3;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------


    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;  // Номер такта
    ScanCh.BScanTape = 1;     // Номер полосы В-развертки
    ScanCh.Id = B65E_WP;      // Идентификатор канала - [Отъезжающий; 65 град; Эхо]
    ScanCh.PulseAmpl = 6;     // Амплитуда зондирующего импульса
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 4;    // Номер генератора
    ScanCh.Receiver = 4;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 3;  // Номер полосы В-развертки
    ScanCh.PulseAmpl = 1;  // Амплитуда зондирующего импульса
    ScanCh.Id = N55MSLU;   // Идентификатор канала - [Нет направления; 55/90 град; ЗТМ; ?; Нерабочая грань]
    ScanCh.GeneratorLine = 1;
    ScanCh.ReceiverLine = 1;
    ScanCh.Generator = 3;    // Номер генератора
    ScanCh.Receiver = 3;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdDirect1;
    ScanChannels.push_back(ScanCh);

    FaceData.ScanChannelIndex = ScanChannels.size() - 1;
    FaceData.GeneratorLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_WorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_WorkSide] = 3;
    FaceData.Receiver[LeftSideTrolley_WorkSide] = 3;
    FaceData.GeneratorLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.ReceiverLine[LeftSideTrolley_UnWorkSide] = 1;
    FaceData.Generator[LeftSideTrolley_UnWorkSide] = 4;
    FaceData.Receiver[LeftSideTrolley_UnWorkSide] = 4;
    FaceData.DeviceSide = dsNone;
    StrokeGenRecFaceData.push_back(FaceData);

    // ------------------------------------------------------------------------------

    // --- Такт №6 ---
    ScanCh.StrokeNumber = 5;  // Номер такта
    ScanCh.BScanTape = 5;     // Номер полосы В-развертки
    ScanCh.Id = N0EMS_WP;     // Идентификатор канала - [Нет направления; 0 град; Эхо+ЗТМ]
    ScanCh.PulseAmpl = 6;     // Амплитуда зондирующего импульса
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 7;    // Номер генератора
    ScanCh.Receiver = 7;     // Номер приемника
    ScanCh.ProbePos = 0;     // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
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
            else if ((chdesc.cdEnterAngle == 55) && (chdesc.cdMethod[GateIdx] == imMirrorShadow))  // Канал 55/90 ()
            {
                par.SensTuningGate[0].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 25);
                par.SensTuningGate[0].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 42);
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


    // Режимы работы

    sModeChannelData cd;
    ModeList.resize(1);

    // Режим "Болтовой стык" - для Схемы 1

    ModeList[0].id = cmTestBoltJoint;
    ModeList[0].StrokeGroupIdx = 1;  // Схема 1

    // Стробы

    cd.id = N0EMS_WP;  // 0 град эхо
    cd.Action = maEndGate_Set;
    cd.GateIdx = 1;  // Номер строба
    cd.Value = 18;   // мкс
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;             // Наезжающий; 45 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = B45E_WP;             // Отъезжающий; 45 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;            // Наезжающий; 45 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[0].List.push_back(cd);

    cd.id = B45E_WP;            // Отъезжающий; 45 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;              // Наезжающий; 45 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[0].List.push_back(cd);

    cd.id = B45E_WP;              // Отъезжающий; 45 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[0].List.push_back(cd);

    cd.id = B45E_WP;            // Отъезжающий; 45 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;            // Наезжающий; 45 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[0].List.push_back(cd);

    // Выкл АСД

    cd.id = F65E_WP;  // Наезжающий; 65 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = F65EW_WP;  // Наезжающий; 65 град; Рабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = F65EU_WP;  // Наезжающий; 65 град; Нерабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = B65E_WP;  // Отъезжающий; 65 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = B65EW_WP;  // Отъезжающий; 65 град; Рабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = B65EU_WP;  // Отъезжающий; 65 град; Нерабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    // Режим 2 эхо
    cd.id = B45E_WP;  // Отъезжающий; 45 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;  // Наезжающий; 45 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[0].List.push_back(cd);

    // Увеличение Ку

    cd.id = B45E_WP;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

/*
    // Копирование Ку из Ш в П

    cd.id = B45E_WP;  // Отъезжающий; 42 град
    cd.Action = maSens_Push;
    cd.GateIdx = 1;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = B45E_WP;  // Отъезжающий; 42 град
    cd.Action = maSens_Pop;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    // Копирование Ку из Ш в П

    cd.id = F45E_WP;  // Наезжающий; 42 град
    cd.Action = maSens_Push;
    cd.GateIdx = 1;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;  // Наезжающий; 42 град
    cd.Action = maSens_Pop;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    // Увеличение Ку

    cd.id = B45E_WP;  // Отъезжающий; 45 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = F45E_WP;  // Наезжающий; 45 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);
*/
    // -----------------------------------------------------
    // Режим "Тип рельса" - для всех схем сплошного контроля
    // -----------------------------------------------------

    sRailTypeTuningGroup g;          // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    sRailTypeTuningChannelAction a;  // Действие для одного канала при настройке на тип рельса
    g.Rails = crSingle;              // Две нити
    g.MasterId = N0EMS_WP;           // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    g.ChType = ctCompInsp;           // Каналы стплошного контроля

    // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.id = N0EMS_WP;                     // 0 град
    a.StrobeIndex = 2;                   // ЗТМ
    a.Action = maMoveStartGate;          // Изменение начала строба
    a.Delta = -2;                        // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.ValueType = rtt_mcs;               // мкс
    a.SkipTestGateLen = true /*false*/;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g.List.push_back(a);
    // конец «0 ЗТМ»  = Tдс +6 мкс;
    a.id = N0EMS_WP;           // 0 град
    a.StrobeIndex = 2;         // ЗТМ
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = +6;              // начало «0 ЗТМ»  =  Tдс +6 мкс;
    a.ValueType = rtt_mcs;     // мкс
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g.List.push_back(a);
    // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
    a.id = N0EMS_WP;                     // 0 град
    a.StrobeIndex = 1;                   // ЭХО
    a.Action = maMoveEndGate;            // Изменение конца строба
    a.Delta = -3;                        // Tдс – 3 мкс;
    a.ValueType = rtt_mcs;               // мкс
    a.SkipTestGateLen = true /*false*/;  // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;            // Настройка на Тип рельса в режима Настройка не влияет на этот строб
    g.List.push_back(a);

    // начало «Наезжающий 45 П»  =  (Нр(мм)-30(мм))/2,95;
    a.id = F45E_WP;              // 45 град
    a.StrobeIndex = 2;           // П
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -30;               // 30 мм
    a.ValueType = rtt_mm;        // mm
    a.SkipTestGateLen = false;   // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // конец «Наезжающий 45 Ш»  = начало « 42 П»-1 мкс ?????
    a.id = F45E_WP;            // 45 град
    a.StrobeIndex = 1;         // Ш
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = -30;             // 30 мм
    a.ValueType = rtt_mm;      // mm
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // начало «Отъезжающий 42 П»  =  (Нр(мм)-30(мм))/2,95;
    a.id = B45E_WP;              // 45 град
    a.StrobeIndex = 2;           // П
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -30;               // 30 мм
    a.ValueType = rtt_mm;        // mm
    a.SkipTestGateLen = false;   // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // конец «Отъезжающий 42 Ш»  = начало « 42 П»-1 мкс ?????
    a.id = B45E_WP;            // 45 град
    a.StrobeIndex = 1;         // Ш
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = -30;             // 30 мм
    a.ValueType = rtt_mm;      // mm
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    RailTypeTuning.push_back(g);

    // -----------------------------------------------------
    // Режим "Тип рельса" - для ручных
    // -----------------------------------------------------

    sRailTypeTuningGroup g1;  // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    g1.Rails = crSingle;      // Одна нить
    g1.MasterId = H0E;        // Эхо
    g1.ChType = ctHandScan;   // Каналы ручного контроля

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
        g1.List.push_back(a);   */
    // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
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
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    BScanTapesList.push_back(tape);
    BScanTapesGroupList.push_back(BScanTapesList);

    // Допустимые отклонения (от нормативных) значений условной чувствительности

    SensValidRangesList.resize(14);

    SensValidRangesList[0].Channel = N55MSLW;
    SensValidRangesList[0].MinSens = 10;
    SensValidRangesList[0].MaxSens = 18;
    SensValidRangesList[0].GateIndex = 1;

    SensValidRangesList[1].Channel = N55MSLU;
    SensValidRangesList[1].MinSens = 10;
    SensValidRangesList[1].MaxSens = 18;
    SensValidRangesList[1].GateIndex = 1;

    SensValidRangesList[2].Channel = F45E_WP;
    SensValidRangesList[2].MinSens = 14;
    SensValidRangesList[2].MaxSens = 18;
    SensValidRangesList[2].GateIndex = 1;

    SensValidRangesList[3].Channel = F45E_WP;
    SensValidRangesList[3].MinSens = 14;
    SensValidRangesList[3].MaxSens = 18;
    SensValidRangesList[3].GateIndex = 2;

    SensValidRangesList[4].Channel = B45E_WP;
    SensValidRangesList[4].MinSens = 14;
    SensValidRangesList[4].MaxSens = 18;
    SensValidRangesList[4].GateIndex = 1;

    SensValidRangesList[5].Channel = B45E_WP;
    SensValidRangesList[5].MinSens = 14;
    SensValidRangesList[5].MaxSens = 18;
    SensValidRangesList[5].GateIndex = 2;

    SensValidRangesList[6].Channel = F65EU_WP;
    SensValidRangesList[6].MinSens = 12;
    SensValidRangesList[6].MaxSens = 14;
    SensValidRangesList[6].GateIndex = 1;

    SensValidRangesList[7].Channel = B65EU_WP;
    SensValidRangesList[7].MinSens = 12;
    SensValidRangesList[7].MaxSens = 14;
    SensValidRangesList[7].GateIndex = 1;

    SensValidRangesList[8].Channel = B65EW_WP;
    SensValidRangesList[8].MinSens = 12;
    SensValidRangesList[8].MaxSens = 14;
    SensValidRangesList[8].GateIndex = 1;

    SensValidRangesList[9].Channel = F65EW_WP;
    SensValidRangesList[9].MinSens = 12;
    SensValidRangesList[9].MaxSens = 14;
    SensValidRangesList[9].GateIndex = 1;

    SensValidRangesList[10].Channel = N0EMS_WP;
    SensValidRangesList[10].MinSens = 10;
    SensValidRangesList[10].MaxSens = 16;
    SensValidRangesList[10].GateIndex = 1;

    SensValidRangesList[11].Channel = N0EMS_WP;
    SensValidRangesList[11].MinSens = 10;
    SensValidRangesList[11].MaxSens = 14;
    SensValidRangesList[11].GateIndex = 2;

    SensValidRangesList[12].Channel = F65E_WP;
    SensValidRangesList[12].MinSens = 12;
    SensValidRangesList[12].MaxSens = 14;
    SensValidRangesList[12].GateIndex = 1;

    SensValidRangesList[13].Channel = B65E_WP;
    SensValidRangesList[13].MinSens = 12;
    SensValidRangesList[13].MaxSens = 14;
    SensValidRangesList[13].GateIndex = 1;

    // --------------------------------

    //    Для Романа

    for (int dBGain = 0; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = GainBase + dBGain / GainStep;
    }

    for (int dBGain = 16; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    /*    for (int dBGain = 23; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        } */

    for (int dBGain = 31; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }


    for (int dBGain = 40; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    /*        for (int dBGain = 54; dBGain <= 80; dBGain++)  {
              UMUGain[dBGain] = UMUGain[dBGain] - 1;
            } */

    for (int dBGain = 55; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    for (int dBGain = 76; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    for (int dBGain = 80; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    // -------------------
}

cDeviceConfig_DB_lan::~cDeviceConfig_DB_lan(void)
{
    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_DB_lan::dBGain_to_UMUGain(char dBGain)
{
    if ((dBGain >= 0) && (dBGain <= 80))
        return UMUGain[dBGain];
    else
        return 0;
}
