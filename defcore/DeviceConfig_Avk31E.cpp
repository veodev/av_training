#include "Definitions.h"
#include "ChannelsTable.h"
#include <iomanip>
#include "DeviceConfig_Avk31E.h"
#include "datatransfers/datatransfer_lan.h"
#include "ChannelsIds.h"

cDeviceConfig_Avk31E::cDeviceConfig_Avk31E(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    //    DeviceID = dcAvicon31;
    DeviceName = "Avicon-31_Estonia";
    UMUCount = 2;
    MaxControlSpeed = 30;                        // Максимальная допустимая скорость контроля [км/ч]
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
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = false;        // фильтрация В-развертки - ВКЛЮЧЕНА
    UMUConnectors = toCompleteControl;  // Используемые разъемы БУМ - сплошного контроля
    MainPathEncoderIndex = 0;
    FilteringBottomSignal = true;                 // Фильтрация (второго) донного
    MirrorChannelTuningMethod = mctmByGenerator;  // Метод настройки зеркальных каналов

    sUmuDescription umuDescription1;
    umuDescription1.protocol = eProtLAN;
    cDataTransferLan::cLanConnectionParams* connectionParams1 = new (std::nothrow) cDataTransferLan::cLanConnectionParams;
#ifndef BUM_IMITATOR_MODE
    ::strcpy(connectionParams1->_remote_ip, "192.168.100.101");
    ::strcpy(connectionParams1->_local_ip, "192.168.100.1");
#else
    ::strcpy(connectionParams1->_remote_ip, "127.0.0.1");
    ::strcpy(connectionParams1->_local_ip, "127.0.0.1");
#endif
    connectionParams1->_port_in = 43001;
    connectionParams1->_port_out = 43000;
    connectionParams1->_port_in_tcp = false;
    connectionParams1->_port_out_tcp = true;
    umuDescription1.connectionParams = connectionParams1;
    umuDescriptions.push_back(umuDescription1);

    sUmuDescription umuDescription2;
    umuDescription2.protocol = eProtLAN;
    cDataTransferLan::cLanConnectionParams* connectionParams2 = new (std::nothrow) cDataTransferLan::cLanConnectionParams;
#ifndef BUM_IMITATOR_MODE
    ::strcpy(connectionParams2->_remote_ip, "192.168.100.102");
    ::strcpy(connectionParams2->_local_ip, "192.168.100.1");
    connectionParams2->_port_out = 43000;
#else
    ::strcpy(connectionParams2->_remote_ip, "127.0.0.1");
    ::strcpy(connectionParams2->_local_ip, "127.0.0.1");
    connectionParams2->_port_out = 53000;
#endif
    connectionParams2->_port_in = 43002;
    connectionParams2->_port_in_tcp = false;
    connectionParams2->_port_out_tcp = true;
    umuDescription2.connectionParams = connectionParams2;
    umuDescriptions.push_back(umuDescription2);

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
    const int scanChannelsCount = 18;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    //    ScanCh.WorkFrequency = wf2_5MHz; // Для всех последующих каналов ручного контроля 2.5 МГц
    ScanCh.BScanGroup = 0;  // В-развертка КП1
    ScanCh.PulseAmpl = 6;   // Амплитуда зондирующего импульса

    // ------  Таблица тактов ------
    ScanCh.StrokeGroupIdx = 1;  // Номер группы в которую входит данный канал (Схема прозвучивания №...)

    // --- Такт №1 ---

    ScanCh.UMUIndex = 0;
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 0;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
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

    ScanCh.BScanTape = 3;         // Номер полосы В-развертки
    ScanCh.Id = F42EE;            // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42EE;            // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);


    // --- Такт №2 ---

    ScanCh.UMUIndex = 0;
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 1;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 2;         // Номер полосы В-развертки
    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;   // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---

    ScanCh.UMUIndex = 0;
    ScanCh.StrokeNumber = 2;     // Номер такта
    ScanCh.BScanTape = 4;        // Номер полосы В-развертки
    ScanCh.Id = N0EMS;           // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;        // Сторона БУМ
    ScanCh.DeviceSide = dsLeft;  // Сторона дефектоскопа
    ScanCh.GeneratorLine = ulRU1;
    ScanCh.Generator = 1;  // Номер генератора
    ScanCh.ReceiverLine = ulRU2;
    ScanCh.Receiver = 5;    // Номер приемника
    ScanCh.ProbePos = 73;   // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);


    /////////////////////////////////////////////////////////////////////////////////////////////

    // --- Такт №1 ---

    ScanCh.UMUIndex = 1;
    ScanCh.StrokeNumber = 0;      // Номер такта
    ScanCh.BScanTape = 5;         // Номер полосы В-развертки
    ScanCh.Id = F70E;             // Идентификатор канала - [Наезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = 48;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B70E;             // Идентификатор канала - [Отъезжающий; 70 град; Эхо;]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = 23;         // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 8;         // Номер полосы В-развертки
    ScanCh.Id = F42EE;            // Идентификатор канала - [Наезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.Id = B42EE;            // Идентификатор канала - [Отъезжающий; 42 град; Эхо;]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -54;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---

    ScanCh.UMUIndex = 1;
    ScanCh.StrokeNumber = 1;      // Номер такта
    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = B58ELW;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 1;         // Номер генератора
    ScanCh.Receiver = 1;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = B58ELU;           // Идентификатор канала - [Отъезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usRight;        // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 5;         // Номер генератора
    ScanCh.Receiver = 5;          // Номер приемника
    ScanCh.ProbePos = -79;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomOut1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 6;         // Номер полосы В-развертки
    ScanCh.Id = F58ELW;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Рабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU2);  // Номер линии генераторов / приемников
    ScanCh.Generator = 0;         // Номер генератора
    ScanCh.Receiver = 0;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    ScanCh.BScanTape = 7;         // Номер полосы В-развертки
    ScanCh.Id = F58ELU;           // Идентификатор канала - [Наезжающий; 58/34 град; Эхо; Нерабочая грань]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    SetBothLines(ScanCh, ulRU1);  // Номер линии генераторов / приемников
    ScanCh.Generator = 4;         // Номер генератора
    ScanCh.Receiver = 4;          // Номер приемника
    ScanCh.ProbePos = -29;        // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;        // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdZoomIn1;
    ScanChannels.push_back(ScanCh);

    // --- Такт №3 ---

    ScanCh.UMUIndex = 1;
    ScanCh.StrokeNumber = 2;      // Номер такта
    ScanCh.BScanTape = 9;         // Номер полосы В-развертки
    ScanCh.Id = N0EMS;            // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    ScanCh.Side = usLeft;         // Сторона БУМ
    ScanCh.DeviceSide = dsRight;  // Сторона дефектоскопа
    ScanCh.GeneratorLine = ulRU1;
    ScanCh.Generator = 1;  // Номер генератора
    ScanCh.ReceiverLine = ulRU2;
    ScanCh.Receiver = 5;    // Номер приемника
    ScanCh.ProbePos = 73;   // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;  // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.ColorDescr = cdTMDirect1;
    ScanChannels.push_back(ScanCh);
