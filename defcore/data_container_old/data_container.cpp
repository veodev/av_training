
#include "data_container.h"

cDataContainer::cDataContainer(int Scheme)
{
    HeadOpen = true;
    memset(&Head.FileID, 0, sizeof(sFileHeader));

    CurSysCoord = 0;
    CurDisCoord = 0;
    MaxSysCoord = 0;
    MinSysCoord = 0;
    MaxDisCoord = 0;
    SaveSysCoord = 0;
    LastOffset = 0;
    StartBMSysCoord = 0;
    BackMotionFlag = false;
    SearchBM = false;
    SearchBMEnd = false;

    /*
        sChannelDescription ChanDesc;
        CID ch;
        int ChIdx = 0;

        for (int ChannelIndex = 0; ChannelIndex < DevCfg->GetScanChannelsCount(); ChannelIndex++)
        {
            ch = DevCfg->GetScanChannel(ChannelIndex);
            ChTbl->ItemByCID(ch, &ChanDesc);
            CIDtoChIdx[1][ch] = ChIdx;
            CIDtoChIdx[2][ch] = ChIdx;
            Head.ChIdxtoCID[ChIdx] = ch;
            Head.ChIdxtoGateIdx[ChIdx] = 1;
            ChIdx++;
            if (ChanDesc.cdGateCount == 2)
            {
                CIDtoChIdx[2][ch] = ChIdx;
                Head.ChIdxtoCID[ChIdx] = ch;
                Head.ChIdxtoGateIdx[ChIdx] = 2;
                ChIdx++;
            }
        }
    */

    if (Scheme == 1)
    {

        // Массив связи CID и номера строба с индексом канала в файле

        CIDtoChIdx[0][0x01] =  1; // 0 ЭХО
        CIDtoChIdx[1][0x01] =  0; // 0 ЗТМ
        CIDtoChIdx[0][0x06] =  2; // 58 Н Нераб
        CIDtoChIdx[0][0x0B] =  3; // 58 O Нераб
        CIDtoChIdx[0][0x17] =  4; // 70 Н
        CIDtoChIdx[0][0x18] =  5; // 70 О
        CIDtoChIdx[0][0x19] =  6; // 42 Н Ш
        CIDtoChIdx[0][0x1A] =  7; // 42 О Ш
        CIDtoChIdx[1][0x19] =  8; // 42 Н П
        CIDtoChIdx[1][0x1A] =  9; // 42 О П
        CIDtoChIdx[0][0x07] = 10; // 58 Н Раб
        CIDtoChIdx[0][0x0C] = 11; // 58 O Раб

//        CIDtoChIdx[1][0x05] = 2; // 58 Н Нераб
//        CIDtoChIdx[1][0x0A] = 3; // 58 O Нераб
//        CIDtoChIdx[1][0x17] = 4; // 70 Н
//        CIDtoChIdx[1][0x18] = 5; // 70 О
//        CIDtoChIdx[1][0x04] = 10; // 58 Н Раб
//        CIDtoChIdx[1][0x09] = 11; // 58 O Раб


        // Массивы связи индекса канала в файле с CID и номером строба

        Head.ChIdxtoCID[0] = 0x01;
        Head.ChIdxtoGateIdx[0] = 1;

        Head.ChIdxtoCID[1] = 0x01;
        Head.ChIdxtoGateIdx[1] = 0;

        Head.ChIdxtoCID[2] = 0x06;
        Head.ChIdxtoGateIdx[2] = 0;

        Head.ChIdxtoCID[3] = 0x0B;
        Head.ChIdxtoGateIdx[3] = 0;

        Head.ChIdxtoCID[4] = 0x17;
        Head.ChIdxtoGateIdx[4] = 0;

        Head.ChIdxtoCID[5] = 0x18;
        Head.ChIdxtoGateIdx[5] = 0;

        Head.ChIdxtoCID[6] = 0x19;
        Head.ChIdxtoGateIdx[6] = 0;

        Head.ChIdxtoCID[7] = 0x1A;
        Head.ChIdxtoGateIdx[7] = 0;

        Head.ChIdxtoCID[8] = 0x19;
        Head.ChIdxtoGateIdx[8] = 1;

        Head.ChIdxtoCID[9] = 0x1A;
        Head.ChIdxtoGateIdx[9] = 1;

        Head.ChIdxtoCID[10] = 0x07;
        Head.ChIdxtoGateIdx[10] = 0;

        Head.ChIdxtoCID[11] = 0x0C;
        Head.ChIdxtoGateIdx[11] = 0;
    }
}

