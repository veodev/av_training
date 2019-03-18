#include "Device.h"
#include "TickCount.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>

#include "datatransfers/datatransfer_padcan.h"

#ifdef WIN32
#include "datatransfers/datatransfer_usbcan_win.h"
#include "prot_umu/prot_umu_usbcan_win.h"
#endif

//#ifdef ANDROID
//#include "datatransfers/datatransfer_usbcan_and.h"
//#include "prot_umu/prot_umu_usbcan_and.h"
//#endif

#include "datatransfers/datatransfer_lan.h"
#include "prot_umu/prot_umu_padcan.h"
#include "prot_umu/prot_umu_lan.h"

#include "CriticalSection.h"
#include "ThreadClassList.h"
#include "ChannelsTable.h"
#include "EventManager.h"
#include "DeviceCalibration.h"

static const float DEVICE_PI = 3.14159265f;

static const unsigned char ALARM_SEND_INTERVAL = 50;          // ms
static const unsigned char BOTOOM_SIGNAL_SEND_INTERVAL = 50;  // ms
static const unsigned char BOTTOM_SIGNAL_AMPL_FILTER_FACTOR = 10;
static const unsigned char DEFECT_53_SIGNALS_COUNT_MAX = 3;

const static signed char AmplIdtoDB[16] = {-12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18};

// Update Debug

#define HansChannels
#define CalibrtationMode
#define ChannelsParams2
#define ChannelsParams
#define EnableAScan_

bool defcoreAssertChecker()
{
    std::cerr << "Defcore assert enabled!" << std::endl;
    return true;
}

cDevice::cDevice(cThreadClassList* ThreadClassList, cDeviceConfig* Config, cDeviceCalibration* Calibration, cChannelsTable* Table, cEventManager* Device_EM, cEventManager* UMU_EM, cCriticalSection* CritSec, cCriticalSection* ExtCritSec)  // Конструктор
{
    DEFCORE_ASSERT(defcoreAssertChecker());
    TimeToFixBadSens = 30;
    ValidRangesTestPeriod_ms = 1000;
    ValidRangesTestTime_ms = 0;
    ResetValidRangesArr();
    FixBadSensState = false;
    SkipSetStrokeTableByChannelSelect = false;
    FirstPathEncoderSimState1 = true;
    FirstPathEncoderSimState2 = true;
    FirstPathEncoderSimNumber = 0;
    LastPathEncoderSimState = false;
    LastSysCoord = 0;
    isFirstCall = true;
    last_speedZone = -1;  // Пока не определена
    speedZone = -1;
    last_speed = 0;
    SensBySpeedState = false;
    SensBySpeed_pathStep = 2;
    SensBySpeed_summDelta = 0;
    RailTypeTrackingMode = false;  // Режим отслеживания (высоты) типа рельса [ВКЛ/ВЫКЛ], для зон стрелочных переводов
    RailTypeTrackingModeChanging = false;
    RailTypeTrackingModeTime = 0;

    SetPathEncoderData_last_path_encoder_index = 0xFF;
    SetPathEncoderData_last_Simulator = false;
    SetPathEncoderData_last_new_value = 0xFF;
    SetPathEncoderData_last_UseAsMain = false;

    CallBackFunction = NULL;
    MarkWidth = 0;
    MarkFlag = false;
    FCalibrationMode = false;
    ManagePathEncoderSimulation = true;
    UsePathEncoderSimulationinSearchMode = false;
    SkipBScanDataInCalibrationMode = false;

    CurrentBScanSessionID = -1;
    BScanSessionID[0] = -1;
    BScanSessionID[1] = -1;
    ShadowMethodChannelSensAutoCalibrationLastTime = GetTickCount_();

    // Присваиваем указатели на конфигурационные объекты

    cfg = Config;
    cal = Calibration;
    tbl = Table;
    DEV_Event = Device_EM;
    UMU_Event = UMU_EM;

    DEV_Event->Tag = 1;
    UMU_Event->Tag = 2;

    cs = CritSec;
    _externalCriticalSection = ExtCritSec;
    thlist = ThreadClassList;

    EndWorkFlag = false;

    // На данном этапе устройство выключено
    State = dsOFF;

    CurChannelType = ctCompInsp;

    _fCurrentDir = 1;
    _fPrevDir = 1;

    // Задаем текущий канал. Пока юзер сам не задал никакого канала, делаем текущим
    // первый канал из таблицы Config
    // CurChannel = Config-> ScanChannels[0].id;

    cIUMU* iumu = 0;
    cIDataTransfer* dt_curr = 0;  // Текущий используемый в цикле cDataTransfer
    dt_lan = new (std::nothrow) cDataTransferLan;
    dt_padcan = new (std::nothrow) cDataTransferCanRaw;

#ifdef WIN32
    dt_usbcan = new (std::nothrow) cDataTransferUsbCanWin;
#endif
    //#ifdef ANDROID
    //    dt_usbcanand = new (std::nothrow) cDataTransferUsbCanAnd;
    //#endif


    // Буфер для протокола и настроек соединения для i-ого бума
    // std::pair<eProtocol, cISocket::cConnectionParams *> buf; // TODO: unused variable?


    for (unsigned int umu_id = 0; umu_id < cfg->GetUMUCount(); ++umu_id) {
        eProtocol prot = eProtUNDEF;
        prot = cfg->getProtocolForUmuNum(umu_id);
        cISocket::cConnectionParams* connectionParams = cfg->getConnectionParamsForUmuNum(umu_id);

        switch (prot) {
        case eProtLAN:
            dt_curr = dt_lan;
            iumu = new (std::nothrow) UMU_LAN;
            iumu->_id = umu_id;
            break;

        case eProtPADCAN:
            dt_curr = dt_padcan;
            iumu = new (std::nothrow) UMU_PADCAN;
            break;

        case eProtUSBCAN:
#ifdef WIN32
            dt_curr = dt_usbcan;
            iumu = new (std::nothrow) UMU_USBCAN;
#endif
            break;

        case eProtUSBCANAND:
            //#ifdef ANDROID
            //            dt_curr = dt_usbcanand;
            //            iumu = new (std::nothrow) UMU_USBCANAND;
            //#endif
            break;

        default:
            assert(false);  // ERROR! Protocol was not set properly!
            // TODO: add handler there
            break;
        }

        if (iumu == 0) {
            assert(false);  // ERROR! Cant create connection!
                            // TODO: add handler there
        }

        iumu->setConnectionId(dt_curr->addConnection(connectionParams));
        iumu->SetData(umu_id, dt_curr, UMU_Event, cs);

        iumu->StartWork();
        iumu->setWState(cIUMU::bsInit);

        thlist->AddTick(DEFCORE_THREAD_FUNCTION(cIUMU, iumu, Tick));

        iumu->DisableAlarm(/*true*/);
        iumu->DisableAScan(/*true*/);
        iumu->DisableBScan(/*true*/);
        iumu->DisableACSumm(/*true*/);

        UMUList.push_back(iumu);
    }

#ifdef DeviceLog
    if (!Log) Log = new LogFileObj(this);
    Log->AddText("Create CANProtDevice object");
#endif

    FCurMode = dmPaused;
    FCurChannel = cfg->ChannelsList[0];

    if (tbl->isHandScan(FCurChannel)) {
        FCurGroupIndex = 0;
        FCurSide = dsNone;
    }
    else {
        FCurGroupIndex = cfg->GetChannelGroup(0);
        if (cfg->getControlledRail() == crSingle) {
            FCurSide = dsNone;
        }
        else {
            FCurSide = dsLeft;
        }
    }

    FCurrentGateIdx = 1;
    FCurCalMode = cmSens;

    FNewGateIdx = FCurrentGateIdx;
    FNewMode = dmSearch;  // FCurMode;
    FNewChannel = FCurChannel;
    FNewSide = FCurSide;
    FNewGroupIndex = FCurGroupIndex;
    FNewCalMode = cmSens;

    UMUGenResList.reserve(cfg->GetUMUCount());
    for (int i = 0; i < cfg->GetUMUCount(); ++i) {
        UMUGenResList.push_back(GenResList());
    }

    cfg->SetChannelGroup(FNewGroupIndex);

    DeviceThreadIndex = thlist->AddTick(DEFCORE_THREAD_FUNCTION(cDevice, this, Tick));

    //    EncoderSim_byHandScan = true; // Включать имитатор ДП при смене типа каналов на ручной и выключать при обратной смене
    ResetPathEncoderGis();
    SetPathEncoderGisState(false);

    prepareRailTypeTuningList();  // Подготовка массива хранение данных для настройки на тип рельса
                                  // Подготовка массива хранение режимов АСД
    int maxCid = Table->GetMaxCID();
    StrobeModeList_.resize(static_cast<unsigned int>(maxCid + 1));
    for (int idx = 0; idx < maxCid; ++idx)
        for (unsigned char Side = 0; Side <= 2; ++Side)
            for (unsigned char Gate = 0; Gate <= 2; ++Gate) {
                StrobeModeList_[static_cast<unsigned int>(idx)].Modes[Side][Gate].Alg = aaNone;
                StrobeModeList_[static_cast<unsigned int>(idx)].Modes[Side][Gate].Mode = amOneEcho;
            }

    // Фильтрация В-разветки

    _enableFiltration = false;
    _fFirstCoord = true;
    _currentDeltaCoord = 0;

    _bottomSignalSpacingData[0][spclevelMinus6].reset();
    _bottomSignalSpacingData[0][spclevelZero].reset();
    _bottomSignalSpacingData[1][spclevelMinus6].reset();
    _bottomSignalSpacingData[1][spclevelZero].reset();

    fltrPrmMinPacketSize = FILTER_PARAM_MIN_PACKET_SIZE_MINUS_12_DB;
    fltrPrmMaxCoordSpacing = FILTER_PARAM_COORD_DELTA_MAX_MINUS_12_DB;
    fltrPrmMaxDelayDelta = FILTER_PARAM_DELAY_DELTA_MAX_MINUS_12_DB;
    fltrPrmMaxSameDelayConsecutive = FILTER_PARAM_SAME_DELAY_MAX_MINUS_12_DB;

    fltrPrmMinPacketSizeForEvent = FILTER_PARAM_MIN_PACKET_SIZE_FOR_EVENT;
    fltrPrmMinSignals0dBInNormModeForEvent = FILTER_PARAM_SIGALS_0DB_IN_NORM_MODE_FOR_EVENT;
    fltrPrmMinSignalsMinus6dBInBoltTestModeForEvent = FILTER_PARAM_SIGALS_MINUS_6DB_IN_BOLT_MODE_FOR_EVENT;
    fltrPrmMinSpacing0dBInNormModeForEvent = FLTR_PRM_SPACING_0DB_IN_NORM_MODE_FOR_EVENT;
    fltrPrmMinSpacingMinus6dBInBoltTestModeForEvent = FLTR_PRM_SPACING_MINUS6DB_IN_BOLT_MODE_FOR_EVENT;

    _displayCoord = 0;
    _fPrevSysCoord = 0;

    _enableSMChSensAutoCalibration = false;  // Состояние автонастройки Ку теневых каналов - ВЫКЛ
    AcousticContactState = 0;

    LastOnLineTestTime = 0;
    FirstUMUOnLineTest = true;
    for (int idx1 = 0; idx1 < 16; ++idx1) {
        UMUOnLineStatus[idx1] = true;
        UMUOnLineStatusCount[idx1] = 0;
    }
    AScanMax.Delay = -1;
    AScanMax.Ampl = -1;
    _pathEncoderResetComandSent = false;
    FCurrentControlMode = cmNormal;
    FNewCurrentControlMode = cmNormal;
}


void cDevice::prepareRailTypeTuningList()
{
    RailTypeTuningDataList.clear();
    RailTypeTuningDataList.reserve(4);

    eDeviceSide StartSide;
    eDeviceSide EndSide;
    sRailTypeTuningData tmp;

    for (size_t i = 0; i < cfg->GetRailTypeTuningCount(); ++i) {  // Выбор канала (с доным сигналом) по которому настраивается тип рельса
        if (cfg->GetRailTypeTuningItem(static_cast<int>(i)).Rails == crSingle) {
            StartSide = dsNone;
            EndSide = dsNone;
        }
        else {
            StartSide = dsLeft;
            EndSide = dsRight;
        }

        for (int SideIdx = StartSide; SideIdx <= EndSide; ++SideIdx) {
            tmp.Side = static_cast<eDeviceSide>(SideIdx);
            tmp.Channel = cfg->GetRailTypeTuningItem(static_cast<int>(i)).MasterId;  // Канал поиска донного сигнала
            tmp.DelayHeight = -1;
            tmp.Time = 0;
            RailTypeTuningDataList.push_back(tmp);
        }
    }
}

bool cDevice::ChangeDeviceConfig(cDeviceConfig* Config)  // Смена конфигурации
{
    if (Config == cfg) return true;

    // Проверка на возможность переключения конфигурации

    if (!Config) return false;
    if (cfg->GetUMUCount() != Config->GetUMUCount()) return false;

    for (unsigned int umu_id = 0; umu_id < cfg->GetUMUCount(); ++umu_id) {
        if (cfg->getProtocolForUmuNum(umu_id) != Config->getProtocolForUmuNum(umu_id)) {
            return false;
        }
    }

    // Переключение конфигурации

    PathEncoderSim(cfg->getMainPathEncoderIndex(), false);  // Выключение имитатора ДП

    if (cfg->getSaveChannelGroup() == -1) cfg->setSaveChannelGroup(FCurGroupIndex);

    cfg = Config;

    if (cfg->getSaveChannelGroup() != -1) FCurGroupIndex = cfg->getSaveChannelGroup();


    FCurMode = dmPaused;
    FCurChannel = cfg->ChannelsList[0];

    if (tbl->isHandScan(FCurChannel)) {
        if (cfg->getSaveChannelGroup() == -1) FCurGroupIndex = 0;
        FCurSide = dsNone;
    }
    else {
        if (cfg->getSaveChannelGroup() == -1) FCurGroupIndex = cfg->GetChannelGroup(0);
        if (cfg->getControlledRail() == crSingle)
            FCurSide = dsNone;
        else
            FCurSide = dsLeft;
    }

    FCurrentGateIdx = 1;
    FNewGateIdx = FCurrentGateIdx;
    FNewMode = dmSearch;  // FCurMode;
    FNewChannel = FCurChannel;
    FNewSide = FCurSide;
    FNewGroupIndex = FCurGroupIndex;

    if (tbl->isHandScan(FNewChannel)) {
        CurChannelType = ctHandScan;
    }
    else {
        CurChannelType = ctCompInsp;
    }

    cfg->SetChannelGroup(FNewGroupIndex);

    prepareRailTypeTuningList();  // Подготовка массива хранение данных для настройки на тип рельса

    SetLeftSideSwitching(cfg->getUMUConnectors());  // Установка используемого разъема БУМ - сплошного контроля или контроля сканера

    Update(true);

    SetPathEncoderData(static_cast<char>(cfg->getMainPathEncoderIndex()), 0, 0, true);
    return true;
}

void cDevice::StartWork()  // Начало работы
{
    thlist->Resume(DeviceThreadIndex);

    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        thlist->Resume(UMUIdx);
    }

    // Фиксируем время начала работы
    StartTime = GetTickCount_();

    // Будем считать что в начале работы прибор находиться на паузе в плане работы каналов
    FCurMode = dmPaused;

    // На всякий случай очищаем массив с информацией по оборудованию
    UnitsData.clear();
    // Добавляем инфу о hrd в массив UnitsData
    sDeviceUnitInfo hrdInfo;
    hrdInfo.Type = dutUMU;
    //    hrdInfo.ID = 0;
    hrdInfo.RemainingTime = -1;
    hrdInfo.SupplyVoltage = -1;

    // Заполнение UnitsData и выключение АСД
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        //  Заполним эти строки в cDevice::Tick

        hrdInfo.WorksNumber = NULL;
        hrdInfo.FirmwareVer = NULL;
        hrdInfo.FirmwareDate = NULL;
        UnitsData.push_back(hrdInfo);

        //Выключение АСД
        // UMUList.at(UMUIdx)->DisableAlarm();
    }

    ResetPathEncoder();

    SetLeftSideSwitching(cfg->getUMUConnectors());  // Установка используемого разъема БУМ - сплошного контроля или контроля сканера


    filterResetPacketProccess();

    _alarmSendTick = GetTickCount_();
    _bottomSignalSendTick = GetTickCount_();

    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) UMUList.at(UMUIdx)->EnableCalcDisCoord();

#ifdef DeviceLog
    Log->AddText("CANProtDevice: StartWork");
    Log->AddText("CANProtDevice state: dsConecting");
#endif
}

cDevice::~cDevice()  // Деструктор
{
    if (SensBySpeedState)  // Возвращение Ку к исходным значениям
        setSensDeltaForAllChannels(-SensBySpeed_summDelta);

    EndWork();
    for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
        thlist->DelTick(static_cast<int>(UMUIdx));
        delete UMUList.at(UMUIdx);
    }
    thlist->DelTick(DeviceThreadIndex);
    UMUList.clear();

    RailTypeTuningDataList.clear();


    // delete strings, allocated in constructor
    for (size_t i = 0; i < UnitsData.size(); ++i) {
        if (UnitsData[i].WorksNumber) {
            delete[] UnitsData[i].WorksNumber;
            UnitsData[i].WorksNumber = NULL;
        }
        if (UnitsData[i].FirmwareVer) {
            delete[] UnitsData[i].FirmwareVer;
            UnitsData[i].FirmwareVer = NULL;
        }
    }

    delete dt_lan;
    delete dt_padcan;
#ifdef WIN32
    delete dt_usbcan;
#endif
    //#ifdef ANDROID
    //    delete dt_usbcanand;
    //#endif

    StrobeModeList_.clear();
    ResetValidRangesArr();
}

int cDevice::connectToDevice()
{
    int res = 0;

    for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
        if (UMUList.at(UMUIdx)->getDataTransfer()->openConnection(UMUList.at(UMUIdx)->getConnectionId()) < 0) {
            res = -1;
        }
    }
    return res;
}

void cDevice::EndWork()  // Конец работы
{
    if (GetControlMode() != cmNormal) {
        SetControlMode(cmNormal, NULL);
    }

    PathEncoderSim(false);
    EndWorkFlag = true;

    if (!UMUList.empty()) {
        for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
            UMUList.at(UMUIdx)->EndWork();
        }

        while (true)  // Ожидание остановки потоков БУМ
        {
            bool EndFlag = true;
            for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
                EndFlag = EndFlag && thlist->Finished(static_cast<int>(UMUIdx));
            }
            if (EndFlag) {
                break;
            }
            SLEEP(1);
        }
    }

    while (!thlist->Finished(DeviceThreadIndex))  // Ожидание остановки потока Device->Tick
    {
        SLEEP(1);
    }

    CleanupUMUEventMgr();

    for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
        UMUList.at(UMUIdx)->getDataTransfer()->closeConnection(UMUList.at(UMUIdx)->getConnectionId());
    }
}

void cDevice::CallBackFunction_(int GroupIndex, eDeviceSide Side, CID Channel, int GateIndex, eValueID ID, int Value)
{
    if (CallBackFunction) {
        if (!tbl->isHandScan(FCurChannel))
            CallBackFunction(GroupIndex, Side, Channel, GateIndex, ID, Value);
        else
            CallBackFunction(-1, Side, Channel, GateIndex, ID, Value);
    }
}

eDeviceMode cDevice::GetDeviceMode() const  // Получить режим работы каналов дефектоскопа
{
    return FNewMode;
}

unsigned long cDevice::GetTime() const  // Время с начала работы класса в мкс
{
    return (GetTickCount_() - StartTime);
}

int cDevice::GetModeIndex() const  // Порядковый номер установленного режима
{
    //	return (int)devMode;
    return 0;
}

bool cDevice::ChannelSensCalibration()  // Настройка чувствительности выбранного канала по пороговому значению усиления результат: true - настроен; false - ошибка, канал не настроен
{
    int ParamN_;
    // Пересчет максимума из отсчетов в дБ
    float Tmp = static_cast<float>(CalAScanMax.Ampl) / 32.0f;
    // TODO: consider revising floating point comparison (Tmp != 0.0f)
    /* if (Tmp != 0.0f) */
    if (CalAScanMax.Ampl != 0)
        ParamN_ = static_cast<int>(20 * std::log10(Tmp));  // Превышение сигналом порогового уровня (N, дБ)
    else
        ParamN_ = 0;

    if ((ParamN_ > 0) && (ParamN_ < 18)) {
        sChannelDescription ChanDesc;
        tbl->ItemByCID(FCurChannel, &ChanDesc);
        // Устанавливаем АТТ равный = (текущий АТТ) - (Превышение над порогом) + (Ку рекомендованное)
        int NewAtt = GetGain() - ParamN_ + ChanDesc.RecommendedSens[FCurrentGateIdx - 1];

        if ((NewAtt < 0) || (NewAtt > (cfg->getGainMax() - cfg->getGainBase()) * cfg->getGainStep())) {
            return false;
        }
        cal->SetSens(FCurSide, FCurChannel, FCurrentGateIdx - 1, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
        cal->SetGain(FCurSide, FCurChannel, FCurrentGateIdx - 1, NewAtt);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidSens, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidTestSens, TestSensValidRanges(FCurSide, FCurChannel, FCurrentGateIdx));
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidGain, NewAtt);

        if ((ChanDesc.cdTuningMode == tmByDelta) && (FCurrentGateIdx == 1))  // Настройка дальной зоны по Дельте
        {
            cal->SetSens(FCurSide, FCurChannel, 1, ChanDesc.RecommendedSens[1]);
            cal->SetGain(FCurSide, FCurChannel, 1, NewAtt + ChanDesc.RecommendedSens[1] - ChanDesc.RecommendedSens[0]);
            CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidSens, ChanDesc.RecommendedSens[1]);
            CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidTestSens, TestSensValidRanges(FCurSide, FCurChannel, FCurrentGateIdx));
            CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidGain, NewAtt + ChanDesc.RecommendedSens[1] - ChanDesc.RecommendedSens[0]);
        }
        SetChannelParams(FCurSide, FCurChannel, true, false, false, false);


        const tChannelCalibrationCopyData& dataCopy = cfg->getCalibrationCopyData();
        // Копирование настроек в Slave канал
        for (tChannelCalibrationCopyData::const_iterator it = dataCopy.begin(); it != dataCopy.end(); ++it)
            if ((it->DeviceSide == FCurSide) && (it->Master == FCurChannel)) {
                cal->SetSens(FCurSide, it->Slave, FCurrentGateIdx - 1, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
                cal->SetGain(FCurSide, it->Slave, FCurrentGateIdx - 1, NewAtt);
                CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidSens, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
                CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidTestSens, TestSensValidRanges(FCurSide, it->Slave, FCurrentGateIdx));
                CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidGain, NewAtt);
                int pd = cal->GetPrismDelay(FCurSide, it->Master);
                cal->SetPrismDelay(FCurSide, it->Slave, pd);
                CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidPrismDelay, pd);
                int tvg = cal->GetTVG(FCurSide, it->Master);
                cal->SetTVG(FCurSide, it->Slave, tvg);
                CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidTVG, tvg);
                break;
            }

        return true;
    }
    else {
        return false;
    }
}

bool cDevice::ChannelSensCalibration_ResetSens()  // Настройка чувствительности - сброс чуствительности канала
{
    cal->SetSens(FCurSide, FCurChannel, FCurrentGateIdx - 1, 0);
    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
    return true;
}


bool cDevice::ChannelSensCalibration_Type2()  // Настройка чувствительности выбранного канала, тип 2
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    // Устанавливаем АТТ равный = (текущий АТТ) + (Ку рекомендованное)
    int NewAtt = GetGain() + ChanDesc.RecommendedSens[FCurrentGateIdx - 1];

    if ((NewAtt < 0) || (NewAtt > (cfg->getGainMax() - cfg->getGainBase()) * cfg->getGainStep())) {
        return false;
    }
    cal->SetSens(FCurSide, FCurChannel, FCurrentGateIdx - 1, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
    cal->SetGain(FCurSide, FCurChannel, FCurrentGateIdx - 1, NewAtt);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidSens, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidTestSens, TestSensValidRanges(FCurSide, FCurChannel, FCurrentGateIdx));
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidGain, NewAtt);

    if ((ChanDesc.cdTuningMode == tmByDelta) && (FCurrentGateIdx == 1))  // Настройка дальной зоны по Дельте
    {
        cal->SetSens(FCurSide, FCurChannel, 1, ChanDesc.RecommendedSens[1]);
        cal->SetGain(FCurSide, FCurChannel, 1, NewAtt + ChanDesc.RecommendedSens[1] - ChanDesc.RecommendedSens[0]);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidSens, ChanDesc.RecommendedSens[1]);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidTestSens, TestSensValidRanges(FCurSide, FCurChannel, FCurrentGateIdx));
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 2, vidGain, NewAtt + ChanDesc.RecommendedSens[1] - ChanDesc.RecommendedSens[0]);
    }

    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);

    const tChannelCalibrationCopyData& dataCopy = cfg->getCalibrationCopyData();
    // Копирование настроек в Slave канал
    for (tChannelCalibrationCopyData::const_iterator it = dataCopy.begin(); it != dataCopy.end(); ++it)
        if ((it->DeviceSide == FCurSide) && (it->Master == FCurChannel)) {
            cal->SetSens(FCurSide, it->Slave, FCurrentGateIdx - 1, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
            cal->SetGain(FCurSide, it->Slave, FCurrentGateIdx - 1, NewAtt);
            CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidSens, ChanDesc.RecommendedSens[FCurrentGateIdx - 1]);
            CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidTestSens, TestSensValidRanges(FCurSide, it->Slave, FCurrentGateIdx));
            CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidGain, NewAtt);
            int pd = cal->GetPrismDelay(FCurSide, it->Master);
            cal->SetPrismDelay(FCurSide, it->Slave, pd);
            CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidPrismDelay, pd);
            break;
        }

    return true;
}
/*
bool cDevice::TestSensValidRanges(eDeviceSide Side, CID Channel, int GateIndex) // Проверка значения условной чувствительности на отклонени от нормативного значения
{
    for(int Idx = 0; Idx < cfg->SensValidRangesList.size(); Idx++)

      if ((cfg->SensValidRangesList[Idx].Channel == Channel) &&
          (cfg->SensValidRangesList[Idx].GateIndex == GateIndex))
      {
          return  ((cal->GetSens(Side, Channel, GateIndex - 1) >= cfg->SensValidRangesList[Idx].MaxSens) &&
                   (cal->GetSens(Side, Channel, GateIndex - 1) <= cfg->SensValidRangesList[Idx].MinSens));
      }

    return false;
}
*/
int cDevice::TestSensValidRanges(eDeviceSide Side, CID Channel, int GateIndex)  // Проверка значения условной чувствительности на отклонени от нормативного значения
{
    size_t rangesCount = cfg->SensValidRangesList.size();
    for (unsigned int Idx = 0; Idx < rangesCount; ++Idx) {
        if ((cfg->SensValidRangesList[Idx].Channel == Channel) && (cfg->SensValidRangesList[Idx].GateIndex == GateIndex)) {
            if (cal->GetSens(Side, Channel, GateIndex - 1) > cfg->SensValidRangesList[Idx].MaxSens) return cal->GetSens(Side, Channel, GateIndex - 1) - cfg->SensValidRangesList[Idx].MaxSens;
            if (cal->GetSens(Side, Channel, GateIndex - 1) < cfg->SensValidRangesList[Idx].MinSens) return cal->GetSens(Side, Channel, GateIndex - 1) - cfg->SensValidRangesList[Idx].MinSens;
        }
    }
    return 0;
}

int cDevice::GetTVG() const  // Получить значение ВРЧ [мкс]
{
    return cal->GetTVG(FCurSide, FCurChannel);
}

bool cDevice::SetTVG(int New)  // Установить значение ВРЧ [мкс]
{
    if (New < 0) return false;

    int EnterAngle;
    if (tbl->GetChannelEnterAngle(&EnterAngle, FCurChannel)) {
        if (EnterAngle == 0) {
            if (New > 20) return false;
        }
        else if (New > 40)
            return false;
    }

    cal->SetTVG(FCurSide, FCurChannel, New);
    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);

    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidTVG, New);
    int tmp;
    if (tbl->GetAnotherChannelGateIndex(FCurChannel, FCurrentGateIdx, &tmp)) {
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, tmp, vidTVG, New);
    }
    return true;
}

int cDevice::GetPrismDelay() const  // Получить значение 2TP [10 * мкс]
{
    return cal->GetPrismDelay(FCurSide, FCurChannel);
}

bool cDevice::SetPrismDelay(int New)  // Установить значение 2TP [10 * мкс]
{
    if (static_cast<float>(New) / 10.0f < cfg->getPrismDelayMin()) New = static_cast<int>(cfg->getPrismDelayMin() * 10);  // TODO: consider revising float -> int rounding
    if (static_cast<float>(New) / 10.0f > cfg->getPrismDelayMax()) New = static_cast<int>(cfg->getPrismDelayMax() * 10);
    cal->SetPrismDelay(FCurSide, FCurChannel, New);
    SetChannelParams(FCurSide, FCurChannel, false, true, false, false);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidPrismDelay, New);
    int tmp;
    if (tbl->GetAnotherChannelGateIndex(FCurChannel, FCurrentGateIdx, &tmp)) {
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, tmp, vidPrismDelay, New);
    }

    const tChannelCalibrationCopyData& dataCopy = cfg->getCalibrationCopyData();
    // Копирование настроек в Slave канал
    for (tChannelCalibrationCopyData::const_iterator it = dataCopy.begin(); it != dataCopy.end(); ++it)
        if ((it->DeviceSide == FCurSide) && (it->Master == FCurChannel)) {
            int pd = cal->GetPrismDelay(FCurSide, it->Master);
            cal->SetPrismDelay(FCurSide, it->Slave, pd);
            CallBackFunction_(FCurGroupIndex, FCurSide, it->Slave, FCurrentGateIdx, vidPrismDelay, pd);
            break;
        }

    return true;
}

int cDevice::GetSens() const  // Получить значение Ky [дБ]
{
    return cal->GetSens(FCurSide, FCurChannel, FCurrentGateIdx - 1);
}

