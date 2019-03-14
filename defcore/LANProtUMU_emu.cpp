#ifdef _AUTOCON
	#include "UMUsTest.inc"
#endif

#include "platforms.h"

#ifdef SIMULATION

#include "LanProtUMU.h"
#include "TickCount.h"
#include <math.h>
#include <memory.h>

    tUMU_3204_StrokeParams StrokeParams[12];
    int PathEncoderData[3][2];
    bool PathSimulator_[2];
    bool GetDeviceID;
    bool Alarm[2][2][12][2];
    bool Alarm_[3] = {false, false, false};
    bool AScan[3]= {false, false, false};
    unsigned char AScanStDelay;
    unsigned char AScanZoom;
    unsigned char AScanMode;
    unsigned char AScanStrob;
    bool BScan[3] = {false, false, false};
    unsigned short GainSegmentStDelay[3][2][12];
    unsigned short GainSegmentEdDelay[3][2][12];
    unsigned short GainSegmentStVal[3][2][12];
    unsigned short GainSegmentEdVal[3][2][12];
	unsigned char GateStDelay[3][2][12][2];
    unsigned char GateEdDelay[3][2][12][2];
    unsigned char GateLevel[3][2][12][2];
    StrobeMode GateMode[3][2][12][2];
    eAlarmAlgorithm GateAlgorithm[3][2][12][2];
    unsigned short PrismDelay[3][2][12];
	AScanData buff;
    unsigned char AScanSrcData[3][2][12][256];
    tUMU_BScanSignals BScanSrcData;
	int ParamA_;
	float ParamM_;
//	int ParamA[3][2][16];
//	float ParamM[3][2][16];

#define CLAMP(x, low, hi) (std::min(std::max((x), (low)), (hi)))


namespace LANProtDebug
{
	Add_Log onAddLog;

    #if defined(DEFCORE_DEBUG)
    #if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    bool useLog = true;
	CRITICAL_SECTION cr;
    #endif
    #endif

    unsigned int rcvbytescnt;

}

using namespace LANProtDebug;


static struct
{
	int State;
    unsigned long CreateTime;
    unsigned long LifeTime;
	float StartDelay;
	float DelayStep;
	float StartAmpl;
	float AmplStep;
	float Delay;
	float Ampl;
} SignalData[2][2][12][8];

unsigned long SimTime;

//---------------------------------------------------------------------------
DEFCORE_FASTCALL UMU_3204::UMU_3204(void)
{
    AScanZoom = 1;

    ParamA_ = 0;
	ParamM_ = 0;

	for (int s = 0; s < 2; s++)
        for (int l = 0; l < 2; l++)
            for (int t = 0; t < 12; t++)
                for (int i = 0; i < 8; i++)	SignalData[s][l][t][i].State = 0;

	SendTickT = 0;
	WState = bsOff;
	RState = rsOff;
	OutStIdx = 0;
	OutEdIdx = 0;
	EndWorkFlag = false;
	SaveEventID = edMScan2Data;

	MessageReadBytesCount = 0;
	LastMessageCount = - 1;
	MessageCount = 0;

	OutBlock = new tLAN_Message;
	memset(OutBlock,0,LANBufferSize);

	SaveAScanZoom = 0;
    AScanMeasureBuff_ = NULL;
    AScanDataBuff_ = NULL;
    BScanBuff_ = NULL;
    AlarmBuff_ = NULL;
	AScanStDelay = NULL;
	AScanZoom = NULL;
	AScanMode = NULL;
	AScanStrob = NULL;

	SaveAScanZoom = NULL;
	SaveAScanSide = NULL;
	SaveAScanStroke = NULL;
	SaveAScanLine = NULL;

	//fix
	MessageReadBytesCount = 0;
	SkipMessageCount = 0;
	ErrorMessageCount = 0;
    LastMessageCount = - 1;
	MessageCount = 0;
	StrokeCount = 1;
	//---------------------------

    FirmwareVerionLo = 0;
    FirmwareVerionHi = 0;
    SerialNumber = 0;

	#ifdef UMULog                           //
	if (!Log) Log = new LogFileObj(this);
	Log->AddIntParam("Create Object UMU:%d", UMUIdx);
	#endif

    #if defined (DEFCORE_DEBUG)
    #if defined (DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
    //initbytcnt();
    #endif
    #endif
}
//---------------------------------------------------------------------------
DEFCORE_FASTCALL UMU_3204::~UMU_3204(void)
{
	delete OutBlock;
//	#ifdef BUMLog                           //
	#ifdef UMULog                           //
	Log->AddIntParam("Delete Object UMU:%d", UMUIdx);
//	if ((!Log) && (Log->Owner == this)) delete Log;
	if ((Log!=NULL) && (Log->Owner == this)) delete Log;
	#endif

    #if defined(DEFCORE_DEBUG)
    #if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
   // deinitbytcnt();
	#endif
    #endif
}

