#include "Definitions.h"
#include "ChannelsTable.h"
#include "DeviceConfig_HeadScan.h"
#include "datatransfers/datatransfer_lan.h"

cDeviceConfig_HeadScan::cDeviceConfig_HeadScan(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold)
    : cDeviceConfig(channelsTable)
{
    //    DeviceID = dcHeadScaner;

    UMUCount = 1;
    DeviceName = "HeadScan";
    MaxControlSpeed = 0;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crSingle;                   // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 2;                           // Минимально допустимое время в призме - 2 мкс
    PrismDelayMax = 15;                          // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/ bScanThreshold;       // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/ aScanThreshold;  // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тип рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = false;  // фильтрация В-развертки - ВЫКЛЮЧЕНА
    UMUConnectors = toScaner;     // Используемые разъемы БУМ - сканера контроля головки
    MainPathEncoderIndex = 1;
    MirrorChannelTuningMethod = mctmByReceiver;  // Метод настройки зеркальных каналов

    Params_.ScanLength = 200;
    Params_.VerticalStartPos = 9;
    Params = &Params_;

    sUmuDescription umuDescription;
    umuDescription.protocol = eProtLAN;
    cDataTransferLan::cLanConnectionParams* connectionParams = new (std::nothrow) cDataTransferLan::cLanConnectionParams;
    ::strcpy(connectionParams->_remote_ip, "192.168.100.100");
    ::strcpy(connectionParams->_local_ip, "192.168.100.1");
    connectionParams->_port_in = 43001;
    connectionParams->_port_out = 43000;
    connectionParams->_port_in_tcp = false;
    connectionParams->_port_out_tcp = true;
    umuDescription.connectionParams = connectionParams;
    umuDescriptions.push_back(umuDescription);

    // === Список каналов ручного контроля ===

    // === Список каналов сплошного контроля ===
    const int scanChannelsCount = 3;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    ScanCh.ColorDescr = cdNotSet; // Идентификатор цвета не используется
    ScanCh.UMUIndex = 0;
    ScanCh.PulseAmpl = 6;  // Амплитуда зондирующего импульса

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1;   // Номер группы в которую входит данный канал (Схема прозвучивания №...)
    ScanCh.Side = usLeft;        // Сторона БУМ
    ScanCh.DeviceSide = dsNone;  // Сторона дефектоскопа

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;       // Номер такта
    ScanCh.BScanTape = 0;          // Номер полосы В-развертки
    ScanCh.Id = 0xB1;              // Идентификатор канала - [-45 град; Теневой]
    ScanCh.ReceiverLine = ulRU2;   // Линия приемника
    ScanCh.Receiver = 4;           // Номер приемника
    ScanCh.GeneratorLine = ulRU1;  // Линия генератора
    ScanCh.Generator = 6;          // Номер генератора
    ScanCh.ProbePos = 0;           // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;         // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    // --- Такт №2 ---
    ScanCh.StrokeNumber = 1;       // Номер такта
    ScanCh.BScanTape = 1;          // Номер полосы В-развертки
    ScanCh.Id = 0xAF;              // Идентификатор канала - [0 град; Теневой]
    ScanCh.ReceiverLine = ulRU2;   // Линия приемника
    ScanCh.Receiver = 5;           // Номер приемника
    ScanCh.GeneratorLine = ulRU1;  // Линия генератора
    ScanCh.Generator = 5;          // Номер генератора
    ScanCh.ProbePos = 0;           // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;         // Смещение ПЭП в искательной системе от оси рельса [мм]
    ScanChannels.push_back(ScanCh);

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 2;       // Номер такта
    ScanCh.BScanTape = 2;          // Номер полосы В-развертки
    ScanCh.Id = 0xB0;              // Идентификатор канала - [+45 град; Теневой]
    ScanCh.ReceiverLine = ulRU2;   // Линия приемника
    ScanCh.Receiver = 6;           // Номер приемника
    ScanCh.GeneratorLine = ulRU1;  // Линия генератора
    ScanCh.Generator = 4;          // Номер генератора
    ScanCh.ProbePos = 0;           // Положение ПЭП в скательной системе [мм]
    ScanCh.ProbeShift = 0;         // Смещение ПЭП в искательной системе от оси рельса [мм]
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
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Для сканера это не нужно
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
                par.PrismTuningGate[1].gStart = 60 - 10;
                par.PrismTuningGate[1].gEnd = 60 + 10;
            }

            SensTuningParams.push_back(par);
        }
    }


    // Данные автонастройки Ку для теневого метода контроля

    SensAutoCalibration.CalibrationDelay = 250;  // мс
    SensAutoCalibration.MinSensChange = 1;       // дБ
    SensAutoCalibration.List.resize(3);
    SensAutoCalibration.List[0].Side = dsNone;
    SensAutoCalibration.List[0].id = 0xAF;
    SensAutoCalibration.List[0].TargetGain = 15;
    SensAutoCalibration.List[1].Side = dsNone;
    SensAutoCalibration.List[1].id = 0xB0;
    SensAutoCalibration.List[1].TargetGain = 15;
    SensAutoCalibration.List[2].Side = dsNone;
    SensAutoCalibration.List[2].id = 0xB1;
    SensAutoCalibration.List[2].TargetGain = 15;

    // --------------------------------

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


    // Список групп каналов
    GroupIndexList.push_back(1);
}

cDeviceConfig_HeadScan::~cDeviceConfig_HeadScan(void)
{
    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_HeadScan::dBGain_to_UMUGain(char dBGain)
{
    if ((dBGain >= 0) && (dBGain <= 80))
        return UMUGain[dBGain];
    else
        return 0;
}