cDataContainer::~cDataContainer()
{
}

bool cDataContainer::CreateFile(QString FileName)
{
    DataFile = new QFile(FileName);
    if (!DataFile->open(QIODevice::WriteOnly)) return false;
    return true;
}

QString cDataContainer::HeaderStrToString(tHeaderStr Text)
{
    QString Result = "";
    for (int i = 1; i < (unsigned short)Text[0]; i++)
        /*  if ((Text [i] != "_") and (Text[i] != 0x0A)) */ Result = Result + Text[i];
    return Result;
}

void cDataContainer::StringToHeaderStr(QString InText, ptHeaderStr OutText)
{
    (*OutText)[0] = (unsigned short)InText.length();
    for (int i = 0; i < InText.length(); i++) (*OutText)[i + 1] = InText[i].unicode();
}

QString cDataContainer::HeaderBigStrToString(tHeaderBigStr Text)
{
    QString Result = "";
    for (int i = 1; i < Text[0]; i++)
        Result = Result + Text[i];
    return Result;
}

void cDataContainer::StringToHeaderBigStr(QString InText, ptHeaderBigStr OutText)
{
    (*OutText)[0] = (unsigned short)InText.length();
    for (int i = 0; i < InText.length(); i++) (*OutText)[i + 1] = InText[i].unicode();
}

// Заполнение заголовка файла:

bool cDataContainer::AddDeviceUnitInfo(eDeviceUnitType UnitType, QString WorksNumber, QString FirmwareVer) // Добавить информацию о блоке прибора
{
    if (HeadOpen)
    {
        Head.UnitsInfo[Head.UnitsCount].UnitType = UnitType;
        StringToHeaderStr(WorksNumber, &Head.UnitsInfo[Head.UnitsCount].WorksNumber);
        StringToHeaderStr(FirmwareVer, &Head.UnitsInfo[Head.UnitsCount].FirmwareVer);
        Head.UnitsCount = Head.UnitsCount + 1;
        Head.UsedItems[uiUnitsCount] = 1; // Количество блоков прибора
        return true;
    }
    else return false;
}

bool cDataContainer::SetRailRoadName(QString Name) // Название железной дороги
{
    if (HeadOpen)
    {
        StringToHeaderStr(Name, &Head.RailRoadName);
        Head.UsedItems[uiRailRoadName] = 1; // Название железной дороги
        return true;
    }
    else return false;
}

bool cDataContainer::SetOrganizationName(QString Org) // Название организации осуществляющей контроль
{
    if (HeadOpen)
    {
        StringToHeaderStr(Org, &Head.Organization);
        Head.UsedItems[uiOrganization] = 1; // Название организации осуществляющей контроль
        return true;
    }
    else return false;
}

bool cDataContainer::SetDirectionCode(unsigned int Code) // Код направления
{
    if (HeadOpen)
    {
        Head.DirectionCode = Code;
        Head.UsedItems[uiDirectionCode] = 1; // Код направления
        return true;
    }
    else return false;
}

bool cDataContainer::SetPathSectionName(QString Name) // Название участка дороги
{
    if (HeadOpen)
    {
        StringToHeaderStr(Name, &Head.PathSectionName);
        Head.UsedItems[uiPathSectionName] = 1; // Название участка дороги
        return true;
    }
    else return false;
}

bool cDataContainer::SetRailPathNumber(int Number) // Номер ж/д пути
{
    if (HeadOpen)
    {
        Head.RailPathNumber = Number;
        Head.UsedItems[uiRailPathNumber] = 1; // Номер ж/д пути
        return true;
    }
    else return false;
}

bool cDataContainer::SetRailSection(int Number) // Звено рельса
{
    if (HeadOpen)
    {
        Head.RailSection = Number;
        Head.UsedItems[uiRailSection] = 1; // Звено рельса
        return true;
    }
    else return false;
}

bool cDataContainer::SetDateTime(unsigned short Day_, unsigned short Month_, unsigned short Year_, unsigned short Hour_, unsigned short Minute_) // Дата/время контроля
{
    if (HeadOpen)
    {
        Head.Year = Year_;
        Head.Month = Month_;
        Head.Day = Day_;
        Head.Hour = Hour_;
        Head.Minute = Minute_;
        //  FCurHour = SystemTime.wHour;
        //  FCurMinute = SystemTime.wMinute;
        Head.UsedItems[uiDateTime] = 1; // Дата время контроля
        return true;
    }
    else return false;
}