//---------------------------------------------------------------------------

void UMU_3204::SetData(int UMUIdx_, int DataChannelId_, cDataTransfer* hrd_, cEventManager* DatSdr_, cCriticalSection* cs_)
{
	UMUIdx = UMUIdx_;
    DataChannelId = DataChannelId_;
    DatSdr = DatSdr_;
	hrd = hrd_;
    cs = cs_;
}

//---------------------------------------------------------------------------
void UMU_3204::StartWork(void)
{
	#ifdef UMUEventLog                           //
	Log->AddIntParam("UMU:%d StartWork", UMUIdx);
	#endif
	if (WState == bsOff)
//        WState = bsInit;
//        WState = bsReady;
        WState = bsPowerOn;
}
//---------------------------------------------------------------------------
void UMU_3204::EndWork(void)
{
	DisableAScan();
	DisableBScan();
	DisableAlarm();
	EndWorkFlag = true;

//	if (hrd != NULL) {
//		hrd->PowerSwitch(UMUIdx, psOff);
//	}
	#ifdef UMUEventLog                           //
	Log->AddIntParam("UMU:%d Power Switch Off", UMUIdx);
	Log->AddIntParam("UMU:%d EndWork", UMUIdx);
	#endif
}

//---------------------------------------------------------------------------
bool UMU_3204::EndWorkTest(void)
{
    return (EndWorkFlag && OutBuffer.empty());
}