int cDevice::GetSens(int GateIndex) const  // Получить значение Ky [дБ]
{
    return cal->GetSens(FCurSide, FCurChannel, GateIndex - 1);
}

int cDevice::GetRecommendedSens() const  // Получить рекомендованное значение Ky [дБ]
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return ChanDesc.RecommendedSens[FCurrentGateIdx - 1];
}

int cDevice::GetRecommendedSens(int GateIndex) const  // Получить рекомендованное значение Ky [дБ]
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return ChanDesc.RecommendedSens[GateIndex - 1];
}

int cDevice::GetGateCount() const  // Количество стробов
{
    return tbl->GetGateCount(FCurChannel);
}

int cDevice::GetGateIndex() const  // Получить строб
{
    return FCurrentGateIdx;
}

bool cDevice::SetGateIndex(int Index)  // Установить строб
{
    if ((Index == 1) || (Index == 2)) {
        FNewGateIdx = Index;
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, -1, vidGateIdx, Index);
        return true;
    }
    else
        return false;
}

bool cDevice::SetSens(int New)  // Установить значение Ky [дБ]
{
    return SetSens(FCurrentGateIdx, New);
}

void cDevice::SetFixBadSensState(bool State)
{
    FixBadSensState = State;
    ValidRangesTestTime_ms = GetTickCount_();
}

void cDevice::ResetValidRangesArr()  // Сброс массива слежения за Ку
{
    ValidRangesArr.clear();
    ValidRangesArr.reserve(32);
}

void cDevice::SetTimeToFixBadSens(int TimeToFixBadSens_)  // Период проверки Ку (сек)
{
    TimeToFixBadSens = TimeToFixBadSens_;
}

bool cDevice::SetSens(int GateIndex, int New)  // Установить значение Ky [дБ]
{
    int Delta = New - GetSens(GateIndex);
    if ((GetGain() + Delta < 0) || (GetGain(GateIndex) + Delta > (cfg->getGainMax() - cfg->getGainBase()) * cfg->getGainStep())) {
        return false;
    }

    cal->SetSens(FCurSide, FCurChannel, GateIndex - 1, New);
    int NewGain = GetGain(GateIndex) + Delta;
    cal->SetGain(FCurSide, FCurChannel, GateIndex - 1, NewGain);

    // Для функция возвращения Ку к рекомендованному через TimeToFixBadSens сек
    int TestSensValid = CheckSetBadSens(FCurSide, FCurChannel, FCurrentGateIdx);
    /*
        int TestSensValid = TestSensValidRanges(FCurSide, FCurChannel, FCurrentGateIdx);
        int ValidRangesIdx = -1;

        for (std::vector<tKuValidRanges>::iterator it = ValidRangesArr.begin(); it != ValidRangesArr.end(); ++it) {
            tKuValidRanges& cur = it.operator*();
            if ((cur.Side == FCurSide) && (cur.Channel == FCurChannel) && (cur.GateIdx == FCurrentGateIdx)) {
                ValidRangesIdx = std::distance(ValidRangesArr.begin(), it);
                break;
            }
        }

        if (TestSensValid == 0) {
            if (ValidRangesIdx != -1) ValidRangesArr[ValidRangesIdx].BadState = false;
        }
        else {
            if (ValidRangesIdx == -1) {
                tKuValidRanges tmp;
                tmp.Time = GetTickCount_();
                tmp.BadState = true;
                sChannelDescription ChanDesc;
                tbl->ItemByCID(FCurChannel, &ChanDesc);
                tmp.GoodSens = ChanDesc.RecommendedSens[FCurrentGateIdx - 1];
                tmp.Side = FCurSide;
                tmp.Channel = FCurChannel;
                tmp.GateIdx = FCurrentGateIdx;
                ValidRangesArr.push_back(tmp);
            }
            else {
                ValidRangesArr[ValidRangesIdx].BadState = true;
                ValidRangesArr[ValidRangesIdx].Time = GetTickCount_();
            }
        }
    */
    // ------------------------------------------------------------

    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidSens, New);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidTestSens, TestSensValid);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidGain, NewGain);
    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
    return true;
}

int cDevice::GetGain()  // Получить значение Aтт [дБ]
{
    return cal->GetGain(FCurSide, FCurChannel, FCurrentGateIdx - 1);
}

int cDevice::GetGain(int GateIndex)  // Получить значение Aтт [дБ]
{
    return cal->GetGain(FCurSide, FCurChannel, GateIndex - 1);
}

bool cDevice::SetGain(int New)  // Установить значение Aтт [дБ]
{
    return SetGain(FCurrentGateIdx, New);
}

bool cDevice::SetGain(int GateIndex, int New)  // Установить значение Aтт [дБ]
{
    if ((New < 0) || (New > (cfg->getGainMax() - cfg->getGainBase()) * cfg->getGainStep())) {
        return false;
    }
    cal->SetGain(FCurSide, FCurChannel, GateIndex - 1, New);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidGain, New);
    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
    return true;
}

GateType cDevice::GetStGate()  // Получить значение начала строба [мкс]
{
    return cal->GetStGate(FCurSide, FCurChannel, FCurrentGateIdx - 1);
}

GateType cDevice::GetStGate(int GateIndex)  // Получить значение начала строба [мкс]
{
    return cal->GetStGate(FCurSide, FCurChannel, GateIndex - 1);
}

bool cDevice::SetStGate(GateType New, bool SkipTestGateLen)  // Установить значение начала строба [мкс]
{
    return SetStGate(FCurrentGateIdx, New, SkipTestGateLen);
}

bool cDevice::SetStGate(int GateIndex, GateType New, bool SkipTestGateLen)  // Установить значение начала строба [мкс]
{
    if ((FCurMode != dmCalibration) && (!FCalibrationMode)) {
        if (!tbl->GetPermissionToChangeGate(FCurChannel, GateIndex, true)) {
            return false;
        }
    }

    if (!SkipTestGateLen) {
        // Проверка длины строба для режима настройки - минимальная длинна 6 мм
        if ((FCurMode == dmCalibration) && (GetEdGate(GateIndex) - New < tbl->RayMMToDelay(FCurChannel, 6))) {
            New = GetEdGate(GateIndex) - tbl->RayMMToDelay(FCurChannel, 6);
        }
        if ((FCurMode != dmCalibration) && (GetEdGate(GateIndex) - New < tbl->GetChannelGateMinLen(FCurChannel, GateIndex - 1))) {
            if (!tbl->GetPermissionToChangeGate(FCurChannel, GateIndex, false)) return false;
            SetEdGate(GateIndex, New + tbl->GetChannelGateMinLen(FCurChannel, GateIndex - 1), true);
        }
    }
    // Устанавливаем требуемое значение
    cal->SetStGate(FCurSide, FCurChannel, GateIndex - 1, New);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidStGate, New);
    SetChannelParams(FCurSide, FCurChannel, true, false, true, false);
    return true;
}

GateType cDevice::GetEdGate(/*bool forBScan*/)  // Получить значение конца строба [мкс]
{
    /*
    if (forBScan && (FCurChannel == N55MSLW)) { // Временно для переворота стробв канала N55MSLW

        return std::max(0, 45 - ((cal->GetEdGate(FCurSide, FCurChannel, FCurrentGateIdx - 1) - 0)));
    } else */
    return cal->GetEdGate(FCurSide, FCurChannel, FCurrentGateIdx - 1);
}

GateType cDevice::GetEdGate(int GateIndex /*, bool forBScan*/)  // Получить значение конца строба [мкс]
{
    /*    if (forBScan && (FCurChannel == N55MSLW)) { // Временно для переворота стробв канала N55MSLW

        return std::max(0, 45 - ((cal->GetEdGate(FCurSide, FCurChannel, GateIndex - 1) - 0)));
    } else */
    return cal->GetEdGate(FCurSide, FCurChannel, GateIndex - 1);
}

bool cDevice::SetEdGate(GateType New, bool SkipTestGateLen)  // Установить значение конца строба [мкс]
{
    return SetEdGate(FCurrentGateIdx, New, SkipTestGateLen);
}

bool cDevice::SetEdGate(int GateIndex, GateType New, bool SkipTestGateLen)  // Установить значение конца строба [мкс]
{
    if ((FCurMode == dmCalibration) && (FCurCalMode == cmSens) && (!FCalibrationMode)) {
        return false;
    }
    if ((FCurMode != dmCalibration) && (!FCalibrationMode)) {
        if (!tbl->GetPermissionToChangeGate(FCurChannel, GateIndex, false)) {
            return false;
        }
    }
    if (!SkipTestGateLen) {  // Осущнствляется контроль длинны строба
        // Проверка длины строба для режима настройки - минимаьная длинна 6 мм
        if ((FCurMode == dmCalibration) && (New - GetStGate() < tbl->RayMMToDelay(FCurChannel, 6))) New = GetStGate() + tbl->RayMMToDelay(FCurChannel, 6);
        // Проверка длины строба для режима поиска
        if ((FCurMode != dmCalibration) && (New - GetStGate() < tbl->GetChannelGateMinLen(FCurChannel, GateIndex - 1))) New = GetStGate() + tbl->GetChannelGateMinLen(FCurChannel, GateIndex - 1);
    }
    // Устанавливаем требуемое значение
    cal->SetEdGate(FCurSide, FCurChannel, GateIndex - 1, New);
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, GateIndex, vidEdGate, New);
    SetChannelParams(FCurSide, FCurChannel, true, false, true, false);
    return true;
}

int cDevice::GetCalibrationTemperature()
{
    return cal->GetCalibrationTemperature(FCurSide, FCurChannel);
}

bool cDevice::SetCalibrationTemperature(int NewVal)
{
    cal->SetCalibrationTemperature(FCurSide, FCurChannel, NewVal);
    return true;
}

bool cDevice::GetSnapShotFileID(sSnapshotFileID* pFileID)
{
    cal->GetSnapShotFileID(pFileID, FCurSide, FCurChannel, FCurrentGateIdx - 1);
    return true;
}

bool cDevice::SetSnapShotFileID(int GateIdx, sSnapshotFileID pFileID)
{
    cal->SetSnapShotFileID(FCurSide, FCurChannel, GateIdx - 1, pFileID);
    return true;
}


void cDevice::StartUseMark()  // Включение метки
{
    if (!MarkFlag) {
        MarkFlag = true;
        if (cal->GetMark(FCurSide, FCurChannel) != 0) {
            cal->SetMark(FCurSide, FCurChannel, std::abs(cal->GetMark(FCurSide, FCurChannel)));
        }
        else {
            cal->SetMark(FCurSide, FCurChannel, GetStrokeLen() / 2);
        }
        SetChannelParams(FCurSide, FCurChannel, false, false, true, false);
        Update(false, true);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, -1, vidGateIdx, 3);
    }
}

void cDevice::StopUseMark()  // Выключение метки
{
    if (MarkFlag) {
        MarkFlag = false;
        cal->SetMark(FCurSide, FCurChannel, -1 * std::abs(cal->GetMark(FCurSide, FCurChannel)));
        Update(false, true);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, -1, vidGateIdx, FCurrentGateIdx);
    }
}

int cDevice::GetMark()  // Получить значение метки [мкс]
{
    return cal->GetMark(FCurSide, FCurChannel) - 1;
}

bool cDevice::GetMarkState()
{
    return GetMark() > 0;
}

bool cDevice::SetMark(int New)  // Установить значение метки [мкс]
{
    if ((New >= 0) && (New < GetAScanLen())) {
        cal->SetMark(FCurSide, FCurChannel, New + 1);
        SetChannelParams(FCurSide, FCurChannel, false, false, true, false);
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, FCurrentGateIdx, vidMark, New);
        return true;
    }
    return false;
}

int cDevice::GetMarkWidth()  // Получить ширину метки [мкс]
{
    return MarkWidth;
}

void cDevice::SetMarkWidth(int New)  // Установить ширину метки [мкс]
{
    MarkWidth = New;
}

int cDevice::GetStMark()  // Получить начало метки [мкс]
{
    return std::max(0, static_cast<int>(cal->GetMark(FCurSide, FCurChannel) - MarkWidth / 2.0));
}

int cDevice::GetEdMark()  // Получить конец метки [мкс]
{
    return static_cast<int>(cal->GetMark(FCurSide, FCurChannel) + MarkWidth / 2.0);
}

bool cDevice::GetNotchState() const  // Есть ли в текущем канале полка
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return ChanDesc.cdUseNotch;
}

int cDevice::GetStNotch()  // Получить начало полки [мкс]
{
    return cal->GetNotchStart(FCurSide, FCurChannel, 0);
}

int cDevice::GetNotchLen()  // Получить длительность полки [мкс]
{
    return cal->GetNotchDur(FCurSide, FCurChannel, 0);
}

int cDevice::GetNotchLevel()  // Получить уровень ослабления в пределах полки [дБ]
{
    return cal->GetNotchLevel(FCurSide, FCurChannel, 0);
}

bool cDevice::SetStNotch(int NewVal)  // Установить начало полки [мкс]
{
    if (FCurrentControlMode != cmNormal) {
        return false;
    }

    if (NewVal < GetTVG()) {
        NewVal = GetTVG();
    }

    if ((NewVal >= GetTVG()) && (NewVal + GetNotchLen() <= GetEdGate(1))) {  // Проверка что полочка помещается в строб ближней зоны
        cal->SetNotchStart(FCurSide, FCurChannel, 0, NewVal);                // Задать начало полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 1, vidStNotch, NewVal);
        SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
        return true;
    }
    return false;
}

bool cDevice::SetNotchLen(int NewVal)  // Установить длительность полки [мкс]
{
    if (FCurrentControlMode != cmNormal) {
        return false;
    }

    if ((NewVal > 0) && (GetStNotch() >= GetTVG()) && (GetStNotch() + NewVal <= GetEdGate(1))) {  // Проверка что полочка помещается в строб ближней зоны

        cal->SetNotchDur(FCurSide, FCurChannel, 0, NewVal);  // Задать начало полки [мкс]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
        CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 1, vidNotchLen, NewVal);
        SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
        return true;
    }
    return false;
}

void cDevice::SetNotchLevel(int NewVal)  // Установить уровень ослабления в пределах полки [дБ]
{
    if (FCurrentControlMode != cmNormal) {
        return;
    }

    if (NewVal < 0) {
        NewVal = 0;
    }
    cal->SetNotchLevel(FCurSide, FCurChannel, 0, NewVal);  // Задать уровень ослабления для полки [дБ]; GateIdx = 0 (ближняя зона), GateIdx = 1 (дальняя зона)
    CallBackFunction_(FCurGroupIndex, FCurSide, FCurChannel, 1, vidNotchLevel, NewVal);
    SetChannelParams(FCurSide, FCurChannel, true, false, false, false);
}

int cDevice::GetStrokeLen() const  // Длительность такта [мкс]
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return ChanDesc.cdStrokeDuration;
}

int cDevice::GetAScanLen() const  // Длительность развертки [мкс]
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return ChanDesc.cdAScanScale * 232 / 10;
}

int cDevice::GetScanZeroProbeAmpl() const  // Получить амплитуду донного сигнала канала сплошного контроля
{
    for (tRailTypeTuningDataList::const_iterator it = RailTypeTuningDataList.begin(); it != RailTypeTuningDataList.end(); ++it) {
        const sRailTypeTuningData& cur = it.operator*();
        if ((GetTickCount_() - cur.Time < 100) && (cur.Channel == 0x3D)) {
            return cur.MaxAmp;
        }
    }
    return 0;
}

int cDevice::GetCurrentBScanSessionID() const  // Получить текущую сессию В-развертки
{
    return CurrentBScanSessionID;
}

int cDevice::GetBScanSessionID(eChannelType ChannelType) const  // Получить сессию В-развертки для заданного типа канала
{
    return BScanSessionID[ChannelType];
}

void cDevice::CleanupUMUEventMgr()
{
    unsigned long EventId;
    while (UMU_Event->EventDataSize() >= 8) {
        UMU_Event->ReadEventData(&EventId, SIZE_OF_TYPE(&EventId), NULL);
        cs->Enter();
        switch (EventId) {
        case edAScanMeas:  // А-развертка, точное значение амплитуды и задержки
        {
            PtUMU_AScanMeasure AScanMeasure_ptr;
            UMU_Event->ReadEventData(&AScanMeasure_ptr, SIZE_OF_TYPE(&AScanMeasure_ptr), NULL);
            delete AScanMeasure_ptr;
            break;
        }
        case edAScanData:  // AScan
        {
            PtUMU_AScanData AScanData_ptr;
            UMU_Event->ReadEventData(&AScanData_ptr, SIZE_OF_TYPE(&AScanData_ptr), NULL);
            delete AScanData_ptr;
            break;
        }
        case edTVGData:  // Отсчеты ВРЧ
        {
            PtUMU_AScanData TVGData_ptr;
            UMU_Event->ReadEventData(&TVGData_ptr, SIZE_OF_TYPE(&TVGData_ptr), NULL);
            delete TVGData_ptr;
            break;
        }
        case edAlarmData: {
            PtUMU_AlarmItem Alarm_ptr;
            UMU_Event->ReadEventData(&Alarm_ptr, SIZE_OF_TYPE(&Alarm_ptr), NULL);
            delete Alarm_ptr;
            break;
        }
        case edBScan2Data:  // BScan
        {
            PtUMU_BScanData BScan2Data_ptr;
            UMU_Event->ReadEventData(&BScan2Data_ptr, SIZE_OF_TYPE(&BScan2Data_ptr), NULL);
            delete BScan2Data_ptr;
            break;
        }
        case edMScan2Data:  // MScan
        {
            PtUMU_BScanData BScan2Data_ptr;
            UMU_Event->ReadEventData(&BScan2Data_ptr, SIZE_OF_TYPE(&BScan2Data_ptr), NULL);
            delete BScan2Data_ptr;
            break;
        }
        case edMSensor:  // Данные от датчиков металла
        {
            PtUMU_MetalSensorData MetalSensorData_ptr;
            UMU_Event->ReadEventData(&MetalSensorData_ptr, SIZE_OF_TYPE(&MetalSensorData_ptr), NULL);
            delete MetalSensorData_ptr;
            break;
        }
        case edPathStepData:  // Данные датчика пути
        {
            PtUMU_PathStepData PathStepData_ptr;
            UMU_Event->ReadEventData(&PathStepData_ptr, SIZE_OF_TYPE(&PathStepData_ptr), NULL);
            delete PathStepData_ptr;
            break;
        }
        case edA15ScanerPathStepData:  // Данные датчика пути сканера А15
        {
            PtUMU_A15ScanerPathStepData A15ScanerPathStepData_ptr;
            UMU_Event->ReadEventData(&A15ScanerPathStepData_ptr, SIZE_OF_TYPE(&A15ScanerPathStepData_ptr), NULL);
            delete A15ScanerPathStepData_ptr;
            break;
        }
        }
        cs->Release();
    }
}

void cDevice::filterResetPacketProccess()
{
    for (unsigned char first = 0; first < MaxSideCount; ++first) {
        for (unsigned char idx = 0; idx < CHANNELS_TOTAL; ++idx) {
            FiltrationData& data = _filtrationData[first][idx];
            std::vector<sMaskItem>& currentMaskList = data.masks;
            if (!currentMaskList.empty()) {
                for (std::vector<sMaskItem>::iterator it = currentMaskList.begin(), total = currentMaskList.end(); it != total; ++it) {
                    it.operator*().reset();
                }
                _filtrationData[first][idx].partition = currentMaskList.begin();
            }
        }
    }

    _fTailCoord = _sysCoord;
    _fPrevDir = _fCurrentDir;
    _fPrevSysCoord = _sysCoord;

    if (!fUMUParamsStorage.empty()) {
        fUMUParamsStorage.clear();
    }
}

void cDevice::filterInit()
{
    filterResetPacketProccess();
    _fFirstCoord = true;
}

bool cDevice::filterCheck53Defect(sMaskItem& mask, std::vector<sMaskItem>::iterator beginIt, std::vector<sMaskItem>::iterator endIt)
{
    unsigned char otherMaskDefectSignals[MASKS_MAX_COUNT];
    memset(otherMaskDefectSignals, 0, sizeof(unsigned char) * MASKS_MAX_COUNT);

    float angleCoeff = std::fabs(static_cast<float>(mask.StartDelay - mask.LastDelay) / (static_cast<float>(mask.StartDisplayCoord - mask.LastDisplayCoord)));

    int shift = 0;

    if (mask.StartDelay > mask.LastDelay) {
        shift = mask.StartDelay + static_cast<int>(angleCoeff * mask.StartDisplayCoord);
    }
    else if (mask.StartDelay < mask.LastDelay) {
        shift = mask.StartDelay - static_cast<int>(angleCoeff * mask.StartDisplayCoord);
    }

    // Пробегаем все координаты переданной маски
    for (int coord = mask.StartDisplayCoord; coord < (mask.LastDisplayCoord + 24); ++coord) {
        int maskDelay = mask.StartDelay;

        if (mask.StartDelay > mask.LastDelay) {
            maskDelay = static_cast<int>(-1 * angleCoeff * static_cast<float>(coord) + shift);
        }
        else if (mask.StartDelay < mask.LastDelay) {
            maskDelay = static_cast<int>(angleCoeff * static_cast<float>(coord) + shift);
        }

        // Пробегаем все возможные маски этой стороны и канала
        for (std::vector<sMaskItem>::const_iterator maskIt = beginIt; maskIt != endIt; ++maskIt) {
            const sMaskItem& otherMask = maskIt.operator*();
            // Находим маску отличную от переданной

#ifndef Delete_SignalsM6dB
            if (otherMask.Busy && (&otherMask != &mask) && (!otherMask.SignalsM6dB.empty())) {
                // Пробегаем все сигналы найденой маски, проверям их на условия
                // дефекта 53.1 снизу и сверху по задерже от переданной маски
                // результаты считаем и возвращаем true при превышении порогового количества.

                for (std::deque<sMaskSignalItem>::const_iterator it = otherMask.SignalsM6dB.begin(), total = otherMask.SignalsM6dB.end(); it != total; ++it) {
                    const sMaskSignalItem& item = it.operator*();

                    if (item.DisCoord == coord) {
                        int delta = maskDelay - item.Signal.Delay;

                        if (std::abs(delta) >= 4 && std::abs(delta) <= 18) {
                            delta > 0 ? mask.Lower53SignalCount++ : mask.Upper53SignalCount++;
                            int index = maskIt - beginIt;
                            DEFCORE_ASSERT(index >= 0 && index < MASKS_MAX_COUNT);
                            otherMaskDefectSignals[index]++;
                        }
                    }
                }
            }
#endif
#ifdef Delete_SignalsM6dB
            if (otherMask.Busy && (&otherMask != &mask) && (!otherMask.Signals_.empty())) {
                // Пробегаем все сигналы найденой маски, проверям их на условия
                // дефекта 53.1 снизу и сверху по задерже от переданной маски
                // результаты считаем и возвращаем true при превышении порогового количества.

                for (std::vector<sMaskSignalItem>::const_iterator it = otherMask.Signals_.begin(), total = otherMask.Signals_.end(); it != total; ++it) {
                    const sMaskSignalItem& item = it.operator*();

                    if (!(AmplIdtoDB[item.Signal.Ampl[AmplDBIdx_int] & 0x0F] >= -6)) {
                        continue;
                    }

                    if (item.DisCoord == coord) {
                        int delta = maskDelay - item.Signal.Delay;

                        if (std::abs(delta) >= 4 && std::abs(delta) <= 18) {
                            delta > 0 ? mask.Lower53SignalCount++ : mask.Upper53SignalCount++;
                            int index = maskIt - beginIt;
                            DEFCORE_ASSERT(index >= 0 && index < MASKS_MAX_COUNT);
                            otherMaskDefectSignals[index]++;
                        }
                    }
                }
            }
#endif
        }
    }

    if (mask.Upper53SignalCount >= DEFECT_53_SIGNALS_COUNT_MAX || mask.Lower53SignalCount >= DEFECT_53_SIGNALS_COUNT_MAX) {
        // Событие дефект 53.1
        mask.Defect53_1Flag = true;

        // Выставляем флаг дефекта для масок попавших под условия.
        for (std::vector<sMaskItem>::iterator maskIt = beginIt; maskIt != endIt; ++maskIt) {
            int index = maskIt - beginIt;
            DEFCORE_ASSERT(index >= 0 && index < MASKS_MAX_COUNT);
            if (otherMaskDefectSignals[index] >= 3) {
                maskIt.operator*().Defect53_1Flag = true;
            }
        }
        return true;
    }
    return false;
}

void cDevice::filterCreateAlarmData(tBScan2Items::const_iterator beginIt, tBScan2Items::const_iterator endIt)
{
    unsigned long currentTick = GetTickCount_();
    if ((currentTick - _alarmSendTick) >= ALARM_SEND_INTERVAL) {
        _alarmSendTick = currentTick;

        tDEV_AlarmHead* DEV_AlarmHead = new (tDEV_AlarmHead);

        bool MSMState[3];
        MSMState[0] = false;
        MSMState[1] = false;
        MSMState[2] = false;

        unsigned int tmp = 0;
        DEV_AlarmHead->Items.resize(static_cast<size_t>(std::distance(beginIt, endIt)));

        for (tBScan2Items::const_iterator it = beginIt; it != endIt; ++it) {
            const tDEV_BScan2HeadItem& currentItem = it.operator*();
            const eDeviceSide Side_ = currentItem.Side;
            DEFCORE_ASSERT(static_cast<unsigned int>(Side_) < 3);
            const CID Channel_ = currentItem.Channel;
            tStrobeMode& currentStrobeMode = StrobeModeList_[static_cast<unsigned int>(Channel_)];

            MSMState[static_cast<unsigned int>(Side_)] = MSMState[static_cast<unsigned int>(Side_)] || (Channel_ == 1);

            tDEV_AlarmHeadItem& currentAlarmItem = DEV_AlarmHead->Items.at(tmp);

            currentAlarmItem.Side = Side_;
            currentAlarmItem.Channel = Channel_;
            currentAlarmItem.State[0] = false;  // Изначаьно считаем что АСД нет
            currentAlarmItem.State[1] = false;
            currentAlarmItem.State[2] = false;
            currentAlarmItem.State[3] = false;

            const int channelMultiply = GetChannelBScanDelayMultiply(Channel_);
            const int evaluationGateLevel = cfg->getEvaluationGateLevel();
            int tmp2[MaxSignalAtBlock];

            for (unsigned char gate_idx = 1; gate_idx <= 2; ++gate_idx) {
                const GateType startGate = static_cast<GateType>(cal->GetStGate(Side_, Channel_, gate_idx - 1));
                const GateType endGate = static_cast<GateType>(cal->GetEdGate(Side_, Channel_, gate_idx - 1));

                for (unsigned int j = 0; j < currentItem.Signals.size(); ++j) {
                    DEFCORE_ASSERT(j < MaxSignalAtBlock);
                    const tUMU_BScanSignal& currentSignal = currentItem.Signals[j];
                    const unsigned char localDelay = currentSignal.Delay;
                    const int divValue = localDelay / channelMultiply;
                    const bool signalInStrobe = divValue >= startGate && divValue <= endGate;
                    const bool signalOutOfThreshold = currentSignal.Ampl[MaxAmpl] >= evaluationGateLevel;

                    // Провека срабытывания АСД для одного из сигналов
                    // Провека осуществляется без учета режима работы (по привышения/ по принижение) - так как это учитывается дальше

                    if (signalInStrobe && signalOutOfThreshold) {
                        tmp2[j] = divValue;
                    }
                    else {
                        tmp2[j] = -1;
                    }
                }

                if (currentStrobeMode.Modes[Side_][gate_idx].Mode == amOneEcho) {
                    for (unsigned int j = 0; j < currentItem.Signals.size(); ++j) {
                        DEFCORE_ASSERT(j < MaxSignalAtBlock);
                        if (tmp2[j] != -1) {
                            currentAlarmItem.State[gate_idx] = true;
                            break;
                        }
                    }
                }
                else if (currentStrobeMode.Modes[Side_][gate_idx].Mode == amTwoEcho) {
                    for (unsigned int j = 0; j < currentItem.Signals.size(); ++j) {
                        DEFCORE_ASSERT(j < MaxSignalAtBlock);
                        for (unsigned int k = 0; k < currentItem.Signals.size(); ++k) {
                            DEFCORE_ASSERT(k < MaxSignalAtBlock);
                            if ((tmp2[j] != -1) && (tmp2[k] != -1) && (j != k)) {
                                currentAlarmItem.State[gate_idx] = (std::abs(tmp2[j] - tmp2[k]) >= 4) && (std::abs(tmp2[j] - tmp2[k]) <= 12);
                                break;
                            }
                        }
                    }
                }
            }

            int gate_idx = 3;

            const GateType startGate = static_cast<GateType>(cal->GetStGate(Side_, Channel_, gate_idx - 1));
            const GateType endGate = static_cast<GateType>(cal->GetEdGate(Side_, Channel_, gate_idx - 1));

            for (unsigned int j = 0; j < currentItem.Signals.size(); ++j) {
                DEFCORE_ASSERT(j < MaxSignalAtBlock);
                const int val = currentItem.Signals[j].Delay / channelMultiply;
                const bool signalInStrobe = (val >= startGate) && (val <= endGate);
                const bool signalOutOfThreshold = currentItem.Signals[j].Ampl[MaxAmpl] >= cfg->getEvaluationGateLevel();
                currentAlarmItem.State[gate_idx] = currentAlarmItem.State[gate_idx] || (signalInStrobe && signalOutOfThreshold);
            }

            // Применение sAlarmInfo из StrobeModeList для данных АСД
            for (unsigned char i = 1; i <= 2; ++i) {
                if (currentStrobeMode.Modes[Side_][i].Alg == aaNone) {
                    currentAlarmItem.State[i] = false;  // АСД выключенно
                }
                else if (currentStrobeMode.Modes[Side_][i].Alg == aaByAbsence) {  // Если надо то инвертирует АСД
                    currentAlarmItem.State[i] = !currentAlarmItem.State[i];
                }
            }
            tmp++;
        }
        // Дополняем данные информацией о АСД для каналов 0 град, если в них не было сигналов

        if (!MSMState[1]) {  // Левая нить
            unsigned int tmp = DEV_AlarmHead->Items.size();
            DEV_AlarmHead->Items.resize(tmp + 1);
            DEV_AlarmHead->Items[tmp].Side = dsLeft;
            DEV_AlarmHead->Items[tmp].Channel = 1;
            DEV_AlarmHead->Items[tmp].State[0] = false;
            DEV_AlarmHead->Items[tmp].State[1] = false;
            DEV_AlarmHead->Items[tmp].State[2] = true;
            DEV_AlarmHead->Items[tmp].State[3] = false;
        }

        if (!MSMState[2]) {  // Правая нить
            unsigned int tmp = DEV_AlarmHead->Items.size();
            DEV_AlarmHead->Items.resize(tmp + 1);
            DEV_AlarmHead->Items[tmp].Side = dsRight;
            DEV_AlarmHead->Items[tmp].Channel = 1;
            DEV_AlarmHead->Items[tmp].State[0] = false;
            DEV_AlarmHead->Items[tmp].State[1] = false;
            DEV_AlarmHead->Items[tmp].State[2] = true;
            DEV_AlarmHead->Items[tmp].State[3] = false;
        }

        DEV_Event->WriteHeadAndPost(edAlarmData, &DEV_AlarmHead, SIZE_OF_TYPE(&DEV_AlarmHead));
    }
}