bool cDataContainer::SetOperatorName(QString Name) // Имя оператора
{
    if (HeadOpen)
    {
        StringToHeaderStr(Name, &Head.OperatorName);
        Head.UsedItems[uiOperatorName] = 1; // Имя оператора
        return true;
    }
    else return false;
}

bool cDataContainer::SetStartMRFCrd(tMRFCrd StartCrd)
{
    if (HeadOpen)
    {
        //FPathCoordSystem = csMetricRF;
        Head.PathCoordSystem = csMetricRF;
        Head.UsedItems[uiPathCoordSystem] = 1; // Система путейской координат
        Head.StartKM = StartCrd.Km;
        Head.StartPk = StartCrd.Pk;
        Head.StartMetre = StartCrd.mm / 1000;
        Head.UsedItems[uiStartMetric] = 1; // Начальная координата - метрическая

        LastMRFCrd = StartCrd;
        LastPostSysCrd = 0;

        return true;
    }
    else return false;
}

bool cDataContainer::SetStartCaCrd(eCoordSys CoordSys, tCaCrd Chainage)
{
    if (HeadOpen)
    {
        //FPathCoordSystem = CoordSys;
        Head.PathCoordSystem = CoordSys;
        Head.UsedItems[uiPathCoordSystem] = 1; // Система путейской координат

        Head.StartChainage = Chainage;
        // FLastCaPost = Chainage;
        Head.UsedItems[uiStartChainage] = 1; // Начальная координата - "Канада"
        return true;
    }
    else return false;
}

bool cDataContainer::SetWorkRailTypeB(tCardinalPoints Val) // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
{
    if (HeadOpen)
    {
        Head.WorkRailTypeB[0] = Val[0];
        Head.WorkRailTypeB[1] = Val[1];
        Head.UsedItems[uiWorkRailTypeB] = 1; // Начальная координата - метрическая
        return true;
    }
    else return false;
}

bool cDataContainer::SetTrackDirection(tCardinalPoints Val) // Код направления: NB, SB, EB, WB, CT, PT, RA, TT
{
    if (HeadOpen)
    {
        Head.TrackDirection[0] = Val[0];
        Head.TrackDirection[1] = Val[1];
        Head.UsedItems[uiTrackDirection] = 1; // Начальная координата - метрическая
        return true;
    }
    else return false;
}

bool cDataContainer::SetTrackID(tCardinalPoints Val) // Track ID: NR, SR, ER, WR
{
    if (HeadOpen)
    {
        Head.TrackID[0] = Val[0];
        Head.TrackID[1] = Val[1];
        Head.UsedItems[uiTrackID] = 1; // Начальная координата - метрическая
        return true;
    }
    else return false;
}

bool cDataContainer::SetCorrespondenceSides(eCorrespondenceSides Val) // Соответствие сторон тележки нитям пути
{
    if (HeadOpen)
    {
        Head.CorrespondenceSides = Val;
        Head.UsedItems[uiCorrSides] = 1;
        return true;
    }
    else return false;
}

bool cDataContainer::CloseHeader(int MovDir, unsigned short ScanStep) // Закончить формирование заголовка
{
    if (HeadOpen)
    {
        memcpy(Head.FileID, AviconFileID, sizeof(tAviconFileID));
        memcpy(Head.DeviceID, Avicon31Scheme1, sizeof(tAviconFileID));
        Head.MoveDir = MovDir;
        Head.UsedItems[uiMoveDir] = 1; // Направление движения
        Head.ScanStep = ScanStep;
        SaveScanStep = ScanStep;

        Head.DeviceVer = 2;
        Head.HeaderVer = 1;
        Head.HCThreshold = 765;

        Head.TableLink = 1357;

        //Head.PathCoordSystem = 231;
        //Head.CheckSumm = 12345;


//          Head.DeviceID = Avicon31Scheme1;


        //  FData.Position[SaveStrIdx] = 0;
        DataFile->write((const char *)(&Head), sizeof(sFileHeader));

        /*
          LastOffset = DataFile->pos();
        {
          SetLength(FCoordList, 1);
          FCoordList[0].KM = Header.StartKM;
          with FCoordList[High(FCoordList)] do
          {
            SetLength(Content, Length(Content) + 1);
            Content[High(Content)].Pk = Header.StartPk;
            Content[High(Content)].Idx = 0;

            FLastKm = High(FCoordList);
            FLastPk = High(Content);
          }
        }
          FLastCheckSumOffset = SizeOf(Head);
          AddEvent(EID_FwdDir, 0, 0); */
        HeadOpen = false;
        return true;
    }
    else return false;
}