//---------------------------------------------------------------------------
void UMU_3204::AddToOutBuff(void) //      -   OutBlock
{
	tLAN_Message tmp;
    memcpy(&tmp,OutBlock,OutBlock->Size+6);
	OutBuffer.push(tmp);
	memset(OutBlock, 0, LanDataMaxSize);
}
//---------------------------------------------------------------------------
void UMU_3204::Unload(void)
{
//  int f = OutBuffer.size();
   	while (!OutBuffer.empty())
	{
		if (hrd->WriteBuffer(DataChannelId, (unsigned char *)&OutBuffer.front(), OutBuffer.front().Size+6))
		{
            #if defined (DEFCORE_DEBUG)
            #if defined (DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
		   //	if (useLog)	onAddLog((unsigned char *)&OutBuffer.front(), 2);
			#endif
            #endif
            OutBuffer.pop();
		}
		else
		{
			#ifdef UMUOutDataLog
			Log->AddText(" - WriteToLAN - Error");
			#endif
		}
	}
//    OutBuffer.clear();
}
//---------------------------------------------------------------------------
void UMU_3204::TickSend(void)    //
{
	unsigned short res;

	switch (WState)
	{
		case bsOff: break;
		case bsInit:
        {
			//  ,      -  
			WState = bsPowerOn;
		}
        case bsPowerOn:
        {
//		  	res = hrd->PowerSwitch(UMUIdx, psOn);
        	res = 1;
			if (res == 1)
			{
				#ifdef UMUEventLog                         //
				Log->AddIntParam("UMU:%d Power Switch OK", UMUIdx);
				#endif
				WState = bsRequest;
				RState = rsHead;
			} else break;
        }
        case bsRequest: 
        {
			RequestTime = GetTickCount_();
			WState = bsWaitAnswer;
//			GetDeviceID();
			#ifdef UMUEventLog                         // 
			Log->AddIntParam("UMU:%d Send Request", UMUIdx);
			#endif
			break;
        }
        case bsWaitAnswer:
        { 
			Unload();
			if (GetTickCount_() - RequestTime > 500) WState = bsRequest;

			if (EventCount[cSerialNumber] > 0) // ?
			{
				WState = bsReady;
				#ifdef UMUEventLog                         // 
				Log->AddIntParam("UMU:%d Get Request OK", UMUIdx);
				#endif
			}

			break;
		}

		case bsReady:
		{
			Unload();
			break;
		}
		case bsStop: break;
	}
}
//---------------------------------------------------------------------------
void UMU_3204::TickReceive(void) //
{

	unsigned short res;
	switch (RState)
	{
		case rsHead:
		{
			{
				res = hrd->ReadBuffer(DataChannelId,(unsigned char *)&LAN_message,6);
				if (res == 6)
				{
					RState = rsBody;
					ReadBytesCount = LAN_message.Size;

                    MessageReadBytesCount = MessageReadBytesCount + LAN_message.Size;

                    if (LastMessageCount != - 1)
                        if (!((LAN_message.MessageCount - LastMessageCount == 0x01) ||
                              ((LAN_message.MessageCount == 0x00) && (LastMessageCount == 0xFF))))
                        {
                             if (LAN_message.MessageCount < LastMessageCount)
                             {
                                SkipMessageCount = SkipMessageCount + LAN_message.MessageCount - LastMessageCount - 1;
                             }
                             else
                             if (LAN_message.MessageCount > LastMessageCount)
                             {
                                SkipMessageCount = SkipMessageCount + 255 - LastMessageCount + LAN_message.MessageCount;
                             }
                             else
                             if (LAN_message.MessageCount == LastMessageCount)
                             {
                                SkipMessageCount = SkipMessageCount + 255;
                             }
                        }

                    LastMessageCount = LAN_message.MessageCount;
                    MessageCount++;

				}
                else
                if (res != 0)
                {
                    #if defined (DEFCORE_DEBUG)
                    #if defined (DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
                    //if (useLog)	onAddLog(NULL, 3); // Ошибка приема данных
					#endif
                    #endif
                }
			}

			break;
		}
		case rsBody:
		{
			res = hrd->ReadBuffer(DataChannelId,(unsigned char *)&LAN_message.Data,ReadBytesCount);
			if (res == ReadBytesCount)
			{
				RState = rsHead;
				UnPack(LAN_message);
                #if defined (DEFCORE_DEBUG)
                #if defined (DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
                //if (useLog)	onAddLog((unsigned char *)&LAN_message, 1);
				#endif
                #endif

			}
            else
            if (res != 0)
            {
                #if defined (DEFCORE_DEBUG)
                #if defined (DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
                //if (useLog)	onAddLog(NULL, 3); // Ошибка приема данных
                #endif
                #endif
            }
			break;
        }
		case rsOff: break;
    }
}
//---------------------------------------------------------------------------
void UMU_3204::UnPack(tLAN_Message Buff)
{
    UNUSED(Buff);
/*
	EventCount[Buff.Id]++;
	switch (Buff.Id)
	{
		   case dPathStep:
					{    */

                            // БУМ - Срабатывание датчика пути и В-развертка
                           /*
							if (BScanBuff_ != NULL)
							{
								cs->Enter();
								DatSdr->CreateNewEvent(SaveEventID);
								if (1 != 0)            SaveEventID = edBScan2Data;
												  else SaveEventID = edMScan2Data;
                                DatSdr->WriteEventData(&BScanBuff_, SIZE_OF_TYPE(&BScanBuff_));
								cs->Release();
								DatSdr->Post();
							}
                            */



							if(BScan[UMUIdx])
                            {
                                cs->Enter();
                                BScanBuff_ = new(tUMU_BScanData);
                                BScanBuff_->UMUIdx = UMUIdx;

                                static DWORD prevPathTime = GetTickCount();

                                PathSimulator_[0]=1;

                                static int newstep = 1;                     // Шаг ДП - единица
                                if((PathEncoderData[UMUIdx][0] + newstep) > ScanLen)
                                    newstep = -1;
                                else if((PathEncoderData[UMUIdx][0] + newstep) < 0)
                                    newstep = 1;
                                //int newstep = RandomRange(-5, 5);  // Случайный шаг ДП

                                PathEncoderData[UMUIdx][0] = PathEncoderData[UMUIdx][0] + newstep;
                                BScanBuff_->Dir[0] = newstep;
                                //PathEncoderData[UMUIdx][0]++;
                                //BScanBuff_->Dir[0] = 1;
                                BScanBuff_->XSysCrd[0] = PathEncoderData[UMUIdx][0];

                                memcpy(&BScanBuff_->Signals.Count[0][0][0], &BScanSrcData, sizeof(tUMU_BScanSignals));


                                DatSdr->CreateNewEvent(SaveEventID);
                                if (PathSimulator_[0]) SaveEventID = edBScan2Data;
                                                  else SaveEventID = edMScan2Data;
                                DatSdr->WriteEventData(&BScanBuff_, SIZE_OF_TYPE(&BScanBuff_));
                                cs->Release();
                                DatSdr->Post();
                            }



/*							break;
					}
			case cFWver: // ОБЩ - Версия и дата прошивки блока
					{
							FirmwareVerionLo = Buff.Data[1] & 0x0F;  //
							FirmwareVerionHi = Buff.Data[2];		 //
							FirmwareYear = 2000 + Buff.Data[3];      //  -
							FirmwareMonth = Buff.Data[4];            //  -
							FirmwareDay = Buff.Data[5];              //  -
							FirmwareHour = Buff.Data[6];             //
							FirmwareMinute = Buff.Data[7];           //
							FirmwareSecond = Buff.Data[8];           //

							#ifdef UMUEventLog                         //
							Log->AddIntParam("UMU:%d Firmware Info", Buff.Data[0]);
							Log->AddIntParam("Day: %d",      Buff.Data[5]);
							Log->AddIntParam("Month: %d",    Buff.Data[4]);
							Log->AddIntParam("Year: %d",     Buff.Data[3]);
							Log->AddIntParam("Hour: %d",     Buff.Data[6]);
							Log->AddIntParam("Minute: %d",   Buff.Data[7]);
							Log->AddIntParam("Second: %d",   Buff.Data[8]);
							Log->AddIntParam("VersionHi: %d", Buff.Data[2]);
							Log->AddIntParam("VersionLo: %d", Buff.Data[1] & 0x0F);
							#endif
							break;
					}
			case cSerialNumber:  // ОБЩ - Серийный номер устройства
					{
							SerialNumber = Buff.Data[0] + (Buff.Data[1] << 8);
							#ifdef UMUEventLog                         //
							Log->AddTwoIntParam("UMU:%d SerialNumber", UMUIdx, SerialNumber);
							#endif
							break;
					}
			case dAScanMeas: // А-развертка, точное значение амплитуды и задержки
					{ */

						cs->Enter();
				  		AScanMeasureBuff_ = new(tUMU_AScanMeasure);
                        DEFCORE_ASSERT(AScanMeasureBuff_ != NULL);

						//Modifyed by KirillB (fix)
						AScanMeasureBuff_->UMUIdx = UMUIdx;
						AScanMeasureBuff_->Stroke = SaveAScanStroke;
						AScanMeasureBuff_->Side = SaveAScanSide;
						AScanMeasureBuff_->Line = SaveAScanLine;

						AScanMeasureBuff_->ParamA = ParamA_;
						AScanMeasureBuff_->ParamM = ParamM_;

						DatSdr->CreateNewEvent(edAScanMeas);
                        DatSdr->WriteEventData(&AScanMeasureBuff_, SIZE_OF_TYPE(&AScanMeasureBuff_));
						cs->Release();
						DatSdr->Post();

/*
						break;
					}
			case dAScanData:     // А-развертка
					{ */
						cs->Enter();
						AScanDataBuff_ = new(tUMU_AScanData);
						AScanDataBuff_->UMUIdx = UMUIdx;
						AScanDataBuff_->Stroke = SaveAScanStroke;
						AScanDataBuff_->Side = SaveAScanSide;
						AScanDataBuff_->Line = SaveAScanLine;
						memcpy(&AScanDataBuff_->Data[0], &buff[0], 232);

						DatSdr->CreateNewEvent(edAScanData);
                        DatSdr->WriteEventData(&AScanDataBuff_, SIZE_OF_TYPE(&AScanDataBuff_));
						cs->Release();
						DatSdr->Post();
/*						break;
					}
			case dBScan2Data:  // БУМ - Данные В-развертки
					{                     */
                    /*
						if (BScan) break;

						UMUSide Side_;
						int Idx3 = 0;
						int Stroke_ = Buff.Data[Idx3] & 0x3F;
						int Line_;
						if (Stroke_> 12)
						{
						   Line_ =  - 1;
						}
						/ *int* / Line_ = ((Buff.Data[Idx3] & 0x40) != 0);
						Idx3++;
						BScanBuff_->EchoData.Count[usLeft][Line_][Stroke_] = (Buff.Data[Idx3]) & 0x0F;
						BScanBuff_->EchoData.Count[usRight][Line_][Stroke_] = (Buff.Data[Idx3] >> 4) & 0x0F;
						Idx3++;
						for (int Idx = 0; Idx < std::max((Buff.Data[1]) & 0x0F, (Buff.Data[1] >> 4) & 0x0F); Idx++)
						{
							BScanBuff_->EchoData.Delay[usLeft][Line_][Stroke_][Idx] = Buff.Data[Idx3];
							Idx3++;
							BScanBuff_->EchoData.Delay[usRight][Line_][Stroke_][Idx] = Buff.Data[Idx3];
							Idx3++;
							for (int Idx2 = 0; Idx2 < 24; Idx2++)
							{
								BScanBuff_->EchoData.Ampl[usLeft][Line_][Stroke_][Idx][Idx2] = Buff.Data[Idx3];
								Idx3++;
								BScanBuff_->EchoData.Ampl[usRight][Line_][Stroke_][Idx][Idx2] = Buff.Data[Idx3];
								Idx3++;
							}
						} */
/*						break;
					}
			case dAlarmData:     // БУМ - Данные АСД
					{ */

                        if (Alarm_[UMUIdx])
                        {
                            cs->Enter();
                            AlarmBuff_ = new(tUMU_AlarmItem);
                            AlarmBuff_->UMUIdx = UMUIdx;
                            AlarmBuff_->Count = StrokeCount;
                            for (int i = 0; i < StrokeCount; i++)
                            {
                                AlarmBuff_->State[0][0][i][0] = 0;
                                AlarmBuff_->State[0][0][i][1] = Alarm[0][0][i][0];
                                AlarmBuff_->State[0][0][i][2] = Alarm[0][0][i][1];
                                AlarmBuff_->State[0][0][i][3] = 0;

                                AlarmBuff_->State[0][1][i][0] = 0;
                                AlarmBuff_->State[0][1][i][1] = Alarm[0][1][i][0];
                                AlarmBuff_->State[0][1][i][2] = Alarm[0][1][i][1];
                                AlarmBuff_->State[0][1][i][3] = 0;

                                AlarmBuff_->State[1][0][i][0] = 0;
                                AlarmBuff_->State[1][0][i][1] = Alarm[1][0][i][0];
                                AlarmBuff_->State[1][0][i][2] = Alarm[1][0][i][1];
                                AlarmBuff_->State[1][0][i][3] = 0;

                                AlarmBuff_->State[1][1][i][0] = 0;
                                AlarmBuff_->State[1][1][i][1] = Alarm[1][1][i][0];
                                AlarmBuff_->State[1][1][i][2] = Alarm[1][1][i][1];
                                AlarmBuff_->State[1][1][i][3] = 0;
                            }

                            DatSdr->CreateNewEvent(edAlarmData);
                            DatSdr->WriteEventData(&AlarmBuff_, SIZE_OF_TYPE(&AlarmBuff_));
                            cs->Release();
                            DatSdr->Post();
                        }



/*						break;
					}
			case dWorkTime:     // БУМ - Время работы БУМ
					{
						WorkTime = 0;
						WorkTime = Buff.Data[0] | (Buff.Data[1] << 8) | (Buff.Data[2] << 16) | Buff.Data[3] << 24; //Buff[2] | Buff[5] << 8;
						#ifdef UMUEventLog                         //
						Log->AddTwoIntParam("UMU:%d WorkTime: %d", UMUIdx, WorkTime);
						#endif
						break;
					}
	}                 */
}



