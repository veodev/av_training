#include "Definitions.h"
#include "ChannelsTable.h"
#include "ChannelsIds.h"
#include "DeviceConfig_ManualSet.h"
#include "datatransfers/datatransfer_lan.h"

cDeviceConfig_ManualSet::cDeviceConfig_ManualSet(cChannelsTable* channelsTable, int aScanThreshold, int bScanThreshold): cDeviceConfig(channelsTable)
{
    UMUCount = 1;
    DeviceName = "ManualSet";
    MaxControlSpeed = 0;                         // Максимальная допустимая скорость контроля [км/ч]
    ControlledRail = crBoth;                     // Контролируемые нити
    TuningGate = tgFixed;                        // Стробы, используемые при настройке каналов контроля
    GainBase = 20;                               // Минимальное значение аттенюатора
    GainMax = 180;                               // Максимальное значение аттенюатора
    GainStep = 0.5;                              // Шаг аттенюатора в децибелах
    PrismDelayMin = 2;                           // Минимально допустимое время в призме - 2 мкс
    PrismDelayMax = 10;                          // Максимальное допустимое время в призме - 10 мкс
    BScanGateLevel = /*5*/bScanThreshold;        // Уровень строба В-развертки [отсчетов]
    EvaluationGateLevel = /*6*/aScanThreshold;   // Уровень строба А-развертки [отсчетов]
    RailTypeTuningGate.gStart = 40;              // Cтроб для поиска максимума при настройке на тип рельса
    RailTypeTuningGate.gEnd = 70;
    RailTypeTuningGate_forSwitch.gStart = 14; // Cтроб для поиска максимума при настройке на тит рельса в зоне стрелочного перевода
    RailTypeTuningGate_forSwitch.gEnd = 70;
    BScanDataFiltration = false;                 // Фильтрация В-развертки - ВЫКЛЮЧЕНА
    UMUConnectors = toCompleteControl;           // Используемые разъемы БУМ - сканера контроля головки
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

    // === Список каналов сплошного контроля ===
    const int scanChannelsCount = 1;
    const int schemeCount = 1;

    ScanChannels.reserve(scanChannelsCount * schemeCount);
    sScanChannelDescription ScanCh;
    ScanCh.ColorDescr = cdNotSet; // Идентификатор цвета не используется
    ScanCh.UMUIndex = 0;
    ScanCh.PulseAmpl = 6;      // Амплитуда зондирующего импульса

    // ------  Таблица тактов Схема 1 ------
    ScanCh.StrokeGroupIdx = 1;  // Номер группы в которую входит данный канал (Схема прозвучивания №...)

    // --- Такт №1 ---
    ScanCh.StrokeNumber = 0;    // Номер такта
    ScanCh.DeviceSide = dsLeft; // Сторона дефектоскопа
    ScanCh.Id = TEST_1;           // Идентификатор канала - [-45 град; Теневой]

    ScanCh.Side = usLeft;       // Сторона БУМ
    ScanCh.ReceiverLine = ulRU1; // Линия приемника
    ScanCh.Receiver = 0;        // Номер приемника
    ScanCh.GeneratorLine = ulRU1; // Линия генератора
    ScanCh.Generator = 0;       // Номер генератора
    ScanChannels.push_back(ScanCh);

    ScanCh.StrokeNumber = 0;    // Номер такта
    ScanCh.DeviceSide = dsLeft; // Сторона дефектоскопа
    ScanCh.Id = TEST_2;           // Идентификатор канала - [-45 град; Теневой]

    ScanCh.Side = usLeft;       // Сторона БУМ
    ScanCh.ReceiverLine = ulRU2; // Линия приемника
    ScanCh.Receiver = 0;        // Номер приемника
    ScanCh.GeneratorLine = ulRU2; // Линия генератора
    ScanCh.Generator = 0;       // Номер генератора
    ScanChannels.push_back(ScanCh);

    ScanCh.StrokeNumber = 0;    // Номер такта
    ScanCh.DeviceSide = dsRight; // Сторона дефектоскопа
    ScanCh.Id = TEST_1;           // Идентификатор канала - [-45 град; Теневой]

    ScanCh.Side = usRight;       // Сторона БУМ
    ScanCh.ReceiverLine = ulRU1; // Линия приемника
    ScanCh.Receiver = 0;        // Номер приемника
    ScanCh.GeneratorLine = ulRU1; // Линия генератора
    ScanCh.Generator = 0;       // Номер генератора
    ScanChannels.push_back(ScanCh);

    ScanCh.StrokeNumber = 0;    // Номер такта
    ScanCh.DeviceSide = dsRight; // Сторона дефектоскопа
    ScanCh.Id = TEST_2;           // Идентификатор канала - [-45 град; Теневой]

    ScanCh.Side = usRight;       // Сторона БУМ
    ScanCh.ReceiverLine = ulRU2; // Линия приемника
    ScanCh.Receiver = 0;        // Номер приемника
    ScanCh.GeneratorLine = ulRU2; // Линия генератора
    ScanCh.Generator = 0;       // Номер генератора
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
/*    sSensTuningParam par;
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


    // Данные автонастройки Ку для теневого метода контроля

    SensAutoCalibration.CalibrationDelay = 250; // мс
    SensAutoCalibration.MinSensChange = 1; // дБ
    SensAutoCalibration.List.resize(3);
    SensAutoCalibration.List[0].Side = dsNone;
    SensAutoCalibration.List[0].id = 0xB2;
    SensAutoCalibration.List[0].TargetGain = 15;
    SensAutoCalibration.List[1].Side = dsNone;
    SensAutoCalibration.List[1].id = 0xB4;
    SensAutoCalibration.List[1].TargetGain = 15;
    SensAutoCalibration.List[2].Side = dsNone;
    SensAutoCalibration.List[2].id = 0xB6;
    SensAutoCalibration.List[2].TargetGain = 15;

    // --------------------------------
*/
    // Список групп каналов
    GroupIndexList.push_back(1);
}

cDeviceConfig_ManualSet::~cDeviceConfig_ManualSet(void)
{

    for (size_t i = 0; i < ModeList.size(); i++) {
        ModeList[i].List.resize(0);
    }
}

unsigned char cDeviceConfig_ManualSet::dBGain_to_UMUGain(char dBGain)
{
    return static_cast<unsigned char>(GainBase + dBGain / GainStep);
}

