/*
 * DeviceConfigurator.h
 *
 *  Created on: 07.05.2012
 *      Author: Denis
 *
 *      персонализирующими конкретный дефектоскоп
 *
 */

#ifndef DEVICECONFIG_test_H
#define DEVICECONFIG_test_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "DeviceConfig.h"

//#include <utils.h>
//#include <wchar.h>
//#include "strptime.cpp"

#include "Definitions.h"
#include "ChannelsTable.h"

class cDeviceConfig_test : public cDeviceConfig  // Конфигуратор Дефектоскопа
{
public:
    cDeviceConfig_test(cChannelsTable* tbl_, int Count, int AScanTh, int BScanTh);  // Конструктор
    ~cDeviceConfig_test(void);                                                      // Деструктор
                                                                                    /*
                                                                                        char * DeviceName;              // Название прибора
                                                                                        int MaxControlSpeed;            // Максимальная допустимая скорость контроля [км/ч]
                                                                                        eControlledRail ControlledRail; // Контролируемые нити
                                                                                        eTuningGate TuningGate;         // Стробы, используемые при настройке каналов контроля
                                                                                        int GainMin;                    // Минимальное значение аттенюатора
                                                                                        int GainMax;                    // Максимальное значение аттенюатора
                                                                                        double GainStep;                // Шаг аттенюатора в децибелах
                                                                                        int BScanGateLevel;             // Уровень строба В-развертки [отсчетов]
                                                                                        int EvaluationGateLevel;        // Уровень строба А-развертки [отсчетов]

                                                                                        int HandChannelGenerator;
                                                                                        int HandChannelReceiver;

                                                                                        bool LoadFromFile(const char * filename);

                                                                                        bool ScanChannelExists(CID id);
                                                                                        bool HandChannelExists(CID id);
                                                                                        //	Считаем все с 0!!!
                                                                                        int	getSChannelsCount();
                                                                                        int getHChannelsCount();
                                                                                        int getRecParamsCount();
                                                                                        int getWorkModesCount();
                                                                                        int getRailTypeTuningCount();

                                                                                        bool getSChannelbyIdx(int Idx, sScanChannelDescription * val);
                                                                                        bool getSChannelbyID(int Idx, sScanChannelDescription * val);
                                                                                        bool getHChannelbyIdx(int Idx, CID * val);
                                                                                        bool getHChannelbyID(int Idx, CID * val);

                                                                                        bool getRecParams(CID id, sRecommendParams * val);

                                                                                        bool getWorkMode(int Idx, sDeviceModeData * val);

                                                                                        bool getSensParam(int Idx, sSensTuningParam * val);

                                                                                        bool getRailTypeTunung(int Idx, sRailTypeTuningGroup * val);
                                                                                    */
protected:
    /*
        tScanChannelsList ScanChannels;         // Список каналов сплошного контроля
        tHandShannelsList HandScanShannels;     // Список каналов ручного контроля
        tSensTuningParamsList SensTuningParams; // Параметры настройки каналов контроля на условную чувствительность
        tDeviceModeDataList ModeList;           // Режимы работы каналов сплошного контроля
        sGate RailTypeTuningGate;               // Строб поиска донного сигнала при настройке на тип рельса
        tRailTypeTuningList RailTypeTuning;     // Данные для выполнения настроки на тип рельса
        tRecommendParamsList RecParams;         // Список рекомендованных значений
    */
private:
    //	cChannelsTable ChTable;                 // Таблица каналов
};


#endif /* DEVICECONFIG_test_H */