//---------------------------------------------------------------------------
//-----  ИСПРАВЛЕННО --------------------------------------------------------
//---------------------------------------------------------------------------

    const unsigned char TableDB[256]=
    {
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x11,0x22,0x22,0x22,
     0x33,0x33,0x33,0x33,0x44,0x44,0x44,0x44,0x44,0x44,0x55,0x55,0x55,0x55,0x55,0x55,
     0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x77,0x77,0x77,0x77,0x77,0x77,0x77,
     0x77,0x77,0x77,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
     0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
     0x99,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
     0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
     0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
     0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
     0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
     0xCC,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,
     0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,
     0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,
     0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,
     0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,
     0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xFF
    };

bool UMU_3204::Tick(void)
{
    // Моделирование сигналов
    SimTime = GetTickCount_();
	for (int s = 0; s < 2; s++)
        for (int l = 0; l < 2; l++)
            for (int t = 0; t < 12; t++)
                for (int i = 0; i < 8; i++)
                {
                    if (SignalData[s][l][t][i].State != 0)
                    {
                        if (GetTickCount_() - SignalData[s][l][t][i].CreateTime < SignalData[s][l][t][i].LifeTime)
                        {
                            SignalData[s][l][t][i].Delay = SignalData[s][l][t][i].Delay + SignalData[s][l][t][i].DelayStep;
                            if (SignalData[s][l][t][i].Ampl >= 0.1)
                            {
                                SignalData[s][l][t][i].Ampl = CLAMP(SignalData[s][l][t][i].Ampl + SignalData[s][l][t][i].AmplStep, 0.f, 255.f);
                            }
                        }
                        else SignalData[s][l][t][i].State = 0;
                    }
                    else
                    if (SignalData[s][l][t][i].State == 0)
                    {
                        if (RandomRange(1, 100) < 50)
                        {
                            SignalData[s][l][t][i].State = 1;
                        }
                        else
                        {
                            SignalData[s][l][t][i].State = 2;
                        };
                        SignalData[s][l][t][i].CreateTime = GetTickCount_();
                        SignalData[s][l][t][i].LifeTime = (int)RandomRange(1500, 5000);
                        SignalData[s][l][t][i].StartDelay = (int)RandomRange(0, StrokeParams[SaveAScanStroke].Duration);
                        SignalData[s][l][t][i].DelayStep = ((float)RandomRange(1, 20) - (float)10) / (float)10;
                        SignalData[s][l][t][i].StartAmpl = (int)RandomRange(1, 255);
                        SignalData[s][l][t][i].AmplStep = (int)RandomRange(1, 10) - 5;
                        SignalData[s][l][t][i].Delay = SignalData[s][l][t][i].StartDelay;
                        SignalData[s][l][t][i].Ampl = CLAMP(SignalData[s][l][t][i].StartAmpl, 0.f, 255.f);
                    }
                }

	// Расчет данных А-разверток
	//if(AScan)//Added by KirillB
	{
		for (int s = 0; s < 2; s++)
			for (int l = 0; l < 2; l++)
				for (int t = 0; t < 12; t++)
				{
					for (int i = 0; i < 256; i++) AScanSrcData[s][l][t][i] = 0;

					for (int i = 0; i < 8; i++)
						if (SignalData[s][l][t][i].State == 2)
						{
							int Delay_ = SignalData[s][l][t][i].Delay + PrismDelay[s][l][t] / 10;
							int Start_ = std::max(  0, (int)(Delay_ - 1));
							int End_ =   std::min(255, (int)(Delay_ + 2));
							for (int Index = Start_; Index < End_; Index++)
							{
								if ((Index >= 0) &&  (Index <= 255))
								{
  								    float koeff = (float)1 - (float)2 * (float)(Index - Start_) / (float)(End_ - Start_);
									if (koeff < 0) {
										koeff = koeff * - 1;
									}
                                    AScanSrcData[s][l][t][Index] = SignalData[s][l][t][i].Ampl * TVGCurve[s][l][t][Index] /*TVGData.Data[Index]*/ / 128 * (1 - koeff);
								}
							}
						}
				}
	}

	// Расчет данных B-разверток

	memset(&BScanSrcData, 0, sizeof(tUMU_BScanSignals));

	//if(BScan)
	{
		for (int s = 0; s < 2; s++)
			for (int l = 0; l < 2; l++)
				for (int t = 0; t < 12; t++)
				{
					int sglcnt = 0;
					for (int i = 0; i < 8; i++)
						if (SignalData[s][l][t][i].State == 2)

							if ((SignalData[s][l][t][i].Delay >= GateStDelay[s][l][t][0]) &&
								(SignalData[s][l][t][i].Delay <= GateEdDelay[s][l][t][0]))
							{
								BScanSrcData.Data[s][l][t][sglcnt].Delay = SignalData[s][l][t][i].Delay;

								int d = (int)SignalData[s][l][t][i].Delay;
								if (d < 0) d = 0;
								if (d > 255) d = 255;
								// int t1 = AScanSrcData[s][l][t][d];

							 /*	for (int tmp = 0; tmp < 24; tmp++)
								{
									int t1 = AScanSrcData[s][l][t][d + tmp / 3 - 2];
									BScanSrcData.Data[s][l][t][sglcnt].Ampl[tmp] = t1; // SignalData[s][l][t][i].Ampl;
								} */


                                #ifndef BScanSize_12Byte
								for (int tmp = 0; tmp < 12; tmp++)
									BScanSrcData.Data[s][l][t][sglcnt].Ampl[tmp] = tmp * SignalData[s][l][t][i].Ampl / 12;
								for (int tmp = 12; tmp < 24; tmp++)
									BScanSrcData.Data[s][l][t][sglcnt].Ampl[tmp] = SignalData[s][l][t][i].Ampl - ((tmp - 12) * SignalData[s][l][t][i].Ampl / 12);
                               #endif

                                #ifdef BScanSize_12Byte

                                int iii = 0;
								for (int tmp = 0; tmp < 12; tmp++)
									if (tmp&1)                                                                                                       /* bug - value greater than 256! */
                                    {                                                                                                                /*  vvv        vvv       vvv     */
                                        BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] = BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] | ((TableDB[(int)(tmp * SignalData[s][l][t][i].Ampl / 12)] & 0x0F) << 4);
                                        iii++;                                                               /* bug - same */
                                    } else BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] = TableDB[(int)(tmp * SignalData[s][l][t][i].Ampl / 12)] & 0x0F;

								for (int tmp = 12; tmp < 24; tmp++)
									if (tmp&1)
                                    {
                                        BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] = BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] | ((TableDB[(int)(SignalData[s][l][t][i].Ampl - ((tmp - 12) * SignalData[s][l][t][i].Ampl / 12))] & 0x0F) << 4);
                                        iii++;                                                                   /* bug - same */
                                    } else BScanSrcData.Data[s][l][t][sglcnt].Ampl[iii] = TableDB[(int)(SignalData[s][l][t][i].Ampl - ((tmp - 12) * SignalData[s][l][t][i].Ampl / 12))] & 0x0F;

                               #endif

								sglcnt++;
							}
					BScanSrcData.Count[s][l][t] = sglcnt;
				}
	}

	// АСД

	for (int s = 0; s < 2; s++)
		for (int l = 0; l < 2; l++)
            for (int t = 0; t < 12; t++)
                for (int g = 0; g < 2; g++)
                {
                    Alarm[s][l][t][g] = false;
					for (int i = GateStDelay[s][l][t][g]; i <= GateEdDelay[s][l][t][g]; i++)
                        {
                            if (AScanSrcData[s][l][t][i] > GateLevel[s][l][t][g])
                            {
                                Alarm[s][l][t][g] = true;
                            }
                        }
                }


	// Заказанная А-развертка

      // Данные А-развертки




    if(AScan[UMUIdx])//Added by KirillB
	{

		for (int i = 0; i < 232; i++) buff[i] = 0;
		for (int i = 0; i < 232; i++)
			buff[i] = AScanSrcData[SaveAScanSide][SaveAScanLine][SaveAScanStroke][(AScanZoom * i) / 10];

		  // Максимумы
		ParamA_ = 0;
		ParamM_ = 0;
		if (SaveAScanZoom != 0)
			for (int i = GateStDelay[SaveAScanSide][SaveAScanLine][SaveAScanStroke][1] * 10 / SaveAScanZoom;
					i <= GateEdDelay[SaveAScanSide][SaveAScanLine][SaveAScanStroke][1] * 10 / SaveAScanZoom; i++)
				{
                    if(i >= 232)
                        continue;
					if (buff[i] > ParamA_)
					{
						ParamA_ = buff[i];
						ParamM_ = i * AScanZoom / 10;
					}
				}

	}
    tLAN_Message Buff;
    UnPack(Buff);
    SLEEP(50);

	return !EndWorkTest();
}