// Сигналы и координата:

void cDataContainer::AddSysCoord(int SysCoord) // Новая координата
{
    CurSysCoord = SysCoord;
    CurDisCoord = CurDisCoord + abs(SysCoord - SaveSysCoord); // Расчет дисплейной координаты (возрастает даже при движении назад)

    MaxSysCoord = qMax(MaxSysCoord, SysCoord);
    MinSysCoord = qMax(MinSysCoord, SysCoord);
    MaxDisCoord = CurDisCoord;

    if ((!BackMotionFlag) and (CurSysCoord < SaveSysCoord)) // Если изменилось направление движения (вперед на назад)
    {
        LastOffset = DataFile->pos();
        //?AddEvent(EID_BwdDir, SaveSysCrd, Last);
        BackMotionFlag = true;
        if (!SearchBMEnd)
        {
            StartBMSysCoord = SaveSysCoord;
            SearchBMEnd = true;
        }
    }
    else if ((BackMotionFlag) && (SaveSysCoord > SaveSysCoord)) // Если изменилось направление движения (назад на вперед)
    {
        LastOffset = DataFile->pos();
        //AddEvent(EID_FwdDir, SaveSysCrd, Last);
        BackMotionFlag = false;
    }

    if ((SearchBMEnd) && (CurSysCoord > StartBMSysCoord))
    {
        SearchBMEnd = false;
        LastOffset = DataFile->pos();
        //if (CurSysCoord - SaveSysCoord != 0) /* AddEvent(EID_EndBM, FStartBMSysCoord, Round(FLastSaveDisCrd + (FStartBMSysCoord - SaveSysCrd) * (FCurSaveDisCrd - FLastSaveDisCrd) / (FCurSaveSysCrd - SaveSysCrd)))*/;
        //                                 else /*AddEvent(EID_EndBM)*/;
    }

    if (DataFile->pos() - LastOffset >= 16384)
    {
        LastOffset = DataFile->pos();
        // AddEvent(EID_LinkPt);
    }

    //LastSysCrdOffset = DataFile->pos();

    // Система записи коррдинат без ссылок

    if ((SaveSysCoord & 0xFFFFFF00) == (SysCoord & 0xFFFFFF00))
    {
        // Короткая координата
        id = EID_SysCrd_NS;
        DataFile->write((const char *)(&id), 1);
        DataFile->write((const char *)(&SysCoord), 1);
    }
    else
    {
        // Полная координата
        id = EID_SysCrd_NF;
        DataFile->write((const char *)(&id), 1);
        DataFile->write((const char *)(&SysCoord), 4);
    }

    SaveSysCoord = SysCoord;
}
// Сигналы В-развертки
bool cDataContainer::AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, unsigned char D0, unsigned char A0, unsigned char D1, unsigned char A1, unsigned char D2, unsigned char A2, unsigned char D3, unsigned char A3, unsigned char D4, unsigned char A4, unsigned char D5, unsigned char A5, unsigned char D6, unsigned char A6, unsigned char D7, unsigned char A7)
{

    unsigned char Cnt[2];
    /*
    if not FScanChNumExists[ScanChNum] then
    {
        Result = False;
        Exit;
    }
    */

    if (Count != 0)
    {
        // Разделение на группы по 4 сигнала
        switch (Count)
        {
        case 1:
        {
            Cnt[0] = 1;
            Cnt[1] = 0;
            break;
        }
        case 2:
        {
            Cnt[0] = 2;
            Cnt[1] = 0;
            break;
        }
        case 3:
        {
            Cnt[0] = 3;
            Cnt[1] = 0;
            break;
        }
        case 4:
        {
            Cnt[0] = 4;
            Cnt[1] = 0;
            break;
        }
        case 5:
        {
            Cnt[0] = 4;
            Cnt[1] = 1;
            break;
        }
        case 6:
        {
            Cnt[0] = 4;
            Cnt[1] = 2;
            break;
        }
        case 7:
        {
            Cnt[0] = 4;
            Cnt[1] = 3;
            break;
        }
        case 8:
        {
            Cnt[0] = 4;
            Cnt[1] = 4;
            break;
        }
        default:
        {
            Cnt[0] = 0;
            Cnt[1] = 0;
            break;
        }
        }
    }
    else return false;

    id = (((unsigned char)(Side == dsRight) & 0x01) << 6) + ((CIDtoChIdx[0][Channel] & 0x0F) << 2) + ((Cnt[0] - 1) & 0x03);
    DataFile->write((const char *)(&id), 1);

    switch (Cnt[0])
    {
    case 1:
    {
        DataFile->write((const char *)(&D0), 1);
        id = (A0 & 0x0F) << 4;
        DataFile->write((const char *)(&id), 1);
        break;
    }
    case 2:
    {
        DataFile->write((const char *)(&D0), 1);
        DataFile->write((const char *)(&D1), 1);
        id = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile->write((const char *)(&id), 1);
        break;
    }
    case 3:
    {
        DataFile->write((const char *)(&D0), 1);
        DataFile->write((const char *)(&D1), 1);
        DataFile->write((const char *)(&D2), 1);
        id = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile->write((const char *)(&id), 1);
        id = ((A2 & 0x0F) << 4);
        DataFile->write((const char *)(&id), 1);
        break;
    }
    case 4:
    {
        DataFile->write((const char *)(&D0), 1);
        DataFile->write((const char *)(&D1), 1);
        DataFile->write((const char *)(&D2), 1);
        DataFile->write((const char *)(&D3), 1);
        id = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile->write((const char *)(&id), 1);
        id = ((A2 & 0x0F) << 4) | (A3 & 0x0F);
        DataFile->write((const char *)(&id), 1);
        break;
    }
    }

    if (Cnt[1] != 0)
    {
        id = (((unsigned char)(Side == dsRight) & 0x01) << 6) + ((CIDtoChIdx[0][Channel] & 0x0F) << 2) + ((Cnt[1] - 1) & 0x03);
        DataFile->write((const char *)(&id), 1);

        switch (Cnt[1])
        {
        case 1:
        {
            DataFile->write((const char *)(&D4), 1);
            id = (A4 & 0x0F) << 4;
            DataFile->write((const char *)(&id), 1);
            break;
        }
        case 2:
        {
            DataFile->write((const char *)(&D4), 1);
            DataFile->write((const char *)(&D5), 1);
            id = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile->write((const char *)(&id), 1);
            break;
        }
        case 3:
        {
            DataFile->write((const char *)(&D4), 1);
            DataFile->write((const char *)(&D5), 1);
            DataFile->write((const char *)(&D6), 1);
            id = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile->write((const char *)(&id), 1);
            id = ((A6 & 0x0F) << 4);
            DataFile->write((const char *)(&id), 1);
            break;
        }
        case 4:
        {
            DataFile->write((const char *)(&D4), 1);
            DataFile->write((const char *)(&D5), 1);
            DataFile->write((const char *)(&D6), 1);
            DataFile->write((const char *)(&D7), 1);
            id = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile->write((const char *)(&id), 1);
            id = ((A6 & 0x0F) << 4) | (A7 & 0x0F);
            DataFile->write((const char *)(&id), 1);
            break;
        }
        }
    }
    return true;
}