/*



<DOCUMENT>

    <PARAMS
        NAME="Avicon-14"
        MAXCONTROLSPEED="4"
        TUNINGGATE="FIXED"
        TESTRAILS="TWO"
        MINGAIN="40"
        MINGAIN="160"
        GAINSTEP="0.5"
        EVALIATIONGATELEVEL="0"
        BSCANGATELEVEL="-6">
    </PARAMS>



    <SCANSCHANNELS>

        GROUP

        <ITEM NUMBER="1" ID="WP1F582" SIDE="Left"  LINE="1" TAPE="3" POS="-65" SHIFT="0" GEN="" REC="" PULSEAMPL="4"> </ITEM>
        <ITEM NUMBER="1" ID="WP1F581" SIDE="Left"  LINE="2" TAPE="3" POS="65" SHIFT="0" GEN="" REC="" PULSEAMPL="4"> </ITEM>
        <ITEM NUMBER="1" ID="WP4B582" SIDE="Right" LINE="1" TAPE="3" POS="-65" SHIFT="0" GEN="" REC="" PULSEAMPL="4"> </ITEM>
        <ITEM NUMBER="1" ID="WP4F582" SIDE="Right" LINE="2" TAPE="3" POS="65" SHIFT="0" GEN="" REC="" PULSEAMPL="4"> </ITEM>

        <ITEM ID="F42E" NUMBER="2" LINE="1" TAPE="2" POS="-65" SHIFT="0" GEN="2" REC="2" PULSEAMPL="4"> </ITEM>
        <ITEM ID="B42E" NUMBER="2" LINE="2" TAPE="2" POS="65" SHIFT="0" GEN="3" REC="3" PULSEAMPL="4"> </ITEM>

        <ITEM ID="F58ELW" NUMBER="3" LINE="1" TAPE="0" POS="-65" SHIFT="0" GEN="5" REC="5" PULSEAMPL="4"> </ITEM>
        <ITEM ID="F58ELU" NUMBER="3" LINE="2" TAPE="0" POS="-65" SHIFT="0" GEN="1" REC="1" PULSEAMPL="4"> </ITEM>

        <ITEM ID="B58ELW" NUMBER="4" LINE="1" TAPE="0" POS="65" SHIFT="0" GEN="5" REC="5" PULSEAMPL="4"> </ITEM>
        <ITEM ID="B58ELU" NUMBER="4" LINE="2" TAPE="0" POS="65" SHIFT="0" GEN="1" REC="1" PULSEAMPL="4"> </ITEM>

        <ITEM ID="F70E" NUMBER="6" LINE="1" TAPE="1" POS="-65" SHIFT="0" GEN="4" REC="4" PULSEAMPL="4"> </ITEM>
        <ITEM ID="B70E" NUMBER="7" LINE="2" TAPE="1" POS="65" SHIFT="0" GEN="1" REC="0" PULSEAMPL="4" EXACTGATEINDEX="0"> </ITEM>
    </SCANSCHANNELS>

    <HANDSCHANNELS>
        <SCHANNEL ID="LH0E"></SCHANNEL>
        <SCHANNEL ID="LH0MS"></SCHANNEL>
        <SCHANNEL ID="LH45"></SCHANNEL>
        <SCHANNEL ID="LH50"></SCHANNEL>
        <SCHANNEL ID="LH58"></SCHANNEL>
        <SCHANNEL ID="LH65"></SCHANNEL>
        <SCHANNEL ID="LH70"></SCHANNEL>

        <SCHANNEL ID="RH0E"></SCHANNEL>
        <SCHANNEL ID="RH0MS"></SCHANNEL>
        <SCHANNEL ID="RH45"></SCHANNEL>
        <SCHANNEL ID="RH50"></SCHANNEL>
        <SCHANNEL ID="RH58"></SCHANNEL>
        <SCHANNEL ID="RH65"></SCHANNEL>
        <SCHANNEL ID="RH70"></SCHANNEL>
    </HANDSCHANNELS>

    <WORKMODES>
        <MODE ID ="TESTBOLTJOINT">
            <STARTGATE ID="N0EMS" GATEINDEX="1" VALUE="9"></STARTGATE>
            <ENDGATE ID="N0EMS" GATEINDEX="1" VALUE="55"></ENDGATE>
            <STARTGATE ID="N0EMS2" GATEINDEX="1" VALUE="9"></STARTGATE>
            <ENDGATE ID="N0EMS2" GATEINDEX="1" VALUE="55"></ENDGATE>
            <ALARM ID="F42E" GATEINDEX="1" VALUE="TWOECHO"> </ALARM>
            <ALARM ID="F42E" GATEINDEX="2" VALUE="OFF"> </ALARM>
            <ALARM ID="B42E" GATEINDEX="1" VALUE="TWOECHO"> </ALARM>
            <ALARM ID="B42E" GATEINDEX="2" VALUE="OFF"> </ALARM>
            <STARTGATE ID="F42E" GATEINDEX="1" VALUE="TWOECHO"> </STARTGATE>
            <STARTGATE ID="B42E" GATEINDEX="1" VALUE="TWOECHO"> </STARTGATE>
            <ALARM ID="B58MB" GATEINDEX="1" VALUE="OFF"> </ALARM>
            <ALARM ID="F70E" GATEINDEX="1" VALUE="OFF"> </ALARM>
            <ALARM ID="B70E" GATEINDEX="1" VALUE="OFF"> </ALARM>
        </MODE>

        <MODE ID ="OTHERMODE_TEST">
            <STARTGATE ID="F58EB"  VALUE="55"></STARTGATE>
            <ENDGATE ID="F58EB"  VALUE="15"></ENDGATE>
            <SENS ID="F58EB" VALUE="12"></SENS>
            <ALARM ID="F58EB" VALUE="OneEcho"></ALARM>
        </MODE>
    </WORKMODES>

    <SENSTUNINGPARAMS>
        <ITEM ID="F42E" STGATE="20" ENDGATE="80" DELTA="2"> </ITEM>
        <ITEM ID="B42E" STGATE="11" ENDGATE="33" DELTA="2"> </ITEM>
        <ITEM ID="F58EB" STGATE="12" ENDGATE="33"> </ITEM>
        <ITEM ID="B58EB" STGATE="12" ENDGATE="80"> </ITEM>
        <ITEM ID="B58MB" STGATE="12" ENDGATE="33"> </ITEM>
        <ITEM ID="N0EMS" GATEINDEX="1" STGATE="178" ENDGATE="186"> </ITEM>
        <ITEM ID="N0EMS" GATEINDEX="2" STGATE="5" ENDGATE="186"> </ITEM>
        <ITEM ID="F70E" STGATE="10" ENDGATE="18"> </ITEM>
        <ITEM ID="B70E" STGATE="10" ENDGATE="18"> </ITEM>
        <ITEM ID="H0E" STGATE="20" ENDGATE="80"> </ITEM>
        <ITEM ID="H0MS" STGATE="20" ENDGATE="80"> </ITEM>

        <ITEM ID="LH45" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="LH50" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="LH58" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="LH65" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="LH70" STGATE="10" ENDGATE="18"> </ITEM>

        <ITEM ID="RH45" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="RH50" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="RH58" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="RH65" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="RH70" STGATE="10" ENDGATE="18"> </ITEM>
    </SENSTUNINGPARAMS>

    <RECOMMENDPARAMS>
        <ITEM ID="F42E" GATEINDEX="1" SENS="14" STGATE="20" ENDGATE="80"> </ITEM>
        <ITEM ID="F42E" GATEINDEX="2" SENS="16" STGATE="20" ENDGATE="80"> </ITEM>
        <ITEM ID="B42E" GATEINDEX="1" SENS="14" STGATE="11" ENDGATE="33"> </ITEM>
        <ITEM ID="B42E" GATEINDEX="2" SENS="16" STGATE="11" ENDGATE="33"> </ITEM>
        <ITEM ID="F58EB" SENS="12" STGATE="12" ENDGATE="33"> </ITEM>
        <ITEM ID="B58EB" SENS="12" STGATE="12" ENDGATE="80"> </ITEM>
        <ITEM ID="B58MB" SENS="12" STGATE="12" ENDGATE="33"> </ITEM>
        <ITEM ID="N0EMS" GATEINDEX="1" SENS="14" STGATE="58" ENDGATE="64"> </ITEM>
        <ITEM ID="N0EMS" GATEINDEX="2" SENS="14" STGATE="2" ENDGATE="56"> </ITEM>
        <ITEM ID="F70E" SENS="16" STGATE="22" ENDGATE="33"> </ITEM>
        <ITEM ID="B70E" SENS="16" STGATE="22" ENDGATE="33"> </ITEM>

        <ITEM ID="H45" SENS="16" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="H50" SENS="14" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="H58" SENS="12" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="H65" SENS="18" STGATE="38" ENDGATE="46"> </ITEM>
        <ITEM ID="H70" SENS="18" STGATE="10" ENDGATE="18"> </ITEM>
    </RECOMMENDPARAMS>

    <TUNINGTORAILTYPE SEARCHGATEMIN="8"	SEARCHGATEMAX="90">

        <GROUP MASTER="N0EMS">
            <ITEM ID="F42E" GATEINDEX="1" ACTION="END" DELTA="-4">
            <ITEM ID="F42E" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="N0EMS" GATEINDEX="1" ACTION="END" DELTA="4">
            <ITEM ID="N0EMS" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="N0EMS" GATEINDEX="2" ACTION="END" DELTA="4">
        </GROUP>

        <GROUP MASTER="N0EMS2">
            <ITEM ID="B42E" GATEINDEX="1" ACTION="END" DELTA="4">
            <ITEM ID="B42E" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="N0EMS2" GATEINDEX="1" ACTION="END" DELTA="4">
            <ITEM ID="N0EMS2" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="N0EMS2" GATEINDEX="2" ACTION="END" DELTA="4">
        </GROUP>

        <GROUP MASTER="LH0MS">
            <ITEM ID="LH0MS" GATEINDEX="1" ACTION="END" DELTA="4">
            <ITEM ID="LH0E" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="LH0E" GATEINDEX="2" ACTION="END" DELTA="4">
        </GROUP>

        <GROUP MASTER="RH0MS">
            <ITEM ID="RH0MS" GATEINDEX="1" ACTION="END" DELTA="4">
            <ITEM ID="RH0E" GATEINDEX="2" ACTION="START" DELTA="4">
            <ITEM ID="RH0E" GATEINDEX="2" ACTION="END" DELTA="4">
        </GROUP>

    </TUNINGTORAILTYPE>

</DOCUMENT>


    */