void UMU_3204::SetStrokeCount(unsigned char Count)  //
{
	StrokeCount = Count;
}
//---------------------------------------------------------------------------
void UMU_3204::SetGainSegment(UMUSide Side, unsigned char Line, unsigned char Stroke, unsigned char StDelay, unsigned char StVal, unsigned char EdDelay, unsigned char EdVal, unsigned char ascan_scale)      //
{
	GainSegmentStDelay[Side][Line][Stroke] = StDelay;
	GainSegmentEdDelay[Side][Line][Stroke] = EdDelay;
	GainSegmentStVal[Side][Line][Stroke] = StVal;
	GainSegmentEdVal[Side][Line][Stroke] = EdVal;

	if (StDelay != EdDelay)
	{
		for (int Delay = StDelay; Delay <= EdDelay; Delay++)
            TVGCurve[Side][Line][Stroke][Delay] = char((float)StVal + (float)((float)EdVal - (float)StVal) * (float)((float)Delay - (float)StDelay) / (float)((float)EdDelay - (float)StDelay));
	}
	else TVGCurve[Side][Line][Stroke][StDelay] = StVal;

	if ((SaveAScanZoom != 0) && (Stroke == SaveAScanStroke) && (Side == SaveAScanSide) && (Line == SaveAScanLine))
	{
		CalcTVGCurve();
		TVGDataBuff_ = new(tUMU_AScanData);
		memcpy(TVGDataBuff_, &TVGData, sizeof(tUMU_AScanData));
		TVGDataBuff_->UMUIdx = UMUIdx;
		cs->Enter();
		DatSdr->CreateNewEvent(edTVGData);
        DatSdr->WriteEventData(&TVGDataBuff_, SIZE_OF_TYPE(&TVGDataBuff_));
		cs->Release();
		DatSdr->Post();
	}
}
//---------------------------------------------------------------------------
void UMU_3204::SetGate(UMUSide Side, unsigned char Line, unsigned char Stroke, unsigned char Idx, unsigned char StDelay, unsigned char EdDelay, unsigned char Level, StrobeMode Mode, eAlarmAlgorithm ASDmode)
{
	GateStDelay[Side][Line][Stroke][Idx] = StDelay;
	GateEdDelay[Side][Line][Stroke][Idx] = EdDelay;
	GateLevel[Side][Line][Stroke][Idx] = Level;
	GateMode[Side][Line][Stroke][Idx] = Mode;
	GateAlgorithm[Side][Line][Stroke][Idx] = ASDmode;
}