/*
    // igor print configuration data for bumEmulator
    //    {
    //        int scheme = -100;
    //        for (auto c : ScanChannels) {
    //            if (c.StrokeGroupIdx != scheme) {
    //                scheme = c.StrokeGroupIdx;
    //                std::cout << "Scheme " << scheme << std::endl;
    //            }
    //            std::cout << "{{" << c.DeviceSide << ", 0x" << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << c.Id << "}, {" << (int) c.UMUIndex << "," << c.Side << "," << (int) c.ReceiverLine << "," << (int) c.StrokeNumber << "}}," << std::endl;
    //        }
    //    }
    // igor
*/
    // Список каналов
    ChannelsList.reserve(ScanChannels.size() /*+ HandChannels.size()*/);
    for (size_t i = 0; i < ScanChannels.size(); i++) {
        ChannelsList.push_back(ScanChannels[i].Id);
    }
    /*    for (size_t i = 0; i < HandChannels.size(); i++) {
            ChannelsList.push_back(HandChannels[i].Id);
        }
      */
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

    // -----------------------------------------------------
    // Режим "Тип рельса" - для всех схем сплошного контроля
    // -----------------------------------------------------

    sRailTypeTuningGroup g;          // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
    sRailTypeTuningChannelAction a;  // Действие для одного канала при настройке на тип рельса
    g.Rails = crBoth;                // Две нити
    g.MasterId = 0x01;               // Идентификатор канала - [Нет направления; 0 град; Эхо + ЗТМ]
    g.ChType = ctCompInsp;           // Каналы стплошного контроля

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
    a.SkipTestGateLen = true /*false*/;  // корректировать значение строба в зависимости от минимальной разрешенной протяженности строба
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

    RailTypeTuning.push_back(g2);

    // --------------------------------

    // Список групп каналов
    GroupIndexList.push_back(1);
    //    GroupIndexList.push_back(2);
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

    SensValidRangesList.resize(0);
    /*
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
    */
    //    Для Романа

    for (int dBGain = 0; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = GainBase + dBGain / GainStep;
    }

    for (int dBGain = 16; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

    for (int dBGain = 31; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }


    for (int dBGain = 40; dBGain <= 80; dBGain++) {
        UMUGain[dBGain] = UMUGain[dBGain] - 1;
    }

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

cDeviceConfig_Avk31E::~cDeviceConfig_Avk31E(void)
{
    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_Avk31E::dBGain_to_UMUGain(char dBGain)
{
    if ((dBGain >= 0) && (dBGain <= 80))
        return UMUGain[dBGain];
    else
        return 0;
}