void cDevice::checkPereskok(PtUMU_BScanData data)
{
    const tUMU_BScanSignals& signalArray = data->Signals;
    const tUMUtoDEVList& compInspList = UMUtoDEVList[ctCompInsp];
    for (tUMUtoDEVList::const_iterator it = compInspList.begin(), totalUMUDEVIt = compInspList.end(); it != totalUMUDEVIt; ++it)
        if (it->UMUIndex == data->UMUIdx) {
            unsigned char count = signalArray.Count[it->UMUSide][it->UMULine][it->UMUStroke];

            if (count != 0) {
                sChannelDescription desc;
                tbl->ItemByCID(it->DEVChannel, &desc);
                bool bad = false;
                for (unsigned char i = 0; i < count; ++i) {
                    const tUMU_BScanSignal& signal = signalArray.Data[it->UMUSide][it->UMULine][it->UMUStroke][i];
                    int delay = signal.Delay / desc.cdBScanDelayMultiply;

                    if (delay > desc.cdBScanGate.gEnd) {
                        std::cerr << "PERESKOK! " << delay << " (" << static_cast<int>(signal.Delay) << "/" << static_cast<int>(desc.cdBScanDelayMultiply) << ") "
                                  << "[" << static_cast<int>(desc.cdBScanGate.gEnd) << "]"
                                  << " SLS:" << static_cast<int>(it->UMUSide) << " " << static_cast<int>(it->UMULine) << " " << static_cast<int>(it->UMUStroke) << " CID: 0x" << std::hex << static_cast<int>(it->DEVChannel) << std::dec << " SIDE:" << static_cast<int>(it->DEVSide)
                                  << " COUNT:" << static_cast<int>(count) << std::endl;
                        bad = true;
                    }
                }
                if (bad) {
                    std::cerr << " BAD PACKET: [";
                    for (unsigned char i = 0; i < count; ++i) {
                        const tUMU_BScanSignal& signal = signalArray.Data[it->UMUSide][it->UMULine][it->UMUStroke][i];
                        std::cerr << static_cast<int>(signal.Delay);
                        if (i != (count - 1)) {
                            std::cerr << ",";
                        }
                    }
                    std::cerr << "]" << std::endl;
                }
            }
        }
}

void cDevice::filterSendSpacing(tDevSignalSpacingFlag& currentSpacingFlag, int tailCoord, const tUMUtoDEVitem& curItem, const tUMU_BScanSignal& signalItem, int displCoord, int spacingLength, unsigned char spLevel)
{
    DEFCORE_ASSERT(spacingLength >= 0);
    DEFCORE_ASSERT(curItem.DEVChannel == N0EMS);
    PtDEV_SignalSpacing ptSignalSpacing = new (tDEV_SignalSpacing);
    ptSignalSpacing->Side = curItem.DEVSide;
    ptSignalSpacing->Channel = N0EMS;

    switch (spLevel) {
    case spclevelMinus6:
        ptSignalSpacing->SpacingLevel = -6;
        break;
    case spclevelZero:
        ptSignalSpacing->SpacingLevel = 0;
        break;
    }

    ptSignalSpacing->StSysCrd = currentSpacingFlag.StartCoord;
    ptSignalSpacing->EdSysCrd = tailCoord;
    ptSignalSpacing->StDisplayCrd = currentSpacingFlag.StartDisplayCoord;

    ptSignalSpacing->EdDisplayCrd = displCoord >= 0 ? displCoord : 0;
    ptSignalSpacing->Length = spacingLength;

    ptSignalSpacing->StDelay = currentSpacingFlag.StartDelay;
    ptSignalSpacing->EdDelay = signalItem.Delay;

    ptSignalSpacing->ThereAre0EchoSignals = currentSpacingFlag.Echo0Count >= NASTR_QUALITY_0ECHO_SIGNALS;

    DEV_Event->WriteHeadAndPost(edSignalSpacing, &ptSignalSpacing, SIZE_OF_TYPE(PtDEV_SignalSpacing));
}

void cDevice::filterSendSignalsPacket(const sMaskItem& cur, const tUMUtoDEVitem& curItem, int length)
{
    DEFCORE_ASSERT(length >= 0);
    PtDEV_Packet DevicePacket = new (tDEV_Packet);
    DevicePacket->StDelay = cur.StartDelay;
    DevicePacket->EdDelay = cur.LastDelay;
    DevicePacket->MaxAmpl = cur.MaxAmpl;
    DevicePacket->Side = curItem.DEVSide;
    DevicePacket->Channel = curItem.DEVChannel;
    DevicePacket->Count_0dB = cur.Count_0dB;
    DevicePacket->Count_6dB = cur.Count_6dB;
    DevicePacket->StSysCrd = cur.StartSysCoord;
    DevicePacket->EdSysCrd = cur.LastSysCoord;
    DevicePacket->StDisplayCrd = cur.StartDisplayCoord;
    DevicePacket->EdDisplayCrd = cur.LastDisplayCoord;
    DevicePacket->Length = length;
    DEV_Event->WriteHeadAndPost(edSignalPacket, &DevicePacket, SIZE_OF_TYPE(PtDEV_Packet));
}

void cDevice::filterSendDefect53_1(const sMaskItem& cur, const tUMUtoDEVitem& curItem)
{
    tDEV_Defect53_1* Defect53 = new tDEV_Defect53_1;

    Defect53->Side = curItem.DEVSide;
    Defect53->Channel = curItem.DEVChannel;
    Defect53->StDisplayCrd = cur.StartDisplayCoord;
    Defect53->EdDisplayCrd = cur.LastDisplayCoord;
    Defect53->StDelay = cur.StartDelay;
    Defect53->EdDelay = cur.LastDelay;

    DEV_Event->WriteHeadAndPost(edDefect53_1, &Defect53, SIZE_OF_TYPE(Defect53));
}

void cDevice::filterAnalyzeBottomSignal(const tUMU_BScanSignal& signalItem, const tUMUtoDEVitem& curItem, const GateType bottomStartStrobe, int tailCoord, int displCoord)
{
    DEFCORE_ASSERT(curItem.DEVChannel == N0EMS);
    const unsigned int dividedDelay = static_cast<unsigned int>(signalItem.Delay / 3);
    const unsigned char mskside = curItem.DEVSide - 1;
    if (dividedDelay >= bottomStartStrobe) {
        signed char ampl = AmplIdtoDB[signalItem.Ampl[AmplDBIdx_int] & 0x0F];

        if (ampl >= -6) {
            _bottomSignalSpacingData[mskside][spclevelMinus6].noBottomSinal = false;
            _bottomSignalSpacingData[mskside][spclevelMinus6].lastBottomSignalDelay = signalItem.Delay;  // сохраняем задержку
        }                                                                                                // для определения задержки начала разрыва

        if (ampl >= 0) {
            _bottomSignalSpacingData[mskside][spclevelZero].noBottomSinal = false;
            _bottomSignalSpacingData[mskside][spclevelZero].lastBottomSignalDelay = signalItem.Delay;  // сохраняем задержку
        }

        for (unsigned char spLevel = spclevelMinus6; spLevel < spclevelTotal; ++spLevel) {
            BottomSignalSpacingData& data = _bottomSignalSpacingData[mskside][spLevel];
            if (data.fSignalSpacingFlag.Flag && !data.noBottomSinal) {
                data.fSignalSpacingFlag.Flag = false;
                int spacingLength = std::abs(tailCoord - data.fSignalSpacingFlag.StartCoord);
                if ((FCurrentControlMode == cmNormal && spLevel == spclevelZero && spacingLength >= fltrPrmMinSpacing0dBInNormModeForEvent) || (FCurrentControlMode == cmTestBoltJoint && spLevel == spclevelMinus6 && spacingLength >= fltrPrmMinSpacingMinus6dBInBoltTestModeForEvent)) {
                    //Создаём событие прерывания линии донного сигнала
                    filterSendSpacing(data.fSignalSpacingFlag, tailCoord, curItem, signalItem, displCoord, spacingLength, spLevel);
                }
            }
        }
    }

    if ((dividedDelay >= NASTR_QUALITY_SIGNAL_ECHO_MIN_DELAY) && (dividedDelay <= NASTR_QUALITY_SIGNAL_ECHO_MAX_DELAY) && _bottomSignalSpacingData[mskside][spclevelMinus6].fSignalSpacingFlag.Flag) {
        _bottomSignalSpacingData[mskside][spclevelMinus6].fSignalSpacingFlag.Echo0Count++;
    }
}

void cDevice::filterInitMasksList(FiltrationData& currentFiltrationData)
{
    currentFiltrationData.masks.reserve(MASKS_MAX_COUNT);
    for (int i = 0; i < MASKS_MAX_COUNT; ++i) {
        currentFiltrationData.masks.push_back(sMaskItem());
    }
    currentFiltrationData.partition = currentFiltrationData.masks.begin();
}

void cDevice::changeChannelCalibrationMode()
{
    if (FNewMode == dmCalibration) {  // Изменение режима настройки канала
        switch (FNewCalMode) {
        case cmSens:
            cal->SetGateMode(gmSensCalibration);
            break;
            // Настройка условной чувствительности
        case cmPrismDelay:
            cal->SetGateMode(gmPrismDelayCalibration);
            break;
            // Настройка времени в призме
        }
    }
    else {
        if (FNewMode == dmSearch) {
            cal->SetGateMode(gmSearch);
        }
    }
}

void cDevice::changeGroupIndex()
{
    cfg->SetChannelGroup(FNewGroupIndex);

    bool ResetChanel = true;  // При смене группы сбрасываем канал если он не из новой группы
    sScanChannelDescription val;
    memset(&val, 0xff, sizeof(sScanChannelDescription));

    int idx = cfg->getFirstSChannelbyID(FNewChannel, &val);
    while (idx != -1) {
        if (val.StrokeGroupIdx == FNewGroupIndex) {
            ResetChanel = false;
            break;
        }
        idx = cfg->getNextSChannelbyID(idx, FNewChannel, &val);
    }

    if (ResetChanel) {  // Сброс канала если при смене группы (так как он не из новой группы)
        unsigned int scanChannelsCount = cfg->GetScanChannelsCount_();
        for (unsigned int i = 0; i < scanChannelsCount; ++i) {
            cfg->getSChannelbyIdx(i, &val);
            if (val.StrokeGroupIdx == FNewGroupIndex) {
                FNewChannel = val.Id;
                FNewSide = dsNone;  // ? Вопрос
                SetAScanFlag = true;
                break;
            }
        }
    }
    SetStrokeTableFlag = true;
}

void cDevice::changeDeviceMode()
{
    switch (FNewMode) {
    case dmPaused:  // Пауза
    {
        // Выключение всех разверток
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            //                    UMUList.at(UMUIdx)->delayedUnloadStart();
            UMUList.at(UMUIdx)->DisableAlarm();
            UMUList.at(UMUIdx)->DisableACSumm();
            UMUList.at(UMUIdx)->DisableBScan();
            UMUList.at(UMUIdx)->DisableAScan();
            UMUList.at(UMUIdx)->Disable();
            //                    UMUList.at(UMUIdx)->delayedUnloadStop();
        }
        // Выключение питания БУМов
        // ?
        SetStrokeTableFlag = false;
        break;
    };
    /*  case dmCalibration: // Настройка каналов контроля
        {
            OnlyOneStrokeFlag = true;
            SetStrokeTableFlag = true;
            SetAScanFlag = true;
            break;
        }; */
    case dmSearch:  // Поиск / Оценка
    {
        SetStrokeTableFlag = true;
        SetAScanFlag = true;
        FNewGateIdx = 1;
        FCurrentGateIdx = 1;
        break;
    };
    case dmCalibration:
        break;
    }
}

void cDevice::changeChannelType()
{
    tbl->isHandScan(FNewChannel) ? CurChannelType = ctHandScan : CurChannelType = ctCompInsp;
    SetStrokeTableFlag = true;
    SetAScanFlag = true;
}

void cDevice::changeChannel()
{
    SetAScanFlag = true;
    SetStrokeTableFlag = SetStrokeTableFlag || (CurChannelType == ctHandScan);  // Для ручника каждый раз перезагружаем таблицу тактов
    tbl->ItemByCID(FNewChannel, &WorkChannelDescription);
}

void cDevice::setDeviceForCalibrationMode()
{
#ifdef CalibrtationMode
    if (CurChannelType == ctCompInsp) {
        DisableAll();

        cfg->resetUMUtoDEVList();
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            cfg->SetUMUIndex(static_cast<int>(UMUIdx));
            int StrCnt = cfg->GetScanStrokeCount();
            if (StrCnt == 0) {
                UMUList.at(UMUIdx)->Disable();
                continue;
            }

            /*				if (cfg->UseLineSwitch)    // Использовать ли реле для подключения линий ручного контроля
                                UMUList.at(UMUIdx)->SetLineSwitching(toCompleteControl);
            */

            UMUList.at(UMUIdx)->SetStrokeCount(static_cast<unsigned char>(StrCnt));
            UMUList.at(UMUIdx)->Enable();
            UsedUMUIdx = UMUIdx;
            // Установка тактов

            GenResList& currentList = UMUGenResList.at(UMUIdx);
            currentList.StrokeCount = static_cast<unsigned char>(StrCnt);
            for (int StrokeIndex = 0; StrokeIndex < currentList.StrokeCount; ++StrokeIndex) {
                DEFCORE_ASSERT(StrokeIndex < MaxStrokeCount);
                tStrokeParams StrokeParams;

                cfg->GetScanChannelsStrokeParams(StrokeIndex, &StrokeParams, FCurChannel);

                currentList.List[usLeft][0][StrokeIndex] = StrokeParams.GenResLeftLine1;
                currentList.List[usLeft][1][StrokeIndex] = StrokeParams.GenResLeftLine2;
                currentList.List[usRight][0][StrokeIndex] = StrokeParams.GenResRightLine1;
                currentList.List[usRight][1][StrokeIndex] = StrokeParams.GenResRightLine2;

                UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
            }
            /*              // Включение В-развертки и др. режимов сделано после установки таблицы тактов
                            // Установка параметров каналов
                            updateChannelsInCurrentGroup();

                            UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;

                            SetChannelParamsFlag = true;
                            CurrentBScanSessionID = UMUList.at(UMUIdx)->EnableBScan();
                            BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии

                            UMUList.at(UMUIdx)->EnableAlarm();
                            UMUList.at(UMUIdx)->DisableACSumm(); */
        }

        UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            updateChannelsInCurrentGroup();


            ManagePathEncoder_();


            SetChannelParamsFlag = true;
            CurrentBScanSessionID = UMUList.at(UMUIdx)->EnableBScan();
            BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии

            UMUList.at(UMUIdx)->EnableAlarm();
            UMUList.at(UMUIdx)->DisableACSumm();
        }
    }
    else {
        DisableAll();

        int UMUIdx;
        tStrokeParams StrokeParams;
        cfg->resetUMUtoDEVList();
        cfg->GetHandStrokeParams(FNewChannel, &StrokeParams, &UMUIdx);

        UMUList.at(UMUIdx)->SetStrokeCount(1);
        UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
        UsedUMUIdx = UMUIdx;
        UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;

        GenResList& currentList = UMUGenResList.at(UMUIdx);
        currentList.List[usLeft][0][0] = StrokeParams.GenResLeftLine1;
        currentList.List[usLeft][1][0] = StrokeParams.GenResLeftLine2;
        currentList.List[usRight][0][0] = StrokeParams.GenResRightLine1;
        currentList.List[usRight][1][0] = StrokeParams.GenResRightLine2;
        currentList.StrokeCount = 1;

        tChannelData ChData;
        cfg->GetChannelData(FNewSide, FNewChannel, &ChData);
        sChannelDescription ChanDesc;
        tbl->ItemByCID(FNewChannel, &ChanDesc);
        UMUList[ChData.UMUIndex]->SetAScanScale(ChanDesc.cdAScanScale);
        SetChannelParams(dsNone, FNewChannel, true, true, true, true);
        /*			if (cfg->UseLineSwitch)               // Использовать реле для подключения линий ручного контроля
                    {
                        if (ChanDesc.cdMethod[0] == imEcho) UMUList.at(UMUIdx)->SetLineSwitching(toCombineSensorHandControl);
                                                       else UMUList.at(UMUIdx)->SetLineSwitching(toSeparateSensorHandControl);
                    } */

        // Установка таблици тактов и включение В-развертки в остальных БУМ
        for (unsigned int UMUIdx_ = 0; UMUIdx_ < cfg->GetUMUCount(); ++UMUIdx_)
            if (UMUIdx_ != UMUIdx) {
                UMUList[UMUIdx_]->SetStrokeCount(1);
                UMUList[UMUIdx_]->SetStrokeParameters(&StrokeParams);
                UMUList[UMUIdx_]->EnableBScan();
            }

        ManagePathEncoder_();
    }
#endif
}

void cDevice::setDeviceForEvaluationOrHandMode()
{
    tChannelData ChData;
    if (cfg->GetChannelData(FNewSide, FNewChannel, &ChData)) {
        /*     if (UsedUMUIdx == ChData.UMUIndex) */ {  // Проверка на то что выбранный канал соответствует активному БУМу
            sChannelDescription ChanDesc;
            tbl->ItemByCID(FNewChannel, &ChanDesc);
            GenResList& currentList = UMUGenResList.at(ChData.UMUIndex);
            DEFCORE_ASSERT(ChData.Stroke < MaxStrokeCount);

            UsedUMUIdx = ChData.UMUIndex;
            CurChannelUMUIndex = ChData.UMUIndex;
            CurChannelUMUSide = ChData.Side;
            CurChannelUMUResLine = ChData.ResLine;
            CurChannelUMUGenLine = ChData.GenLine;
            CurChannelUMUStroke = ChData.Stroke;
            CurChannelUMURes = currentList.List[ChData.Side][ChData.ResLine][ChData.Stroke] & 0x0F;
            CurChannelUMUGen = (currentList.List[ChData.Side][ChData.GenLine][ChData.Stroke] >> 4) & 0x0F;
            CurChannelUMUAScanDuration = ChanDesc.cdAScanDuration;
            CurChannelUMUStrokeDuration = ChanDesc.cdStrokeDuration;

            if (cfg->getUseLineSwitch())  // Использовать реле для подключения линий ручного контроля
            {
                if (tbl->isHandScan(FCurChannel)) {
                    if (ChanDesc.cdEnterAngle != 0) {
                        UMUList[ChData.UMUIndex]->SetLineSwitching(toCombineSensorHandControl);
                    }
                    else {
                        UMUList[ChData.UMUIndex]->SetLineSwitching(toSeparateSensorHandControl);
                    }
                }
                else {
                    UMUList[ChData.UMUIndex]->SetLineSwitching(toCompleteControl);
                }
            }

            if (!MarkFlag) {
                UMUList[ChData.UMUIndex]->EnableAScan(ChData.Side, ChData.ResLine, ChData.Stroke, 0, ChanDesc.cdAScanScale, BIN8(00000000), FCurrentGateIdx);
                //                qDebug() << "UPDATE EnableAScan";
            }
            else {
                UMUList[ChData.UMUIndex]->EnableAScan(ChData.Side, ChData.ResLine, ChData.Stroke, 0, ChanDesc.cdAScanScale, BIN8(00000000), 3);
            }

            CurrentBScanSessionID = UMUList[ChData.UMUIndex]->EnableBScan();
            BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии

            if (AcousticContactState == 1) {
                UMUList[ChData.UMUIndex]->EnableACSumm();
            }
            if (AcousticContactState == 2) {
                UMUList[ChData.UMUIndex]->EnableAC();
            }
        }
    }
}

void cDevice::setDeviceForSearchModeContiniousControl()
{
    DisableAll();
    cfg->resetUMUtoDEVList();
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        cfg->SetUMUIndex(static_cast<int>(UMUIdx));
        int StrCnt = cfg->GetScanStrokeCount();
        if (StrCnt == 0) {
            UMUList.at(UMUIdx)->Disable();
            continue;
        }

        UMUList.at(UMUIdx)->SetStrokeCount(static_cast<unsigned char>(StrCnt));
        UMUList.at(UMUIdx)->Enable();
        UsedUMUIdx = UMUIdx;
        // Установка тактов

        GenResList& currentList = UMUGenResList.at(UMUIdx);
        currentList.StrokeCount = static_cast<unsigned char>(StrCnt);
        for (int StrokeIndex = 0; StrokeIndex < currentList.StrokeCount; ++StrokeIndex) {
            DEFCORE_ASSERT(StrokeIndex < MaxStrokeCount);
            tStrokeParams StrokeParams;
            cfg->GetScanChannelsStrokeParams(StrokeIndex, &StrokeParams /*, ChannelNotSet*/);

            currentList.List[usLeft][0][StrokeIndex] = StrokeParams.GenResLeftLine1;
            currentList.List[usLeft][1][StrokeIndex] = StrokeParams.GenResLeftLine2;
            currentList.List[usRight][0][StrokeIndex] = StrokeParams.GenResRightLine1;
            currentList.List[usRight][1][StrokeIndex] = StrokeParams.GenResRightLine2;

            UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
        }
    }

    if (cfg->GetUMUCount() > 1) SLEEP(1000);

    ManagePathEncoder_();

    // Включение В-развертки
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        CurrentBScanSessionID = UMUList.at(UMUIdx)->EnableBScan();
        BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии
    }

    UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;
#ifdef ChannelsParams
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        // Установка параметров каналов и включение разверток и др. режимов после установки табблици тактов

        updateChannelsInCurrentGroup();
        SetChannelParamsFlag = true;
        UMUList.at(UMUIdx)->EnableAlarm();

        if (AcousticContactState == 1) {
            UMUList.at(UMUIdx)->EnableACSumm();
        }
        if (AcousticContactState == 2) {
            UMUList.at(UMUIdx)->EnableAC();
        }
    }
#endif
}

void cDevice::setDeviceForSearchModeManualControl()
{
#ifdef HansChannels
    DisableAll();
    int UMUIdx = -1;

    // Установка тактов
    tStrokeParams StrokeParams;
    // memset(&StrokeParams,0,sizeof(tStrokeParams));
    cfg->resetUMUtoDEVList();
    cfg->GetHandStrokeParams(FNewChannel, &StrokeParams, &UMUIdx);
    DEFCORE_ASSERT(UMUIdx != -1);
    UsedUMUIdx = UMUIdx;

    //   -  Для теста убранно
    GenResList& currentList = UMUGenResList.at(UMUIdx);
    currentList.List[usLeft][0][0] = StrokeParams.GenResLeftLine1;
    currentList.List[usLeft][1][0] = StrokeParams.GenResLeftLine2;
    currentList.List[usRight][0][0] = StrokeParams.GenResRightLine1;
    currentList.List[usRight][1][0] = StrokeParams.GenResRightLine2;
    currentList.StrokeCount = 1;

    // Для теста добавленно
    /*    GenResList& currentList = UMUGenResList.at(UMUIdx);
        currentList.List[usLeft][0][0] = StrokeParams.GenResLeftLine1;
        currentList.List[usLeft][1][0] = StrokeParams.GenResLeftLine2;
        currentList.List[usRight][0][0] = StrokeParams.GenResRightLine1;
        currentList.List[usRight][1][0] = StrokeParams.GenResRightLine2;

        currentList.List[usLeft][0][1] = StrokeParams.GenResLeftLine1;
        currentList.List[usLeft][1][1] = StrokeParams.GenResLeftLine2;
        currentList.List[usRight][0][1] = StrokeParams.GenResRightLine1;
        currentList.List[usRight][1][1] = StrokeParams.GenResRightLine2;
        currentList.StrokeCount = 2;
    */

    //-------------------- старый код ------------------------------------
    /*
        UMUList.at(UMUIdx)->SetStrokeCount(1);
        UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);

        tChannelData ChData;
        if (cfg->GetChannelData(FNewSide, FNewChannel, &ChData)) {
            sChannelDescription ChanDesc;
            tbl->ItemByCID(FNewChannel, &ChanDesc);
            UMUList[ChData.UMUIndex]->SetAScanScale(static_cast<unsigned char>(ChanDesc.cdAScanScale));

            SetChannelParams(dsNone, FNewChannel, true, true, true, true);
            SetChannelParamsFlag = true;
            CurrentBScanSessionID = UMUList[ChData.UMUIndex]->EnableBScan();
            BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии
            UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;
            UMUList[ChData.UMUIndex]->EnableAlarm();

            if (AcousticContactState == 2) {
                UMUList[ChData.UMUIndex]->DisableACSumm();
            }
        }

        // Установка таблици тактов и включение В-развертки в остальных БУМ
        for (unsigned int UMUIdx_ = 0; UMUIdx_ < cfg->GetUMUCount(); ++UMUIdx_)
            if (UMUIdx_ != UMUIdx) {
                UMUList[UMUIdx_]->SetStrokeCount(1);
                UMUList[UMUIdx_]->SetStrokeParameters(&StrokeParams);
                UMUList[UMUIdx_]->EnableBScan();
            }
    */
    //-------------------- новый код ------------------------------------

    tChannelData ChData;
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FNewChannel, &ChanDesc);

    if (cfg->GetChannelData(FNewSide, FNewChannel, &ChData)) {
        // Установка ТТ

        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            //       Для теста убранно
            UMUList.at(UMUIdx)->SetStrokeCount(1);
            UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
            //       Для теста добавленно
            /*
                        UMUList.at(UMUIdx)->SetStrokeCount(2);
                        UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
                        StrokeParams.StrokeIdx = 1;
                        UMUList.at(UMUIdx)->SetStrokeParameters(&StrokeParams);
            */
        }
        UMUtoDEVList[static_cast<int>(CurChannelType)] = cfg->UMUtoDEVList;

        if (cfg->GetUMUCount() > 1) SLEEP(1000);

        ManagePathEncoder_();

        // Включение В-развертки
        for (unsigned int UMUIdx_ = 0; UMUIdx_ < cfg->GetUMUCount(); ++UMUIdx_) {
            if (UMUIdx_ == ChData.UMUIndex) {
                CurrentBScanSessionID = UMUList[UMUIdx_]->EnableBScan();
                BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии
            }
            else
                UMUList[UMUIdx_]->EnableBScan();
        }

        // Включение остального и загрузка иных данных
        for (unsigned int UMUIdx_ = 0; UMUIdx_ < cfg->GetUMUCount(); ++UMUIdx_) {
            UMUList[UMUIdx_]->SetAScanScale(static_cast<unsigned char>(ChanDesc.cdAScanScale));

            if (UMUIdx_ == ChData.UMUIndex) {
                SetChannelParams(dsNone, FNewChannel, true, true, true, true);
                SetChannelParamsFlag = true;
            };
            UMUList[UMUIdx_]->EnableAlarm();

            if (AcousticContactState == 2) {
                UMUList[UMUIdx_]->DisableACSumm();
            }
        }
    }
#endif
}

void cDevice::ManagePathEncoder_()
{
    if (ManagePathEncoderSimulation) {
        if ((tbl->isHandScan(FNewChannel))) {
            SetPathEncoderData(cfg->getMainPathEncoderIndex(), true, 0, true);   // Выбор ДП
            SetPathEncoderData(cfg->getMainPathEncoderIndex(), true, 0, false);  // Установка нуля
            PathEncoderSim(cfg->getMainPathEncoderIndex(), true);                // Вкл имитатора
        }
        else {
            PathEncoderSim(cfg->getMainPathEncoderIndex(), false);                                              // Выкл имитатора
            SetPathEncoderData(static_cast<char>(cfg->getMainPathEncoderIndex()), false, 0, true);              // Выбор ДП
            SetPathEncoderData(cfg->getMainPathEncoderIndex(), UsePathEncoderSimulationinSearchMode, 0, true);  // Установка нуля
                                                                                                                //            PathEncoderSim(cfg->getMainPathEncoderIndex(), UsePathEncoderSimulationinSearchMode);
        }
    }
}

