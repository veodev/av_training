#include "Definitions.h"
#include "ChannelsTable.h"
#include "ChannelsIds.h"

#include "DeviceConfig_Av15_usbcan.h"

cDeviceConfig_Av15_usbcan::cDeviceConfig_Av15_usbcan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    UMUCount = 1;
    DeviceName = "Avicon-15";
    MaxControlSpeed = 4;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crSingle;                   // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 140;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 0.2;                         // Минимально допустимое время в призме - 0.2 мкс
    PrismDelayMax = 15;                          // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/ bScanThreshold;       // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/ aScanThreshold;  // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тит рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = false;                 // фильтрация В-развертки - ВКЛЮЧЕНА
    UseLineSwitch = true;                        // Реле для подключения линий ручного контроля - ВКЛ
    UMUConnectors = toCompleteControl;           // Используемые разъемы БУМ - сплошного контроля
    MirrorChannelTuningMethod = mctmByReceiver;  // Метод настройки зеркальных каналов

    sUmuDescription umuDescription;
    umuDescription.connectionParams = NULL;
#ifdef WIN32
    umuDescription.protocol = eProtUSBCAN;
#endif
#ifndef WIN32
    umuDescription.protocol = eProtUSBCANAND;
#endif
    umuDescriptions.push_back(umuDescription);

    // === Список каналов ручного контроля ===
    const int handChannelsCount = 7;
    HandChannels.reserve(handChannelsCount);
    sHandChannelDescription HScanCh;
    HScanCh.Active = false;
    HScanCh.UMUIndex = 0;
    HScanCh.ReceiverLine = ulRU1;
    HScanCh.GeneratorLine = ulRU1;
    HScanCh.Generator = 7;
    HScanCh.Receiver = 7;
    HScanCh.PulseAmpl = 2;
    HScanCh.Side = usLeft;
    HScanCh.Id = H0MS;
    //    HScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2_5 МГц
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0E;
    HandChannels.push_back(HScanCh);

    HScanCh.PulseAmpl = 4;
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
    const int scanChannelsCount = 8;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;

    // init
    ScanCh.Used = false;
    ScanCh.ReceiverState = false;
    ScanCh.GeneratorLine = 0;
    ScanCh.ReceiverLine = 0;
    ScanCh.Generator = 0;
    ScanCh.Receiver = 0;
    ScanCh.BScanTape = 0;
    ScanCh.StrokeGroupIdx = 0;
    ScanCh.StrokeNumber = 0;
    ScanCh.BScanGroup = 0;
    ScanCh.UMUIndex = 0;
    ScanCh.PulseAmpl = 4;  // Амплитуда зондирующего импульса
    ScanCh.ProbePos = 0;
    ScanCh.ProbeShift = 0;
    ScanCh.Id = 0;
    ScanCh.DeviceSide = dsNone;
    ScanCh.Side = usLeft;
    ScanCh.ColorDescr = cdZoomIn1;

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1;  // Номер группы в которую входит данный канал (Схема прозвучивания единственная)

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 3;          // Номер приемника
    ScanCh.ProbePos = -57;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);


    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -57;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58EB;            // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Обе грани]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -82;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58EB;            // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Обе грани]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -32;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;      // Номер такта
    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = B58MB;            // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Обе грани]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -32;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    // --- Такт №6 ---
    ScanCh.PulseAmpl = 2;         // Амплитуда зондирующего импульса
    ScanCh.StrokeNumber = 5;      // Номер такта
    ScanCh.BScanTape = 4;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 7;          // Номер приемника
    ScanCh.ProbePos = 82;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №7 ---
    ScanCh.PulseAmpl = 4;         // Амплитуда зондирующего импульса
    ScanCh.StrokeNumber = 6;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = 57;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №8 ---
    ScanCh.StrokeNumber = 7;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsNone;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 32;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
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
                //				par.SensTuningGate[1].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 178);
                //				par.SensTuningGate[1].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 186);
                par.SensTuningGate[GateIdx].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 178);
                par.SensTuningGate[GateIdx].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 186);
            }
            else if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] != imMirrorShadow))  // Прямой ввод ЭХО
            {
                //                par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                //                par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 41 + 7);
                par.SensTuningGate[GateIdx].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                par.SensTuningGate[GateIdx].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 41 + 7);
            }
            else if (chdesc.cdEnterAngle < 65)  // Наклонный но не 65, 70-ти градусные
            {
                //              par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                //              par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 42 + 4);
                //              par.SensTuningGate[1] = par.SensTuningGate[0];
                par.SensTuningGate[GateIdx].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                par.SensTuningGate[GateIdx].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 42 + 4);
                //                par.SensTuningGate[1] = par.SensTuningGate[0];
            }
            else  // 65, 70-ти градусные
            {
                //              par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                //              par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 14 + 4);
                //              par.SensTuningGate[1] = par.SensTuningGate[0];
                par.SensTuningGate[GateIdx].gStart = (int) tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                par.SensTuningGate[GateIdx].gEnd = (int) tbl->DepthToDelay(ChannelsList[index], 14 + 4);
                //                par.SensTuningGate[1] = par.SensTuningGate[0];
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
                par.PrismTuningGate[1].gStart = 20 - 10;  // 60 - 10;
                par.PrismTuningGate[1].gEnd = 20 + 10;    // 60 + 10;
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

    cd.id = 0x01;  // 0 град эхо
    cd.Action = maEndGate_Set;
    cd.GateIdx = 1;  // Номер строба
    cd.Value = 18;   // мкс
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;                // Наезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x1A;                // Отъезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[0].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;                 // Наезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[0].List.push_back(cd);

    cd.id = 0x1A;                 // Отъезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[0].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[0].List.push_back(cd);

    // Выкл АСД

    cd.id = 0x17;  // Наезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = 0x18;  // Отъезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    cd.id = B58MB;  // 58/34 град; Зеркальный
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[0].List.push_back(cd);

    // Режим 2 эхо
    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[0].List.push_back(cd);

    // Увеличение Ку

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    // Копирование Ку из Ш в П
/*
    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_Push;
    cd.GateIdx = 1;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_Pop;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    // Копирование Ку из Ш в П

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_Push;
    cd.GateIdx = 1;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_Pop;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    // Увеличение Ку

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[0].List.push_back(cd);
*/
    // -----------------------------------------------------
    // Режим "Тип рельса" - для всех схем сплошного контроля
    // -----------------------------------------------------

    sRailTypeTuningGroup g;          // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    sRailTypeTuningChannelAction a;  // Действие для одного канала при настройке на тип рельса
    g.Rails = crSingle;              // Одна нить
    g.MasterId = 0x01;               // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    g.ChType = ctCompInsp;           // Каналы стплошного контроля

    // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.id = 0x01;                 // 0 град
    a.StrobeIndex = 2;           // ЗТМ
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -2;                // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.ValueType = rtt_mcs;       // мкс
    a.SkipTestGateLen = true;    // false игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g.List.push_back(a);
    // конец «0 ЗТМ»  = Tдс +6 мкс;
    a.id = 0x01;               // 0 град
    a.StrobeIndex = 2;         // ЗТМ
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = +6;              // начало «0 ЗТМ»  =  Tдс +6 мкс;
    a.ValueType = rtt_mcs;     // мкс
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = true;
    g.List.push_back(a);
    // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
    a.id = 0x01;               // 0 град
    a.StrobeIndex = 1;         // ЭХО
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = -3;              // Tдс – 3 мкс;
    a.ValueType = rtt_mcs;     // мкс
    a.SkipTestGateLen = true;  // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;  // Настройка на Тип рельса в режима Настройка не влияет на этот строб
    g.List.push_back(a);

    // начало «Наезжающий 42 П»  =  (Нр(мм)-30(мм))/2,95;
    a.id = 0x19;                 // 42 град
    a.StrobeIndex = 2;           // П
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -30;               // 30 мм
    a.ValueType = rtt_mm;        // mm
    a.SkipTestGateLen = false;   // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // конец «Наезжающий 42 Ш»  = начало « 42 П»-1 мкс ?????
    a.id = 0x19;               // 42 град
    a.StrobeIndex = 1;         // Ш
    a.Action = maMoveEndGate;  // Изменение конца строба
    a.Delta = -30;             // 30 мм
    a.ValueType = rtt_mm;      // mm
    a.SkipTestGateLen = true;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // начало «Отъезжающий 42 П»  =  (Нр(мм)-30(мм))/2,95;
    a.id = 0x1A;                 // 42 град
    a.StrobeIndex = 2;           // П
    a.Action = maMoveStartGate;  // Изменение начала строба
    a.Delta = -30;               // 30 мм
    a.ValueType = rtt_mm;        // mm
    a.SkipTestGateLen = false;   // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
    a.ActiveInTuning = false;
    g.List.push_back(a);

    // конец «Отъезжающий 42 Ш»  = начало « 42 П»-1 мкс ?????
    a.id = 0x1A;               // 42 град
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

    sBScanTape tape;
    tBScanTapesList BScanTapesList;
    tape.tapeConformity = 0;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 1;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 2;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 3;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 4;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    BScanTapesGroupList.push_back(BScanTapesList);

    // Допустимые отклонения (от нормативных) значений условной чувствительности

    SensValidRangesList.resize(11);

    SensValidRangesList[0].Channel = F70E;  // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    SensValidRangesList[0].MinSens = 12;
    SensValidRangesList[0].MaxSens = 16;
    SensValidRangesList[0].GateIndex = 1;

    SensValidRangesList[1].Channel = B70E;  // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    SensValidRangesList[1].MinSens = 12;
    SensValidRangesList[1].MaxSens = 16;
    SensValidRangesList[1].GateIndex = 1;

    SensValidRangesList[2].Channel = F42E;  // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    SensValidRangesList[2].MinSens = 14;
    SensValidRangesList[2].MaxSens = 18;
    SensValidRangesList[2].GateIndex = 1;

    SensValidRangesList[3].Channel = F42E;  // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    SensValidRangesList[3].MinSens = 14;
    SensValidRangesList[3].MaxSens = 18;
    SensValidRangesList[3].GateIndex = 2;

    SensValidRangesList[4].Channel = B42E;  // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    SensValidRangesList[4].MinSens = 14;
    SensValidRangesList[4].MaxSens = 18;
    SensValidRangesList[4].GateIndex = 1;

    SensValidRangesList[5].Channel = B42E;  // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    SensValidRangesList[5].MinSens = 14;
    SensValidRangesList[5].MaxSens = 18;
    SensValidRangesList[5].GateIndex = 2;

    SensValidRangesList[6].Channel = F58EB;  // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Обе грани]
    SensValidRangesList[6].MinSens = 12;
    SensValidRangesList[6].MaxSens = 16;
    SensValidRangesList[6].GateIndex = 1;

    SensValidRangesList[7].Channel = B58EB;  // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Обе грани]
    SensValidRangesList[7].MinSens = 12;
    SensValidRangesList[7].MaxSens = 16;
    SensValidRangesList[7].GateIndex = 1;

    SensValidRangesList[8].Channel = N0EMS;  // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    SensValidRangesList[8].MinSens = 10;
    SensValidRangesList[8].MaxSens = 16;
    SensValidRangesList[8].GateIndex = 1;

    SensValidRangesList[9].Channel = N0EMS;  // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    SensValidRangesList[9].MinSens = 10;
    SensValidRangesList[9].MaxSens = 14;
    SensValidRangesList[9].GateIndex = 2;

    SensValidRangesList[10].Channel = B58MB;  // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Обе грани]
    SensValidRangesList[10].MinSens = 12;
    SensValidRangesList[10].MaxSens = 16;
    SensValidRangesList[10].GateIndex = 1;
}

cDeviceConfig_Av15_usbcan::~cDeviceConfig_Av15_usbcan(void)
{
    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_Av15_usbcan::dBGain_to_UMUGain(char dBGain)
{
    return static_cast<unsigned char>(GainBase + dBGain / GainStep);
}