bool cDataContainer::AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, tEchoBlock EchoBlock)
{
    return AddEcho(Side, Channel, Count, EchoBlock[0].Delay, EchoBlock[0].Ampl,
                   EchoBlock[1].Delay, EchoBlock[1].Ampl,
                   EchoBlock[2].Delay, EchoBlock[2].Ampl,
                   EchoBlock[3].Delay, EchoBlock[3].Ampl,
                   EchoBlock[4].Delay, EchoBlock[4].Ampl,
                   EchoBlock[5].Delay, EchoBlock[5].Ampl,
                   EchoBlock[6].Delay, EchoBlock[6].Ampl,
                   EchoBlock[7].Delay, EchoBlock[7].Ampl);
}

void cDataContainer::AddSensor1State(eDeviceSide Side, bool State) // Данные датчика болтового стыка
{
    LastOffset = DataFile->pos();
    id = EID_Sensor1;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&State), 1);
    //  AddEvent(EID_Sensor1);
}

// Настройки каналов:

void cDataContainer::AddSens(eDeviceSide Side, CID Channel, int GateIndex, char NewValue) // Изменение условной чувствительности
{
    //FCurSaveParam[Side, EvalCh].Ku = NewValue;
    LastOffset = DataFile->pos();
    id = EID_Ku;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[GateIndex][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 1);
    //AddEvent(EID_Ku);
}

void cDataContainer::AddGain(eDeviceSide Side, CID Channel, unsigned char NewValue) // Изменение положения нуля аттенюатора (0 дБ условной чувствительности)
{
    //FCurSaveParam[Side, EvalCh].Att = NewValue;
    LastOffset = DataFile->pos();
    id = EID_Att;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[0][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 1);
    //AddEvent(EID_Att);
}