bool cDevice::filterUploadSignals(int coord, signed char dir, int tailCoord)
{
    //Защита от ошибок в переданных координатах
    //для того чтобы координата выгрузки фильтра не обогнала текущую координату
    if (dir > 0) {
        if (tailCoord > coord) {
            return false;
        }
    }
    else {
        if (tailCoord < coord) {
            return false;
        }
    }

    _bottomSignalSpacingData[0][spclevelMinus6].noBottomSinal = true;
    _bottomSignalSpacingData[0][spclevelZero].noBottomSinal = true;
    _bottomSignalSpacingData[1][spclevelMinus6].noBottomSinal = true;
    _bottomSignalSpacingData[1][spclevelZero].noBottomSinal = true;

    tDEV_BScan2Head* filteredDEV_BScan2Head = new (tDEV_BScan2Head);
    // tUMU_BScanData* bufUMUBscanData = ;

    int displCoord = _displayCoord - static_cast<int>(fltrPrmMinPacketSize) - 1;

    const tUMUtoDEVList& compInspList = UMUtoDEVList[ctCompInsp];
    filteredDEV_BScan2Head->Items.reserve(compInspList.size());
    for (tUMUtoDEVList::const_iterator it = compInspList.begin(), totalUMUDEVIt = compInspList.end(); it != totalUMUDEVIt; ++it) {
        const tUMUtoDEVitem& curItem = it.operator*();
        // tUMU_BScanSignal* currentSignals = bufUMUBscanData->Signals.Data[curItem.UMUSide][curItem.UMULine][curItem.UMUStroke];
        DEFCORE_ASSERT((curItem.DEVSide - 1) < MaxSideCount);
        CID chId = curItem.DEVChannel;

        GateType bottomStartStrobe = 0;
        if (chId == N0EMS) {
            bottomStartStrobe = static_cast<GateType>(cal->GetStGate(curItem.DEVSide, chId, 1));
        }

        FiltrationData& currentFiltrationData = _filtrationData[curItem.DEVSide - 1][chId];
        std::vector<sMaskItem>& currentMasksList = currentFiltrationData.masks;
        if (currentMasksList.empty()) {
            filterInitMasksList(currentFiltrationData);
        }

        DEFCORE_ASSERT(currentFiltrationData.partition.operator*().Busy == false);

        if (currentFiltrationData.partition != currentMasksList.begin()) {
            DEFCORE_ASSERT((currentFiltrationData.partition - 1).operator*().Busy == true);
            bool needRepartition = false;

            std::vector<tDEV_BScan2HeadItem> bScanItems;

            for (std::vector<sMaskItem>::iterator it = currentMasksList.begin(); it != currentFiltrationData.partition; ++it) {
                sMaskItem& cur = it.operator*();
                DEFCORE_ASSERT(cur.Busy);
                // if (cur.Busy) {
                // Защита от ситуации когда пачки расположены друг над другом
                // Если конец фильтра дошел до первой координаты пачки то
                // то необходимо определится с решением о её выгрузке.
                // Проверяем её длинну.
                // Если она равна минимальной длинне пачки с учетом максимального разрыва по координате то считаем её
                // хорошей и отдаём на выгрузку
                // Если нет то удаляем.
                if (tailCoord == cur.StartSysCoord) {
                    if ((std::abs(cur.LastSysCoord - cur.StartSysCoord) + fltrPrmMaxCoordSpacing) >= fltrPrmMinPacketSize) {
                        cur.OK = true;
                    }
                    else {
                        cur.reset();
                        needRepartition = true;
                        continue;
                    }
                }

                // Обработка апроксимированной маски для поиска дефекта 53.1
                if (cur.Aprox) {
                    if ((_displayCoord - cur.LastDisplayCoord) >= 12) {
                        // Проверка на дефект 53.1
                        if (FCurrentControlMode == cmTestBoltJoint) {
                            if ((chId == F42E || chId == B42E || chId == F45E || chId == B45E) && currentFiltrationData.partition != currentMasksList.begin()) {
                                if ((cur.StartDelay >= 45 && cur.StartDelay <= 115) && (cur.LastDelay >= 45 && cur.LastDelay <= 115)) {
                                    filterCheck53Defect(cur, currentMasksList.begin(), currentFiltrationData.partition);
                                }
                            }
                        }

                        if (cur.Defect53_1Flag) {
                            filterSendDefect53_1(cur, curItem);
                        }

                        // Закрываем апроксимированную маску здесь т.к. она уже выгружена
                        cur.reset();
                        needRepartition = true;
                        continue;
                    }  // if displayCoord
                }      // if Aprox


                if (cur.OK && !cur.Signals_.empty()) {
                    std::vector<sMaskSignalItem>::iterator itTail = std::find_if(cur.Signals_.begin(), cur.Signals_.end(), MaskTailFunctor(tailCoord));

                    if (bScanItems.empty()) {
                        tDEV_BScan2HeadItem item;
                        item.Channel = chId;
                        item.Side = curItem.DEVSide;
                        bScanItems.push_back(item);
                    }

                    for (std::vector<sMaskSignalItem>::iterator it = cur.Signals_.begin(); it != itTail; ++it) {
                        const tUMU_BScanSignal& signalItem = it.operator*().Signal;
                        bScanItems.back().Signals.push_back(signalItem);
                        if (chId == N0EMS) {
                            filterAnalyzeBottomSignal(signalItem, curItem, bottomStartStrobe, tailCoord, displCoord);
                        }
                        if (bScanItems.back().Signals.size() >= MaxSignalAtBlock && it != itTail) {
                            tDEV_BScan2HeadItem item;
                            item.Channel = chId;
                            item.Side = curItem.DEVSide;
                            bScanItems.push_back(item);
                        }
                    }

                    cur.Signals_.erase(cur.Signals_.begin(), itTail);

                    if (cur.Signals_.empty()) {
                        int length = std::abs(cur.LastSysCoord - cur.StartSysCoord);
                        if (length >= static_cast<int>(fltrPrmMinPacketSizeForEvent)) {
                            if ((FCurrentControlMode == cmNormal && cur.Count_0dB >= fltrPrmMinSignals0dBInNormModeForEvent) || (FCurrentControlMode == cmTestBoltJoint && cur.Count_6dB >= 8)) {
                                filterSendSignalsPacket(cur, curItem, length);
                            }
                        }

                        if (chId == F42E || chId == B42E || chId == F45E || chId == B45E) {
#ifndef Delete_SignalsM6dB
                            if (cur.SignalsM6dB.size() >= DEFECT_53_SIGNALS_COUNT_MAX)
#endif
#ifdef Delete_SignalsM6dB
                                if (cur.Count_6dB >= DEFECT_53_SIGNALS_COUNT_MAX)
#endif

                                    // Закрываем маску на 20 мм позже.
                                    cur.Aprox = true;
                                else {
                                    cur.reset();
                                    needRepartition = true;
                                }
                        }
                        else {
                            cur.reset();
                            needRepartition = true;
                        }
                    }
                }  // cur.OK
            }      // for mask count

            if (needRepartition) {
                currentFiltrationData.partition = std::partition(currentMasksList.begin(), currentFiltrationData.partition, &isMaskBusy);
            }

            filteredDEV_BScan2Head->Items.insert(filteredDEV_BScan2Head->Items.end(), bScanItems.begin(), bScanItems.end());
        }  // if fMasks count
    }      // for item
    displCoord = std::max(_displayCoord - static_cast<int>(fltrPrmMinPacketSize), 0);

    for (unsigned char side = 0; side < MaxSideCount; ++side) {
        for (unsigned char spLevel = spclevelMinus6; spLevel < spclevelTotal; ++spLevel) {
            BottomSignalSpacingData& data = _bottomSignalSpacingData[side][spLevel];
            tDevSignalSpacingFlag& flag = data.fSignalSpacingFlag;
            if (data.noBottomSinal && !flag.Flag) {
                flag.Flag = true;
                flag.StartCoord = tailCoord;
                flag.StartDisplayCoord = displCoord;
                flag.StartDelay = data.lastBottomSignalDelay;
                flag.Echo0Count = 0;
            }
        }
    }

    filteredDEV_BScan2Head->XSysCrd[0] = tailCoord;
    // bufUMUBscanData->XSysCrd[0] = tailCoord;

    filteredDEV_BScan2Head->Dir[0] = dir;
    // bufUMUBscanData->Dir[0] = dir;

    if (!fUMUParamsStorage.empty()) {
        const sUMUParams& umuParams = fUMUParamsStorage.front();

        if (umuParams.XSysCrd == tailCoord) {
            filteredDEV_BScan2Head->XDisCrd[0] = umuParams.XDisCrd;
            // bufUMUBscanData->XDisCrd[0] = umuParams.XDisCrd;
            filteredDEV_BScan2Head->Dir[0] = umuParams.Dir;
            // bufUMUBscanData->Dir[0] = umuParams.Dir;

            filteredDEV_BScan2Head->PathEncodersIndex = umuParams.PathEncodersIndex;
            filteredDEV_BScan2Head->ChannelType = umuParams.ChannelType;
            filteredDEV_BScan2Head->Simulator[0] = umuParams.Simulator;
            filteredDEV_BScan2Head->ACItems = umuParams.ACHeadItems;
            filteredDEV_BScan2Head->BScanSessionID = umuParams.BScanSession;

            // bufUMUBscanData->BScanSession = umuParams.BScanSession;
            // bufUMUBscanData->UMUIdx = umuParams.UMUIdx;

            fUMUParamsStorage.pop_front();
        }
        else {
            delete filteredDEV_BScan2Head;
            // delete bufUMUBscanData;

            // Защита от ошибок в векторе с параметрами БУМ
            // Если координата выгрузки обогнала координату сохранёных параметров
            // выходим с ошибкой.
            if (dir > 0) {
                if (tailCoord > umuParams.XSysCrd) {
                    return false;
                }
            }
            else {
                if (tailCoord < umuParams.XSysCrd) {
                    return false;
                }
            }
            // Не поступало такой координаты Ничего не выгружаем.
            return true;
        }
    }
    else {
        delete filteredDEV_BScan2Head;
        // delete bufUMUBscanData;
        return true;  // error
    }
    // Создание данных АСД из данных В-развертки
    filterCreateAlarmData(filteredDEV_BScan2Head->Items.begin(), filteredDEV_BScan2Head->Items.end());
    postBScan2Data(edBScan2Data, filteredDEV_BScan2Head);
    return true;
}

// Отправка данных об амплитуде донного сигнала
void cDevice::sendBottomSignalAmpl()
{
    unsigned long currentTick = GetTickCount_();

    if ((currentTick - _bottomSignalSendTick) >= BOTOOM_SIGNAL_SEND_INTERVAL) {
        _bottomSignalSendTick = currentTick;
        int ampl = GetScanZeroProbeAmpl();
        _bottomSignalAmplFilter.push_back(ampl);

        if (_bottomSignalAmplFilter.size() >= BOTTOM_SIGNAL_AMPL_FILTER_FACTOR) {
            tDEV_BottomSignalAmpl* DEV_BottomSignalAmpl = new tDEV_BottomSignalAmpl;

            long sum = 0;
            for (std::deque<int>::const_iterator it = _bottomSignalAmplFilter.begin(); it != _bottomSignalAmplFilter.end(); ++it) {
                sum += it.operator*();
            }

            ampl = static_cast<int>(sum / BOTTOM_SIGNAL_AMPL_FILTER_FACTOR);

            *DEV_BottomSignalAmpl = ampl;

            DEV_Event->WriteHeadAndPost(edBottomSignalAmpl, &DEV_BottomSignalAmpl, SIZE_OF_TYPE(&DEV_BottomSignalAmpl));

            _bottomSignalAmplFilter.pop_front();
        }
    }
}

void cDevice::mergeChannelsTwoACValues(tDEV_BScan2Head* DEV_BScan2Head, std::map<CID, tDEV_ACHeadItem>& mapForSide, CID cid1, CID cid2) const
{
    DEFCORE_ASSERT(DEV_BScan2Head != NULL);
    if (mapForSide.find(cid1) != mapForSide.end() && mapForSide.find(cid2) != mapForSide.end()) {
        bool thirdPair = mapForSide[cid1].ACValue || mapForSide[cid2].ACValue;
        mapForSide[cid1].ACValue = thirdPair;
        mapForSide[cid2].ACValue = thirdPair;
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid1]);
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid2]);
    }
}

void cDevice::mergeChannelsFourACValues(tDEV_BScan2Head* DEV_BScan2Head, std::map<CID, tDEV_ACHeadItem>& mapForSide, CID cid1, CID cid2, CID cid3, CID cid4) const
{
    DEFCORE_ASSERT(DEV_BScan2Head != NULL);
    if (mapForSide.find(cid1) != mapForSide.end() && mapForSide.find(cid2) != mapForSide.end() && mapForSide.find(cid3) != mapForSide.end() && mapForSide.find(cid4) != mapForSide.end()) {
        bool thirdPair = mapForSide[cid1].ACValue || mapForSide[cid2].ACValue || mapForSide[cid3].ACValue || mapForSide[cid4].ACValue;
        mapForSide[cid1].ACValue = thirdPair;
        mapForSide[cid2].ACValue = thirdPair;
        mapForSide[cid3].ACValue = thirdPair;
        mapForSide[cid4].ACValue = thirdPair;
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid1]);
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid2]);
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid3]);
        DEV_BScan2Head->ACItems.push_back(mapForSide[cid4]);
    }
}

void cDevice::filterUseNewMask(sMaskItem& currentNewMask, const tUMU_BScanSignal& newSignal, signed char direction)
{
    const signed char ampl = AmplIdtoDB[newSignal.Ampl[AmplDBIdx_int] & 0x0F];
    DEFCORE_ASSERT(!currentNewMask.Busy);

    currentNewMask.Busy = true;
    currentNewMask.OK = false;

    currentNewMask.Signals_.clear();
#ifndef Delete_SignalsM6dB
    currentNewMask.SignalsM6dB.clear();
#endif

    sMaskSignalItem signalItem;
    signalItem.Dir = direction;
    signalItem.SysCoord = _sysCoord;
    signalItem.DisCoord = _displayCoord;
    signalItem.Signal = newSignal;

    currentNewMask.Signals_.push_back(signalItem);

    if (ampl >= 0) {
        currentNewMask.Count_0dB = 1;
    }
    else {
        currentNewMask.Count_0dB = 0;
    }

    // Сохраняем отдельно сигналы -6дБ для алгоритма поиска дефекта 53.1
    if (ampl >= -6) {
        currentNewMask.Count_6dB = 1;
#ifndef Delete_SignalsM6dB
        currentNewMask.SignalsM6dB.push_back(signalItem);
#endif
    }
    else {
        currentNewMask.Count_6dB = 0;
    }

    currentNewMask.MaxAmpl = ampl;

    currentNewMask.SameDelay = static_cast<signed char>(fltrPrmMaxSameDelayConsecutive);
    currentNewMask.LastSysCoord = _sysCoord;
    currentNewMask.StartSysCoord = _sysCoord;
    currentNewMask.LastDisplayCoord = _displayCoord;
    currentNewMask.StartDisplayCoord = _displayCoord;
    currentNewMask.LastDelay = newSignal.Delay;
    currentNewMask.StartDelay = newSignal.Delay;

    // Иницализация параметров для авто поиска дефекта 53.1
    currentNewMask.Aprox = false;
    currentNewMask.Upper53SignalCount = 0;
    currentNewMask.Lower53SignalCount = 0;
    currentNewMask.Defect53_1Flag = false;
}

void cDevice::filterPutSignalsInMask(sMaskItem& currentMask, const tUMU_BScanSignal& currentSignal, signed char direction)
{
    sMaskSignalItem signalItem;

    signalItem.Dir = direction;
    signalItem.SysCoord = _sysCoord;
    signalItem.DisCoord = _displayCoord;
    signalItem.Signal = currentSignal;

    currentMask.Signals_.push_back(signalItem);
    currentMask.LastSysCoord = _sysCoord;
    currentMask.LastDisplayCoord = _displayCoord;
    currentMask.LastDelay = currentSignal.Delay;

    const signed char ampl = AmplIdtoDB[currentSignal.Ampl[AmplDBIdx_int] & 0x0F];

    if (ampl >= -6) {
        currentMask.Count_6dB++;
#ifndef Delete_SignalsM6dB
        currentMask.SignalsM6dB.push_back(signalItem);
#endif
        if (ampl >= 0) {
            currentMask.Count_0dB++;
        }
    }

    if (ampl > currentMask.MaxAmpl) {
        currentMask.MaxAmpl = ampl;
    }
}

bool cDevice::filterIsDelayAcceptable(const sMaskItem& currentMask, CID channel, eChannelDir channelDirType, int absDeltaCoord, unsigned int delay) const
{
    if (channel == N0EMS) {
        if (delay <= (currentMask.LastDelay + fltrPrmMaxDelayDelta) && delay >= (currentMask.LastDelay - fltrPrmMaxDelayDelta)) {
            return true;
        }
    }
    else {
        unsigned int delayDelta = absDeltaCoord ? (fltrPrmMaxDelayDelta * absDeltaCoord) : fltrPrmMaxDelayDelta;

        // Проверка сигнала на попадания в маску по задержке
        // Проверка изменения координаты - в плюс или в минус ?
        if (_currentDeltaCoord > 0) {
            switch (channelDirType) {
            case cdZoomIn:
                if (delay <= currentMask.LastDelay && (delay + delayDelta) >= currentMask.LastDelay) {
                    return true;
                }
                break;
            case cdZoomOut:
                if (delay >= currentMask.LastDelay && delay <= (currentMask.LastDelay + delayDelta)) {
                    return true;
                }
                break;
            case cdNone:
                break;
            }
        }
        else {
            switch (channelDirType) {
            case cdZoomIn:
                if (delay >= currentMask.LastDelay && delay <= (currentMask.LastDelay + delayDelta)) {
                    return true;
                }
                break;
            case cdZoomOut:
                if (delay <= currentMask.LastDelay && (delay + delayDelta) >= currentMask.LastDelay) {
                    return true;
                }
                break;
            case cdNone:
                break;
            }
        }
    }
    return false;
}

bool cDevice::filterTryInsertSignal(CID channel, eChannelDir channelDirType, signed char direction, sMaskItem& currentMask, const tUMU_BScanSignal& currentSignal)
{
    int absDeltaCoord = std::abs(_sysCoord - currentMask.LastSysCoord);

    // Проверка разрыва координаты
    if (absDeltaCoord <= fltrPrmMaxCoordSpacing) {
        if (filterIsDelayAcceptable(currentMask, channel, channelDirType, absDeltaCoord, currentSignal.Delay)) {
            if (channel != N0EMS) {
                if (currentSignal.Delay != currentMask.LastDelay) {
                    currentMask.SameDelay = static_cast<signed char>(fltrPrmMaxSameDelayConsecutive);  // Задержка разная - обновляем счетчик
                }
                else {  // Задержка одиноковая - проверяем счетчик
                    --currentMask.SameDelay;
                    if (currentMask.SameDelay <= 0) {
                        return false;
                    }
                }
            }

            if (std::abs(_sysCoord - currentMask.StartSysCoord) >= fltrPrmMinPacketSize) {
                currentMask.OK = true;
            }

            if (currentMask.Signals_.size() <= 127) {
                filterPutSignalsInMask(currentMask, currentSignal, direction);
            }
            return true;
        }  // if delayAcceble
    }
    return false;
}

int cDevice::debugGetTotalFilterSize()
{
    int totalSize = 0;
    for (int i = 0; i < MaxSideCount; ++i) {
        for (int j = 0; j < CHANNELS_TOTAL; ++j) {
            FiltrationData& data = _filtrationData[i][j];
            totalSize += data.debugGetSize();
        }
    }
    return totalSize;
}

void cDevice::filterAddNewSignal(const tDEV_BScan2Head* data)
{
    DEFCORE_ASSERT(data != NULL);
    for (tBScan2Items::const_iterator it = data->Items.begin(), total = data->Items.end(); it != total; ++it) {
        const tDEV_BScan2HeadItem& currentHeadItem = it.operator*();
        CID channel = currentHeadItem.Channel;
        unsigned char side = currentHeadItem.Side - 1;
        DEFCORE_ASSERT(side < MaxSideCount);
        eChannelDir channelDirType = tbl->GetChannelDir(channel);
        FiltrationData& currentFiltrationData = _filtrationData[side][channel];
        std::vector<sMaskItem>& currentMaskItemRow = currentFiltrationData.masks;
        if (currentMaskItemRow.empty()) {
            filterInitMasksList(currentFiltrationData);
        }

        unsigned int signalCount = currentHeadItem.Signals.size();
        DEFCORE_ASSERT(signalCount <= MaxSignalAtBlock);
        for (unsigned int SIdx = 0; SIdx < signalCount; ++SIdx) {  // Перебор сигналов
            const tUMU_BScanSignal& currentSignal = currentHeadItem.Signals[SIdx];
            bool newPacket = true;

            DEFCORE_ASSERT(currentFiltrationData.partition.operator*().Busy == false);
            if (currentFiltrationData.partition != currentMaskItemRow.begin()) {  // Есть ли формрруемые пачки
                DEFCORE_ASSERT((currentFiltrationData.partition - 1).operator*().Busy == true);
                for (std::vector<sMaskItem>::iterator it = currentMaskItemRow.begin(); it != currentFiltrationData.partition; ++it) {
                    sMaskItem& currentMask = it.operator*();
                    DEFCORE_ASSERT(currentMask.Busy);
                    if (filterTryInsertSignal(channel, channelDirType, data->Dir[0], currentMask, currentSignal)) {
                        newPacket = false;
                        break;
                    }
                }  // for просматриваем маски
            }

            // Сигнал не попал ни в одну из масок Создаем новую
            if (newPacket) {
                if (currentFiltrationData.partition != currentMaskItemRow.end()) {
                    filterUseNewMask(currentFiltrationData.partition.operator*(), currentSignal, data->Dir[0]);
                    ++currentFiltrationData.partition;
                }
            }
        }  // for SIdx перебор сигналов
    }
}

void cDevice::tickAScanData()
{
    cs->Enter();
    PtUMU_AScanData AScanData_ptr;
    UMU_Event->ReadEventData(&AScanData_ptr, SIZE_OF_TYPE(&AScanData_ptr), NULL);

    tDEV_AScanHead* DEV_AScanHead = new (tDEV_AScanHead);

    if (!cfg->GetChannelBySLS(CurChannelType, AScanData_ptr->UMUIdx, AScanData_ptr->Side, AScanData_ptr->Line, AScanData_ptr->Stroke, &DEV_AScanHead->Side, &DEV_AScanHead->Channel)) {
        delete AScanData_ptr;  //!!!
        delete DEV_AScanHead;
        cs->Release();
        return;
    }
    DEV_Event->WriteHeadBodyAndPost(edAScanData, &DEV_AScanHead, SIZE_OF_TYPE(&DEV_AScanHead), &AScanData_ptr, SIZE_OF_TYPE(&AScanData_ptr));
    cs->Release();
}

void cDevice::tickAScanMeas()
{
    cs->Enter();
    PtUMU_AScanMeasure AScanMeasure_ptr;
    UMU_Event->ReadEventData(&AScanMeasure_ptr, SIZE_OF_TYPE(&AScanMeasure_ptr), NULL);  // Получаем указатель на данные созданные классом UMU

    tDEV_AScanMeasure* DEV_AScanMeas;
    DEV_AScanMeas = new (tDEV_AScanMeasure);  // TODO: check delete

    // Added by KirillB
    if (!cfg->GetChannelBySLS(CurChannelType, AScanMeasure_ptr->UMUIdx, AScanMeasure_ptr->Side, AScanMeasure_ptr->Line, AScanMeasure_ptr->Stroke, &DEV_AScanMeas->Side, &DEV_AScanMeas->Channel)) {
        delete AScanMeasure_ptr;  //!!!
        delete DEV_AScanMeas;
        cs->Release();
        return;
    }

    DEV_AScanMeas->ParamM = AScanMeasure_ptr->ParamM;  // Положение максимального сигнала, в стробе АСД. [мкс]
    DEV_AScanMeas->ParamA = AScanMeasure_ptr->ParamA;  // Амплитуда максимального сигнала, в стробе АСД.

    if (AScanMeasure_ptr->ParamA > AScanMax.Ampl) {
        AScanMax.Delay = DEV_AScanMeas->ParamM;
        AScanMax.Ampl = AScanMeasure_ptr->ParamA;
    }
    CalAScanMax.Delay = DEV_AScanMeas->ParamM;
    CalAScanMax.Ampl = AScanMeasure_ptr->ParamA;

    delete AScanMeasure_ptr;  // Уничтожаем данные созданные классом UMU
    AScanMeasure_ptr = NULL;

    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);

    if (ChanDesc.cdEnterAngle != 0)  // Глубина залегания отражателя [H, мм]
    {
        DEV_AScanMeas->ParamH = static_cast<int>((((3.26f * static_cast<float>(DEV_AScanMeas->ParamM)) * std::cos(static_cast<float>(ChanDesc.cdEnterAngle) * DEVICE_PI / 180.0f))));
    }
    else {
        DEV_AScanMeas->ParamH = static_cast<int>((5.9f * DEV_AScanMeas->ParamM));
    }

    if (ChanDesc.cdMethod[0] != imShadow) {
        DEV_AScanMeas->ParamH = static_cast<int>(DEV_AScanMeas->ParamH / 2.0f);
    }

    // Расстояние до отражателя, по лучу [R, мм]
    if (ChanDesc.cdEnterAngle != 0) {
        DEV_AScanMeas->ParamR = static_cast<int>((3.26f * static_cast<float>(DEV_AScanMeas->ParamM) / 2.0f));
    }
    else {
        DEV_AScanMeas->ParamR = static_cast<int>((5.9f * static_cast<float>(DEV_AScanMeas->ParamM) / 2.0f));
    }

    // Расстояние от точки выхода луча до отражателя [L, мм]

    if (ChanDesc.cdEnterAngle != 0) {
        DEV_AScanMeas->ParamL = static_cast<int>(((3.26f * static_cast<float>(DEV_AScanMeas->ParamM) / 2.0f) * std::sin(static_cast<float>(ChanDesc.cdEnterAngle) * DEVICE_PI / 180.0f)));
    }
    else {
        DEV_AScanMeas->ParamL = 0;
    }

    // Превышение сигналом порогового уровня [N, дБ]
    if (DEV_AScanMeas->ParamA != 0) {
        DEV_AScanMeas->ParamN = static_cast<int>((20.0f * std::log10(DEV_AScanMeas->ParamA / 32.0f)));
    }
    else {
        DEV_AScanMeas->ParamN = 0;
    }

    // Коэффициент выявляемости [Кд, дБ]
    DEV_AScanMeas->ParamKd = DEV_AScanMeas->ParamN - GetSens();  // Коэффициент выявляемости (Кд, дБ)

    LastAScanMeasure = *DEV_AScanMeas;
    LastAScanMeasureGetTime = GetTickCount_();

    DEV_Event->WriteHeadAndPost(edAScanMeas, &DEV_AScanMeas, SIZE_OF_TYPE(&DEV_AScanMeas));
    cs->Release();
}

void cDevice::tickTVGData()
{
    cs->Enter();
    PtUMU_AScanData TVGData_ptr;
    UMU_Event->ReadEventData(&TVGData_ptr, SIZE_OF_TYPE(&TVGData_ptr), NULL);
    tDEV_AScanHead* DEV_AScanHead = new (tDEV_AScanHead);
    if (!cfg->GetChannelBySLS(CurChannelType, TVGData_ptr->UMUIdx, TVGData_ptr->Side, TVGData_ptr->Line, TVGData_ptr->Stroke, &DEV_AScanHead->Side, &DEV_AScanHead->Channel)) {
        delete TVGData_ptr;  //!!!
        delete DEV_AScanHead;
        cs->Release();
        return;
    }
    DEV_Event->WriteHeadBodyAndPost(edTVGData, &DEV_AScanHead, SIZE_OF_TYPE(&DEV_AScanHead), &TVGData_ptr, SIZE_OF_TYPE(&TVGData_ptr));
    cs->Release();
}

void cDevice::tickAlarmData()
{
    if (_enableFiltration && (_currentDeltaCoord != 0) && (CurChannelType != ctHandScan)) {  // TODO: Is everything right here? Without this (CurChannelType != ctHandScan),
        // there is no sound in the hand mode. Check this!
        cs->Enter();
        PtUMU_AlarmItem Alarm_ptr;
        UMU_Event->ReadEventData(&Alarm_ptr, SIZE_OF_TYPE(&Alarm_ptr), NULL);
        delete Alarm_ptr;
        cs->Release();
        return;
    }

    PtUMU_AlarmItem Alarm_ptr;
    // AlarmData_from_UMU = true;
    tDEV_AlarmHead* DEV_AlarmHead = new (tDEV_AlarmHead);

    cs->Enter();
    UMU_Event->ReadEventData(&Alarm_ptr, SIZE_OF_TYPE(&Alarm_ptr), NULL);

    DEV_AlarmHead->Items.resize(UMUtoDEVList[CurChannelType].size());
    unsigned int idx = 0;

    // Преобразование геометрии БУМ в Device - новый вариант
    const tUMUtoDEVList::const_iterator alarmEndIterator = UMUtoDEVList[CurChannelType].end();
    for (tUMUtoDEVList::const_iterator it = UMUtoDEVList[CurChannelType].begin(); it != alarmEndIterator; ++it) {
        if (it->UMUIndex == Alarm_ptr->UMUIdx) {
            const tUMUtoDEVitem& item = it.operator*();

            bool* alarmState = Alarm_ptr->State[item.UMUSide][item.UMULine][item.UMUStroke];
            eDeviceSide DEV_Side = item.DEVSide;
            CID DEV_Channel = item.DEVChannel;

            tDEV_AlarmHeadItem& curAlarmItem = DEV_AlarmHead->Items[idx];
            curAlarmItem.Side = DEV_Side;
            curAlarmItem.Channel = DEV_Channel;
            std::copy(alarmState, &alarmState[4], curAlarmItem.State);

            // Применение sAlarmInfo из StrobeModeList для данных АСД
            sAlarmInfo* curMode = StrobeModeList_[DEV_Channel].Modes[DEV_Side];
            for (unsigned char i = 1; i <= 2; ++i) {
                if (curMode[i].Alg == aaNone) {
                    curAlarmItem.State[i] = false;  // АСД выключенно
                }
                else {
                    curAlarmItem.State[i] ^= static_cast<int>((curMode[i].Alg == aaByAbsence));  // Если надо то инвертирует АСД
                }
            }
            ++idx;
        }
    }


    delete Alarm_ptr;
    Alarm_ptr = NULL;
    DEV_Event->WriteHeadAndPost(edAlarmData, &DEV_AlarmHead, SIZE_OF_TYPE(&DEV_AlarmHead));
    cs->Release();
}