//---------------------------------------------------------------------------
void UMU_3204::SetPrismDelay(UMUSide Side, unsigned char Line, unsigned char Stroke, unsigned short Delay) //
{
	PrismDelay[Side][Line][Stroke] = Delay;
}

//---------------------------------------------------------------------------
void UMU_3204::EnableAScan(UMUSide Side, unsigned char Line, unsigned char Stroke, unsigned short StDelay, unsigned char Zoom, unsigned char Mode, unsigned char Strob) //   -
{
	AScan[UMUIdx] = true;

	AScanStDelay = StDelay;
	AScanZoom = Zoom;
	AScanMode = Mode;
	AScanStrob = Strob;

   	SaveAScanZoom = Zoom;
	SaveAScanSide = Side;
	SaveAScanStroke = Stroke;
	SaveAScanLine = Line;

	CalcTVGCurve();
	TVGDataBuff_ = new(tUMU_AScanData);
	TVGDataBuff_->UMUIdx = UMUIdx;
	memcpy(TVGDataBuff_, &TVGData, sizeof(tUMU_AScanData));
	cs->Enter();
	DatSdr->CreateNewEvent(edTVGData);
    DatSdr->WriteEventData(&TVGDataBuff_, SIZE_OF_TYPE(&TVGDataBuff_));
	cs->Release();
	DatSdr->Post();
}