void cDataContainer::AddTVG(eDeviceSide Side, CID Channel, unsigned char NewValue) // Изменение ВРЧ
{
    //FCurSaveParam[Side, EvalCh].TVG = NewValue;
    LastOffset = DataFile->pos();
    id = EID_TVG;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[0][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 1);
    //AddEvent(EID_TVG);
    /*
      if (FConfig.GetSecondEvalByEval(EvalCh, SecondEvalCh)) then
      {
        FCurSaveParam[Side, SecondEvalCh].TVG = NewValue;
        LastOffset = DataFile->pos();
        id = EID_TVG;
        DataFile->write((const char *)(&id, 1);
        DataFile->write((const char *)(&Side, 1);
        DataFile->write((const char *)(&SecondEvalCh, 1);
        DataFile->write((const char *)(&NewValue, 1);
        AddEvent(EID_TVG);
      }

        */
}

void cDataContainer::AddStGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue) // Изменение положения начала строба
{
    //FCurSaveParam[Side, EvalCh].StStr = NewValue;
    LastOffset = DataFile->pos();
    id = EID_StStr;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[GateIndex][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 1);
    //AddEvent(EID_StStr);
}

void cDataContainer::AddEndGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue) // Изменение положения конца строба
{
    //FCurSaveParam[Side, EvalCh].EndStr = NewValue;
    LastOffset = DataFile->pos();
    id = EID_EndStr;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[GateIndex][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 1);
    //AddEvent(EID_EndStr);
}

void cDataContainer::AddPrismDelay(eDeviceSide Side, CID Channel, unsigned short NewValue) // Изменение 2Тп мкс * 10 (word)
{
    //FCurSaveParam[Side, EvalCh].PrismDelay = NewValue;
    LastOffset = DataFile->pos();
    id = EID_PrismDelay;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[0][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&NewValue), 2);
    //AddEvent(EID_PrismDelay);
    /*
    if (FConfig.GetSecondEvalByEval(EvalCh, SecondEvalCh)) then
    {
    FCurSaveParam[Side, SecondEvalCh].PrismDelay = NewValue;
    LastOffset = DataFile->pos();
    id = EID_PrismDelay;
    DataFile->write((const char *)(&id, 1);
    DataFile->write((const char *)(&Side, 1);
    DataFile->write((const char *)(&SecondEvalCh, 1);
    DataFile->write((const char *)(&NewValue, 2);
    AddEvent(EID_PrismDelay);
    } */
}

// Режимы:

void cDataContainer::AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime, bool AddChInfo, eDeviceSide Side, CID Channel, int GateIndex) // Информация о смене режима
{
    LastOffset = DataFile->pos();
    id = EID_Mode;
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&ModeIdx), 2);
    DataFile->write((const char *)(&AddChInfo), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[GateIndex][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&InPreviousModeTime), 2);
    //AddEvent(EID_Mode);
}

void cDataContainer::AddSetRailType(void) // Настройка на тип рельса
{
    LastOffset = DataFile->pos();
    id = EID_SetRailType;
    DataFile->write((const char *)(&id), 1);
    //AddEvent(EID_SetSideType);
}

void cDataContainer::AddHeadPh(eDeviceSide Side, CID Channel, int GateIndex, bool Enabled)
{
    LastOffset = DataFile->pos();
    id = EID_HeadPh;
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    id = CIDtoChIdx[GateIndex][Channel];
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&Enabled), 1);
    //AddEvent(EID_HeadPh);
}

// Отметки:

void cDataContainer::AddTextLabel(QString Text)
{
    LastOffset = DataFile->pos();
    id = EID_TextLabel;
    DataFile->write((const char *)(&id), 1);
    id = qMin(256, Text.length());
    DataFile->write((const char *)(&id), 1);
    for (int chidx = 0; chidx < Text.length(); chidx++)
        DataFile->write((const char *)(&Text[chidx].unicode()), 2 /*sizeof(char)*/);
    //AddEvent(EID_TextLabel);
}

void cDataContainer::AddDefLabel(eDeviceSide Side, QString Text)
{
    LastOffset = DataFile->pos();
    id = EID_DefLabel;
    DataFile->write((const char *)(&id), 1);
    id = qMin(256, Text.length());
    DataFile->write((const char *)(&id), 1);
    id = (unsigned char)(Side == dsRight);
    DataFile->write((const char *)(&id), 1);
    for (int chidx = 0; chidx < Text.length(); chidx++)
        DataFile->write((const char *)(&Text[chidx].unicode()), 2 /*sizeof(char)*/);
    //AddEvent(EID_DefLabel);
}