void cDevice::convertBScanFormat(const PtUMU_BScanData BScan2Data_ptr, tDEV_BScan2Head* DEV_BScan2Head) const
{
    std::copy(BScan2Data_ptr->Simulator, &BScan2Data_ptr->Simulator[MaxPathEncoders], DEV_BScan2Head->Simulator);
    std::copy(BScan2Data_ptr->Dir, &BScan2Data_ptr->Dir[MaxPathEncoders], DEV_BScan2Head->Dir);
    std::copy(BScan2Data_ptr->XSysCrd, &BScan2Data_ptr->XSysCrd[MaxPathEncoders], DEV_BScan2Head->XSysCrd);
    std::copy(BScan2Data_ptr->XDisCrd, &BScan2Data_ptr->XDisCrd[MaxPathEncoders], DEV_BScan2Head->XDisCrd);

    DEV_BScan2Head->PathEncodersIndex = BScan2Data_ptr->PathEncodersIndex;

    std::map<CID, tDEV_ACHeadItem> acCommonMap[3];  // 3 Maps for each side

    // Reserve optimisation
    DEV_BScan2Head->Items.reserve(UMUtoDEVList[DEV_BScan2Head->ChannelType].size());
    // Преобразование геометрии БУМ в Device - новый вариант
    const tUMUtoDEVList::const_iterator bscanEndIterator = UMUtoDEVList[DEV_BScan2Head->ChannelType].end();
    for (tUMUtoDEVList::const_iterator it = UMUtoDEVList[DEV_BScan2Head->ChannelType].begin(); it != bscanEndIterator; ++it) {
        if (it->UMUIndex == BScan2Data_ptr->UMUIdx) {
            const tUMUtoDEVitem& item = it.operator*();
            unsigned char UMUSide_ = item.UMUSide;
            unsigned char UMULine = item.UMULine;
            unsigned char UMUStroke = item.UMUStroke;
            eDeviceSide DEV_Side = item.DEVSide;
            CID DEV_Channel = item.DEVChannel;

            // Перенос данных АК
            if (BScan2Data_ptr->AC.DataExists) {
                tDEV_ACHeadItem newItem;
                newItem.Side = DEV_Side;
                newItem.Channel = DEV_Channel;
                newItem.ACValue = BScan2Data_ptr->AC.Data[UMUSide_][UMULine][UMUStroke];
                newItem.ACSumm = BScan2Data_ptr->AC.Summ[UMUSide_][UMULine][UMUStroke];
                newItem.ACTh = BScan2Data_ptr->AC.Th[UMUSide_][UMULine][UMUStroke];

                if (DEV_Channel == F58ELW || DEV_Channel == F58ELU || DEV_Channel == B58ELW || DEV_Channel == B58ELU || DEV_Channel == F42E || DEV_Channel == B42E || DEV_Channel == F58MLW || DEV_Channel == F58MLU || DEV_Channel == B58MLW || DEV_Channel == B58MLU) {
                    acCommonMap[UMUSide_][DEV_Channel] = newItem;
                }
                else {
                    DEV_BScan2Head->ACItems.push_back(newItem);
                }
            }

            // Перенос данных В-развертки
            if (BScan2Data_ptr->Signals.Count[UMUSide_][UMULine][UMUStroke] != 0) {
                tDEV_BScan2HeadItem newItem;
                newItem.Side = DEV_Side;
                newItem.Channel = DEV_Channel;
                unsigned int count = BScan2Data_ptr->Signals.Count[UMUSide_][UMULine][UMUStroke];
                newItem.Signals.resize(count);
                std::copy(BScan2Data_ptr->Signals.Data[UMUSide_][UMULine][UMUStroke], &(BScan2Data_ptr->Signals.Data[UMUSide_][UMULine][UMUStroke][count]), newItem.Signals.begin());
                //                newItem.Count = BScan2Data_ptr->Signals.Count[UMUSide_][UMULine][UMUStroke];
                //                newItem.BScanDataPtr = reinterpret_cast<PtUMU_BScanSignalList>(&(BScan2Data_ptr->Signals.Data[UMUSide_][UMULine][UMUStroke]));
                DEV_BScan2Head->Items.push_back(newItem);
            }
        }
    }

    for (int i = 0; i < 3; ++i) {  // TODO - Кастыль перенести в DeviceConfig
        std::map<CID, tDEV_ACHeadItem>& mapForSide = acCommonMap[i];
        if (!mapForSide.empty()) {
            mergeChannelsTwoACValues(DEV_BScan2Head, mapForSide, F58ELW, F58ELU);
            mergeChannelsTwoACValues(DEV_BScan2Head, mapForSide, B58ELW, B58ELU);
            mergeChannelsFourACValues(DEV_BScan2Head, mapForSide, F58ELW, F58ELU, B58MLW, B58MLU);
            mergeChannelsFourACValues(DEV_BScan2Head, mapForSide, B58ELW, B58ELU, F58MLW, F58MLU);
            mergeChannelsTwoACValues(DEV_BScan2Head, mapForSide, F42E, B42E);
        }
    }
}

void cDevice::collectRailTuningData(tDEV_BScan2Head* DEV_BScan2Head)
{
    eBScanDataType bScanDataType = GetBScanDataType();

    sGate tuningGate;
    if (!RailTypeTrackingMode)
        tuningGate = cfg->GetRailTypeTuningGate();
    else
        tuningGate = cfg->GetRailTypeTuningGate_forSwitch();

    bool Changing = false;

    // Сбор данных для режима "Настройка на тип рельса"
    for (tRailTypeTuningDataList::iterator it = RailTypeTuningDataList.begin(), totalRailsIt = RailTypeTuningDataList.end(); it != totalRailsIt; ++it) {  // Выбор канала (с доным сигналом) по которому настраивается тип рельса
        sRailTypeTuningData& tuningItem = it.operator*();
        for (tBScan2Items::iterator it = DEV_BScan2Head->Items.begin(), totalBScanIt = DEV_BScan2Head->Items.end(); it != totalBScanIt; ++it) {
            const tDEV_BScan2HeadItem& bscanItem = it.operator*();
            if ((tuningItem.Side == bscanItem.Side) && (tuningItem.Channel == bscanItem.Channel)) {
                int MaxAmp = -1;
                unsigned char CurrMaxAmp = 0;
                int Delay_mcs = 0;
                int channelMultiply = GetChannelBScanDelayMultiply(bscanItem.Channel);

                int save = tuningItem.DelayHeight;

                for (unsigned int j = 0; j < bscanItem.Signals.size(); ++j) {  // Просматриваем сигналы

                    Delay_mcs = bscanItem.Signals[j].Delay / channelMultiply;
                    if (bScanDataType == bstWithForm) {
                        CurrMaxAmp = bscanItem.Signals[j].Ampl[MaxAmpl];
                    }
                    else {
                        CurrMaxAmp = bscanItem.Signals[j].Ampl[AmplDBIdx_int];
                    }

                    // проверка на попадание в строб поиска ДС и поиск сигнала с максимальной амплитудой
                    if ((Delay_mcs >= tuningGate.gStart) && (Delay_mcs <= tuningGate.gEnd) && (CurrMaxAmp > MaxAmp)) {
                        // Сохраняем параметры ДС
                        tuningItem.DelayHeight = Delay_mcs;
                        tuningItem.Time = GetTickCount_();
                        tuningItem.MaxAmp = CurrMaxAmp;
                        tuningItem.SignalIdx = j;
                        MaxAmp = CurrMaxAmp;
                    }
                }

                RailTypeTrackingModeChanging = RailTypeTrackingModeChanging || ((std::abs(save - tuningItem.DelayHeight) > 3));

                break;
            }
        }
    }
}

void cDevice::RailTypeTracking()
{
    if (RailTypeTrackingMode)
        if (GetTickCount_() - RailTypeTrackingModeTime > 20) {  // Частота выполнения oслеживания ДС

            RailTypeTrackingModeTime = GetTickCount_();
            if (RailTypeTrackingModeChanging) {
                RailTypeTrackingModeChanging = false;
                CalibrationToRailType();
            }
        }
}

void cDevice::shadowMethodSensAutoCalibration(tDEV_BScan2Head* DEV_BScan2Head)
{
    if (GetTickCount_() - ShadowMethodChannelSensAutoCalibrationLastTime >= cfg->SensAutoCalibration.CalibrationDelay) {
        ShadowMethodChannelSensAutoCalibrationLastTime = GetTickCount_();
        for (std::vector<sSensAutoCalibrationItem>::const_iterator calIt = cfg->SensAutoCalibration.List.begin(), total = cfg->SensAutoCalibration.List.end(); calIt != total; ++calIt) {
            const sSensAutoCalibrationItem& currentCalibItem = calIt.operator*();
            for (tBScan2Items::const_iterator it = DEV_BScan2Head->Items.begin(), total = DEV_BScan2Head->Items.end(); it != total; ++it) {
                const tDEV_BScan2HeadItem& currentHeadItem = it.operator*();
                if ((currentCalibItem.id == currentHeadItem.Channel) && (currentCalibItem.Side == currentHeadItem.Side)) {
                    int MaxAmp = -1;

                    for (unsigned int j = 0; j < currentHeadItem.Signals.size(); ++j) {  // Просматриваем сигналы
                        if (currentHeadItem.Signals[j].Ampl[MaxAmpl] > MaxAmp) {
                            MaxAmp = currentHeadItem.Signals[j].Ampl[MaxAmpl];
                        }
                    }

                    if (MaxAmp > 0) {
                        // Вычисление нового значения АТТ (Новый АТТ = Текущий АТТ - Амплитуда максимального сигнала в дБ + Желаемая максимальная амплитуда сигнала)
                        eDeviceSide DEV_Side = currentCalibItem.Side;
                        CID DEV_Channel = currentCalibItem.id;

                        int New_Gain = cal->GetGain(DEV_Side, DEV_Channel, 0) - Table_ADCVal_to_DB[MaxAmp] + currentCalibItem.TargetGain;

                        if ((New_Gain < 10) || (New_Gain > 80)) {
                            New_Gain = 50;
                        }

                        if (std::abs(New_Gain - cal->GetGain(DEV_Side, DEV_Channel, 0)) > cfg->SensAutoCalibration.MinSensChange) {
                            // Сохранение значенией автонастройки Ку для теневого метода контроля - устанавливаются они позднее
                            unsigned int tmp = SensAutoCalibrationData.size();
                            SensAutoCalibrationData.resize(tmp + 1);
                            SensAutoCalibrationData[tmp].Side = DEV_Side;
                            SensAutoCalibrationData[tmp].Channel = DEV_Channel;
                            SensAutoCalibrationData[tmp].NewGain = New_Gain;
                        }
                    }
                }
            }
        }
    }
}

void cDevice::updateShadowMethodAutoGain()
{
    // Установка значенией автонастройки Ку для теневого метода контроля
    for (std::vector<tSensAutoCalibrationDataItem>::const_iterator it = SensAutoCalibrationData.begin(), total = SensAutoCalibrationData.end(); it != total; ++it) {
        const tSensAutoCalibrationDataItem& item = it.operator*();
        cal->SetGain(item.Side, item.Channel, 0, item.NewGain);
        CallBackFunction_(FCurGroupIndex, item.Side, item.Channel, 0, vidGain, item.NewGain);
        SetChannelParams(item.Side, item.Channel, true, false, false, false);
    }
    SensAutoCalibrationData.clear();
}

bool cDevice::filterGetUploadCondition() const
{
    if (_currentDeltaCoord > 0) {
        return _fTailCoord < (_sysCoord - fltrPrmMinPacketSize);
    }
    if (_currentDeltaCoord < 0) {
        return _fTailCoord > (_sysCoord + fltrPrmMinPacketSize);
    }
    return false;
}

void cDevice::postBScan2Data(unsigned long EventId, tDEV_BScan2Head* DEV_BScan2Head)
{
    if (SkipBScanDataInCalibrationMode && (FCurMode == dmCalibration)) {  // Если в режиме настройка данные В/М-развертки не нуждны, то они удаляются
        delete DEV_BScan2Head;
    }
    else {
        DEV_Event->WriteHeadAndPost(EventId, &DEV_BScan2Head, SIZE_OF_TYPE(&DEV_BScan2Head));
    }
}

void cDevice::tickBScanData(unsigned long EventId)
{
    PtUMU_BScanData BScan2Data_ptr = NULL;
    tDEV_BScan2Head* DEV_BScan2Head = new (tDEV_BScan2Head);


    cs->Enter();
    UMU_Event->ReadEventData(&BScan2Data_ptr, SIZE_OF_TYPE(&BScan2Data_ptr), NULL);


    /*    if (BScan2Data_ptr->UMUIdx != 1) {

            cs->Release();
            return;
        }
      */
    if (BScan2Data_ptr->AC.DataExists) {
        DEV_BScan2Head->ACItems.reserve(6);
    }
    // DEV_BScan2Head->DeviceID = cfg->DeviceID; // Установка идентификатора конфигураци - типа прибора
    // Проверка сесии В-развертки и назначения типа канала

    DEV_BScan2Head->BScanSessionID = BScan2Data_ptr->BScanSession;
    if ((BScanSessionID[ctHandScan] == BScan2Data_ptr->BScanSession) && (BScan2Data_ptr->BScanSession != -1)) {
        DEV_BScan2Head->ChannelType = ctHandScan;
    }
    else if ((BScanSessionID[ctCompInsp] == BScan2Data_ptr->BScanSession) && (BScan2Data_ptr->BScanSession != -1)) {
        DEV_BScan2Head->ChannelType = ctCompInsp;
    }
    else {
        delete DEV_BScan2Head;
        delete BScan2Data_ptr;
        cs->Release();
        return;
    }

    //#ifdef DEFCORE_DEBUG
    //    if (DEV_BScan2Head->ChannelType == ctCompInsp) {
    //        checkPereskok(BScan2Data_ptr);
    //    }
    //#endif

    convertBScanFormat(BScan2Data_ptr, DEV_BScan2Head);
    collectRailTuningData(DEV_BScan2Head);

    // Автонастройка Ку для теневого метода контроля

    if (_enableSMChSensAutoCalibration) {
        shadowMethodSensAutoCalibration(DEV_BScan2Head);
    }

    if (EventId == edBScan2Data) {
        LastSysCoord = DEV_BScan2Head->XSysCrd[0];
    }

    _sysCoord = DEV_BScan2Head->XSysCrd[0];


    _currentDeltaCoord = DEV_BScan2Head->Dir[0];
    _displayCoord = DEV_BScan2Head->XDisCrd[0];

    if (_currentDeltaCoord == 0) {
        _alarmSendTick = GetTickCount_();

        delete BScan2Data_ptr;
        BScan2Data_ptr = NULL;
        DEV_Event->WriteHeadAndPost(EventId, &DEV_BScan2Head, SIZE_OF_TYPE(&DEV_BScan2Head));

        if (cfg->getSendBottomSignal()) {
            sendBottomSignalAmpl();
        }
        cs->Release();
        updateShadowMethodAutoGain();
        return;
    }
    // ----------------------------------------------------------------------------------------------------------------------
    // Фильтр сигналов
    // ----------------------------------------------------------------------------------------------------------------------
    if (_enableFiltration && (DEV_BScan2Head->ChannelType != ctHandScan) && (DEV_BScan2Head->PathEncodersIndex == 0) && (FCurMode != dmCalibration) && EventId == edBScan2Data) {
        if (_pathEncoderResetComandSent) {
            // Про проверка события ресета системной координаты
            if (TestResetPathEncoder()) {
                _pathEncoderResetComandSent = false;
                _fFirstCoord = true;
            }
        }

        if (_fFirstCoord) {
            // Первая координата после ресета
            // Инициализируем параметры фильтра и отправляем её как есть.

            _fPrevSysCoord = _sysCoord;

            if (_currentDeltaCoord > 0) {
                _fPrevDir = 1;
            }
            else if (_currentDeltaCoord < 0) {
                _fPrevDir = -1;
            }

            // Полный ресет фильтра. В нем очищаются все маски
            filterResetPacketProccess();

            // Инициализируем хвост фильтра
            if (_currentDeltaCoord > 0) {
                _fTailCoord = _sysCoord - fltrPrmMinPacketSize - (_currentDeltaCoord - 1);
            }
            else if (_currentDeltaCoord < 0) {
                _fTailCoord = _sysCoord + fltrPrmMinPacketSize + (_currentDeltaCoord + 1);
            }

            _fFirstCoord = false;
            delete BScan2Data_ptr;
            BScan2Data_ptr = NULL;
            DEV_Event->WriteHeadAndPost(EventId, &DEV_BScan2Head, SIZE_OF_TYPE(&DEV_BScan2Head));
            cs->Release();
            return;
        }

        // Проверяим изменилось ли направление движения на противоположное.
        if (_currentDeltaCoord > 0) {
            _fCurrentDir = 1;
        }
        else if (_currentDeltaCoord < 0) {
            _fCurrentDir = -1;
        }

        if (_fCurrentDir != _fPrevDir) {
            // Направление движения изменилось!
            // Отправляем координаты и снгналы накопленные в фильтре
            // до предидущей координаты включительно

            bool upLoadCondition = _fPrevDir > 0 ? _fTailCoord <= _fPrevSysCoord : _fTailCoord >= _fPrevSysCoord;

            while (upLoadCondition) {
                if (!filterUploadSignals(_fPrevSysCoord, _fPrevDir, _fTailCoord)) {
                    break;
                }
                _fPrevDir > 0 ? _fTailCoord++ : _fTailCoord--;
                upLoadCondition = _fPrevDir > 0 ? _fTailCoord <= _fPrevSysCoord : _fTailCoord >= _fPrevSysCoord;
            }

            // Полный ресет фильтра. В нем очищаются все маски
            filterResetPacketProccess();

            // Инициализируем хвост фильтра
            if (_currentDeltaCoord > 0) {
                _fTailCoord = _sysCoord - fltrPrmMinPacketSize - 1;
            }
            else if (_currentDeltaCoord < 0) {
                _fTailCoord = _sysCoord + fltrPrmMinPacketSize + 1;
            }
        }

        sUMUParams umuParams;

        umuParams.UMUIdx = BScan2Data_ptr->UMUIdx;
        umuParams.Dir = BScan2Data_ptr->Dir[0];
        umuParams.PathEncodersIndex = DEV_BScan2Head->PathEncodersIndex;
        umuParams.Simulator = DEV_BScan2Head->Simulator[0];
        umuParams.XSysCrd = _sysCoord;
        umuParams.XDisCrd = _displayCoord;
        umuParams.ChannelType = DEV_BScan2Head->ChannelType;
        umuParams.BScanSession = BScan2Data_ptr->BScanSession;
        umuParams.ACHeadItems = DEV_BScan2Head->ACItems;

        fUMUParamsStorage.push_back(umuParams);

        _fPrevDir = _fCurrentDir;
        _fPrevSysCoord = _sysCoord;

        // Перебор каналов

        filterAddNewSignal(DEV_BScan2Head);


        //Сохранение сигналов
        bool upLoadCondition = filterGetUploadCondition();

        while (upLoadCondition) {
            if (!filterUploadSignals(_sysCoord, _fCurrentDir, _fTailCoord)) {
                // Ошибка ресет и инициализация фильтра
                filterResetPacketProccess();
                if (_currentDeltaCoord > 0) {
                    _fTailCoord = _sysCoord - fltrPrmMinPacketSize - 1;
                }
                else if (_currentDeltaCoord < 0) {
                    _fTailCoord = _sysCoord + fltrPrmMinPacketSize + 1;
                }
                break;
            }
            if (_currentDeltaCoord > 0) {
                _fTailCoord++;
                upLoadCondition = _fTailCoord < (_sysCoord - fltrPrmMinPacketSize);
            }
            else {
                if (_currentDeltaCoord < 0) {
                    _fTailCoord--;
                    upLoadCondition = _fTailCoord > (_sysCoord + fltrPrmMinPacketSize);
                }
            }
        }

        delete DEV_BScan2Head;
        delete BScan2Data_ptr;
    }
    else {  //Без фильтрации
        delete BScan2Data_ptr;
        BScan2Data_ptr = NULL;
        postBScan2Data(EventId, DEV_BScan2Head);
    }
    if (cfg->getSendBottomSignal()) {
        sendBottomSignalAmpl();
    }
    cs->Release();
    updateShadowMethodAutoGain();
}

bool pred(const PtUMU_BScanData& a)
{
    return !a;
};

void cDevice::tickBScanData_2UMU(unsigned long EventId)
{
    PtUMU_BScanData BScan2Data_ptr = NULL;

    cs->Enter();
    UMU_Event->ReadEventData(&BScan2Data_ptr, SIZE_OF_TYPE(&BScan2Data_ptr), NULL);

    PathEncoderDebug[BScan2Data_ptr->UMUIdx][BScan2Data_ptr->PathEncodersIndex].Simulator = BScan2Data_ptr->Simulator[BScan2Data_ptr->PathEncodersIndex];
    PathEncoderDebug[BScan2Data_ptr->UMUIdx][BScan2Data_ptr->PathEncodersIndex].XSysCrd = BScan2Data_ptr->XSysCrd[BScan2Data_ptr->PathEncodersIndex];
    PathEncoderDebug[BScan2Data_ptr->UMUIdx][BScan2Data_ptr->PathEncodersIndex].XDisCrd = BScan2Data_ptr->XDisCrd[BScan2Data_ptr->PathEncodersIndex];

    if ((BScanSessionID[ctHandScan] != BScan2Data_ptr->BScanSession) && (BScanSessionID[ctCompInsp] != BScan2Data_ptr->BScanSession)) {
        delete BScan2Data_ptr;
        cs->Release();
        return;
    }

    if (UMUBScan2Data_ptrs[0].size() > 0) {
        if ((BScan2Data_ptr->UMUIdx == 0) && (BScan2Data_ptr->BScanSession > UMUBScan2Data_ptrs[0].back()->BScanSession)) {
            UMUBScan2Data_ptrs[0].clear();
        }
    }
    if (UMUBScan2Data_ptrs[1].size() > 0) {
        if ((BScan2Data_ptr->UMUIdx == 1) && (BScan2Data_ptr->BScanSession > UMUBScan2Data_ptrs[1].back()->BScanSession)) {
            UMUBScan2Data_ptrs[1].clear();
        }
    }
    /*
    #ifdef DeviceLog
    if (BScan2Data_ptr->UMUIdx == 0) {
        Log->Add(Format("T: %d; UMU: %d; SysCrd: %d; DisCrd: %d", ARRAYOFCONST((GetTickCount(), BScan2Data_ptr->UMUIdx, BScan2Data_ptr->XSysCrd[0], BScan2Data_ptr->XDisCrd[0]))));
    } else Log->Add(Format("T: %d; UMU: %d;                                  SysCrd: %d; DisCrd: %d", ARRAYOFCONST((GetTickCount(), BScan2Data_ptr->UMUIdx, BScan2Data_ptr->XSysCrd[0], BScan2Data_ptr->XDisCrd[0]))));
    #endif
   */

    if (BScan2Data_ptr->UMUIdx < 2)
        UMUBScan2Data_ptrs[BScan2Data_ptr->UMUIdx].push_back(BScan2Data_ptr);
    else {
        BScan2Data_ptr->UMUIdx = 0;
        delete BScan2Data_ptr;
        //  delete BScan2Data_ptr;
        cs->Release();
        return;
    }

#ifdef DeviceLog
    std::vector<PtUMU_BScanData>::iterator it_0;
    std::vector<PtUMU_BScanData>::iterator it_1;
    //    PtUMU_BScanData tmp = UMUBScan2Data_ptrs[BScan2Data_ptr->UMUIdx].back();
    //    if (tmp->UMUIdx == 0) {
    //        Log->Add(Format("T: %d; UMU: %d; SysCrd: %d; DisCrd: %d", ARRAYOFCONST((GetTickCount(), tmp->UMUIdx, tmp->XSysCrd[0], tmp->XDisCrd[0]))));
    //    } else Log->Add(Format("T: %d; UMU: %d;                                  SysCrd: %d; DisCrd: %d", ARRAYOFCONST((GetTickCount(), tmp->UMUIdx, tmp->XSysCrd[0], tmp->XDisCrd[0]))));


    Log->Add(" ");
    Log->Add("--LIST UMU0-----------------------------------------------");
    for (it_0 = UMUBScan2Data_ptrs[0].begin(); it_0 != UMUBScan2Data_ptrs[0].end(); ++it_0) {
        PtUMU_BScanData tmp = it_0.operator*();
        if (tmp)
            Log->Add(Format("UMU: %d; SysCrd: %d; DisCrd: %d", ARRAYOFCONST((tmp->UMUIdx, tmp->XSysCrd[0], tmp->XDisCrd[0]))));
        else
            Log->Add("NULL");
    }
    Log->Add("--LIST UMU1--");
    for (it_1 = UMUBScan2Data_ptrs[1].begin(); it_1 != UMUBScan2Data_ptrs[1].end(); ++it_1) {
        PtUMU_BScanData tmp = it_1.operator*();
        if (tmp)
            Log->Add(Format("UMU: %d; SysCrd: %d; DisCrd: %d", ARRAYOFCONST((tmp->UMUIdx, tmp->XSysCrd[0], tmp->XDisCrd[0]))));
        else
            Log->Add("NULL");
    }

#endif


    //    it_0 = UMUBScan2Data_ptrs[BScan2Data_ptr->UMUIdx].begin();
    //    PtUMU_BScanData ptr = it_0.operator *();
    //    if (ptr->AC.DataExists) it_1 = UMUBScan2Data_ptrs[BScan2Data_ptr->UMUIdx].begin();

    if ((UMUBScan2Data_ptrs[0].size() == 0) || (UMUBScan2Data_ptrs[1].size() == 0)) {
        cs->Release();
        return;
    }
    /*
        it_0 = UMUBScan2Data_ptrs[0].front();
        it_1 = UMUBScan2Data_ptrs[1].front();


        if ((it_0.operator *() != NULL) && (it_1.operator *() != NULL))

            if ((it_0.operator *())->XDisCrd[0] == (it_1.operator *())->XDisCrd[0])
            if ((it_0.operator *())->XDisCrd[0] < (it_1.operator *())->XDisCrd[0])
            if ((it_0.operator *())->XDisCrd[0] > (it_1.operator *())->XDisCrd[0])

        for (it_0 = UMUBScan2Data_ptrs[0].begin(); it_0 != UMUBScan2Data_ptrs[0].end(); ++it_0)
            for (it_1 = UMUBScan2Data_ptrs[1].begin(); it_1 != UMUBScan2Data_ptrs[1].end(); ++it_1) {
                if ((it_1.operator *() != NULL) && (it_0.operator *() != NULL))

        if ((it_0.operator *())->XDisCrd[0] == (it_1.operator *())->XDisCrd[0]) {

      */

    //    if (UMUBScan2Data_ptrs[0][0]->XDisCrd[0] < UMUBScan2Data_ptrs[0][1]->XDisCrd[0])

    merge2UMUData(EventId);

    cs->Release();
}

void cDevice::merge2UMUData(unsigned long EventId)
{
    std::vector<PtUMU_BScanData>::iterator left = UMUBScan2Data_ptrs[0].begin();
    std::vector<PtUMU_BScanData>::iterator leftEnd = UMUBScan2Data_ptrs[0].end();
    std::vector<PtUMU_BScanData>::iterator right = UMUBScan2Data_ptrs[1].begin();
    std::vector<PtUMU_BScanData>::iterator rightEnd = UMUBScan2Data_ptrs[1].end();

    while (left != leftEnd && right != rightEnd) {
        if (left.operator*() == NULL) {
            ++left;
            continue;
        }
        if (right.operator*() == NULL) {
            ++right;
            continue;
        }

        if (left.operator*()->XDisCrd[0] == right.operator*()->XDisCrd[0]) {
            if ((left.operator*())->BScanSession != (right.operator*())->BScanSession) {
                cs->Release();
                return;
            }

#ifdef DeviceLog
            Log->Add(Format(" -------- Res Crd: %d; ------------", ARRAYOFCONST(((it_0.operator*())->XDisCrd[0]))));
#endif

            tDEV_BScan2Head* DEV_BScan2Head = new (tDEV_BScan2Head);

            if (((left.operator*())->AC.DataExists) || ((right.operator*())->AC.DataExists)) {
                DEV_BScan2Head->ACItems.reserve(6);
            }

            DEV_BScan2Head->BScanSessionID = std::min((left.operator*())->BScanSession, (right.operator*())->BScanSession);

            DEV_BScan2Head->ChannelType = ctCompInsp;
            convertBScanFormat(left.operator*(), DEV_BScan2Head);   // ?
            convertBScanFormat(right.operator*(), DEV_BScan2Head);  // ?

            collectRailTuningData(DEV_BScan2Head);

            // Автонастройка Ку для теневого метода контроля

            if (_enableSMChSensAutoCalibration) {
                shadowMethodSensAutoCalibration(DEV_BScan2Head);
            }

            if (EventId == edBScan2Data) {
                LastSysCoord = DEV_BScan2Head->XSysCrd[0];
            }

            _sysCoord = DEV_BScan2Head->XSysCrd[0];
            _currentDeltaCoord = DEV_BScan2Head->Dir[0];
            _displayCoord = DEV_BScan2Head->XDisCrd[0];

            if (_currentDeltaCoord == 0) {
                _alarmSendTick = GetTickCount_();

                // DEV_Event->WriteHeadBodyAndPost(EventId, &DEV_BScan2Head, SIZE_OF_TYPE(&DEV_BScan2Head), &BScan2Data_ptr, SIZE_OF_TYPE(&BScan2Data_ptr));
                postBScan2Data(EventId, DEV_BScan2Head);
                //                postBScan2Data(edScanData, right.operator*(), DEV_BScan2Head);

                if (cfg->getSendBottomSignal()) {
                    sendBottomSignalAmpl();
                }
                cs->Release();
                updateShadowMethodAutoGain();
                return;
            }

            //Без фильтрации
            postBScan2Data(EventId, DEV_BScan2Head);
            //            postBScan2Data(edScanData, right.operator*(), DEV_BScan2Head);

            if (cfg->getSendBottomSignal()) {
                sendBottomSignalAmpl();
            }
            updateShadowMethodAutoGain();

            delete right.operator*();
            delete left.operator*();
            left.operator*() = NULL;
            right.operator*() = NULL;

            ++left;
            ++right;
        }
        else {
            if (left.operator*()->XDisCrd[0] > right.operator*()->XDisCrd[0]) {
                uploadBscanPacket(right, EventId);
            }
            else {
                uploadBscanPacket(left, EventId);
            }
        }
    }

    UMUBScan2Data_ptrs[0].erase(remove_if(UMUBScan2Data_ptrs[0].begin(), UMUBScan2Data_ptrs[0].end(), pred), UMUBScan2Data_ptrs[0].end());
    UMUBScan2Data_ptrs[1].erase(remove_if(UMUBScan2Data_ptrs[1].begin(), UMUBScan2Data_ptrs[1].end(), pred), UMUBScan2Data_ptrs[1].end());
    //    qDebug() << UMUBScan2Data_ptrs[0].size() << "/" << UMUBScan2Data_ptrs[1].size();
}

