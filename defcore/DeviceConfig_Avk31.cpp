#include "Definitions.h"
#include "ChannelsTable.h"
#include "ChannelsIds.h"

#include "DeviceConfig_Avk31.h"

#include "datatransfers/datatransfer_lan.h"

#define BUM_IMITATOR_MODE ;

cDeviceConfig_Avk31::cDeviceConfig_Avk31(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    UMUCount = 1;
    DeviceName = "Avicon-31";
    MaxControlSpeed = 5;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crBoth;                     // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 0.2f;                        // Минимально допустимое время в призме - 0.2 мкс
    PrismDelayMax = 15;                          // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/ bScanThreshold;       // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/ aScanThreshold;  // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тит рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14;  // Cтроб для поиска максимума при настройке на тип рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = true;         // фильтрация В-развертки - ВКЛЮЧЕНА
    UMUConnectors = toCompleteControl;  // Используемые разъемы БУМ - сплошного контроля
    MainPathEncoderIndex = 0;
    FilteringBottomSignal = true;                 // Фильтрация (второго) донного
    MirrorChannelTuningMethod = mctmByGenerator;  // Метод настройки зеркальных каналов

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
    HScanCh.ReceiverLine = ulRU2;
    HScanCh.GeneratorLine = ulRU2;
    HScanCh.Generator = 7;
    HScanCh.Receiver = 7;
    HScanCh.PulseAmpl = 7;
    HScanCh.Side = usLeft;
    HScanCh.Active = false;  // TODO: should they be false?

    HScanCh.Id = H0MS;
    //    HScanCh.WorkFrequency = wf5MHz; // Для всех последующих каналов ручного контроля 5 МГц
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0E;
    HandChannels.push_back(HScanCh);

    HScanCh.Side = usRight;
    HScanCh.Id = H45;
    //    HScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2.5 МГц
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H50;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H58;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H65;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H70;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = H0E_5MGz;
    HScanCh.Side = usLeft;
    HandChannels.push_back(HScanCh);

    //    HScanCh.Id = H0MS_5MGz;
    //    HandChannels.push_back(HScanCh);

    // === Список каналов сплошного контроля ===
    const int scanChannelsCount = 28;
    const int schemeCount = 2;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    //    ScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2.5 МГц
    ScanCh.UMUIndex = 0;
    ScanCh.BScanGroup = 0;  // В-развертка КП1
    ScanCh.PulseAmpl = 6;   // Амплитуда зондирующего импульса

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1;  // Номер группы в которую входит данный канал (Схема прозвучивания №...)

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 5;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 8;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);


    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;      // Номер такта
    ScanCh.BScanTape = 4;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 9;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    //  ------ Таблица тактов Схема 2 ---
    ScanCh.StrokeGroupIdx = 2;  // Номер группы в которую входит данный канал (Схема прозвучивания №...)
    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 5;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 8;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;      // Номер такта
    ScanCh.BScanTape = 4;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 9;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);


    // --- Такт №6 ---

    ScanCh.StrokeNumber = 5;      // Номер такта
    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = F58MLW;           // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58MLU;           // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Нерабочая грань ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0x0F;      // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = F58MLU;           // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58MLW;           // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0x0F;      // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №7 ---

    ScanCh.StrokeNumber = 6;      // Номер такта
    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = B58MLU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58MLW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0x0F;      // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = B58MLW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58MLU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Нерабочая грань ]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0x0F;      // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    CalibrationCopyData.resize(4);

    CalibrationCopyData[0].DeviceSide = dsLeft;
    CalibrationCopyData[0].Master = F58MLW;
    CalibrationCopyData[0].Slave = F58MLU;

    CalibrationCopyData[1].DeviceSide = dsRight;
    CalibrationCopyData[1].Master = F58MLU;
    CalibrationCopyData[1].Slave = F58MLW;

    CalibrationCopyData[2].DeviceSide = dsLeft;
    CalibrationCopyData[2].Master = B58MLU;
    CalibrationCopyData[2].Slave = B58MLW;

    CalibrationCopyData[3].DeviceSide = dsRight;
    CalibrationCopyData[3].Master = B58MLW;
    CalibrationCopyData[3].Slave = B58MLU;

    // ------ Таблица тактов Схема 3 ------
    ScanCh.StrokeGroupIdx = 3;  // Номер группы в которую входит данный канал (Схема прозвучивания №...)
    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 5;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;      // Номер такта
    ScanCh.BScanTape = 4;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 9;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №6 ---
    ScanCh.StrokeNumber = 5;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = F22E;             // Идентификатор канала - [Наезжающий; 22 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 3;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B22E;             // Идентификатор канала - [Отъезжающий; 22 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 7;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 8;         // Номер полосы В-развертки
    ScanCh.Id = F22E;             // Идентификатор канала - [Наезжающий; 22 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 3;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B22E;             // Идентификатор канала - [Отъезжающий; 22 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 7;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // ------ Таблица тактов Схема 4 ------
    ScanCh.StrokeGroupIdx = 4;  // Номер группы в которую входит данный канал (Схема прозвучивания №4)

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 5;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = F42E;             // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42E;             // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn2;
    ScanChannels.push_back(ScanCh);

    // --- Такт №4 ---
    ScanCh.StrokeNumber = 3;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 2;         // Номер генератора
    ScanCh.Receiver = 2;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut2;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 6;         // Номер генератора
    ScanCh.Receiver = 6;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №5 ---
    ScanCh.StrokeNumber = 4;      // Номер такта
    ScanCh.BScanTape = 4;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 9;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = 73;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №6 ---
    ScanCh.StrokeNumber = 5;      // Номер такта
    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = N55MSLW;          // Идентификатор канала - [55/90 град; ЗТМ; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 3;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = N55MSLU;          // Идентификатор канала - [55/90 град; ЗТМ; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 7;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 8;         // Номер полосы В-развертки
    ScanCh.Id = N55MSLW;          // Идентификатор канала - [55/90 град; ЗТМ; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 7;         // Номер генератора
    ScanCh.Receiver = 7;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = N55MSLU;          // Идентификатор канала - [55/90 град; ЗТМ; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 3;         // Номер генератора
    ScanCh.Receiver = 3;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
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
            par.DBDelta = 0;                                                                 // TODO: 0?
                                                                                             // Для Настройки Ку
            if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] == imMirrorShadow))  // Прямой ввод ЗТМ
            {
                //				par.SensTuningGate[1].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 178);
                //				par.SensTuningGate[1].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 186);
                par.SensTuningGate[GateIdx].gStart = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], /*178*/ 180)) - 2;  // Строб относительно рельса высотой 180 мм
                par.SensTuningGate[GateIdx].gEnd = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], /*186*/ 180)) + 6;
            }
            else if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] != imMirrorShadow))  // Прямой ввод ЭХО
            {
                //                par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                //                par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 41 + 7);
                par.SensTuningGate[GateIdx].gStart = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 39 - 4));
                par.SensTuningGate[GateIdx].gEnd = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 41 + 7));
            }
            else if (chdesc.cdEnterAngle < 65)  // Наклонный но не 65, 70-ти градусные
            {
                //              par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                //              par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 42 + 4);
                //              par.SensTuningGate[1] = par.SensTuningGate[0];
                par.SensTuningGate[GateIdx].gStart = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 42 - 4));
                par.SensTuningGate[GateIdx].gEnd = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 42 + 4));
                //                par.SensTuningGate[1] = par.SensTuningGate[0];
            }
            else  // 65, 70-ти градусные
            {
                //              par.SensTuningGate[0].gStart = (int)tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                //              par.SensTuningGate[0].gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 14 + 4);
                //              par.SensTuningGate[1] = par.SensTuningGate[0];
                par.SensTuningGate[GateIdx].gStart = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 14 - 4));
                par.SensTuningGate[GateIdx].gEnd = static_cast<int>(tbl->DepthToDelay(ChannelsList[index], 14 + 4));
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
                par.PrismTuningGate[1].gStart = 20 - 10;
                par.PrismTuningGate[1].gEnd = 20 + 10;
            }

            SensTuningParams.push_back(par);
        }
    }


    // Режимы работы

    sModeChannelData cd;
    ModeList.resize(4);

    // Режим "Болтовой стык" - для Схемы 1

    ModeList[0].id = cmTestBoltJoint;
    ModeList[0].StrokeGroupIdx = 1;  // Схема 1
    ModeList[0].List.reserve(15);

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

    /*	// Копирование Ку из Ш в П - Копировние отключенно так как из за него идую глюки

        cd.id = 0x1A; // Отъезжающий; 42 град
        cd.Action = maSens_Push;
        cd.GateIdx = 1;	    // Номер строба
        ModeList[0].List.push_back(cd);

        cd.id = 0x1A; // Отъезжающий; 42 град
        cd.Action = maSens_Pop;
        cd.GateIdx = 2;	    // Номер строба
        ModeList[0].List.push_back(cd);

        // Копирование Ку из Ш в П

        cd.id = 0x19; // Наезжающий; 42 град
        cd.Action = maSens_Push;
        cd.GateIdx = 1;	    // Номер строба
        ModeList[0].List.push_back(cd);

        cd.id = 0x19; // Наезжающий; 42 град
        cd.Action = maSens_Pop;
        cd.GateIdx = 2;	    // Номер строба
        ModeList[0].List.push_back(cd);
    */


    // --------------------------------

    // Режим "Болтовой стык" - для Схемы 2

    ModeList[1].id = cmTestBoltJoint;
    ModeList[1].StrokeGroupIdx = 2;  // Схема 2
    ModeList[1].List.reserve(19);

    // Стробы

    cd.id = 0x01;  // 0 град эхо
    cd.Action = maEndGate_Set;
    cd.GateIdx = 1;  // Номер строба
    cd.Value = 18;   // мкс
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;                // Наезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[1].List.push_back(cd);

    cd.id = 0x1A;                // Отъезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[1].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;                 // Наезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[1].List.push_back(cd);

    cd.id = 0x1A;                 // Отъезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[1].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[1].List.push_back(cd);

    // Выкл АСД

    cd.id = 0x17;  // Наезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    cd.id = 0x18;  // Отъезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    cd.id = 0x10;  // Наезжающий; 58 град Зрк рабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    cd.id = 0x15;  // Отъезжающий; 58 град Зрк рабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    cd.id = 0x11;  // Наезжающий; 58 град Зрк нерабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    cd.id = 0x16;  // Отъезжающий; 58 град Зрк нерабочая грань
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[1].List.push_back(cd);

    // Режим 2 эхо
    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[1].List.push_back(cd);

    /*	// Копирование Ку из Ш в П - Копировние отключенно так как из за него идую глюки

        cd.id = 0x1A; // Отъезжающий; 42 град
        cd.Action = maSens_Push;
        cd.GateIdx = 1;	    // Номер строба
        ModeList[1].List.push_back(cd);

        cd.id = 0x1A; // Отъезжающий; 42 град
        cd.Action = maSens_Pop;
        cd.GateIdx = 2;	    // Номер строба
        ModeList[1].List.push_back(cd);

        // Копирование Ку из Ш в П

        cd.id = 0x19; // Наезжающий; 42 град
        cd.Action = maSens_Push;
        cd.GateIdx = 1;	    // Номер строба
        ModeList[1].List.push_back(cd);

        cd.id = 0x19; // Наезжающий; 42 град
        cd.Action = maSens_Pop;
        cd.GateIdx = 2;	    // Номер строба
        ModeList[1].List.push_back(cd);
    */
    // Увеличение Ку

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[1].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[1].List.push_back(cd);

    // --------------------------------

    // Режим "Болтовой стык" - для Схемы 3

    ModeList[2].id = cmTestBoltJoint;
    ModeList[2].StrokeGroupIdx = 3;  // Схема 3
    ModeList[2].List.reserve(17);

    // Стробы

    cd.id = 0x01;  // 0 град эхо
    cd.Action = maEndGate_Set;
    cd.GateIdx = 1;  // Номер строба
    cd.Value = 18;   // мкс
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;                // Наезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[2].List.push_back(cd);

    cd.id = 0x1A;                // Отъезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[2].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;                 // Наезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[2].List.push_back(cd);

    cd.id = 0x1A;                 // Отъезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[2].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[2].List.push_back(cd);

    // Выкл АСД

    cd.id = 0x17;  // Наезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[2].List.push_back(cd);

    cd.id = 0x18;  // Отъезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[2].List.push_back(cd);

    cd.id = 0xA1;  // Наезжающий; 22 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[2].List.push_back(cd);

    cd.id = 0xA2;  // Отъезжающий; 22 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[2].List.push_back(cd);

    // Режим 2 эхо
    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[2].List.push_back(cd);

    // Увеличение Ку

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[2].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[2].List.push_back(cd);


    // --------------------------------

    // Режим "Болтовой стык" - для Схемы 4

    ModeList[3].id = cmTestBoltJoint;
    ModeList[3].StrokeGroupIdx = 4;  // Схема 4
    ModeList[3].List.reserve(15);

    // Стробы

    cd.id = 0x01;  // 0 град эхо
    cd.Action = maEndGate_Set;
    cd.GateIdx = 1;  // Номер строба
    cd.Value = 18;   // мкс
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;                // Наезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[3].List.push_back(cd);

    cd.id = 0x1A;                // Отъезжающий; 42 град
    cd.Action = maEndGate_Push;  // Запомнить
    cd.GateIdx = 1;              // Номер строба
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[3].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Set;  // Установить
    cd.GateIdx = 1;             // Номер строба
    cd.Value = 49;              // мкс
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;                 // Наезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[3].List.push_back(cd);

    cd.id = 0x1A;                 // Отъезжающий; 42 град
    cd.Action = maStartGate_Set;  // Установить
    cd.GateIdx = 2;               // Номер строба
    cd.Value = 50;                // мкс
    ModeList[3].List.push_back(cd);

    cd.id = 0x1A;               // Отъезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;               // Наезжающий; 42 град
    cd.Action = maEndGate_Pop;  // Вспомнить
    cd.GateIdx = 2;             // Номер строба
    ModeList[3].List.push_back(cd);

    // Выкл АСД

    cd.id = 0x17;  // Наезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[3].List.push_back(cd);

    cd.id = 0x18;  // Отъезжающий; 70 град
    cd.Action = maAlarm;
    cd.GateIdx = 1;
    cd.Value = amOff;
    ModeList[3].List.push_back(cd);

    // Режим 2 эхо
    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maAlarm;
    cd.GateIdx = 2;  // Номер строба
    cd.Value = amTwoEcho;
    ModeList[3].List.push_back(cd);

    // Увеличение Ку

    cd.id = 0x1A;  // Отъезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[3].List.push_back(cd);

    cd.id = 0x19;  // Наезжающий; 42 град
    cd.Action = maSens_SetDelta;
    cd.GateIdx = 2;  // Номер строба
    ModeList[3].List.push_back(cd);

    // -----------------------------------------------------
    // Режим "Тип рельса" - для всех схем сплошного контроля
    // -----------------------------------------------------

    sRailTypeTuningGroup g;          // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    sRailTypeTuningChannelAction a;  // Действие для одного канала при настройке на тип рельса
    g.Rails = crBoth;                // Две нити
    g.MasterId = 0x01;               // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    g.ChType = ctCompInsp;           // Каналы стплошного контроля
    g.List.reserve(7);

    // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.id = 0x01;                         // 0 град
    a.StrobeIndex = 2;                   // ЗТМ
    a.Action = maMoveStartGate;          // Изменение начала строба
    a.Delta = -2;                        // начало «0 ЗТМ»  =  Tдс – 2 мкс
    a.ValueType = rtt_mcs;               // мкс
    a.SkipTestGateLen = true /*false*/;  // игнорировать значение минимальной протяженности строба
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
    a.id = 0x01;                         // 0 град
    a.StrobeIndex = 1;                   // ЭХО
    a.Action = maMoveEndGate;            // Изменение конца строба
    a.Delta = -3;                        // Tдс – 3 мкс;
    a.ValueType = rtt_mcs;               // мкс
    a.SkipTestGateLen = true /*false*/;  // игнорировать значение минимальной протяженности строба
    a.ActiveInTuning = false;            // Настройка на Тип рельса в режима Настройка не влияет на этот строб
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
        a.id = 0x1E;         // 0 град
        a.StrobeIndex = 1;   // ЗТМ
        a.Action = maMoveStartGate;  // Изменение начала строба
        a.Delta = - 2;       // начало «0 ЗТМ»  =  Tдс – 2 мкс
        a.ValueType = rtt_mcs; // мкс
        a.SkipTestGateLen = true; // игнорировать значение минимальной протяженности строба
        g1.List.push_back(a);
                             // конец «0 ЗТМ»  = Tдс +6 мкс;
        a.id = 0x1E;         // 0 град
        a.StrobeIndex = 1;   // ЗТМ
        a.Action = maMoveEndGate;  // Изменение конца строба
        a.Delta = + 6;       // начало «0 ЗТМ»  =  Tдс +6 мкс;
        a.ValueType = rtt_mcs; // мкс
        a.SkipTestGateLen = true; // игнорировать значение минимальной протяженности строба
        g1.List.push_back(a);  */
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
    /*                        // конец «0 ЭХО» = начало «0 ЗТМ» -1 мкс = Tдс – 2 мкс - 1 мкс;
       a.id = H0E;          // 0 град
       a.StrobeIndex = 1;   // ЭХО
       a.Action = maMoveEndGate;  // Изменение конца строба
       a.Delta = - 2;       // Tдс – 2 мкс;
       a.ValueType = rtt_mcs; // мкс
       a.SkipTestGateLen = false; // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
       a.ActiveInTuning = false;
       g2.List.push_back(a);
   */
    RailTypeTuning.push_back(g2);

    // --------------------------------

    // Список групп каналов
    GroupIndexList.push_back(1);
    GroupIndexList.push_back(2);
    //    GroupIndexList.push_back(3);
    //    GroupIndexList.push_back(4);
    // --------------------------------
    //
    sBScanTape tape;
    tBScanTapesList BScanTapesList;
    tape.tapeConformity = 5;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 6;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 7;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 8;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 9;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
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
    BScanTapesGroupList.push_back(BScanTapesList);

    BScanTapesList.clear();
    tape.tapeConformity = 5;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 6;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 7;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 8;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 9;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 0;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 1;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 2;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 3;
    tape.isVisibleInBoltJointMode = false;
    BScanTapesList.push_back(tape);
    tape.tapeConformity = 4;
    tape.isVisibleInBoltJointMode = true;
    BScanTapesList.push_back(tape);
    BScanTapesGroupList.push_back(BScanTapesList);
    BScanTapesGroupList.push_back(BScanTapesList);

    // Допустимые отклонения (от нормативных) значений условной чувствительности

    SensValidRangesList.resize(16);

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

    SensValidRangesList[6].Channel = F58ELW;  // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    SensValidRangesList[6].MinSens = 12;
    SensValidRangesList[6].MaxSens = 16;
    SensValidRangesList[6].GateIndex = 1;

    SensValidRangesList[7].Channel = F58ELU;  // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    SensValidRangesList[7].MinSens = 12;
    SensValidRangesList[7].MaxSens = 16;
    SensValidRangesList[7].GateIndex = 1;

    SensValidRangesList[8].Channel = B58ELW;  // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    SensValidRangesList[8].MinSens = 12;
    SensValidRangesList[8].MaxSens = 16;
    SensValidRangesList[8].GateIndex = 1;

    SensValidRangesList[9].Channel = B58ELU;  // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    SensValidRangesList[9].MinSens = 12;
    SensValidRangesList[9].MaxSens = 16;
    SensValidRangesList[9].GateIndex = 1;

    SensValidRangesList[10].Channel = N0EMS;  // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    SensValidRangesList[10].MinSens = 10;
    SensValidRangesList[10].MaxSens = 16;
    SensValidRangesList[10].GateIndex = 1;

    SensValidRangesList[11].Channel = N0EMS;  // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    SensValidRangesList[11].MinSens = 10;
    SensValidRangesList[11].MaxSens = 14;
    SensValidRangesList[11].GateIndex = 2;

    SensValidRangesList[12].Channel = F58MLW;  // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Рабочая грань]
    SensValidRangesList[12].MinSens = 12;
    SensValidRangesList[12].MaxSens = 16;
    SensValidRangesList[12].GateIndex = 1;

    SensValidRangesList[13].Channel = B58MLU;  // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Нерабочая грань ]
    SensValidRangesList[13].MinSens = 12;
    SensValidRangesList[13].MaxSens = 16;
    SensValidRangesList[13].GateIndex = 1;

    SensValidRangesList[14].Channel = F58MLU;  // Идентификатор канала - [Наезжающий; 58/34 град; Зеркальный; Нерабочая грань]
    SensValidRangesList[14].MinSens = 12;
    SensValidRangesList[14].MaxSens = 16;
    SensValidRangesList[14].GateIndex = 1;

    SensValidRangesList[15].Channel = B58MLW;  // Идентификатор канала - [Отъезжающий; 58/34 град; Зеркальный; Рабочая грань]
    SensValidRangesList[15].MinSens = 12;
    SensValidRangesList[15].MaxSens = 16;
    SensValidRangesList[15].GateIndex = 1;

    //    Для Романа

    for (int dBGain = 0; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = GainBase + dBGain / GainStep;
    }

    for (int dBGain = 16; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }
    /*
        for (int dBGain = 23; dBGain <= 80; dBGain++)  {
          UMUGain[dBGain] = UMUGain[dBGain] - 1;
        }
    */
    for (int dBGain = 31; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }


    for (int dBGain = 40; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }
    /*
            for (int dBGain = 54; dBGain <= 80; dBGain++)  {
              UMUGain[dBGain] = UMUGain[dBGain] - 1;
            }
    */
    for (int dBGain = 55; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    for (int dBGain = 76; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    for (int dBGain = 80; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }
}

cDeviceConfig_Avk31::~cDeviceConfig_Avk31(void)
{
    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_Avk31::dBGain_to_UMUGain(char dBGain)
{
    if ((dBGain >= 0) && (dBGain <= 80))
        return UMUGain[dBGain];
    else
        return 0;
}