void cDataContainer::AddStrelka(QString Text)
{
    LastOffset = DataFile->pos();
    id = EID_Switch;
    DataFile->write((const char *)(&id), 1);
    id = qMin(256, Text.length());
    DataFile->write((const char *)(&id), 1);
    for (int chidx = 0; chidx < Text.length(); chidx++)
        DataFile->write((const char *)(&Text[chidx].unicode()), 2 /*sizeof(char)*/);
    //  AddEvent(EID_Switch);
}

void cDataContainer::AddStBoltStyk(void)
{
    LastOffset = DataFile->pos();
    id = EID_StBoltStyk;
    DataFile->write((const char *)(&id), 1);
    //AddEvent(EID_StBoltStyk);
}

void cDataContainer::AddEdBoltStyk(void)
{
    LastOffset = DataFile->pos();
    id = EID_EndBoltStyk;
    DataFile->write((const char *)(&id), 1);
    //AddEvent(EID_EndBoltStyk);
}

void cDataContainer::AddTime(unsigned short Hour_, unsigned short Minute_)
{
    LastOffset = DataFile->pos();
    id = EID_Time; // Отметка времени
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&Hour_), 1);
    DataFile->write((const char *)(&Minute_), 1);
    //AddEvent(EID_Time);
}

void cDataContainer::AddHandScan()
{
    LastOffset = DataFile->pos();
    id = EID_HandScan;
    DataFile->write((const char *)(&id), 1);
}

void cDataContainer::AddMRFPost(tMRFPost Post)
{
    LastOffset = DataFile->pos();
    id = EID_Stolb;
    DataFile->write((const char *)(&id), 1);
    sCoordPostMRF Stolb;
    Stolb.Km[0] = Post.Km[0];
    Stolb.Km[1] = Post.Km[1];
    Stolb.Pk[0] = Post.Pk[0];
    Stolb.Pk[1] = Post.Pk[1];
    memset(&Stolb.Reserv[0], 0, 32 * 4);
    DataFile->write((const char *)(&Stolb), 144);

    LastMRFCrd.Km = Post.Km[1];
    LastMRFCrd.Pk = Post.Pk[1];
    LastMRFCrd.mm = 0;
    LastPostSysCrd = CurSysCoord;

    //  FLastPostDisCrd = FCurSaveDisCrd;
    //  FLastMRFPost.Km = Km1;
    //  FLastMRFPost.Pk = Pk1;
    //  FLastMRFPost.mm = 0;
    //  FExHeader.EndLevel2 = (FCurSaveSysCrd - FLastStolbSysCrd) * Head.ScanStep div 100;
    //  AddEvent(EID_Stolb);
    //  Inc(FStolbCount);
}

void cDataContainer::AddCaPost(tCaCrd Chainage)
{
    LastOffset = DataFile->pos();
    id = EID_StolbChainage;
    DataFile->write((const char *)(&id), 1);
    sCoordPostChainage Stolb;
    Stolb.Val = Chainage;
    memset(&Stolb.Reserv[0], 0, 32 * 4);
    DataFile->write((const char *)(&Stolb), 136);

//  FExHeader.EndLevel0 = Val;
//  FExHeader.EndLevel1 = 0;
//  FExHeader.EndLevel2 = (FCurSaveSysCrd - FLastStolbSysCrd) * Head.ScanStep div 100;
//  FLastPostSysCrd = FCurSaveSysCrd;
//  FLastPostDisCrd = FCurSaveDisCrd;
//  FLastCaPost = Chainage;
//  AddEvent(EID_StolbChainage);
}


void cDataContainer::CloseFile(void)
{
    LastOffset = DataFile->pos();
    id = EID_EndFile;
    DataFile->write((const char *)(&id), 1);

    unsigned int id2 = 0xFFFFFFFF;
    DataFile->write((const char *)(&id2), 4);

    unsigned char id3[9];
    memset(&(id3[0]), 0xFF, 9);
    DataFile->write((const char *)(&id3), 9);

    DataFile->flush();
}

tMRFCrd cDataContainer::GetCurrectCoord(void)
{
    tMRFCrd tmp;

    tmp.Km = LastMRFCrd.Km;
    tmp.Pk = LastMRFCrd.Pk;
    if (SaveMoveDir > 0)
        tmp.mm = (int)((float)(CurSysCoord - LastPostSysCrd) * (float)SaveScanStep / (float)100);
    else
        tmp.mm = 100000 - (int)((float)(CurSysCoord - LastPostSysCrd) * (float)SaveScanStep / (float)100);
    return tmp;
}