void cDevice::uploadBscanPacket(std::vector<PtUMU_BScanData>::iterator it, unsigned long eventId)
{
    tDEV_BScan2Head* DEV_BScan2Head = new (tDEV_BScan2Head);

    if ((it.operator*())->AC.DataExists) {
        DEV_BScan2Head->ACItems.reserve(6);
    }

    DEV_BScan2Head->BScanSessionID = (it.operator*())->BScanSession;
    DEV_BScan2Head->ChannelType = ctCompInsp;
    convertBScanFormat(it.operator*(), DEV_BScan2Head);  // ?

    collectRailTuningData(DEV_BScan2Head);

    // Автонастройка Ку для теневого метода контроля

    if (_enableSMChSensAutoCalibration) {
        shadowMethodSensAutoCalibration(DEV_BScan2Head);
    }

    if (eventId == edBScan2Data) {
        LastSysCoord = DEV_BScan2Head->XSysCrd[0];
    }

    _sysCoord = DEV_BScan2Head->XSysCrd[0];
    _currentDeltaCoord = DEV_BScan2Head->Dir[0];
    _displayCoord = DEV_BScan2Head->XDisCrd[0];

    if (_currentDeltaCoord == 0) {
        _alarmSendTick = GetTickCount_();

        delete it.operator*();
        it.operator*() = NULL;
        postBScan2Data(eventId, DEV_BScan2Head);


        if (cfg->getSendBottomSignal()) {
            sendBottomSignalAmpl();
        }
        cs->Release();
        updateShadowMethodAutoGain();
        return;
    }

    //Без фильтрации
    postBScan2Data(eventId, DEV_BScan2Head);
    delete it.operator*();
    it.operator*() = NULL;
    if (cfg->getSendBottomSignal()) {
        sendBottomSignalAmpl();
    }
    updateShadowMethodAutoGain();


    ++it;
}

void cDevice::tickPathStepData()
{
    cs->Enter();
    // Просто перенос данных из сруктуры - tUMU_PathStepData в tDEV_PathStepData

    PtUMU_PathStepData PathStepData_ptr;
    UMU_Event->ReadEventData(&PathStepData_ptr, SIZE_OF_TYPE(&PathStepData_ptr), NULL);
    if (PathStepData_ptr->PathEncodersIndex == cfg->getMainPathEncoderIndex()) {
        LastSysCoord = PathStepData_ptr->XSysCrd[0];
    }
    tDEV_PathStepData* DEV_PathStepData = new (tDEV_PathStepData);
    memcpy(DEV_PathStepData, PathStepData_ptr, sizeof(tUMU_PathStepData));

    DEV_Event->WriteHeadAndPost(edPathStepData, &DEV_PathStepData, SIZE_OF_TYPE(&DEV_PathStepData));
    cs->Release();

    delete PathStepData_ptr;
}

void cDevice::tickA15ScanerPathStepData()
{
    cs->Enter();
    // Просто пересылка данных под видом PtDEV_A15ScanerPathStepData
    PtUMU_A15ScanerPathStepData A15ScanerPathStepData_ptr;
    UMU_Event->ReadEventData(&A15ScanerPathStepData_ptr, SIZE_OF_TYPE(&A15ScanerPathStepData_ptr), NULL);
    DEV_Event->WriteHeadAndPost(edA15ScanerPathStepData, &A15ScanerPathStepData_ptr, SIZE_OF_TYPE(&A15ScanerPathStepData_ptr));
    cs->Release();
}

void cDevice::tickMetalSensor()
{
    cs->Enter();
    PtUMU_MetalSensorData MetalSensorData_ptr;
    UMU_Event->ReadEventData(&MetalSensorData_ptr, SIZE_OF_TYPE(&MetalSensorData_ptr), NULL);
    tDEV_MetalSensorData* DEV_MetalSensorData = new (tDEV_MetalSensorData);

    DEV_MetalSensorData->UMUIdx = MetalSensorData_ptr->UMUIdx;
    switch (MetalSensorData_ptr->Side) {
    case usLeft:
        DEV_MetalSensorData->Side = dsLeft;
        break;  // Левая
    case usRight:
        DEV_MetalSensorData->Side = dsRight;
        break;  // Правая
    }
    DEV_MetalSensorData->SensorId = MetalSensorData_ptr->SensorId;
    DEV_MetalSensorData->State = MetalSensorData_ptr->State;

    DEV_Event->WriteHeadBodyAndPost(edMSensor, &DEV_MetalSensorData, SIZE_OF_TYPE(&DEV_MetalSensorData), &DEV_MetalSensorData, SIZE_OF_TYPE(&DEV_MetalSensorData));
    cs->Release();

    delete MetalSensorData_ptr;
}

void cDevice::ManageSensBySpeed(bool State, float pathStep_mm)
{
    SensBySpeedState = State;
    SensBySpeed_pathStep = pathStep_mm;
}

void cDevice::tickDeviceSpeed()
{
    cs->Enter();
    unsigned int speedInStepsPerSecond;
    UMU_Event->ReadEventData(&speedInStepsPerSecond, sizeof(unsigned int), NULL);
    DEV_Event->WriteHeadAndPost(edDeviceSpeed, &speedInStepsPerSecond, sizeof(unsigned int));
    cs->Release();

    float speed = (float) speedInStepsPerSecond * SensBySpeed_pathStep * (float) 3600 / (float) 1000000;
    last_speed = speed;
    if (SensBySpeedState) {
        if (speedZone == -1) {
            if (speed < 10) {
                speedZone = 1;
            }
            else if (speed < 20) {
                speedZone = 2;
            }
            else {
                speedZone = 3;
            }
        }
        else {
            if ((speedZone == 1) && (speed >= 21)) {
                speedZone = 3;
            }
            else if ((speedZone == 1) && (speed >= 11)) {
                speedZone = 2;
            }
            else if ((speedZone == 2) && (speed < 9)) {
                speedZone = 1;
            }
            else if ((speedZone == 2) && (speed >= 21)) {
                speedZone = 3;
            }
            else if ((speedZone == 3) && (speed < 9)) {
                speedZone = 1;
            }
            else if ((speedZone == 3) && (speed < 19)) {
                speedZone = 2;
            }
            else
                speedZone = last_speedZone;
        }

        if ((last_speedZone != -1) && (last_speedZone != speedZone)) {
            if ((last_speedZone == 1) && (speedZone == 2)) {
                setSensDeltaForAllChannels(+2);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, +2);
            }
            else if ((last_speedZone == 1) && (speedZone == 3)) {
                setSensDeltaForAllChannels(+4);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, +4);
            }
            else if ((last_speedZone == 2) && (speedZone == 1)) {
                setSensDeltaForAllChannels(-2);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, -2);
            }
            else if ((last_speedZone == 2) && (speedZone == 3)) {
                setSensDeltaForAllChannels(+2);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, +2);
            }
            else if ((last_speedZone == 3) && (speedZone == 1)) {
                setSensDeltaForAllChannels(-4);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, -4);
            }
            else if ((last_speedZone == 3) && (speedZone == 2)) {
                setSensDeltaForAllChannels(-2);
                CallBackFunction_(FCurGroupIndex, dsLeft, 1, 0x01, vidChangeSensBySpeed, -2);
            }

            Update(false);
        }
        last_speedZone = speedZone;
    }
}

void cDevice::setSensDeltaForAllChannels(int delta)
{
    if (cfg->getControlledRail() == crSingle) return;

    SensBySpeed_summDelta = SensBySpeed_summDelta + delta;

    sScanChannelDescription val;
    sChannelDescription ChanDesc;

    unsigned int scanChannelsCount = cfg->GetScanChannelsCount_();
    for (unsigned int i = 0; i < scanChannelsCount; ++i) {
        cfg->getSChannelbyIdx(i, &val);
        if ((val.UMUIndex == 0) && (val.StrokeGroupIdx == FCurGroupIndex)) {
            tbl->ItemByCID(val.Id, &ChanDesc);
            for (int ds = dsLeft; ds <= dsRight; ds++)
                for (int GateIdx = 1; GateIdx <= ChanDesc.cdGateCount; ++GateIdx) {
                    int newSens = cal->GetSens(static_cast<eDeviceSide>(ds), val.Id, GateIdx - 1) + delta;
                    if ((newSens < -128) || (newSens > 128)) continue;
                    int newGain = cal->GetGain(static_cast<eDeviceSide>(ds), val.Id, GateIdx - 1) + delta;
                    if ((newGain < -128) || (newGain > 128)) continue;
                    cal->SetSens(static_cast<eDeviceSide>(ds), val.Id, GateIdx - 1, newSens);
                    cal->SetGain(static_cast<eDeviceSide>(ds), val.Id, GateIdx - 1, newGain);
                    SetChannelParams(static_cast<eDeviceSide>(ds), val.Id, true, false, false, false);
                    CallBackFunction_(FCurGroupIndex, static_cast<eDeviceSide>(ds), val.Id, GateIdx, vidSens, newSens);
                    CallBackFunction_(FCurGroupIndex, static_cast<eDeviceSide>(ds), val.Id, GateIdx, vidGain, newGain);
                }
        }
    }
}

void cDevice::tickCheckUMUConnection()
{
    unsigned long Time = GetTickCount_();

    if (Time - LastOnLineTestTime > 3000) {  // Проверяем 1 раз в 3 сек.

        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            UMUList.at(UMUIdx)->GetDeviceID();
            if ((!FirstUMUOnLineTest) && (Time - UMUList.at(UMUIdx)->getLastEventTime() > 5000)) {  // Нет ответа от БУМ больше 5 сек.
                // TODO: unsigned integer overflow: 1498812397749 - 1498812398190 cannot be represented in type 'unsigned long'

                UMUOnLineStatusCount[UMUIdx]++;
                if (UMUOnLineStatusCount[UMUIdx] > 3) {
                    UMUOnLineStatus[UMUIdx] = false;
                }
            }
            else {
                UMUOnLineStatus[UMUIdx] = true;
                UMUOnLineStatusCount[UMUIdx] = 0;
            }
        }
        FirstUMUOnLineTest = false;

        // Заполнение версии прошивки и серийного номера
        //      (Выполняется единожды по получении данных)
        for (size_t i = 0; i < UnitsData.size(); ++i) {
            if (!UnitsData[i].WorksNumber &&      // WorksNumber не заполнен
                UMUList[i]->getSerialNumber()) {  // Но уже получен
                UnitsData[i].WorksNumber = new char[32];
                snprintf(UnitsData[i].WorksNumber, 32, "%d", UMUList[i]->getSerialNumber());
            }
            if (!UnitsData[i].FirmwareVer &&          // FirmwareVer не заполненa
                UMUList[i]->getFirmwareVerionHi()) {  // Но уже полученa
                UnitsData[i].FirmwareVer = new char[32];
                snprintf(UnitsData[i].FirmwareVer, 32, "%d.%d.%d", UMUList[i]->getFirmwareVerionHi(), UMUList[i]->getFirmwareVerionLo(), getBuildNumberForUMU(0));
            }
        }
        LastOnLineTestTime = GetTickCount_();
    }
}

void cDevice::tickFixBadSensState()
{
    if (FixBadSensState) {
        unsigned long Time = GetTickCount_();
        if (Time - ValidRangesTestTime_ms >= ValidRangesTestPeriod_ms) {
            ValidRangesTestTime_ms = Time - ValidRangesTestPeriod_ms;
            for (std::vector<tKuValidRanges>::iterator it = ValidRangesArr.begin(), total = ValidRangesArr.end(); it != total; ++it) {
                tKuValidRanges& currentValidRange = it.operator*();
                if (currentValidRange.BadState) {
                    if (GetTickCount_() - currentValidRange.Time > TimeToFixBadSens * 1000) {
                        int TestSensValid = TestSensValidRanges(currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx);
                        if (TestSensValid != 0) {
                            int New = currentValidRange.GoodSens;
                            int Delta = New - cal->GetSens(currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx - 1);
                            cal->SetSens(currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx - 1, New);
                            int NewGain = cal->GetGain(currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx - 1) + Delta;

                            cal->SetGain(currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx - 1, NewGain);
                            CallBackFunction_(FCurGroupIndex, currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx, vidSens, New);
                            CallBackFunction_(FCurGroupIndex, currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx, vidTestSens, 0);
                            CallBackFunction_(FCurGroupIndex, currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx, vidGain, NewGain);
                            CallBackFunction_(FCurGroupIndex, currentValidRange.Side, currentValidRange.Channel, currentValidRange.GateIdx, vidReturnSens, currentValidRange.GoodSens);
                            SetChannelParams(currentValidRange.Side, currentValidRange.Channel, true, false, false, false);
                        }

                        currentValidRange.BadState = false;
                    }
                }
            }
        }
    }
}

bool cDevice::Tick()  // Метод жизнедеятельности класса (вызывать в цикле)
{
    // int totalFilterSize = debugGetTotalFilterSize();
    // std::cerr << "FILTER SIZE:" << totalFilterSize << " bytes, " << totalFilterSize / 1024 << " kb, " << totalFilterSize / (1024 * 1024) << " MB" << std::endl;
    unsigned long EventId;

    if (UMU_Event->WaitForEvent()) {
        _externalCriticalSection->Enter();
        while (UMU_Event->EventDataSize() >= 8) {
            UMU_Event->ReadEventData(&EventId, SIZE_OF_TYPE(&EventId), NULL);
            switch (EventId) {
            case edBScan2Data:  // BScan
            case edMScan2Data:  // MScan
            {
                if ((cfg->GetUMUCount() == 1) || (EventId == edMScan2Data))
                    tickBScanData(EventId);
                else
                    tickBScanData_2UMU(EventId);
                break;
            }
            case edPathStepData:  // Данные датчика пути
            {
                tickPathStepData();
                break;
            }
            case edA15ScanerPathStepData:  // Данные датчика пути сканера А15
            {
                tickA15ScanerPathStepData();
                break;
            }
            case edAlarmData: {
                tickAlarmData();
                break;
            }
            case edAScanMeas:  // А-развертка, точное значение амплитуды и задержки
            {
                tickAScanMeas();
                break;
            }
            case edAScanData:  // AScan
            {
                tickAScanData();
                break;
            }
            case edTVGData:  // Отсчеты ВРЧ
            {
                tickTVGData();
                break;
            }
            case edMSensor:  // Данные от датчиков металла
            {
                tickMetalSensor();
                break;
            }
            case edDeviceSpeed: {
                tickDeviceSpeed();
                break;
            }
            default: {
                assert(true);
                break;
            }
            }
        }
        _externalCriticalSection->Release();
    }
    _externalCriticalSection->Enter();
    // Проверка связи с БУМами
    tickCheckUMUConnection();
    tickFixBadSensState();
    _externalCriticalSection->Release();

    RailTypeTracking();
    return !EndWorkFlag;
}

// Установка параметров канала
void cDevice::SetChannelParams(eDeviceSide Side, CID Channel, bool Gain_Flag, bool PrismDelay_Flag, bool Gate_Flag, bool BScanGate_Flag)
{
    DEFCORE_ASSERT(Channel < 255);
    //    int XScale[6] = { 100, 200, 400, 600, 800, 1000};
    //    int YScale[6] = { 100, 200, 400, 600, 800, 1000};

    //	int XScale[6] = {  95, 196, 405, 600, 800, 1000};
    //	int YScale[6] = { 497, 624, 767, 856, 935, 1000};

    //	int XScale[6] = { 95, 196, 405, 600, 800, 1000};
    //	int YScale[6] = { 400, 650, 875, 970, 995, 1000};

    int XScale[6] = {120, 300, 500, 700, 900, 1000};
    int YScale[6] = {0, 460, 700, 875, 960, 1000};

    eDeviceSide SaveCurSide = FCurSide;
    int SaveCurChannel = FCurChannel;
    int SaveCurGateIdx = FCurrentGateIdx;

    FCurSide = Side;
    FCurChannel = Channel;

    tChannelData ChData;
    if (!cfg->GetChannelData(Side, Channel, &ChData)) {
        FCurSide = SaveCurSide;
        FCurChannel = SaveCurChannel;
        FCurrentGateIdx = SaveCurGateIdx;
        return;
    }
    cIUMU* currentUMU = UMUList[ChData.UMUIndex];
    DEFCORE_ASSERT(currentUMU);
    //    UMUList[ChData.UMUIndex]->delayedUnloadStart();
    if (!tbl->isHandScan(Channel)) {     // Для каналов сканирования
        sScanChannelDescription SCDesc;  // Проверка используется ли в данный монт этот канал
        memset(&SCDesc, 0xff, sizeof(sScanChannelDescription));
        int Idx = cfg->getFirstSChannelbyID(Channel, &SCDesc);
        while (Idx != -1) {
            if ((SCDesc.StrokeGroupIdx == FCurGroupIndex) && (!SCDesc.Used)) {
                FCurSide = SaveCurSide;
                FCurChannel = SaveCurChannel;
                FCurrentGateIdx = SaveCurGateIdx;
                return;
            }
            Idx = cfg->getNextSChannelbyID(Idx, Channel, &SCDesc);
        }
    }

    sChannelDescription ChanDesc;
    tbl->ItemByCID(Channel, &ChanDesc);
    /*
        if (Gain_Flag) {    // Установка Ку, АТТ, ВРЧ
            if ((GetGateCount() != 2) || (FCurMode == dmCalibration)) {
                unsigned short StDelay = 0;
                unsigned char StVal = static_cast<unsigned char>(cfg->GainBase);
                //unsigned char EdVal = static_cast<unsigned char>(cfg->GainBase);

                for (unsigned char PointIdx = 0; PointIdx <= 5; PointIdx++) {
                    unsigned short EdDelay = static_cast<unsigned short>(GetTVG() * XScale[PointIdx] / 1000);
                    unsigned char EdVal = static_cast<unsigned char>(cfg->GainBase + GetGain() / cfg->GainStep * YScale[PointIdx] / 1000);
                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                                                        static_cast<unsigned char>(ChData.ResLine),
                                                                                        static_cast<unsigned char>(ChData.Stroke),
                                                                                        static_cast<unsigned char>(StDelay),
                                                                                        StVal,
                                                                                        static_cast<unsigned char>(EdDelay),
                                                                                        EdVal,
                                                                                        ChanDesc.cdAScanScale);
                    StDelay = EdDelay;
                    StVal = EdVal;
                }
                // Второй отрезок - от ВРЧ [мкс] до конца [мкс]
                UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                         static_cast<unsigned char>(ChData.ResLine),
                                                         static_cast<unsigned char>(ChData.Stroke),
                                                         static_cast<unsigned char>(GetTVG()),
                                                         static_cast<unsigned char>(cfg->GainBase + GetGain() / cfg->GainStep), //TODO: double -> char precision loss
                                                         static_cast<unsigned char>(ChanDesc.cdAScanDuration),
                                                         static_cast<unsigned char>(cfg->GainBase + GetGain() / cfg->GainStep),
                                                         ChanDesc.cdAScanScale);
            }
            else {
                // Первый отрезок - от 0 [мкс] до ВРЧ [мкс]
                unsigned short StDelay = 0;
                unsigned char StVal = static_cast<unsigned char>(cfg->GainBase);
                for (unsigned char PointIdx = 0; PointIdx <= 5; PointIdx++) {
                    unsigned short EdDelay = static_cast<unsigned short>(GetTVG() * XScale[PointIdx] / 1000);
                    unsigned char EdVal = static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep * YScale[PointIdx] / 1000);

                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                             static_cast<unsigned char>(ChData.ResLine),
                                                             static_cast<unsigned char>(ChData.Stroke),
                                                             static_cast<unsigned char>(StDelay),
                                                             StVal,
                                                             static_cast<unsigned char>(EdDelay),
                                                             EdVal,
                                                             ChanDesc.cdAScanScale);
                    StDelay = EdDelay;
                    StVal = EdVal;
                }


                if (GetTVG() < GetEdGate(2)) {

                // Второй отрезок - от ВРЧ [мкс] до конца строба основного канала. Значение АТТ основного канала

                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                             static_cast<unsigned char>(ChData.ResLine),
                                                             static_cast<unsigned char>(ChData.Stroke),
                                                             static_cast<unsigned char>(GetTVG()),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep),
                                                             static_cast<unsigned char>(GetStGate(2)),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep),
                                                             ChanDesc.cdAScanScale);

                // Третий отрезок - от конца строба основного канала до конца развертки. Значение АТТ дополнительного канала

                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                             static_cast<unsigned char>(ChData.ResLine),
                                                             static_cast<unsigned char>(ChData.Stroke),
                                                             static_cast<unsigned char>(GetStGate(2)),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                             static_cast<unsigned char>(ChanDesc.cdAScanDuration),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                             ChanDesc.cdAScanScale);
                }
                else {
                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                             static_cast<unsigned char>(ChData.ResLine),
                                                             static_cast<unsigned char>(ChData.Stroke),
                                                             static_cast<unsigned char>(GetTVG()),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep),
                                                             static_cast<unsigned char>(ChanDesc.cdAScanDuration),
                                                             static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep),
                                                             ChanDesc.cdAScanScale);
                }

            }

            if (ChanDesc.cdUseNotch) {

                unsigned short StDelay = GetStNotch();
                unsigned short EdDelay = GetStNotch() + GetNotchLen();
                unsigned char Val = static_cast<unsigned char>(cfg->GainBase + (GetGain(1) - GetNotchLevel()) / cfg->GainStep);

                if (StDelay != 0)
                    UMUList[static_cast<unsigned int>(ChData.UMUIndex)]->SetGainSegment(ChData.Side,
                                                                                        static_cast<unsigned char>(ChData.ResLine),
                                                                                        static_cast<unsigned char>(ChData.Stroke),
                                                                                        static_cast<unsigned char>(StDelay),
                                                                                        Val,
                                                                                        static_cast<unsigned char>(EdDelay),
                                                                                        Val,
                                                                                        ChanDesc.cdAScanScale);
            }
        } */

    if (Gain_Flag) {                                                                                                      // Установка Ку, АТТ, ВРЧ
        if ((GetGateCount() != 2) || (FCurMode == dmCalibration) /* || (RailTypeTrackingMode && (Channel == N0EMS))*/) {  // TODO: HARD CODE
            // unsigned short StDelay = 0;
            // unsigned char StVal = static_cast<unsigned char>(cfg->GainBase);
            // unsigned char EdVal = static_cast<unsigned char>(cfg->GainBase);

            currentUMU->SetGainSegmentPoint(ChData.Side, static_cast<unsigned char>(ChData.ResLine), static_cast<unsigned char>(ChData.Stroke), 0, static_cast<unsigned char>(cfg->getGainBase()), ChanDesc.cdAScanScale, cIUMU::gptStart);


            for (unsigned char PointIdx = 0; PointIdx <= 5; PointIdx++) {
                currentUMU->SetGainSegmentPoint(ChData.Side,
                                                ChData.ResLine,
                                                ChData.Stroke,
                                                static_cast<unsigned char>(GetTVG() * XScale[PointIdx] / 1000),
                                                // static_cast<unsigned char>(cfg->GainBase + GetGain() / cfg->GainStep * YScale[PointIdx] / 1000),
                                                cfg->dBGain_to_UMUGain(GetGain() * YScale[PointIdx] / 1000),
                                                ChanDesc.cdAScanScale,
                                                cIUMU::gptCentr);
            }
            // Второй отрезок - от ВРЧ [мкс] до конца [мкс]
            currentUMU->SetGainSegmentPoint(ChData.Side,
                                            ChData.ResLine,
                                            ChData.Stroke,
                                            ChanDesc.cdAScanDuration,
                                            //  static_cast<unsigned char>(static_cast<unsigned char>(cfg->GainBase + GetGain() / cfg->GainStep)),
                                            cfg->dBGain_to_UMUGain(GetGain()),
                                            ChanDesc.cdAScanScale,
                                            cIUMU::gptEnd);
            CheckSetBadSens(FCurSide, FCurChannel, 1);
        }
        else {
            currentUMU->SetGainSegmentPoint(ChData.Side, static_cast<unsigned char>(ChData.ResLine), static_cast<unsigned char>(ChData.Stroke), 0, static_cast<unsigned char>(cfg->getGainBase()), ChanDesc.cdAScanScale, cIUMU::gptStart);
            // Первый отрезок - от 0 [мкс] до ВРЧ [мкс]

            for (unsigned char PointIdx = 0; PointIdx <= 5; PointIdx++) {
                currentUMU->SetGainSegmentPoint(ChData.Side,
                                                ChData.ResLine,
                                                ChData.Stroke,
                                                static_cast<unsigned char>(GetTVG() * XScale[PointIdx] / 1000),
                                                //  static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep * YScale[PointIdx] / 1000),
                                                cfg->dBGain_to_UMUGain(GetGain(1) * YScale[PointIdx] / 1000),
                                                ChanDesc.cdAScanScale,
                                                cIUMU::gptCentr);
            }


            if (GetTVG() < GetEdGate(2)) {
                // Второй отрезок - от ВРЧ [мкс] до конца строба основного канала. Значение АТТ основного канала
                currentUMU->SetGainSegmentPoint(ChData.Side,
                                                ChData.ResLine,
                                                ChData.Stroke,
                                                static_cast<unsigned char>(GetStGate(2)),
                                                // static_cast<unsigned char>(cfg->GainBase + GetGain(1) / cfg->GainStep),
                                                cfg->dBGain_to_UMUGain(GetGain(1)),
                                                ChanDesc.cdAScanScale,
                                                cIUMU::gptCentr);

                // Третий отрезок - от конца строба основного канала до конца развертки. Значение АТТ дополнительного канала
                currentUMU->SetGainSegmentPoint(ChData.Side,
                                                ChData.ResLine,
                                                ChData.Stroke,
                                                static_cast<unsigned char>(GetStGate(2)),
                                                // static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                cfg->dBGain_to_UMUGain(GetGain(2)),
                                                ChanDesc.cdAScanScale,
                                                cIUMU::gptCentr);


                currentUMU->SetGainSegmentPoint(ChData.Side,
                                                ChData.ResLine,
                                                ChData.Stroke,
                                                static_cast<unsigned char>(GetEdGate(2)),
                                                // static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                cfg->dBGain_to_UMUGain(GetGain(2)),
                                                ChanDesc.cdAScanScale,
                                                cIUMU::gptCentr);

                unsigned char SetValue;
                // SetValue = cfg->GainBase + GetGain(2) / cfg->GainStep;
                SetValue = cfg->dBGain_to_UMUGain(GetGain(2));

                if (FNewCurrentControlMode == cmTestBoltJoint) {
                    size_t size = ControlModeRestore[Side].size();
                    for (size_t i = 0; i < size; ++i) {
                        if ((ControlModeRestore[Side][i].Action == maSens_SetDelta) && (ControlModeRestore[Side][i].id == Channel) && (ControlModeRestore[Side][i].GateIdx == 2)) {
                            SetValue = cfg->dBGain_to_UMUGain(ControlModeRestore[Side][i].GainValue);
                            break;
                        }
                    }
                }


                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, static_cast<unsigned char>(GetEdGate(2)), static_cast<unsigned char>(SetValue), ChanDesc.cdAScanScale, cIUMU::gptCentr);
                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, ChanDesc.cdAScanDuration, static_cast<unsigned char>(SetValue), ChanDesc.cdAScanScale, cIUMU::gptEnd);


                /*

                     if (FNewCurrentControlMode != cmTestBoltJoint)

                         currentUMU->SetGainSegmentPoint(ChData.Side,
                                                                  static_cast<unsigned char>(ChData.ResLine),
                                                                  static_cast<unsigned char>(ChData.Stroke),
                                                                  static_cast<unsigned char>(ChanDesc.cdAScanDuration),
                                                                  static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                                  ChanDesc.cdAScanScale,
                                                                  cIUMU::gptEnd);
                     else {
                             // Режим cmTestBoltJoint - проверка болтового стыка

                             unsigned char SetValue = cfg->GainBase + GetGain(2) / cfg->GainStep;

                             for (int i = 0; i < ControlModeRestore[ChData.Side].size(); ++i)

                                 if  ((ControlModeRestore[ChData.Side][i].Action == maSens_SetDelta)  &&
                                      (ControlModeRestore[ChData.Side][i].id == Channel) &&
                                      (ControlModeRestore[ChData.Side][i].GateIdx == 2)) {

                                      SetValue = 10; // cfg->GainBase + ControlModeRestore[ChData.Side][i].GainValue / cfg->GainStep;
                                      break;
                                 }

                             currentUMU->SetGainSegmentPoint(ChData.Side,
                                                                      static_cast<unsigned char>(ChData.ResLine),
                                                                      static_cast<unsigned char>(ChData.Stroke),
                                                                      static_cast<unsigned char>(GetEdGate(2)),
                                                                      static_cast<unsigned char>(cfg->GainBase + GetGain(2) / cfg->GainStep),
                                                                      ChanDesc.cdAScanScale,
                                                                      cIUMU::gptCentr);

                             currentUMU->SetGainSegmentPoint(ChData.Side,
                                                                      static_cast<unsigned char>(ChData.ResLine),
                                                                      static_cast<unsigned char>(ChData.Stroke),
                                                                      static_cast<unsigned char>(GetEdGate(2)),
                                                                      SetValue,
                                                                      ChanDesc.cdAScanScale,
                                                                      cIUMU::gptCentr);

                             currentUMU->SetGainSegmentPoint(ChData.Side,
                                                                      static_cast<unsigned char>(ChData.ResLine),
                                                                      static_cast<unsigned char>(ChData.Stroke),
                                                                      static_cast<unsigned char>(ChanDesc.cdAScanDuration),
                                                                      SetValue,
                                                                      ChanDesc.cdAScanScale,
                                                                      cIUMU::gptEnd);
                          }
                          */
            }
            else {
                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, static_cast<unsigned char>(GetTVG()), static_cast<unsigned char>(cfg->getGainBase() + GetGain(1) / cfg->getGainStep()), ChanDesc.cdAScanScale, cIUMU::gptCentr);
                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, ChanDesc.cdAScanDuration, static_cast<unsigned char>(cfg->getGainBase() + GetGain(1) / cfg->getGainStep()), ChanDesc.cdAScanScale, cIUMU::gptEnd);
            }

            CheckSetBadSens(FCurSide, FCurChannel, 1);
            CheckSetBadSens(FCurSide, FCurChannel, 2);
        }

        if (ChanDesc.cdUseNotch) {
            if (GetStNotch() != 0) {
                unsigned char Val = static_cast<unsigned char>(cfg->getGainBase() + (GetGain(1) - GetNotchLevel()) / cfg->getGainStep());
                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, static_cast<unsigned char>(GetStNotch()), Val, ChanDesc.cdAScanScale, cIUMU::gptStart);
                currentUMU->SetGainSegmentPoint(ChData.Side, ChData.ResLine, ChData.Stroke, static_cast<unsigned char>(GetStNotch() + GetNotchLen()), Val, ChanDesc.cdAScanScale, cIUMU::gptEnd);
            }
        }
    }


    if (PrismDelay_Flag) {  // Установка задержки в призме
        currentUMU->SetPrismDelay(ChData.Side, ChData.GenLine, ChData.Stroke, static_cast<unsigned short>(GetPrismDelay()));
    }
    if (BScanGate_Flag)  // Установка строба для В-развертки
    {
        currentUMU->SetGate(ChData.Side,
                            ChData.ResLine,
                            ChData.Stroke,
                            0,
                            static_cast<unsigned char>(ChanDesc.cdBScanGate.gStart),
                            static_cast<unsigned char>(ChanDesc.cdBScanGate.gEnd),
                            static_cast<unsigned char>(cfg->getBScanGateLevel()),
                            smOneEcho,      // для строба для В-развертки - не играет роли
                            aaByPresence);  // для строба для В-развертки - не играет роли
    }
    // Установка строба
    if (Gate_Flag)  // Строб - режим Метка
    {
        if (MarkFlag && (Side == FCurSide) && (Channel == FCurChannel)) {  // Ставим метку для текущего канала
            currentUMU->SetGate(ChData.Side,
                                ChData.ResLine,
                                ChData.Stroke,
                                3,
                                static_cast<unsigned char>(GetStMark() /* - MarkWidth / 2.0*/),
                                static_cast<unsigned char>(GetEdMark() /* + MarkWidth / 2.0*/),
                                static_cast<unsigned char>(cfg->getEvaluationGateLevel()),
                                static_cast<eStrobeMode>(amOneEcho),
                                ChanDesc.cdAlarm[FCurrentGateIdx - 1]);
        }
        else {  // Обычный строб оценки
            for (int GateIdx = 1; GateIdx <= ChanDesc.cdGateCount; ++GateIdx) {
                // Установка зоны отсечения второго ДС
                if (ChanDesc.cdGateCount == 2) {
                    if (GateIdx == 2) {
                        currentUMU->SetBSFilterStDelay(ChData.Side, static_cast<unsigned char>(ChData.ResLine), static_cast<unsigned char>(ChData.Stroke), static_cast<unsigned char>(GetStGate(GateIdx)));
                    }
                }
                else {
                    currentUMU->SetBSFilterStDelay(ChData.Side, static_cast<unsigned char>(ChData.ResLine), static_cast<unsigned char>(ChData.Stroke), 255);
                }
                currentUMU->SetGate(ChData.Side,
                                    ChData.ResLine,
                                    ChData.Stroke,
                                    static_cast<unsigned char>(GateIdx),
                                    static_cast<unsigned char>(GetStGate(GateIdx)),
                                    static_cast<unsigned char>(GetEdGate(GateIdx)),
                                    static_cast<unsigned char>(cfg->getEvaluationGateLevel()),
                                    /*amTwoEcho - для проверки режима два эхо */
                                    static_cast<eStrobeMode>(static_cast<eGateAlarmMode>(std::min(1, static_cast<int>(StrobeModeList_[static_cast<unsigned int>(Channel)].Modes[Side][GateIdx].Mode)))),
                                    ChanDesc.cdAlarm[FCurrentGateIdx - 1]);
                StrobeModeList_[static_cast<unsigned int>(Channel)].Modes[Side][GateIdx].Alg = ChanDesc.cdAlarm[GateIdx - 1];
            }
        }
    }

    currentUMU->CalcTVGCurve();
    AScanMax.Delay = -1;
    AScanMax.Ampl = -1;

    FCurSide = SaveCurSide;
    FCurChannel = SaveCurChannel;
    FCurrentGateIdx = SaveCurGateIdx;

    //	AScanChange = AScanChange || Gain_Flag || PrismDelay_Flag || Gate_Flag;
    //    UMUList[ChData.UMUIndex]->delayedUnloadStop();
}

