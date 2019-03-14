/* ТЕСТ
 * DeviceConfigurator.cpp
 *
 *  Created on: 07.05.2012
 *      Author: Денис
 */

//#include "stdafx.h"
#include "DeviceConfig_Autocon.h"
#include "UMUsTest.inc"

cDeviceConfig_Autocon::cDeviceConfig_Autocon(cChannelsTable * tbl_, int AScanTh, int BScanTh):cDeviceConfig(tbl_)
{

  //  strcpy(DeviceName, "Test Device");
//    DeviceID = dcAutocon;
    UMUCount = 3;
	MaxControlSpeed = 2;            // Максимальная допустимая скорость контроля [км/ч]
	ControlledRail = crSingle;      // Контролируемые нити
	TuningGate= tgFixed;            // Стробы, используемые при настройке каналов контроля
	GainBase = 20;                  // Минимальное значение аттенюатора
	GainMax = 180;                  // Максимальное значение аттенюатора
	GainStep = 0.5;                 // Шаг аттенюатора в децибелах
    PrismDelayMin = 0.2;             // Минимально допустимое время в призме - 0.2 мкс
    PrismDelayMax = 115;           // Максимальное допустимое время в призме - 110 мкс
	BScanGateLevel = /*5*/BScanTh;            // Уровень строба В-развертки [отсчетов]
	EvaluationGateLevel = /*6*/AScanTh;       // Уровень строба А-развертки [отсчетов]
    MirrorChannelTuningMethod = mctmByReceiver; // Метод настройки зеркальных каналов

	//HandChannelGenerator = 7;
	//HandChannelReceiver = 7;


    // ------------- Список каналов ручного контроля --------------------------

    sHandChannelDescription HScanСh;

    #ifndef HANDSCAN_UMU_0
	HScanСh.UMUIndex = 1;
    #endif
    #ifdef HANDSCAN_UMU_0
	HScanСh.UMUIndex = 0;
    #endif

	
    #ifndef HANDSCAN_LINE_NEW_UMU
	HScanСh.ReceiverLine = ulRU1;
	HScanСh.GeneratorLine = ulRU1;
    #endif
    #ifdef HANDSCAN_LINE_NEW_UMU
	HScanСh.ReceiverLine = ulRU2;
	HScanСh.GeneratorLine = ulRU2;
    #endif

	HScanСh.Id = 0x1E;
    HScanСh.Side = usLeft;
	HScanСh.Generator = 7;
	HScanСh.Receiver = 7;
	HScanСh.PulseAmpl = 7;
	HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x1D;
    HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x1F;
    HScanСh.Side = usRight;
	HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x20;
    HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x21;
    HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x22;
    HandChannels.push_back(HScanСh);

	HScanСh.Id = 0x23;
	HandChannels.push_back(HScanСh);

    //  ------------ Список каналов сплошного контроля ------------------------
    //  Таблица тактов БУМ1 началась (КП1, КП2, КП4) (движение №3 верхней каретки)

    sScanChannelDescription ScanСh;


    ScanСh.ColorDescr = cdNotSet; // Идентификатор цвета не используется

	ScanСh.DeviceSide = dsNone;
	ScanСh.UMUIndex = 0;       // Номер БУМ
	ScanСh.StrokeGroupIdx = 3; // Номер группы в которую входит данный канал
	ScanСh.PulseAmpl = 6;      // Амплитуда зондирующего импульса

    // ---------- Такт №1 --------------------

	ScanСh.StrokeNumber = 0;   // Номер такта
	ScanСh.BScanTape = 0;      // Номер полосы В-развертки
    ScanСh.BScanGroup = 1;     // В-развертка КП1

    ScanСh.Id = 0x40;          // Идентификатор канала - [КП-1 - Канал: 58 Н2 ип; Номер пластины - 8]
	ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1); // Номер линии генераторов / приемников

    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x3F;          // Идентификатор канала - [КП-1 - Канал: 58 Н1 ип; Номер пластины - 10]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh,ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);


    ScanСh.BScanGroup = 4;     // В-развертка КП4
	ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x68;          // Идентификатор канала - [КП-4 - Канал: 58 О6/8 ип; Номер пластины - 7]
	ScanСh.Side = usRight;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x5F;          // Идентификатор канала - [КП-4 - Канал: 58 Н6/8 ип; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

	// ---------- Такт №2 --------------------

	ScanСh.StrokeNumber = 1;   // Номер такта
	ScanСh.BScanTape = 0;      // Номер полосы В-развертки
    ScanСh.BScanGroup = 1;     // В-развертка КП1

    ScanСh.Id = 0x4D;          // Идентификатор канала - [КП-1 - Канал: 58 O2 ип; Номер пластины - 4]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x4C;          // Идентификатор канала - [КП-1 - Канал: 58 O1 ип; Номер пластины - 6]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x67;          // Идентификатор канала - [КП-4 - Канал: 58 O6/4 ип; Номер пластины - 8]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x52;          // Идентификатор канала - [КП-2 - Канал: 0 ЗТМ; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

	// ---------- Такт №3 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
	ScanСh.StrokeNumber = 2;   // Номер такта
	ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x49;          // Идентификатор канала - [КП-1 - Канал: 65 О1 ип; Номер пластины - 1]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x41;          // Идентификатор канала - [КП-1 - Канал: 65 Н1 ип; Номер пластины - 11]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4

    ScanСh.Id = 0x64;          // Идентификатор канала - [КП-4 - Канал: 65 О1 ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x53;          // Идентификатор канала - [КП-2 - Канал: 45 О2/0; Номер пластины - 2]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

	// ---------- Такт №4 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
	ScanСh.StrokeNumber = 3;   // Номер такта
	ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x4A;          // Идентификатор канала - [КП-1 - Канал: 65 О2 ип; Номер пластины - 2]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x42;          // Идентификатор канала - [КП-1 - Канал: 65 Н2 ип; Номер пластины - 12]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4

    ScanСh.Id = 0x65;          // Идентификатор канала - [КП-4 - Канал: 65 О2 ип; Номер пластины - 10]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x54;          // Идентификатор канала - [КП-2 - Канал: 45 О2/4; Номер пластины - 3]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

	// ---------- Такт №5 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
	ScanСh.StrokeNumber = 4;   // Номер такта
	ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x4B;          // Идентификатор канала - [КП-1 - Канал: 65 О3 ип; Номер пластины - 3]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4

    ScanСh.Id = 0x60;          // Идентификатор канала - [КП-4 - Канал: 65 Н1 ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x66;          // Идентификатор канала - [КП-4 - Канал: 65 О3 ип; Номер пластины - 11]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x50;          // Идентификатор канала - [КП-2 - Канал: 45 Н2/0; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

	// ---------- Такт №6 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
	ScanСh.StrokeNumber = 5;   // Номер такта
	ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x43;          // Идентификатор канала - [КП-1 - Канал: 65 Н3 ип; Номер пластины - 13]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4

    ScanСh.Id = 0x61;          // Идентификатор канала - [КП-4 - Канал: 65 Н2 ип; Номер пластины - 2]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x5E;          // Идентификатор канала - [КП-4 - Канал: 58 Н6/4 ип; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x51;          // Идентификатор канала - [КП-2 - Канал: 45 Н2/4; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

	// ---------- Такт №7 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
	ScanСh.StrokeNumber = 6;   // Номер такта
	ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x46;          // Идентификатор канала - [КП-1 - Канал: 0 ЭХО; Номер пластины - 7]
	ScanСh.Side = usLeft;	   // Сторона БУМ
	SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
	ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
	ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
	ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 4;     // В-развертка КП4
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x62;          // Идентификатор канала - [КП-4 - Канал: 65 Н3 ип; Номер пластины - 3]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x63;          // Идентификатор канала - [ КП-4 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов БУМ1 ЗАКОНЧИЛАСЬ (КП1, КП2, КП4) (движение №3 верхней каретки)
    //  -----------------------------------------------------------------------


    //  Таблица тактов БУМ2 началась (КП3, КП5, КП8) (движение №5 нижней каретки)

    ScanСh.UMUIndex = 1;       // Номер БУМ
    ScanСh.StrokeGroupIdx = 5; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.StrokeNumber = 0;   // Номер такта
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки
    ScanСh.BScanGroup = 8;     // В-развертка КП8

    ScanСh.Id = 0x8A; //0x7F;          // Идентификатор канала - [КП-8 - Канал: 58 Н7/5 ип; Номер пластины - 8]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x96; //0x8B;          // Идентификатор канала - [КП-8 - Канал: 58 O8/6 ип; Номер пластины - 6]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5

    ScanСh.Id = 0x69;          // Идентификатор канала - [КП-5 - Канал: 58 Н5/3 ип; Номер пластины - 8]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x73;          // Идентификатор канала - [КП-5 - Канал: 58 O5/7 ип; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 1;   // Номер такта
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x95; //0x8A;          // Идентификатор канала - [КП-8 - Канал: 58 O7/5 ип; Номер пластины - 4]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x8B; //0x80;          // Идентификатор канала - [КП-8 - Канал: 58 Н8/6 ип; Номер пластины - 10]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5

    ScanСh.Id = 0x6A;          // Идентификатор канала - [КП-5 - Канал: 58 Н5/7 ип; Номер пластины - 7]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3
    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x5A;          // Идентификатор канала - [КП-3 - Канал: 0 ЗТМ; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 2;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x92; //0x87;          // Идентификатор канала - [КП-8 - Канал: 65 О1 ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x8C; //0x81;          // Идентификатор канала - [КП-8 - Канал: 65 Н1 ип; Номер пластины - 11]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5
    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x6E;          // Идентификатор канала - [КП-5 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x58;          // Идентификатор канала - [КП-3 - Канал: 45 Н1/0; Номер пластины - 2]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №4 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 3;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x93; //0x88;          // Идентификатор канала - [КП-8 - Канал: 65 О2 ип; Номер пластины - 2]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x8D; //0x82;          // Идентификатор канала - [КП-8 - Канал: 65 Н2 ип; Номер пластины - 12]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x72;          // Идентификатор канала - [КП-5 - Канал: 58 O5/3 ип; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x59;          // Идентификатор канала - [КП-3 - Канал: 45 Н1/3; Номер пластины - 3]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №5 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 4;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x94; //0x89;          // Идентификатор канала - [КП-8 - Канал: 65 О3 ип; Номер пластины - 3]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5

    ScanСh.Id = 0x6F;          // Идентификатор канала - [КП-5 - Канал: 65 О1 ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x6B;          // Идентификатор канала - [КП-5 - Канал: 65 Н1 ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x5B;          // Идентификатор канала - [КП-3 - Канал: 45 О1/0; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №6 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 5;   // Номер такта
    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x90; //0x85;          // Идентификатор канала - [КП-8 - Канал: 0 ЭХО; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x70;          // Идентификатор канала - [КП-5 - Канал: 65 О2 ип; Номер пластины - 2]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x6C;          // Идентификатор канала - [КП-5 - Канал: 65 Н2 ип; Номер пластины - 10]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x5C;          // Идентификатор канала - [КП-3 - Канал: 45 О1/3; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №7 --------------------

    ScanСh.BScanGroup = 8;     // В-развертка КП8
    ScanСh.StrokeNumber = 6;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x8E; //0x83;          // Идентификатор канала - [КП-8 - Канал: 65 Н3 ип; Номер пластины - 13]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 5;     // В-развертка КП5

    ScanСh.Id = 0x71;          // Идентификатор канала - [КП-5 - Канал: 65 О3 ип; Номер пластины - 3]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x6D;          // Идентификатор канала - [КП-5 - Канал: 65 Н3 ип; Номер пластины - 11]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов БУМ2 ЗАКОНЧИЛАСЬ (КП3, КП5, КП8) (движение №5 нижней каретки)
    //  -----------------------------------------------------------------------


    //  Таблица тактов №1 БУМ3 началась (КП6, КП7) (движение №6 нижней каретки)

    ScanСh.UMUIndex = 2;       // Номер БУМ
    ScanСh.StrokeGroupIdx = 6; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.StrokeNumber = 0;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x7A; // 0x91;          // Идентификатор канала - [КП-6 - Канал: 65 О1 ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x81; //0x98;          // Идентификатор канала - [КП-7 - Канал: 65 Н1 ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x85; //0x9C;          // Идентификатор канала - [КП-7 - Канал: 65 О1 ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
                               // Номер линии генераторов / приемников
    #ifndef HANDSCAN_LINE_NEW_UMU
	SetBothLines(ScanСh, ulRU1);
    #endif
    #ifdef HANDSCAN_LINE_NEW_UMU
	SetBothLines(ScanСh, ulRU2);
    #endif
    ScanСh.Generator = 7;      // Номер генератора
    ScanСh.Receiver = 7;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 6;     // В-развертка КП6

    ScanСh.Id = 0x78; // 0x8F;          // Идентификатор канала - [КП-6 - Канал: 65 Н3 ип; Номер пластины - 11]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.StrokeNumber = 1;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x7B; // 0x92;          // Идентификатор канала - [КП-6 - Канал: 65 О2 ип; Номер пластины - 2]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x82; //0x99;          // Идентификатор канала - [КП-7 - Канал: 65 Н2 ип; Номер пластины - 2]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x80; //0x97;          // Идентификатор канала - [КП-7 - Канал: 58 Н8 ип; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x86; //0x9D;          // Идентификатор канала - [КП-7 - Канал: 65 О2 ип; Номер пластины - 10]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.StrokeNumber = 2;   // Номер такта
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x7C; //0x93;          // Идентификатор канала - [КП-6 - Канал: 65 О3 ип; Номер пластины - 3]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x83; //0x9A;          // Идентификатор канала - [КП-7 - Канал: 65 Н3 ип; Номер пластины - 3]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x7F; //0x96;          // Идентификатор канала - [КП-7 - Канал: 58 Н7 ип; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x87; //0x9E;          // Идентификатор канала - [КП-7 - Канал: 65 О3 ип; Номер пластины - 11]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №4 --------------------

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.StrokeNumber = 3;   // Номер такта
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x7D;//0x94;          // Идентификатор канала - [КП-6 - Канал: 58 O7 ип; Номер пластины - 4]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x74; // 0x95; //0x8A;          // Идентификатор канала - [КП-6 - Канал: 58 Н7 ип; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x88; //0x9F;          // Идентификатор канала - [КП-7 - Канал: 58 О8 ип; Номер пластины - 7]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x76; // 0x8C  // Идентификатор канала - [КП-6 - Канал: 65 Н1 ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №5 --------------------

    ScanСh.StrokeNumber = 4;   // Номер такта
    ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x7E; //0x95;          // Идентификатор канала - [КП-6 - Канал: 58 O8 ип; Номер пластины - 5]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.Id = 0x75; // 0x96; //0x8B;          // Идентификатор канала - [КП-6 - Канал: 58 Н8 ип; Номер пластины - 8]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x89; //0xA0;          // Идентификатор канала - [КП-7 - Канал: 58 О7 ип; Номер пластины - 8]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 6;      // Номер генератора
    ScanСh.Receiver = 6;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 6;     // В-развертка КП6
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x77; // 0x8E;          // Идентификатор канала - [КП-6 - Канал: 65 Н2 ип; Номер пластины - 10]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 5;      // Номер генератора
    ScanСh.Receiver = 5;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №6 --------------------

    ScanСh.StrokeNumber = 5;   // Номер такта
    ScanСh.BScanTape = 3;      // Номер полосы В-развертки
    ScanСh.Id = 0x79; // 0x90;          // Идентификатор канала - [КП-6 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usLeft;	   // Сторона БУМ
                               // Номер линии генераторов / приемников
    #ifndef HANDSCAN_LINE_NEW_UMU
	SetBothLines(ScanСh, ulRU1);
    #endif
    #ifdef HANDSCAN_LINE_NEW_UMU
	SetBothLines(ScanСh, ulRU2);
    #endif

    ScanСh.Generator = 7;      // Номер генератора
    ScanСh.Receiver = 7;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 7;     // В-развертка КП7

    ScanСh.Id = 0x84; //0x9B;          // Идентификатор канала - [КП-7 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов №1 БУМ3 ЗАКОНЧИЛАСЬ (КП6, КП7) (движение №6 нижней каретки)
    //  -----------------------------------------------------------------------


    //  Таблица тактов №2 БУМ3 началась (КП1, КП8) (движение №8 зеркало 45 справа от стыка)

    ScanСh.StrokeGroupIdx = 8; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
    ScanСh.StrokeNumber = 0;   // Номер такта
    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x48;          // Идентификатор канала - [КП-1 - Канал: 45 ОЗ п; Номер пластины - 5]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0x0F;   // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 8;     // В-развертка КП8

    ScanСh.Id = 0x91; //0x86;  // Идентификатор канала - [КП-8 - Канал: 45 О и; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.StrokeNumber = 1;   // Номер такта

    ScanСh.Id = 0x91; //0x86;  // Идентификатор канала - [КП-8 - Канал: 45 О ип; Номер пластины - 5]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.BScanGroup = 1;     // В-развертка КП1
    ScanСh.StrokeNumber = 2;   // Номер такта

    ScanСh.Id = 0x47;          // Идентификатор канала - [КП-1 - Канал: 45 О ип; Номер пластины - 5]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0;      // Номер генератора
    ScanСh.Receiver = 0;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов №2 БУМ3 ЗАКОНЧИЛАСЬ (КП1, КП8) (движение №8 зеркало 45 справа от стыка)
    //  -----------------------------------------------------------------------


    //  Таблица тактов №3 БУМ3 началась (КП1, КП8) (движение №10 зеркало 45 слева от стыка)

    ScanСh.StrokeGroupIdx = 10; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.StrokeNumber = 0;   // Номер такта

    ScanСh.Id = 0x45;          // Идентификатор канала - [КП-1 - Канал: 45 НЗ п; Номер пластины - 9]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0x0F;   // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 8;     // В-развертка КП8

    ScanСh.Id = 0x8F; // 0x84; // Идентификатор канала - [КП-8 - Канал: 45 Н ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.StrokeNumber = 1;   // Номер такта

    ScanСh.Id = 0x8F; // 0x84; // Идентификатор канала - [КП-8 - Канал: 45 Н ип; Номер пластины - 9]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.StrokeNumber = 2;   // Номер такта
    ScanСh.BScanGroup = 1;     // В-развертка КП1

    ScanСh.Id = 0x44;          // Идентификатор канала - [КП-1 - Канал: 45 Н ип; Номер пластины - 9]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU1);       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов №3 БУМ3 ЗАКОНЧИЛАСЬ (КП1, КП8) (движение №10 зеркало 45 слева от стыка)
    //  -----------------------------------------------------------------------


    //  Таблица тактов №4 БУМ3 началась (КП2, КП3) (движение №12 зеркало 65 справа от стыка)

    ScanСh.StrokeGroupIdx = 12; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.StrokeNumber = 0;   // Номер такта
    ScanСh.BScanGroup = 2;     // В-развертка КП2
    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x56;          // Идентификатор канала - [КП-2 - Канал: 65 ОЗ п; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0x0F;   // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3

    ScanСh.Id = 0x5D;          // Идентификатор канала - [КП-3 - Канал: 65 О и; Номер пластины - 7]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.StrokeNumber = 1;   // Номер такта

    ScanСh.Id = 0x5D;          // Идентификатор канала - [КП-3 - Канал: 65 О ип; Номер пластины - 7]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.StrokeNumber = 2;   // Номер такта
    ScanСh.BScanGroup = 2;     // В-развертка КП2

    ScanСh.Id = 0x55;          // Идентификатор канала - [КП-2 - Канал: 65 О ип; Номер пластины - 1]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов №4 БУМ3 ЗАКОНЧИЛАСЬ (КП2, КП3) (движение №12 зеркало 65 справа от стыка)
    //  -----------------------------------------------------------------------


    //  Таблица тактов №5 БУМ3 началась (КП2, КП3) (движение №14 зеркало 65 слева от стыка)

    ScanСh.StrokeGroupIdx = 14; // Номер группы в которую входит данный канал

    // ---------- Такт №1 --------------------

    ScanСh.StrokeNumber = 0;   // Номер такта

    ScanСh.Id = 0x4F;          // Идентификатор канала - [КП-2 - Канал: 65 НЗ п; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 0x0F;   // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanGroup = 3;     // В-развертка КП3

    ScanСh.Id = 0x57;          // Идентификатор канала - [КП-3 - Канал: 65 Н и; Номер пластины - 1]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №2 --------------------

    ScanСh.StrokeNumber = 1;   // Номер такта

    ScanСh.Id = 0x57;          // Идентификатор канала - [КП-3 - Канал: 65 Н и; Номер пластины - 1]
    ScanСh.Side = usRight;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    // ---------- Такт №3 --------------------

    ScanСh.StrokeNumber = 2;   // Номер такта
    ScanСh.BScanGroup = 2;     // В-развертка КП2

    ScanСh.Id = 0x4E;          // Идентификатор канала - [КП-2 - Канал: 65 Н ип; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    SetBothLines(ScanСh, ulRU2);       // Номер линии генераторов / приемников
    ScanСh.Generator = 2;      // Номер генератора
    ScanСh.Receiver = 2;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов №5 БУМ3 ЗАКОНЧИЛАСЬ (КП2, КП3) (движение №14 зеркало 65 слева от стыка)
    //  -----------------------------------------------------------------------

    // Фильтрация БУМов

    #ifdef FILTER_UMU_COUNT_TO_ONE
    for (int idx = ScanChannels.size() - 1; idx >= 0; idx--)
        if (ScanChannels[idx].UMUIndex != 0)
        {
            ScanChannels.erase(ScanChannels.begin() + idx);
            // vector<int>(ScanChannels).swap(ScanChannels);
        }
    #endif

    #ifdef FILTER_UMU_COUNT_TO_TWO
    for (int idx = ScanChannels.size() - 1; idx >= 0; idx--)
        if (ScanChannels[idx].UMUIndex == 2)
        {
            ScanChannels.erase(ScanChannels.begin() + idx);
        }
    #endif


    // ----------------------

    for (int idx = ScanChannels.size() - 1; idx >= 0; idx--)
        if (ScanChannels[idx].UMUIndex != 0)
        {
            ScanChannels[idx].ReceiverState = true;
        }

	//  ------------ Список каналов -------------------------------------------

	for (unsigned int i = 0; i < ScanChannels.size(); i++)
		ChannelsList.push_back(ScanChannels[i].Id);

	for (unsigned int i = 0; i < HandChannels.size(); i++)
		ChannelsList.push_back(HandChannels[i].Id);

	//  --------- Инициализация Таблицы стробов для настройки каналов ---------

	sSensTuningParam par;
	sChannelDescription chdesc;

	for (unsigned int index = 0; index < ChannelsList.size(); index++)
	{
		tbl->ItemByCID(ChannelsList[index], &chdesc);
		for (int GateIdx = 0; GateIdx < chdesc.cdGateCount; GateIdx++)
		{
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
            if (chdesc.cdEnterAngle != 0) {

                par.PrismTuningGate[0].gStart = 36 - 10;
                par.PrismTuningGate[0].gEnd = 36 + 10;
                par.PrismTuningGate[1].gStart = 36 - 10;
                par.PrismTuningGate[1].gEnd = 36 + 10;
            }

            else if (chdesc.cdEnterAngle == 0) { // Прямой ввод

                par.PrismTuningGate[0].gStart = 20 - 10;
                par.PrismTuningGate[0].gEnd = 20 + 10;
                par.PrismTuningGate[1].gStart = 20 - 10;
                par.PrismTuningGate[1].gEnd = 20 + 10;
            }

			SensTuningParams.push_back(par);
		}
	}

	//  --------- Список групп каналов ---------

    GroupIndexList.push_back(3);
    GroupIndexList.push_back(5);
    GroupIndexList.push_back(6);
    GroupIndexList.push_back(8);
    GroupIndexList.push_back(10);
    GroupIndexList.push_back(12);
    GroupIndexList.push_back(14);

	//  --------- Список рекомендованных значений ---------
/*
	sRecommendParams RecPar;
	RecPar.GateIdx = 0;
	RecPar.Sens = 16;
	RecPar.AlarmGate.gStart = 12;
	RecPar.AlarmGate.gEnd = 45;

	for (int i = 0; i < ShannelsList.size(); i++)
	{
		RecPar.id = ShannelsList[i];
		RecParams.push_back(RecPar);
	}
    */
}

cDeviceConfig_Autocon::~cDeviceConfig_Autocon(void)
{
   ///
}

unsigned char cDeviceConfig_Autocon::dBGain_to_UMUGain(char dBGain)
{
    return static_cast<unsigned char>(GainBase + dBGain / GainStep);
}

