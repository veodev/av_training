/* ТЕСТ
 * DeviceConfigurator.cpp
 * 
 *  Created on: 07.05.2012
 *      Author: Денис 
 */

//#include "stdafx.h"
#include "DeviceConfig_test.h"


cDeviceConfig_test::cDeviceConfig_test(cChannelsTable * tbl_, int Count, int AScanTh, int BScanTh):cDeviceConfig(tbl_)
{
  //  strcpy(DeviceName, "Test Device");
	MaxControlSpeed = 2;            // Максимальная допустимая скорость контроля [км/ч]
//	ControlledRail = crSingle;      // Контролируемые нити
	ControlledRail = crBoth;        // Контролируемые нити - две
	TuningGate= tgFixed;            // Стробы, используемые при настройке каналов контроля
	GainBase = 20;                  // Минимальное значение аттенюатора
	GainMax = 180;                  // Максимальное значение аттенюатора
	GainStep = 0.5;                 // Шаг аттенюатора в децибелах
	BScanGateLevel = /*5*/BScanTh;            // Уровень строба В-развертки [отсчетов]
	EvaluationGateLevel = /*6*/AScanTh;       // Уровень строба А-развертки [отсчетов]
	//DefaultGain = 40;               // Значение АТТ поумолчанию
	//DefaultTVG = 5;                 // Значение ВРЧ поумолчанию
	//DefaultPrismDelay = 2;          // Значение 2Тп поумолчанию

	//HandChannelGenerator = 7;
	//HandChannelReceiver = 7;


    // ------------- Список каналов ручного контроля --------------------------

    sHandChannelDescription HScanCh;

    HScanCh.Id = 0x1D;
    HScanCh.UMUIndex = 0;
    HScanCh.Side = usRight;
    HScanCh.Line = 0;
    HScanCh.Generator = 7;
    HScanCh.Receiver = 7;
    HScanCh.PulseAmpl = 7;
//	HScanCh.Duration = 255;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x1E;
    HScanCh.Side = usLeft;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x1F;
    HScanCh.Side = usRight;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x20;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x21;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x22;
    HandChannels.push_back(HScanCh);

    HScanCh.Id = 0x23;
    HandChannels.push_back(HScanCh);

    //  ------------ Список каналов сплошного контроля ------------------------

    sScanChannelDescription ScanCh;

    // --- Левая нить ----

    ScanCh.Id = 0x01;          // Идентификатор канала
    ScanCh.DeviceSide = dsLeft;
    ScanCh.UMUIndex = 0;       // Номер БУМ
    ScanCh.StrokeGroupIdx = 0; // Номер группы в которую входит данный канал
    ScanCh.StrokeNumber = 0;   // Номер такта
    ScanCh.Side = usLeft;      // Сторона БУМ
    ScanCh.Line = 0;           // Номер линии генераторов / приемников
    ScanCh.Generator = 0;      // Номер генератора
    ScanCh.Receiver = 0;       // Номер приемника
    ScanCh.PulseAmpl = 6;      // Амплитуда зондирующего импульса
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.BScanTape = -1;     // Номер полосы В-развертки
    ScanChannels.push_back(ScanCh);

    if (Count >= 2)
    {
        ScanCh.Id = 0x03;          // Идентификатор канала
        ScanCh.StrokeNumber = 1;   // Номер такта
        ScanCh.Generator = 1;      // Номер генератора
        ScanCh.Receiver = 1;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 3)
    {
        ScanCh.Id = 0x04;          // Идентификатор канала
        ScanCh.StrokeNumber = 2;   // Номер такта
        ScanCh.Generator = 2;      // Номер генератора
        ScanCh.Receiver = 2;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 4)
    {
        ScanCh.Id = 0x05;          // Идентификатор канала
        ScanCh.StrokeNumber = 3;   // Номер такта
        ScanCh.Generator = 3;      // Номер генератора
        ScanCh.Receiver = 3;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 5)
    {
        ScanCh.Id = 0x06;          // Идентификатор канала
        ScanCh.StrokeNumber = 4;   // Номер такта
        ScanCh.Generator = 4;      // Номер генератора
        ScanCh.Receiver = 4;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 6)
    {
        ScanCh.Id = 0x17;          // Идентификатор канала
        ScanCh.StrokeNumber = 5;   // Номер такта
        ScanCh.Generator = 5;      // Номер генератора
        ScanCh.Receiver = 5;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 7)
    {
        ScanCh.Id = 0x19;          // Идентификатор канала
        ScanCh.StrokeNumber = 6;   // Номер такта
        ScanCh.Generator = 6;      // Номер генератора
        ScanCh.Receiver = 6;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    // --- Правая нить ----

    ScanCh.Id = 0x01;          // Идентификатор канала
    ScanCh.DeviceSide = dsRight;
    ScanCh.UMUIndex = 0;       // Номер БУМ
    ScanCh.StrokeGroupIdx = 0; // Номер группы в которую входит данный канал
    ScanCh.StrokeNumber = 0;   // Номер такта
    ScanCh.Side = usRight;     // Сторона БУМ
    ScanCh.Line = 0;           // Номер линии генераторов / приемников
    ScanCh.Generator = 0;      // Номер генератора
    ScanCh.Receiver = 0;       // Номер приемника
    ScanCh.PulseAmpl = 6;      // Амплитуда зондирующего импульса
    ScanCh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanCh.BScanTape = -1;     // Номер полосы В-развертки
    ScanChannels.push_back(ScanCh);

    if (Count >= 2)
    {
        ScanCh.Id = 0x03;          // Идентификатор канала
        ScanCh.StrokeNumber = 1;   // Номер такта
        ScanCh.Generator = 1;      // Номер генератора
        ScanCh.Receiver = 1;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 3)
    {
        ScanCh.Id = 0x04;          // Идентификатор канала
        ScanCh.StrokeNumber = 2;   // Номер такта
        ScanCh.Generator = 2;      // Номер генератора
        ScanCh.Receiver = 2;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 4)
    {
        ScanCh.Id = 0x05;          // Идентификатор канала
        ScanCh.StrokeNumber = 3;   // Номер такта
        ScanCh.Generator = 3;      // Номер генератора
        ScanCh.Receiver = 3;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 5)
    {
        ScanCh.Id = 0x06;          // Идентификатор канала
        ScanCh.StrokeNumber = 4;   // Номер такта
        ScanCh.Generator = 4;      // Номер генератора
        ScanCh.Receiver = 4;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 6)
    {
        ScanCh.Id = 0x17;          // Идентификатор канала
        ScanCh.StrokeNumber = 5;   // Номер такта
        ScanCh.Generator = 5;      // Номер генератора
        ScanCh.Receiver = 5;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    if (Count >= 7)
    {
        ScanCh.Id = 0x19;          // Идентификатор канала
        ScanCh.StrokeNumber = 6;   // Номер такта
        ScanCh.Generator = 6;      // Номер генератора
        ScanCh.Receiver = 6;       // Номер приемника
        ScanChannels.push_back(ScanCh);
    }

    //  ------------ Список каналов -------------------------------------------

    for (size_t i = 0; i < ScanChannels.size(); i++)
        ChannelsList.push_back(ScanChannels[i].Id);

    for (size_t i = 0; i < HandChannels.size(); i++)
        ChannelsList.push_back(HandChannels[i].Id);

	//  --------- Инициализация Таблицы стробов для настройки каналов ---------

	sSensTuningParam par;
	sChannelDescription chdesc;

    for (size_t index = 0; index < ChannelsList.size(); index++)
	{
        tbl->ItemByCID(ChannelsList[index], &chdesc);
		for (int GateIdx = 0; GateIdx < chdesc.cdGateCount; GateIdx++)
		{
            par.id = ChannelsList[index];
			par.GateIndex = GateIdx;

			// Для Настройки Ку
			if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] == imMirrorShadow))      // Прямой ввод ЗТМ
			{
                par.SensTuningGate.gStart = (int)tbl->DepthToDelay(ChannelsList[index], 178);
                par.SensTuningGate.gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 186);
			}
			else
			if ((chdesc.cdEnterAngle == 0) && (chdesc.cdMethod[GateIdx] != imMirrorShadow))      // Прямой ввод ЭХО
			{
                par.SensTuningGate.gStart = (int)tbl->DepthToDelay(ChannelsList[index], 39 - 4);
                par.SensTuningGate.gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 41 + 7);
            }
            else
            if (chdesc.cdEnterAngle < 65)     // Наклонный но не 65, 70-ти градусные
            {
                par.SensTuningGate.gStart = (int)tbl->DepthToDelay(ChannelsList[index], 42 - 4);
                par.SensTuningGate.gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 42 + 4);
            }
            else                // 65, 70-ти градусные
            {
                par.SensTuningGate.gStart = (int)tbl->DepthToDelay(ChannelsList[index], 14 - 4);
                par.SensTuningGate.gEnd = (int)tbl->DepthToDelay(ChannelsList[index], 14 + 4);
            }

			// Для Настройки 2ТП
			if (chdesc.cdEnterAngle != 0)     // Прямой ввод
			{
				par.PrismTuningGate.gStart = 34 - 6;
				par.PrismTuningGate.gEnd = 39 + 6;
			}
			else
			if (chdesc.cdEnterAngle == 0)
			{
				par.PrismTuningGate.gStart = 19 - 2;
				par.PrismTuningGate.gEnd = 21 + 2;
			}
			SensTuningParams.push_back(par);
		}
	}

	//  --------- Список групп каналов ---------

    GroupIndexList.push_back(0);

	//  --------- Список рекомендованных значений ---------
/*
	sRecommendParams RecPar;
	RecPar.GateIdx = 0;
	RecPar.Sens = 16;
	RecPar.AlarmGate.gStart = 12;
	RecPar.AlarmGate.gEnd = 45;

    for (size_t i = 0; i < ChannelsList.size(); i++)
	{
        RecPar.id = ChannelsList[i];
		RecParams.push_back(RecPar);
	}
*/
}

cDeviceConfig_test::~cDeviceConfig_test(void)
{
   ///
}

/*
bool cDeviceConfig_test::LoadFromFile(const char * filename)
{



	return true;
}
*/