float cDevice::DelayToDepthByHeight(float Delay)  // Глубина по высоте рельса
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    if (ChanDesc.cdEnterAngle != 0) {  // Глубина залегания отражателя [H, мм]
        return ((((3.26f * Delay) * static_cast<float>(std::cos(static_cast<float>(ChanDesc.cdEnterAngle) * DEVICE_PI / 180.0f)))) / 2.0f);
    }

    return (5.9f * Delay / 2.0f);
}

float cDevice::getDelayToDepthFactor() const
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    if (ChanDesc.cdEnterAngle != 0) {
        return (3.26f / 2.0f);
    }

    return (5.9f / 2.0f);
}

float cDevice::getPixelToDepthFactor() const
{
    return getPixelToDelayFactor() * getDelayToDepthFactor();
}

float cDevice::DelayToDepth(float Delay) const
{
    return Delay * getDelayToDepthFactor();
}

float cDevice::DepthToDelay(float Depth) const
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return Depth / getDelayToDepthFactor();
}

int cDevice::DelayToPixel(float Delay)
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return static_cast<int>((10.0f * Delay) / static_cast<float>(ChanDesc.cdAScanScale));
}

float cDevice::getPixelToDelayFactor() const
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return (static_cast<float>(ChanDesc.cdAScanScale / 10.0f));
}

float cDevice::getDelayToPixelFactor() const
{
    sChannelDescription ChanDesc;
    tbl->ItemByCID(FCurChannel, &ChanDesc);
    return (10.0f / static_cast<float>(ChanDesc.cdAScanScale));
}

int cDevice::PixelToDelay(int Pixel)
{
    return static_cast<int>(getPixelToDelayFactor() * Pixel);
}

float cDevice::PixelToDelayFloat(int Pixel) const
{
    return getPixelToDelayFactor() * Pixel;
}

void cDevice::SetBScanGateLevel(int BScanGateLevel_)  // Установка уровеня строба В-развертки [отсчетов]
{
    cfg->SetBScanGateLevel(BScanGateLevel_);
    Update(true);
}

void cDevice::SetMode(eDeviceMode Mode)  // Установить режим работы дефектоскопа
{
    FNewMode = Mode;
}

eDeviceMode cDevice::GetMode() const  // Режим работы дефектоскопа
{
    return FCurMode;
}

void cDevice::SetCalibrationMode(eCalibrationMode Mode)  // Установить режим настройки канала
{
    FNewCalMode = Mode;
}

eCalibrationMode cDevice::GetCalibrationMode() const  // Режим настройки канала - Ку или 2Тп
{
    return FCurCalMode;
}

bool cDevice::SetChannel(CID id)  // Выбор канала
{
    DEFCORE_ASSERT(id < 255);
    FNewChannel = id;
    StopUseMark();

    if (tbl->isHandScan(id)) {  // Ручник
                                //        SetChannelGroup(0); // Убрано что бы схема прозвучивания не сбрасывалась на первую после переключения из ручника
        SetSide(dsNone);
        FNewGateIdx = 1;
        SetPathEncoderData(0, true, 0, false);
        RequestTVGCurve();
        return true;
    }

    // Сплошник
    sScanChannelDescription val;
    int Idx = cfg->getFirstSChannelbyID(id, &val);  // Поиск канала в текущей ChannelGroup
    while (Idx != -1) {
        if (val.StrokeGroupIdx == GetChannelGroup()) {
            if ((FCurSide == dsNone) && (cfg->getControlledRail() != crSingle)) {
                SetSide(dsLeft);
            }

            RequestTVGCurve();
            return true;
        }
        Idx = cfg->getNextSChannelbyID(Idx, id, &val);
    }

    if (cfg->getFirstSChannelbyID(id, &val) != -1) {  // Поиск канала в других ChannelGroup
        SetChannelGroup(val.StrokeGroupIdx);
        if ((FCurSide == dsNone) && (cfg->getControlledRail() != crSingle)) SetSide(dsLeft);

        RequestTVGCurve();
        return true;
    }
    return false;
}

bool cDevice::SetSide(eDeviceSide Side)  // Выбор стороны
{
    FNewSide = Side;
    StopUseMark();
    return true;
}

void cDevice::SetChannelGroup(int GroupIndex)  //
{
    FNewGroupIndex = GroupIndex;
}

bool cDevice::SetFaceData(tTrolleySide Side)  // Установка соответствия рабочих/нерабочих граней
{
    if (cfg->SetFaceData(Side, cal)) {
        Update(true);
        return true;
    }
    return false;
}

void cDevice::SetControlMode(eDeviceControlMode Mode, void* Params)  // Установить режим контроля
{
    if (FCurMode == dmPaused) {
        return;
    }

    FCalibrationMode = true;
    if (FCurrentControlMode != Mode) {
        eDeviceSide SaveSide = GetSide();  // Запоминаем текущую сторону
        CID SaveChannel = GetChannel();    // и канал

        eDeviceSide StartSide;
        eDeviceSide EndSide;

        if (cfg->getControlledRail() == crSingle) {  // Определаем рабочии стороны в зависимости от типа прибора
            StartSide = dsNone;
            EndSide = dsNone;
        }
        else {
            StartSide = dsLeft;
            EndSide = dsRight;
        };

        if (Mode == cmNormal) {  // Возвращаем все как было !!!
            FNewCurrentControlMode = Mode;
            for (int SideIdx = StartSide; SideIdx <= EndSide; ++SideIdx) {
                eDeviceSide Side = static_cast<eDeviceSide>(SideIdx);
                while (!ControlModeRestore[Side].empty()) {
                    sModeChannelData tmp = ControlModeRestore[Side].back();
                    ControlModeRestore[Side].pop_back();
                    switch (tmp.Action) {
                    case maStartGate_Set: {  // Установка значения
                        SetSide(Side);
                        SetChannel(tmp.id);
                        Update(false);
                        SetStGate(tmp.GateIdx, tmp.Value, true);
                        Update(false);
                        break;
                    }
                    case maEndGate_Set: {  // Установка значения
                        SetSide(Side);
                        SetChannel(tmp.id);
                        Update(false);
                        SetEdGate(tmp.GateIdx, tmp.Value, true);
                        Update(false);
                        break;
                    }
                    case maAlarm: {  // Установка значения
                        SetAlarmMode(Side, tmp.id, tmp.GateIdx, static_cast<eGateAlarmMode>(tmp.Value));
                        break;
                    }
                    case maSens_SetDelta: {
                        cal->SetGain(Side, tmp.id, tmp.GateIdx - 1, tmp.GainValue);
                        cal->SetSens(Side, tmp.id, tmp.GateIdx - 1, tmp.Value);

                        SetSide(Side);
                        SetChannel(tmp.id);
                        Update(false);
                        SetSens(tmp.GateIdx, GetSens(2));  // TODO: Why 2?
                        Update(false);
                        break;
                    }
                    case maSens_Set: {
                        SetSide(Side);
                        SetChannel(tmp.id);
                        Update(false);
                        SetGain(tmp.GateIdx, tmp.Value);
                        Update(false);
                        tmp = ControlModeRestore[Side].back();
                        ControlModeRestore[Side].pop_back();
                        SetSens(tmp.GateIdx, tmp.Value);
                        Update(false);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            FCurrentControlMode = Mode;
            SetSide(SaveSide);
            SetChannel(SaveChannel);
            Update(false);
            StopUseMark();
            FCalibrationMode = false;
            return;
        }

        FNewCurrentControlMode = Mode;
        for (int i = 0; i < cfg->GetControlModeCount(); ++i) {
            sDeviceModeData Item = cfg->GetControlModeItem(i);
            if ((Item.id == Mode) && (Item.StrokeGroupIdx == FCurGroupIndex)) {
                sModeChannelData tmp;
                for (int SideIdx = StartSide; SideIdx <= EndSide; ++SideIdx) {
                    eDeviceSide Side = static_cast<eDeviceSide>(SideIdx);
                    for (std::vector<sModeChannelData>::const_iterator it = Item.List.begin(), total = Item.List.end(); it != total; ++it) {
                        const sModeChannelData& currentItem = it.operator*();
                        switch (currentItem.Action) {
                        case maStartGate_Set: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Value = cal->GetStGate(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            SetStGate(currentItem.GateIdx, currentItem.Value, true);
                            Update(false);
                            break;
                        }
                        case maStartGate_Push: {
                            ControlModeStack[Side].push_back(cal->GetStGate(Side, currentItem.id, currentItem.GateIdx - 1));
                            break;
                        }
                        case maStartGate_Pop: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Action = maStartGate_Set;
                            tmp.Value = cal->GetStGate(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            SetStGate(currentItem.GateIdx, ControlModeStack[Side].back(), true);
                            ControlModeStack[Side].pop_back();
                            Update(false);
                            break;
                        }
                        case maSens_Push: {
                            ControlModeStack[Side].push_back(cal->GetSens(Side, currentItem.id, currentItem.GateIdx - 1));
                            ControlModeStack[Side].push_back(cal->GetGain(Side, currentItem.id, currentItem.GateIdx - 1));
                            break;
                        }
                        case maSens_Pop: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Action = maSens_Set;
                            tmp.Value = cal->GetSens(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            tmp.Value = cal->GetGain(Side, currentItem.id, currentItem.GateIdx - 1);
                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            cal->SetGain(Side, currentItem.id, currentItem.GateIdx - 1, ControlModeStack[Side].back());
                            ControlModeStack[Side].pop_back();
                            cal->SetSens(Side, currentItem.id, currentItem.GateIdx - 1, ControlModeStack[Side].back());
                            ControlModeStack[Side].pop_back();
                            Update(false);
                            break;
                        }
                        case maEndGate_Set: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Value = cal->GetEdGate(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            SetEdGate(currentItem.GateIdx, currentItem.Value, true);
                            Update(false);
                            break;
                        }
                        case maEndGate_Push: {
                            ControlModeStack[Side].push_back(cal->GetEdGate(Side, currentItem.id, currentItem.GateIdx - 1));
                            break;
                        }
                        case maEndGate_Pop: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Action = maEndGate_Set;
                            tmp.Value = cal->GetEdGate(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            SetEdGate(currentItem.GateIdx, ControlModeStack[Side].back(), true);
                            Update(false);
                            ControlModeStack[Side].pop_back();
                            break;
                        }
                        case maAlarm: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Value = GetAlarmMode(Side, currentItem.id, currentItem.GateIdx);
                            tmp.GainValue = 0;
                            ControlModeRestore[Side].push_back(tmp);
                            SetAlarmMode(Side, currentItem.id, currentItem.GateIdx, static_cast<eGateAlarmMode>(currentItem.Value));
                            break;
                        }

                        case maSens_SetDelta: {
                            tmp = currentItem;  // сохранение текущего значения
                            tmp.Value = cal->GetSens(Side, currentItem.id, currentItem.GateIdx - 1);
                            tmp.GainValue = cal->GetGain(Side, currentItem.id, currentItem.GateIdx - 1);

                            ControlModeRestore[Side].push_back(tmp);
                            SetSide(Side);  // Установка нового значения
                            SetChannel(currentItem.id);
                            Update(false);
                            if (Params) {
                                cal->SetSens(Side, currentItem.id, 1, GetSens(1));
                                cal->SetGain(Side, currentItem.id, 1, GetGain(1));
                                SetSens(currentItem.GateIdx, GetSens(1) + *(reinterpret_cast<int*>(Params)));
                                SetChannelParams(Side, currentItem.id, true, false, true, false);
                            }
                            Update(false);
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
                StopUseMark();
                break;
            }
        }
        SetSide(SaveSide);
        SetChannel(SaveChannel);
        Update(false);
        FCurrentControlMode = Mode;
    }
    FCalibrationMode = false;
    RequestTVGCurve();
}

eDeviceControlMode cDevice::GetControlMode() const  // Получить режим контроля
{
    return FCurrentControlMode;
}

int cDevice::GetChannelGroup() const  //
{
    return FCurGroupIndex;
}

CID cDevice::GetChannel() const
{
    return FCurChannel;  // Текущий канал
}

eChannelType cDevice::GetChannelType() const
{
    if (tbl->isHandScan(FCurChannel)) {
        return ctHandScan;
    }
    return ctCompInsp;
}

eDeviceSide cDevice::GetSide() const
{
    return FCurSide;  // Текущая сторона
}

void cDevice::updateChannelsInCurrentGroup()
{
    const std::vector<CID>& currentCIDs = cfg->getCIDsForCurrentGroup();

    for (std::vector<CID>::const_iterator it = currentCIDs.begin(), total = currentCIDs.end(); it != total; ++it) {
        switch (cfg->getControlledRail())  // Установка параметров канала
        {
        case crSingle:  // Одна нить
        {
            SetChannelParams(dsNone, it.operator*(), true, true, true, true);
            break;
        }
        case crBoth:  // Две нити
        {
            SetChannelParams(dsLeft, it.operator*(), true, true, true, true);
            SetChannelParams(dsRight, it.operator*(), true, true, true, true);
            break;
        }
        }
    }
}

void cDevice::Update(bool ResetStrokes, bool EnableAScanFlag)
{
    // FDV - Проверка соответсвия группы каналов установленной в cfg и в device - если они разняться то устанавливаем в device из cfg
    if (cfg->GetCurrentChannelGroup() != FCurGroupIndex) {
        FNewGroupIndex = cfg->GetCurrentChannelGroup();
    }

    SetChannelParamsFlag = false;
    SetStrokeTableFlag = ResetStrokes || ((FNewMode == dmCalibration) && (FNewMode != FCurMode));
    OnlyOneStrokeFlag = false;
    SetAScanFlag = ResetStrokes || SetStrokeTableFlag;

    ChangeMode = (FNewMode != FCurMode);
    ChangeChannel = (FNewChannel != FCurChannel) || (FCurrentGateIdx != FNewGateIdx);
    ChangeSide = (FNewSide != FCurSide);
    ChangeGroupIndex = (FNewGroupIndex != FCurGroupIndex);
    ChangeCalMode = (FCurCalMode != FNewCalMode);

    // Если до этого работал имитатор ДП а теперь он не нужет то выключаем его
    /*    if (ManagePathEncoderSimulation) {
            if ((tbl->isHandScan(FCurChannel)) && (!tbl->isHandScan(FNewChannel))) {
                PathEncoderSim(cfg->getMainPathEncoderIndex(), false);
                SetPathEncoderData(static_cast<char>(cfg->getMainPathEncoderIndex()), false, 0, true);
            }
        }
    */
    if (!SkipSetStrokeTableByChannelSelect) {
        if (FNewMode == dmCalibration) {
            SetStrokeTableFlag = SetStrokeTableFlag || ChangeChannel || ChangeSide || ChangeGroupIndex;
        }
    }

    // Выбор группы каналов
    if (ChangeGroupIndex) {
        changeGroupIndex();
    }

    bool ChangeChannelType = (isFirstCall) ? true : (tbl->isHandScan(FNewChannel) != tbl->isHandScan(FCurChannel));
    isFirstCall = false;

    bool isChangeChannelCalibrationMode = ((ChangeMode || ChangeCalMode) && (FNewMode == dmCalibration)) || ((ChangeMode) && (FNewMode == dmSearch));
    if (isChangeChannelCalibrationMode) {
        changeChannelCalibrationMode();
    }

    FCurMode = FNewMode;
    FCurChannel = FNewChannel;
    FCurSide = FNewSide;
    FCurCalMode = FNewCalMode;
    FCurGroupIndex = FNewGroupIndex;
    FCurrentGateIdx = FNewGateIdx;

    if (ChangeSide) {
        SetAScanFlag = true;
    }


    // Изменяется режим работы дефектоскопа
    if (ChangeMode) {
        changeDeviceMode();
    }
    OnlyOneStrokeFlag = (FCurMode == dmCalibration);  // Настройка каналов контроля

    // Изменяется тип канала контроля
    if (ChangeChannelType) {
        changeChannelType();
    }

    // Изменяется канал контроля
    if (ChangeChannel) {
        changeChannel();
    }
    // Устанавка таблицы тактов
    //    if (FNewMode != dmPaused) // Если не режим Пауза // TODO:

    cs->Enter();
    if (SetStrokeTableFlag) {
        UMUBScan2Data_ptrs[0].clear();
        UMUBScan2Data_ptrs[1].clear();
    }
    cs->Release();

    if (SetStrokeTableFlag && !OnlyOneStrokeFlag) {  // Режим Поиск
        cfg->ResetUsedList();
        if (CurChannelType == ctCompInsp) {  // Каналы сплошного контроля
            setDeviceForSearchModeContiniousControl();
        }
        else {  // Каналы ручного контроля
            setDeviceForSearchModeManualControl();
        }
    }
    else if (SetStrokeTableFlag && OnlyOneStrokeFlag) {  // Режим Настройка
        setDeviceForCalibrationMode();
    }

#ifdef ChannelsParams2
    if (ChangeCalMode && !SetChannelParamsFlag) {
        SetChannelParams(FCurSide, FCurChannel, false, false, true, false);
    }
#endif

#ifdef EnableAScan_
    // Запускаем А-развертку (и В-развертку тоже) для выбранного канала
    if (SetAScanFlag || EnableAScanFlag) {
        setDeviceForEvaluationOrHandMode();
    }
#endif
    /*
        if (ManagePathEncoderSimulation) {
            if ((tbl->isHandScan(FNewChannel))) {

                SetPathEncoderData(cfg->getMainPathEncoderIndex(), true, 0, true); // Выбор ДП
                SetPathEncoderData(cfg->getMainPathEncoderIndex(), true, 0, false); // Установка нуля
                PathEncoderSim(cfg->getMainPathEncoderIndex(), true); // Вкл имитатора
            }
            else {
                PathEncoderSim(cfg->getMainPathEncoderIndex(), false); // Выкл имитатора
                SetPathEncoderData(static_cast<char>(cfg->getMainPathEncoderIndex()), false, 0, true); // Выбор ДП
                SetPathEncoderData(cfg->getMainPathEncoderIndex(), UsePathEncoderSimulationinSearchMode, 0, true); // Установка нуля
    //            PathEncoderSim(cfg->getMainPathEncoderIndex(), UsePathEncoderSimulationinSearchMode);
            }
        }
        */
}

unsigned int cDevice::GetUsedUMUIdx() const
{
    return UsedUMUIdx;  // Номер активного БУМа
}

void cDevice::DisableAll()  // Выключить БУМы
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->DisableAlarm();
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->DisableACSumm();
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->DisableAScan();
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->DisableBScan();
    }
}

bool cDevice::TestDisableAll()  // Проверка были ли выключены БУМы
{
    bool StopFlag = true;  // Ожидание окончания поступления данных прошлой сесии

    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        StopFlag = StopFlag && (UMUList.at(UMUIdx)->_common_state._bscan_session_id.size() == 0);
    }

    return StopFlag && (UMU_Event->EventDataSize() == 0) && (DEV_Event->EventDataSize() == 0);
}

void cDevice::EnableAll()  // Включить БУМы
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        CurrentBScanSessionID = UMUList.at(UMUIdx)->EnableBScan();
        BScanSessionID[static_cast<int>(CurChannelType)] = CurrentBScanSessionID;  // Включаем В-развертку и запоминаем номер сесии
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        if (AcousticContactState == 2) {
            UMUList.at(UMUIdx)->EnableAC();
        }
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->EnableAScan(UMUList.at(UMUIdx)->_common_state._save_ascan_side,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_line,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_stroke,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_st_delay,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_zoom,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_mode,
                                        UMUList.at(UMUIdx)->_common_state._save_ascan_strob);
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->EnableAlarm();
    }
}

void cDevice::ResetPathEncoder()  // Сбросить ДП на 0
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->SetPathEncoderData(0, false, 0, true);
    }
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->SetPathEncoderData(0, false, 0, false);
    }
    _pathEncoderResetComandSent = true;
}

bool cDevice::TestResetPathEncoder()  // Проверка выполнения сброса ДП на 0
{
    bool res = false;
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        res = res || UMUList.at(UMUIdx)->_common_state._expectation_setting_coord;
    }
    return !res;
}

bool cDevice::SetAcousticContactThreshold_(unsigned char PercentageIncrease_, unsigned char ThRecalcCount)  // Команда - Выполнить установку порога АК
{
    if (CurChannelType == ctCompInsp)  // Только для каналов сплошного контроля
    {
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            UMUList.at(UMUIdx)->CalcACThreshold_(PercentageIncrease_, ThRecalcCount);
        }
        return true;
    }

    return false;
}

void cDevice::SetAcousticContact(int Mode, bool State)  // Вкл / выкл контроля акустического контакта
{
    if (Mode == 1) {
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            if (State) {
                UMUList.at(UMUIdx)->EnableACSumm();
            }
            else {
                UMUList.at(UMUIdx)->DisableACSumm();
            }
        }
    }
    if (Mode == 2) {
        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            if (State) {
                UMUList.at(UMUIdx)->EnableAC();  // else UMUList.at(UMUIdx)->DisableAC();
            }
        }
    }
    if (State) {
        AcousticContactState = Mode;
    }
    else {
        AcousticContactState = 0;
    }
}

void cDevice::InitTuningGateList()  // Инициализация стробов для настройки каналов
{
    for (tSensTuningParamsList::const_iterator it = cfg->SensTuningParams.begin(), total = cfg->SensTuningParams.end(); it != total; ++it) {
        const sSensTuningParam& currentItem = it.operator*();

        cal->SetGateMode(gmSensCalibration);
        cal->SetStGate(dsNone, currentItem.id, 0, currentItem.SensTuningGate[0].gStart);
        cal->SetEdGate(dsNone, currentItem.id, 0, currentItem.SensTuningGate[0].gEnd);
        cal->SetStGate(dsNone, currentItem.id, 1, currentItem.SensTuningGate[1].gStart);
        cal->SetEdGate(dsNone, currentItem.id, 1, currentItem.SensTuningGate[1].gEnd);
        cal->SetGateMode(gmPrismDelayCalibration);
        cal->SetStGate(dsNone, currentItem.id, 0, currentItem.PrismTuningGate[0].gStart);
        cal->SetEdGate(dsNone, currentItem.id, 0, currentItem.PrismTuningGate[0].gEnd);
        cal->SetStGate(dsNone, currentItem.id, 1, currentItem.PrismTuningGate[1].gStart);
        cal->SetEdGate(dsNone, currentItem.id, 1, currentItem.PrismTuningGate[1].gEnd);

        cal->SetGateMode(gmSensCalibration);
        cal->SetStGate(dsLeft, currentItem.id, 0, currentItem.SensTuningGate[0].gStart);
        cal->SetEdGate(dsLeft, currentItem.id, 0, currentItem.SensTuningGate[0].gEnd);
        cal->SetStGate(dsLeft, currentItem.id, 1, currentItem.SensTuningGate[1].gStart);
        cal->SetEdGate(dsLeft, currentItem.id, 1, currentItem.SensTuningGate[1].gEnd);
        cal->SetStGate(dsRight, currentItem.id, 0, currentItem.SensTuningGate[0].gStart);
        cal->SetEdGate(dsRight, currentItem.id, 0, currentItem.SensTuningGate[0].gEnd);
        cal->SetStGate(dsRight, currentItem.id, 1, currentItem.SensTuningGate[1].gStart);
        cal->SetEdGate(dsRight, currentItem.id, 1, currentItem.SensTuningGate[1].gEnd);
        cal->SetGateMode(gmPrismDelayCalibration);
        cal->SetStGate(dsLeft, currentItem.id, 0, currentItem.PrismTuningGate[0].gStart);
        cal->SetEdGate(dsLeft, currentItem.id, 0, currentItem.PrismTuningGate[0].gEnd);
        cal->SetStGate(dsLeft, currentItem.id, 1, currentItem.PrismTuningGate[1].gStart);
        cal->SetEdGate(dsLeft, currentItem.id, 1, currentItem.PrismTuningGate[1].gEnd);
        cal->SetStGate(dsRight, currentItem.id, 0, currentItem.PrismTuningGate[0].gStart);
        cal->SetEdGate(dsRight, currentItem.id, 0, currentItem.PrismTuningGate[0].gEnd);
        cal->SetStGate(dsRight, currentItem.id, 1, currentItem.PrismTuningGate[1].gStart);
        cal->SetEdGate(dsRight, currentItem.id, 1, currentItem.PrismTuningGate[1].gEnd);
    }
    cal->SetGateMode(gmSearch);
}

tDEV_AScanMax cDevice::GetAScanMax()  // Получение максимума сигнала A-развертки (маркер для режима настройки)
{
    return AScanMax;
}

int cDevice::GetBScanEchoCount(eChannelType ChType, PtUMU_BScanData DataPtr, eDeviceSide Side, CID id)
{
    DEFCORE_ASSERT(DataPtr != NULL);
    DEFCORE_ASSERT(id < 255);
    int UMUIdx;
    eUMUSide UMUSide;
    int UMULine;
    int UMUStroke;

    cfg->GetSLSByChannel(ChType, Side, id, &UMUIdx, &UMUSide, &UMULine, &UMUStroke);
    DEFCORE_ASSERT(static_cast<unsigned int>(UMUSide) < 2);
    DEFCORE_ASSERT(UMULine < 2);
    DEFCORE_ASSERT(UMUStroke < MaxStrokeCount);
    if (DataPtr->UMUIdx == UMUIdx)
        return DataPtr->Signals.Count[UMUSide][UMULine][UMUStroke];
    else
        return 0;
}