// ----------------------------------------------------------------------------

tMRFCrd GetPrevMRFPostCrd(tMRFCrd Post, int MoveDir) // Получить дредыдущий столб
{
    if (MoveDir > 0)
    {
        Post.Pk--;
        if (Post.Pk == 0)
        {
            Post.Pk = 10;
            Post.Km--;
        }
    }
    else
    {
        Post.Pk++;
        if (Post.Pk == 11)
        {
            Post.Pk = 1;
            Post.Km++;
        }
    };
    Post.mm = 0;
    return Post;
}

tMRFCrd GetNextMRFPostCrd(tMRFCrd Crd, int MoveDir) // Получить следующий столб
{
    if (MoveDir > 0)
    {
        Crd.Pk++;
        if (Crd.Pk == 11)
        {
            Crd.Pk = 1;
            Crd.Km++;
        }
    }
    else
    {
        Crd.Pk--;
        if (Crd.Pk == 0)
        {
            Crd.Pk = 10;
            Crd.Km--;
        }
    };
    Crd.mm = 0;
    return Crd;
}

tMRFPost GetMRFPost(tMRFCrd Crd, int MoveDir) // Получить столб
{
    tMRFPost tmp;

    if (MoveDir > 0)
    {
        if (Crd.Pk == 1)
        {
            tmp.Km[0] = Crd.Km - 1;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = 10;
            tmp.Pk[1] = 1;
        }
        else
        {
            tmp.Km[0] = Crd.Km;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = Crd.Pk - 1;
            tmp.Pk[1] = Crd.Pk;
        };
    }
    else
    {
        if (Crd.Pk == 10)
        {
            tmp.Km[0] = Crd.Km + 1;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = 1;
            tmp.Pk[1] = 10;
        }
        else
        {
            tmp.Km[0] = Crd.Km;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = Crd.Pk + 1;
            tmp.Pk[1] = Crd.Pk;
        };
    };
    return tmp;
}

/*
function MRFCrdParamsToCrd(CoordParams: TMRFCrdParams; MoveDir: Integer): TMRFCrd;
begin
Result.Km:= CoordParams.LeftStolb[ssRight].Km;
Result.Pk:= CoordParams.LeftStolb[ssRight].Pk;
if MoveDir > 0 then Result.mm:= CoordParams.ToLeftStolbMM
               else Result.mm:= CoordParams.ToRightStolbMM;
end;

QString MRFCrdToStr(tMRFCrd Post, int Index); // Координату в текст
{
    int M = Post.mm div 1000;
    int MM = Round(1000 * Frac(Post.mm / 1000));

    case Index of
      0: Result = Format('%D ' + FKM_Name + ' %D ' + FPK_Name + ' %D ' + FMetr_Name + ' %D ' + FMilimetr_Name, [Data.Km, Data.Pk, M, MM]);
      1: Result = Format('%D ' + FKM_Name + ' %D ' + FPK_Name + ' %D ' + FMetr_Name + ' %D ' + FSantiMetr_Name, [Data.Km, Data.Pk, M, MM div 10]);
      2: Result = Format('%D ' + FKM_Name + ' %D ' + FPK_Name + ' %D ' + FMetr_Name + '', [Data.Km, Data.Pk, M]);
      3: Result = Format('%D / %D / %D', [Data.Km, Data.Pk, M]);
    end;
}
*/

void cDataContainer::AddSatelliteCoord(float Latitude, float Longitude) // Географическая координата
{
    LastOffset = DataFile->pos();
    id = EID_SatelliteCoord;
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&Latitude), 4);
    DataFile->write((const char *)(&Longitude), 4);
}

void cDataContainer::AddSCReceiverStatus(unsigned char State) // Состояние приемника GPS
{
    LastOffset = DataFile->pos();
    id = EID_SCReceiverStatus;
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&State), 1);
    char Reserv[8];
    memset(&Reserv[0], 0, 8);
    DataFile->write((const char *)(&Reserv), 8);
}

void cDataContainer::AddMedia(void * DataPrt, tMediaType Type, unsigned int Size) // Медиа данные
{
    LastOffset = DataFile->pos();
    id = EID_Media;
    DataFile->write((const char *)(&id), 1);
    DataFile->write((const char *)(&Type), 1);
    DataFile->write((const char *)(&Size), 4);
    DataFile->write((const char *)DataPrt, Size);
}