//---------------------------------------------------------------------------
void UMU_3204::DisableAScan(void)  //   -
{
	AScan[UMUIdx] = false;
}

//---------------------------------------------------------------------------
void UMU_3204::EnableBScan(void)  //   -
{
	BScan[UMUIdx] = true;
}

//---------------------------------------------------------------------------
void UMU_3204::DisableBScan(void)  //   B-
{
	BScan[UMUIdx] = false;
}

//---------------------------------------------------------------------------
void UMU_3204::EnableAlarm(void)   //
{
	Alarm_[UMUIdx] = true;
}
//---------------------------------------------------------------------------
void UMU_3204::DisableAlarm(void)
{
	Alarm_[UMUIdx] = false;
}

//---------------------------------------------------------------------------
void UMU_3204::PathSimulator(unsigned char Number, bool State) //  /
{
	PathSimulator_[Number] = State;
}

//---------------------------------------------------------------------------
void UMU_3204::SetStrokeParameters(tUMU_3204_StrokeParams strokeParams)
{
	StrokeParams[strokeParams.StrokeIdx] = strokeParams;
}

//---------------------------------------------------------------------------
void UMU_3204::SetPathEncoderData(char PathEncoderIdx, int NewValue) // Установка значения датчика пути
{
   PathEncoderData[UMUIdx][static_cast<unsigned char>(PathEncoderIdx)] = NewValue;
}