PtUMU_BScanSignal cDevice::GetBScanEchoData(eChannelType ChType, PtUMU_BScanData DataPtr, eDeviceSide Side, CID id, int Index)
{
    DEFCORE_ASSERT(DataPtr != NULL);
    DEFCORE_ASSERT(id < 255);
    int UMUIdx;
    eUMUSide UMUSide;
    int UMULine;
    int UMUStroke;

    cfg->GetSLSByChannel(ChType, Side, id, &UMUIdx, &UMUSide, &UMULine, &UMUStroke);
    DEFCORE_ASSERT(static_cast<unsigned int>(UMUSide) < 2);
    DEFCORE_ASSERT(UMULine < 2);
    DEFCORE_ASSERT(UMUStroke < MaxStrokeCount);
    DEFCORE_ASSERT(Index < MaxSignalAtBlock);
    if (DataPtr->UMUIdx == UMUIdx) {
        return &(DataPtr->Signals.Data[UMUSide][UMULine][UMUStroke][Index]);
    }

    return NULL;
}

bool cDevice::GetAlarmData(eChannelType ChType, PtUMU_AlarmItem DataPtr, eDeviceSide Side, CID id, int GateIdx, bool* State)
{
    int UMUIdx;
    eUMUSide UMUSide;
    int UMULine;
    int UMUStroke;

    cfg->GetSLSByChannel(ChType, Side, id, &UMUIdx, &UMUSide, &UMULine, &UMUStroke);
    DEFCORE_ASSERT(static_cast<unsigned int>(UMUSide) < 2);
    DEFCORE_ASSERT(UMULine < 2);
    DEFCORE_ASSERT(UMUStroke < MaxStrokeCount);
    DEFCORE_ASSERT(GateIdx < 4);
    if (DataPtr->UMUIdx == UMUIdx) {
        *State = DataPtr->State[UMUSide][UMULine][UMUStroke][GateIdx];
        return true;
    }

    return false;
}

void cDevice::PathEncoderSim(bool State)
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->PathSimulator(0, State);
    }
}

void cDevice::PathEncoderSim(int Number, bool State)
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->PathSimulator(Number, State);
    }
}

void cDevice::PathEncoderSimEx(unsigned char Number, unsigned short Interval_ms, bool state)  // БУИ - Имитатор датчика пути (расширенный вариант)
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->PathSimulatorEx(Number, Interval_ms, state);
    }
}

void cDevice::PathEncoderSimScaner(bool state, unsigned short Interval_ms, unsigned char Param_A, unsigned char Param_B)  // БУИ - Имитатор датчика пути сканера
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->ScanerPathSimulator(state, Interval_ms, Param_A, Param_B);
    }
}

void cDevice::SetPathEncoderData(char path_encoder_index, bool Simulator, int new_value, bool UseAsMain)  // Установка значения датчика пути. Если UseAsMain установлен, полная координата для датчика (имитатора) не изменяется
{
    if ((SetPathEncoderData_last_path_encoder_index != path_encoder_index) ||
        //        (SetPathEncoderData_last_Simulator != Simulator) ||
        (SetPathEncoderData_last_new_value != new_value) || (SetPathEncoderData_last_UseAsMain != UseAsMain)) {
        SetPathEncoderData_last_path_encoder_index = path_encoder_index;
        //        SetPathEncoderData_last_Simulator = Simulator;
        SetPathEncoderData_last_new_value = new_value;
        SetPathEncoderData_last_UseAsMain = UseAsMain;

        for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
            UMUList.at(UMUIdx)->SetPathEncoderData(path_encoder_index, Simulator, new_value, UseAsMain);
        }
    }
}

void cDevice::SetLeftSideSwitching(UMULineSwitching state)  // Переключение линий с сплошного контроля на сканер
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->SetLeftSideSwitching(state);
    }
}

unsigned int cDevice::GetUMUCount() const
{
    return cfg->GetUMUCount();
}

bool cDevice::GetUMUOnLineStatus(unsigned int Idx) const
{
    return UMUOnLineStatus[Idx];
}

void cDevice::GetUAkkQuery()
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->GetUAkkQuery();  // GetUAkkValue()
    }
}

int cDevice::GetUMUSkipMessageCount(unsigned int Idx)
{
    int res = static_cast<UMU_LAN*>(UMUList[Idx])->getSkipMessageCount();
    return res;
}

int cDevice::GetUMUErrorMessageCount(unsigned int Idx)
{
    return UMUList[Idx]->getErrorMessageCount();
}

unsigned int cDevice::GetUMUDownloadSpeed(unsigned int Idx)
{
    if (Idx >= UMUList.size()) {
        return 0;
    }
    return UMUList[Idx]->GetDownloadSpeed();
}

unsigned int cDevice::GetUMUUploadSpeed(unsigned int Idx)
{
    if (Idx >= UMUList.size()) {
        return 0;
    }
    return UMUList[Idx]->GetUploadSpeed();
}

void cDevice::RequestTVGCurve()
{
    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) {
        UMUList.at(UMUIdx)->RequestTVGCurve();
    }
}

bool cDevice::ChannelInCurrentGroup(CID Channel)
{
    sScanChannelDescription desc;
    size_t totalCount = cfg->GetAllScanChannelsCount();
    for (size_t i = 0; i < totalCount; ++i) {
        if (!cfg->getSChannelbyIdx(i, &desc)) {
            continue;
        }
        if (Channel != desc.Id) {
            continue;
        }
        if (FCurGroupIndex == desc.StrokeGroupIdx) {
            return true;
        }
    }
    return false;
}

void cDevice::ResetPathEncoderGis()
{
    memset(&(PathEncoderGis[0]), 0, sizeof(PathEncoderGis));
}

void cDevice::SetPathEncoderGisState(bool State)
{
    PathEncoderGisState = State;
}

void cDevice::SetCallBack(CallBackProcPtr function)  // Устанавливает callback функцию
{
    CallBackFunction = function;
}
void cDevice::SetAlarmMode(eDeviceSide Side, CID Channel, int GateIdx, eGateAlarmMode Mode)  // Установить режим работы АСД
{
    StrobeModeList_[static_cast<unsigned int>(Channel)].Modes[Side][GateIdx].Mode = Mode;
    CallBackFunction_(FCurGroupIndex, Side, Channel, GateIdx, vidAlarm, static_cast<int>(Mode));
    SetChannelParams(Side, Channel, false, false, true, false);
}

eGateAlarmMode cDevice::GetAlarmMode(eDeviceSide Side, CID Channel, int GateIdx)  // Получить режим работы АСД
{
    return StrobeModeList_[static_cast<unsigned int>(Channel)].Modes[Side][GateIdx].Mode;
}

eBScanDataType cDevice::GetBScanDataType() const
{
    if (!UMUList.empty()) {
        return UMUList[0]->getBScanDataType();
    }

    return bstUnknown;
}

pCalibrationToRailTypeResult cDevice::CalibrationToRailType()
{
    int SaveGates[3][CHANNELS_TOTAL][2];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < CHANNELS_TOTAL; ++j) {
            SaveGates[i][j][0] = 0;
            SaveGates[i][j][1] = 0;
        }
    }

    eDeviceSide StartSide;
    eDeviceSide EndSide;

    eDeviceSide SaveSide = GetSide();
    CID SaveChannel = GetChannel();

    // В режиме «Настройка» данная операция производится только при нахождении в
    // канале «0ЗТМ» или «0ЭХО» и выполняют только для каналов текущей нити (см. ниже)
    if ((CurChannelType == ctCompInsp) && (FCurMode == dmCalibration) && !((SaveChannel == N0EMS) || (SaveChannel == N0EMS2) || (SaveChannel == N0EMS_WP))) return NULL;
    if ((CurChannelType == ctHandScan) && (FCurMode == dmCalibration) && !((SaveChannel == H0MS) || (SaveChannel == H0E) || (SaveChannel == H0E_5MGz) || (SaveChannel == H0MS_5MGz))) return NULL;

    // [*] Для канала "0 ЗТМ" строб АСД и строб настроечный - это всегда одно и тоже.
    // Т.е. при нажатии кнопки "Тип рельса" в режиме "Настройка" должен меняться
    // настроечный троб канала  ЗТМ и все стробы АСД данной нити, связанные с кнопкой
    // "Тип рельса", включая и строб АСД канала 0ЗТМ.
    // Исходя из вышеописанного для изменения стробов АСД временно включаем режим стробов - "Поиск"
    if ((CurChannelType == ctCompInsp) && (FCurMode == dmCalibration)) {
        cal->SetGateMode(gmSearch);
    }

    FCalibrationMode = true;

    // Не забыть настройку на тип рельса для режима "настройка"
    sRailTypeTuningGroup tmp1;
    sRailTypeTuningChannelAction tmp2;
    sCalibrationToRailTypeResItem ResultItem;

    if (!RailTypeTrackingMode) {
        // Сдвиг строbа ЗТМ канала для отключения фильтрации ДС
        if (cfg->getFilteringBottomSignal()) {
            size_t totalCount = cfg->GetRailTypeTuningCount();
            for (size_t i = 0; i < totalCount; ++i) {  // Выбор канала (с доным сигналом) по которому настраивается тип рельса
                tmp1 = cfg->GetRailTypeTuningItem(i);

                if (tmp1.ChType != GetChannelType()) {
                    continue;  // Проверка типа канала
                }

                if (tmp1.Rails == crSingle) {
                    StartSide = dsNone;
                    EndSide = dsNone;
                }
                else {
                    // В режиме «Настройка» данная операция производится только для каналов текущей нити
                    if (FCurMode == dmCalibration) {
                        StartSide = FCurSide;
                        EndSide = FCurSide;
                    }
                    else {
                        StartSide = dsLeft;
                        EndSide = dsRight;
                    }
                };

                // Отключение фильтрации ДС путем установки строба на максимальное значение

                if (CurChannelType == ctCompInsp) {
                    for (int SideIdx = StartSide; SideIdx <= EndSide; ++SideIdx) {
                        DEFCORE_ASSERT(tmp1.MasterId < CHANNELS_TOTAL);
                        SaveGates[SideIdx][tmp1.MasterId][0] = cal->GetStGate(static_cast<eDeviceSide>(SideIdx), tmp1.MasterId, 1);  // GetStGate(2);
                        SaveGates[SideIdx][tmp1.MasterId][1] = cal->GetEdGate(static_cast<eDeviceSide>(SideIdx), tmp1.MasterId, 1);  // GetEdGate(2);
                        cal->SetStGate(static_cast<eDeviceSide>(SideIdx), tmp1.MasterId, 1, 254);
                        cal->SetEdGate(static_cast<eDeviceSide>(SideIdx), tmp1.MasterId, 1, 255);
                        SetChannelParams(static_cast<eDeviceSide>(SideIdx), tmp1.MasterId, true, false, true, false);
                    }
                }
            }
        }

        unsigned long Timer = GetTickCount_();
        while (GetTickCount_() - Timer < 300) {
            SLEEP(1);
        }
    }

    CalibrationToRailTypeResult.clear();  // Сбрасываем массив результатов настройки на тип рельса
    size_t totalCount = cfg->GetRailTypeTuningCount();
    for (size_t i = 0; i < totalCount; ++i) {  // Выбор канала (с доным сигналом) по которому настраивается тип рельса
        tmp1 = cfg->GetRailTypeTuningItem(i);
        if (tmp1.ChType != GetChannelType()) {
            continue;  // Проверка типа канала
        }
        if (tmp1.Rails == crSingle) {
            StartSide = dsNone;
            EndSide = dsNone;
        }
        else {
            // В режиме «Настройка» данная операция производится только для каналов текущей нити
            if (FCurMode == dmCalibration) {
                StartSide = FCurSide;
                EndSide = FCurSide;
            }
            else {
                StartSide = dsLeft;
                EndSide = dsRight;
            }
        };

        for (int SideIdx = StartSide; SideIdx <= EndSide; ++SideIdx) {
            // Поиск данных (высоты релльса) для канала tmp1.MasterId и соотв. нити
            for (tRailTypeTuningDataList::const_iterator it = RailTypeTuningDataList.begin(), total = RailTypeTuningDataList.end(); it != total; ++it) {
                const sRailTypeTuningData& currentDataItem = it.operator*();
                if ((currentDataItem.Side == static_cast<eDeviceSide>(SideIdx)) &&  // Для нужной нити
                    (currentDataItem.Channel == tmp1.MasterId)) {                   //                 и канала
                    if ((GetTickCount_() - currentDataItem.Time < 2000) &&          // Данные должны быть свежими - давность < 2 сек.
                        (currentDataItem.DelayHeight != -1)) {                      // Если высота рельса вычисленна - то настраиваем стробы для каналов из списка
                        for (size_t k = 0; k < tmp1.List.size(); ++k)
                            // Выбор канала к которому применяются действия по настройки на тип рельса

                            // При отслеживании ДС отключает работу с не прямыми каналами
                            // if ((!RailTypeTrackingMode) ||  (RailTypeTrackingMode && (tmp1.List[k].id == tmp1.MasterId)))

                            if (((tmp1.List[k].id == SaveChannel) && (tmp1.List[k].ActiveInTuning) && (FCurMode == dmCalibration)) || (FCurMode != dmCalibration)) {  // Если мы в режиме настройка то работаем только с одним каналом и если он используется в режиме настройка
                                tmp2 = tmp1.List[k];
                                SetSide(static_cast<eDeviceSide>(SideIdx));
                                SetChannel(tmp2.id);
                                Update(false);
                                switch (tmp2.Action) {
                                case maMoveStartGate: {
                                    if (tmp2.ValueType == rtt_mcs)
                                        SetStGate(tmp2.StrobeIndex, currentDataItem.DelayHeight + tmp2.Delta, tmp2.SkipTestGateLen);
                                    else {
                                        float mm_Height = currentDataItem.DelayHeight * 2.95f;
                                        float Delay;
                                        int Angle;
                                        tbl->GetChannelEnterAngle(&Angle, tmp2.id);
                                        if (Angle != 0) {
                                            Delay = 2.0f * (mm_Height + tmp2.Delta) / (3.26f * std::cos(Angle * DEVICE_PI / 180.0f));
                                        }
                                        else {
                                            Delay = 2.0f * (mm_Height + tmp2.Delta) / 5.9f;
                                        }
                                        SetStGate(tmp2.StrobeIndex, Delay, tmp2.SkipTestGateLen);
                                        if ((tmp2.StrobeIndex == 2) && (Delay >= GetEdGate(2))) {
                                            SetEdGate(2, Delay + 15, true);
                                        }
                                    }
                                    break;
                                };  // Изменение начала строба
                                case maMoveEndGate: {
                                    if (tmp2.ValueType == rtt_mcs) {
                                        SetEdGate(tmp2.StrobeIndex, currentDataItem.DelayHeight + tmp2.Delta, tmp2.SkipTestGateLen);
                                    }
                                    else {
                                        float mm_Height = currentDataItem.DelayHeight * 2.95f;
                                        float Delay;
                                        int Angle;
                                        tbl->GetChannelEnterAngle(&Angle, tmp2.id);
                                        if (Angle != 0) {
                                            Delay = 2.0f * (mm_Height + tmp2.Delta) / (3.26f * std::cos(Angle * DEVICE_PI / 180.0f));
                                        }
                                        else {
                                            Delay = 2.0f * (mm_Height + tmp2.Delta) / 5.9f;
                                        }
                                        SetEdGate(tmp2.StrobeIndex, Delay, tmp2.SkipTestGateLen);
                                    }
                                    break;
                                };  // Изменение конца строба
                                }
                                Update(false);
                            }
                        ResultItem.Result = true;                                    // Настройка выполненна
                        ResultItem.RailHeight = currentDataItem.DelayHeight * 2.95;  // Высота рельса в миллиметрах
                    }
                    else {
                        ResultItem.Result = false;   // Настройка невыполненна
                        ResultItem.RailHeight = -1;  // Высота рельса в миллиметрах
                    }
                    // Результат настройки на тип рельса для одного канала
                    ResultItem.Side = static_cast<eDeviceSide>(SideIdx);  // Сторона прибора
                    ResultItem.id = tmp1.MasterId;                        // Канал контроля
                    CalibrationToRailTypeResult.push_back(ResultItem);
                    break;
                }
            }
        }
    }


    // Возвращение начальных значений стробов в случае не удачной настройки
    if (cfg->getFilteringBottomSignal()) {
        if (CurChannelType == ctCompInsp) {
            for (tCalibrationToRailTypeResult::const_iterator it = CalibrationToRailTypeResult.begin(), total = CalibrationToRailTypeResult.end(); it != total; ++it) {
                const sCalibrationToRailTypeResItem& currentItem = it.operator*();
                if (!currentItem.Result) {
                    DEFCORE_ASSERT(currentItem.id < CHANNELS_TOTAL);
                    cal->SetStGate(currentItem.Side, currentItem.id, 1, SaveGates[currentItem.Side][currentItem.id][0]);
                    cal->SetEdGate(currentItem.Side, currentItem.id, 1, SaveGates[currentItem.Side][currentItem.id][1]);
                    SetChannelParams(currentItem.Side, currentItem.id, true, false, true, false);
                }
            }
        }
    }

    // Возвращаем сторону и канал которые были установленна до вызова данного метода
    SetSide(SaveSide);
    SetChannel(SaveChannel);
    Update(false);


    // Возвращаем режим стробов на исходный (см. выше [*] ) и заносим строб АСД в строб настройки Ку
    if ((CurChannelType == ctCompInsp) && (FCurMode == dmCalibration)) {
        // запоминаем строб АСД
        sGate save;
        save.gStart = GetStGate(2);
        save.gEnd = GetEdGate(2);

        // возвращаем режим
        switch (FCurCalMode) {
        case cmSens: {
            cal->SetGateMode(gmSensCalibration);
            break;
        }  // Настройка условной чувствительности
        case cmPrismDelay: {
            cal->SetGateMode(gmPrismDelayCalibration);
            break;
        }  // Настройка времени в призме
        }

        // заносим в строб настройки
        SetStGate(2, save.gStart, true);
        SetEdGate(2, save.gEnd, true);
    }

    FCalibrationMode = false;
    return &CalibrationToRailTypeResult;
}

void cDevice::SetRailTypeTrackingMode(bool State)  // Режим отслеживания (высоты) типа рельса, для зон стрелочных переводов
{
    RailTypeTrackingMode = State;

    for (unsigned int UMUIdx = 0; UMUIdx < cfg->GetUMUCount(); ++UMUIdx) UMUList.at(UMUIdx)->SetBScanFiltrationMode(State);

    if (!State) CalibrationToRailType();
}

bool cDevice::PrismDelayAutoCalibration(float* DeltaPrismDelay)  // Автоматическая настройка времени в призме
{
    bool ChangeGate = false;
    sGate SaveGate;
    SaveGate.gStart = 0;
    SaveGate.gEnd = 0;
    bool Result;

    if (FCurMode != dmCalibration) return false;  // Должен быть включен режим настройки
    FCalibrationMode = true;

    int EnterAngle;  // Угол ввода текущего канала
    tbl->GetChannelEnterAngle(&EnterAngle, LastAScanMeasure.Channel);

    sGate WorkGate;
    if (EnterAngle == 0) {
        WorkGate.gStart = 20 - 10;
        WorkGate.gEnd = 20 + 10;
    }
    else {
        WorkGate.gStart = 36 - 10;
        WorkGate.gEnd = 36 + 10;
    }

    if ((GetStGate(GetGateIndex()) != WorkGate.gStart) || (GetEdGate(GetGateIndex()) != WorkGate.gEnd)) {  // Нужно переставить строб

        SaveGate.gStart = GetStGate();
        SaveGate.gEnd = GetEdGate();
        ChangeGate = true;

        // SetStGate(DepthToDelay(49), true);
        // SetEdGate(DepthToDelay(69), true);
        SetStGate(WorkGate.gStart, true);
        SetEdGate(WorkGate.gEnd, true);

        // Ожидание данных 0.2 секунды
        unsigned long Time = GetTickCount_();
        while (GetTickCount_() - Time < 200) {
            SLEEP(5);
        }
    }

    if ((GetTickCount_() - LastAScanMeasureGetTime < 500) &&  // Данные устраивают по времени
        (LastAScanMeasure.Channel == GetChannel()) &&         // Сторона и канал правильные
        (LastAScanMeasure.Side == GetSide()) && (LastAScanMeasure.ParamA >= cfg->getEvaluationGateLevel()) && (LastAScanMeasure.ParamA < 254)) {
        float TargetDelay;  // Задержка которую мы должны получить после настройки

        if (EnterAngle != 0) {
            TargetDelay = 118.0f / 3.26f;  // 59 мм * 2 = 118
        }
        else {
            TargetDelay = 118.0f / 5.9f;  // 59 мм * 2 = 118
        }

        if (DeltaPrismDelay) {
            *DeltaPrismDelay = static_cast<float>(LastAScanMeasure.ParamM) - TargetDelay;
        }

        SetPrismDelay(static_cast<int>(static_cast<float>(GetPrismDelay()) + (static_cast<float>(LastAScanMeasure.ParamM) - TargetDelay) * 10.0f));
        Result = true;
    }
    else {
        Result = false;
    }

    if (ChangeGate) {
        SetStGate(SaveGate.gStart, true);
        SetEdGate(SaveGate.gEnd, true);
    }
    FCalibrationMode = false;
    return Result;
}

int cDevice::GetChannelBScanDelayMultiply(CID Channel) const
{
    DEFCORE_ASSERT(Channel < 255);
    int Multiply = 0;
    tbl->GetChannelBScanDelayMultiply(&Multiply, Channel);
    return Multiply;
}

void cDevice::EnableFiltration(bool isEnable)
{
    if (!cfg->getBScanDataFiltration() && isEnable) {
        return;
    }

    _enableFiltration = isEnable;

    if (_enableFiltration) {
        filterInit();
    }
}

void cDevice::setFilterParams(tBScan2FilterParamId prmId, unsigned int value)
{
    switch (prmId) {
    case fprmMinPacketSize:
        fltrPrmMinPacketSize = value;
        break;
    case fprmMaxCoordSpacing:
        fltrPrmMaxCoordSpacing = value;
        break;
    case fprmMaxDelayDelta:
        fltrPrmMaxDelayDelta = value;
        break;
    case fprmMaxSameDelayConsecutive:
        fltrPrmMaxSameDelayConsecutive = value;
        break;
    case fprmMinPacketSizeForEvent:
        fltrPrmMinPacketSizeForEvent = value;
        break;
    case frmmMinSignals0dBInNormModeForEvent:
        fltrPrmMinSignals0dBInNormModeForEvent = value;
        break;
    case frmmMinSpacing0dBInNormModeForEvent:
        fltrPrmMinSpacing0dBInNormModeForEvent = value;
        break;
    case fprmMinSpacingMinus6dBInBoltTestModeForEvent:
        fltrPrmMinSpacingMinus6dBInBoltTestModeForEvent = value;
        break;
    }
}


void cDevice::EnableSMChSensAutoCalibration(bool isEnable)  // Автонастройка Ку теневых каналов
{
    _enableSMChSensAutoCalibration = isEnable;
}

void cDevice::MakeTestSensCallBackForAllChannel()  // Формирование TestSens CallBack -ов для всех каналов контроля
{
    for (unsigned int i = 0; i < cfg->UMUtoDEVList.size(); ++i) {
        for (int GateIdx = 1; GateIdx <= tbl->GetGateCount(cfg->UMUtoDEVList[i].DEVChannel); ++GateIdx) {
            CallBackFunction_(FCurGroupIndex, cfg->UMUtoDEVList[i].DEVSide, cfg->UMUtoDEVList[i].DEVChannel, GateIdx, vidTestSens, TestSensValidRanges(cfg->UMUtoDEVList[i].DEVSide, cfg->UMUtoDEVList[i].DEVChannel, GateIdx));
        }
    }
}
//--------------------------------------------------------------------
// получение напряжения аккумулятора в Вольтах
// из UMUList[0] c протоколами  eProtUSBCAN и 	eProtUSBCANAND
bool cDevice::GetUAkkByVolt(double* pVoltage)
{
    assert(UMUList.size() != 0);
    assert(pVoltage != NULL);

    eProtocol proto;
    proto = UMUList[0]->getProtocol();
    if ((proto == eProtUSBCAN) || (proto == eProtUSBCANAND)) {
        *pVoltage = UMUList[0]->GetUAkkValue() / 10.0;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------
// получение напряжения аккумулятора в процентах от заряда
// из UMUList[0] c протоколами  eProtUSBCAN и 	eProtUSBCANAND
bool cDevice::GetUAkkByPercent(unsigned int* pVoltageByPercent)
{
    assert(UMUList.size() != 0);
    assert(pVoltageByPercent != NULL);

    eProtocol proto;
    proto = UMUList[0]->getProtocol();
    if ((proto == eProtUSBCAN) || (proto == eProtUSBCANAND)) {
        int percent = static_cast<int>(std::floor((static_cast<float>(UMUList[0]->GetUAkkValue()) / 10.0f - 12.9f) / 3.3f * 100.0f));
        if (percent < 0) {
            *pVoltageByPercent = 0;
        }
        else {
            if (percent > 100) {
                *pVoltageByPercent = 100;
            }
            else {
                if (percent > 0) {
                    *pVoltageByPercent = static_cast<unsigned int>(percent);
                }
                else {
                    *pVoltageByPercent = 0;
                }
            }
        }
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
// Установка значения АТТ для зоны контроля АК
void cDevice::SetACControlATT(int ATT)
{
    cfg->setACStartDelay(static_cast<unsigned char>(cfg->getGainBase() + ATT / cfg->getGainStep()));
    Update(true);
}

//---------------------------------------------------------------------------
// Проверка КУ
/*
void cDevice::CheckBadKU()
{
    int scanChannelsCount = cfg->GetScanChannelsCount_();
    for (int ChannelIndex_ = 0; ChannelIndex_ < scanChannelsCount; ++ChannelIndex_) {
        int ChannelIndex = cfg->GetScanChannelIndex_(ChannelIndex_);
        if (ChannelIndex != -1) {
            int gateCount = tbl->GetGateCount(FCurChannel);
            for (int StrobNum_ = 1; StrobNum_ <= gateCount; ++StrobNum_) {
                int StrobNum = tbl->GetGateCount(StrobNum_);
                switch (cfg->getControlledRail())  // Установка параметров канала
                {
                case crSingle:  // Одна нить
                {
                    CheckSetBadKU(dsNone, ChannelIndex, StrobNum);
                    break;
                }
                case crBoth:  // Две нити
                {
                    CheckSetBadKU(dsLeft, ChannelIndex, StrobNum);
                    CheckSetBadKU(dsRight, ChannelIndex, StrobNum);
                    break;
                }
                }
            }
        }
    }
}
*/
int cDevice::CheckSetBadSens(eDeviceSide Side, CID Channel, int GateIdx)
{
    DEFCORE_ASSERT(Channel < 255);
    int TestSensValid = TestSensValidRanges(Side, Channel, GateIdx);
    int ValidRangesIdx = -1;

    for (std::vector<tKuValidRanges>::iterator it = ValidRangesArr.begin(); it != ValidRangesArr.end(); ++it) {
        tKuValidRanges& cur = it.operator*();
        if ((cur.Side == Side) && (cur.Channel == Channel) && (cur.GateIdx == GateIdx)) {
            ValidRangesIdx = std::distance(ValidRangesArr.begin(), it);
            break;
        }
    }

    if (TestSensValid == 0) {
        if (ValidRangesIdx != -1) {
            ValidRangesArr.at(ValidRangesIdx).BadState = false;
        }
    }
    else {
        if (ValidRangesIdx == -1) {
            tKuValidRanges tmp;
            tmp.Time = GetTickCount_();
            tmp.BadState = true;
            sChannelDescription ChanDesc;
            tbl->ItemByCID(Channel, &ChanDesc);
            tmp.GoodSens = ChanDesc.RecommendedSens[GateIdx - 1];
            tmp.Side = Side;
            tmp.Channel = Channel;
            tmp.GateIdx = GateIdx;
            ValidRangesArr.push_back(tmp);
        }
        else {
            ValidRangesArr.at(ValidRangesIdx).BadState = true;
            ValidRangesArr.at(ValidRangesIdx).Time = GetTickCount_();
        }
    }
    return TestSensValid;
}

bool cDevice::IsCopySlaveChannel(eDeviceSide Side, CID Channel)
{
    return cfg->IsCopySlaveChannel(Side, Channel);
}

int cDevice::getSerialNumberForUMU(unsigned int UMUIdx)
{
    return UMUList.at(UMUIdx)->getSerialNumber();
}

std::string cDevice::getFirmwareVersionForUMU(unsigned int UMUIdx)
{
    std::stringstream ss;
    ss << UMUList.at(UMUIdx)->getFirmwareVerionHi();
    ss << ".";
    ss << UMUList.at(UMUIdx)->getFirmwareVerionLo();
    return ss.str();
}

int cDevice::getBuildNumberForUMU(unsigned int UMUIdx)
{
    if (UMUList.at(UMUIdx)->getProtocolType() == eProtLAN) {
        UMU_LAN* umu = reinterpret_cast<UMU_LAN*>(UMUList.at(UMUIdx));
        DEFCORE_ASSERT(umu);
        return umu->getBuildNumberControllerFirmware();
    }

    return 0;
}

std::string cDevice::getFPGAVersionForUMU(unsigned int UMUIdx)
{
    if (UMUList.at(UMUIdx)->getProtocolType() == eProtLAN) {
        UMU_LAN* umu = reinterpret_cast<UMU_LAN*>(UMUList.at(UMUIdx));
        DEFCORE_ASSERT(umu);
        std::stringstream ss;
        ss << umu->getFpgaProjVerId4() << "." << umu->getFpgaProjVerId3() << "." << umu->getFpgaProjVerId2() << "." << umu->getFpgaProjVerId1();
        return ss.str();
    }

    return "";
}

void cDevice::setUsePathEncoderSimulationinSearchMode(bool value)
{
    for (size_t UMUIdx = 0; UMUIdx < UMUList.size(); ++UMUIdx) {
        UMUList.at(UMUIdx)->SetLastSysCrd(cfg->getMainPathEncoderIndex(), LastSysCoord);
    }
    SetPathEncoderData(cfg->getMainPathEncoderIndex(), value, LastSysCoord, false);
    UsePathEncoderSimulationinSearchMode = value;
    Update(false);
}
