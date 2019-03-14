/* ТЕСТ
 * DeviceConfigurator.cpp
 *
 *  Created on: 07.05.2012
 *      Author: Денис
 */

//#include "stdafx.h"
#include "DeviceConfig_Autocon_Alignment.h"
#include "Autocon.inc"

cDeviceConfig_Autocon_Alignment::cDeviceConfig_Autocon_Alignment(cChannelsTable * tbl_, int AScanTh, int BScanTh):cDeviceConfig(tbl_)
{

  //  strcpy(DeviceName, "Test Device");
	MaxControlSpeed = 2;            // Максимальная допустимая скорость контроля [км/ч]
	ControlledRail = crSingle;      // Контролируемые нити
	TuningGate= tgFixed;            // Стробы, используемые при настройке каналов контроля
	GainMin = 20;                   // Минимальное значение аттенюатора
	GainMax = 180;                  // Максимальное значение аттенюатора
	GainStep = 0.5;                 // Шаг аттенюатора в децибелах
	BScanGateLevel = /*5*/BScanTh;            // Уровень строба В-развертки [отсчетов]
	EvaluationGateLevel = /*6*/AScanTh;       // Уровень строба А-развертки [отсчетов]

	//HandChannelGenerator = 7;
	//HandChannelReceiver = 7;


    //  ------------ Список каналов сплошного контроля ------------------------
    //  Таблица тактов БУМ1 началась (КП1, КП2, КП4) (верхняя каретка)

    sScanChannelDescription ScanСh;

	ScanСh.DeviceSide = dsNone;
	ScanСh.UMUIndex = 0;       // Номер БУМ
    ScanСh.StrokeGroupIdx = 1; // Номер группы в которую входит данный канал
	ScanСh.PulseAmpl = 6;      // Амплитуда зондирующего импульса

    // ---------- Такт №1 --------------------

	ScanСh.StrokeNumber = 0;   // Номер такта
	ScanСh.BScanTape = 0;      // Номер полосы В-развертки

    ScanСh.Id = 0x46;          // Идентификатор канала - [КП-1 - Канал: 0 ЭХО; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 1;      // Номер полосы В-развертки

    ScanСh.Id = 0x52;          // Идентификатор канала - [КП-2 - Канал: 0 ЗТМ; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    ScanСh.Line = ulRU2;       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 3;      // Номер полосы В-развертки

    ScanСh.Id = 0x63;          // Идентификатор канала - [ КП-4 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов БУМ1 закончилась (КП1, КП2, КП4) (верхняя каретка)
    //  -----------------------------------------------------------------------


    //  Таблица тактов БУМ2 началась (КП3, КП5, КП8) (нижняя каретка)

    ScanСh.UMUIndex = 1;       // Номер БУМ
    ScanСh.StrokeGroupIdx = 2; // Номер группы в которую входит данный канал

    ScanСh.BScanTape = 2;      // Номер полосы В-развертки

    ScanСh.Id = 0x5A;          // Идентификатор канала - [КП-3 - Канал: 0 ЗТМ; Номер пластины - 4]
    ScanСh.Side = usRight;	   // Сторона БУМ
    ScanСh.Line = ulRU2;       // Номер линии генераторов / приемников
    ScanСh.Generator = 3;      // Номер генератора
    ScanСh.Receiver = 3;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 4;      // Номер полосы В-развертки

    ScanСh.Id = 0x6E;          // Идентификатор канала - [КП-5 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 1;      // Номер генератора
    ScanСh.Receiver = 1;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 7;      // Номер полосы В-развертки

    ScanСh.Id = 0x90;          // Идентификатор канала - [КП-8 - Канал: 0 ЭХО; Номер пластины - 7]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    //  Таблица тактов БУМ2 закончилась (КП3, КП5, КП8) (нижняя каретка)
    //  -----------------------------------------------------------------------


    //  Таблица тактов БУМ3 началась (КП6, КП7) (нижняя каретка)

    ScanСh.UMUIndex = 2;       // Номер БУМ
    ScanСh.StrokeGroupIdx = 3; // Номер группы в которую входит данный канал

    ScanСh.BScanTape = 5;      // Номер полосы В-развертки

    ScanСh.Id = 0x79;          // Идентификатор канала - [КП-6 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usLeft;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 7;      // Номер генератора
    ScanСh.Receiver = 7;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);

    ScanСh.BScanTape = 6;

    ScanСh.Id = 0x84;          // Идентификатор канала - [КП-7 - Канал: 0 ЗТМ; Номер пластины - 6]
    ScanСh.Side = usRight;	   // Сторона БУМ
    ScanСh.Line = ulRU1;       // Номер линии генераторов / приемников
    ScanСh.Generator = 4;      // Номер генератора
    ScanСh.Receiver = 4;       // Номер приемника
    ScanСh.ProbePos = 0;       // Положение ПЭП в скательной системе [мм]
    ScanСh.ProbeShift = -1;    // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanСh);


    //  Таблица тактов БУМ3 закончилась (КП6, КП7) (нижняя каретка)
    //  -----------------------------------------------------------------------




    // Фильтрация БУМов

    #ifdef FILTER_UMU_COUNT
    for (int idx = ScanChannels.size() - 1; idx >= 0; idx--)
        if (ScanChannels[idx].UMUIndex != 0)
        {
            ScanChannels.erase(ScanChannels.begin() + idx);
            // vector<int>(ScanChannels).swap(ScanChannels);
        }
    #endif

    // ----------------------

    for (int idx = ScanChannels.size() - 1; idx >= 0; idx--)
        if (ScanChannels[idx].UMUIndex != 0)
        {
            ScanChannels[idx].ReceiverState = true;
        }



	//  ------------ Список каналов -------------------------------------------

	for (int i = 0; i < ScanChannels.size(); i++)
		ChannelsList.push_back(ScanChannels[i].Id);

	for (int i = 0; i < HandChannels.size(); i++)
		ChannelsList.push_back(HandChannels[i].Id);

	//  --------- Инициализация Таблицы стробов для настройки каналов ---------

	sSensTuningParam par;
	sChannelDescription chdesc;

	for (int index = 0; index < ChannelsList.size(); index++)
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

    GroupIndexList.push_back(1);
    GroupIndexList.push_back(2);
    GroupIndexList.push_back(3);

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

/*
bool cDeviceConfig_Autocon::LoadFromFile(const char * filename)
{



	return true;
}
*/