//---------------------------------------------------------------------------
int UMU_3204::GetUAKK(void)
{
	return 12;
}

//---------------------------------------------------------------------------
//----- НЕ ТРЕБУЕТ ИСПРАВЛЕНИЯ ----------------------------------------------
//---------------------------------------------------------------------------

unsigned int UMU_3204::GetStrokeCount(void)       //
{
	return StrokeCount;
}

//---------------------------------------------------------------------------

void UMU_3204::SetAScanScale(unsigned char Zoom)
{
	SaveAScanZoom = Zoom;
}

//---------------------------------------------------------------------------

void UMU_3204::CalcTVGCurve(void) //
{
	if (SaveAScanZoom != 0)
	{
		int Delay = 0;
		int DelayIdx;
		int DelayIdx_ = 0;

		TVGData.Side = SaveAScanSide;
		TVGData.Line = SaveAScanLine;
		TVGData.Stroke = SaveAScanStroke;

		while (true)
		{
			DelayIdx = (int)((10 * Delay) / SaveAScanZoom);
			for (int Idx = DelayIdx_; Idx <= DelayIdx; Idx++)
			{
				if (Idx < 232) TVGData.Data[Idx] = TVGCurve[SaveAScanSide][SaveAScanLine][SaveAScanStroke][Delay];
			}
			if (DelayIdx > 232) break;
			DelayIdx_ = DelayIdx;
			Delay++;
		}
	}
}

//---------------------------------------------------------------------------

void UMU_3204::RequestTVGCurve(void)
{
    CalcTVGCurve();
    TVGDataBuff_ = new(tUMU_AScanData);
    TVGDataBuff_->UMUIdx = UMUIdx;
    memcpy(TVGDataBuff_, &TVGData, sizeof(tUMU_AScanData));
    cs->Enter();
    DatSdr->CreateNewEvent(edTVGData);
    DatSdr->WriteEventData(&TVGDataBuff_, SIZE_OF_TYPE(&TVGDataBuff_));
    cs->Release();
    DatSdr->Post();
}

//---------------------------------------------------------------------------
void UMU_3204::GetDeviceID(void)
{
    FirmwareInfoTime = GetTickCount_();
    SerialNumberTime = GetTickCount_();
}

//---------------------------------------------------------------------------
//-----     НЕ ИСПРАВЛЕННО     ----------------------------------------------
//---------------------------------------------------------------------------

void UMU_3204::Enable(void) //
{
}

//---------------------------------------------------------------------------
void UMU_3204::Disable(void) //
{
}

//---------------------------------------------------------------------------
void UMU_3204::Reboot(void) //
{
}

//---------------------------------------------------------------------------
void UMU_3204::GetWorkTime(void)
{
}

//---------------------------------------------------------------------------
void UMU_3204::Stop(void)
{
}

//---------------------------------------------------------------------------

unsigned int UMU_3204::GetDownloadSpeed()
{
    return 0;
}

unsigned int UMU_3204::GetUploadSpeed()
{
    return 0;
}

#endif
