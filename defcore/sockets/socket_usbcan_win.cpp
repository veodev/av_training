#include "socket_usbcan_win.h"

extern "C"
{
//#include "av15CE4.h"
#include "av11_21.h"
}

cSocketUSBCanWin::cSocketUSBCanWin() :cISocket()
{
	_fInitCompleted = false;
	_hdll = LoadLibrary(TEXT("av11.DLL"));
	if (_hdll)
	{
		pInit = (ifuncv) GetProcAddress(_hdll,"initlib");
		pFinish = (vfuncv) GetProcAddress(_hdll,"finishlib");
		pRead = (ReadfuncPtr) GetProcAddress(_hdll,"ReadFromUSB");
		pWrite = (WritefuncPtr) GetProcAddress(_hdll,"WriteToUSB");
		pPowerSwitch = (PowerSwitchPtr) GetProcAddress(_hdll,"pwrswitch");
	}
	if ((pInit) && (pFinish) && (pRead) && (pWrite) && (pPowerSwitch))
	{
		_fInitCompleted = true;
	}
}

bool cSocketUSBCanWin::connect(const cConnectionParams *socket_params = 0)
{
const int attempts = 20;
	if (_fInitCompleted == false)
	{
		return false;
	}
	for (int ii=0; ii<attempts; ++ii)
	{
	 int res;
		res = pInit();
		if ( (res & 1) && (pPowerSwitch(1) == 1) )
		{
			_socket = 0;
			_state = true;
			pInit();
			return true;
		}
			else
			{
				SleepEx(CONNECT_POLLING_PERIOD,true);
			}
	}
	return false;
}
//
void cSocketUSBCanWin::disconnect()
{
	if (_fInitCompleted == false)
	{
		return;
	}
	if (_socket >= 0)
	{
		pFinish();   // после ее выполнения при новом вызове connect()
// необходимо будет вызывать pPowerSwitch(1), чтобы в БУМ15 разрешить
// передачу данных к ПК
//
		_state = false;
		_socket = -1;
	}
}
//
int  cSocketUSBCanWin::reciveData(unsigned char *msg, const int length)
{
	if (_fInitCompleted == false)
	{
		return 0;
	}
     return (int) pRead(msg,(unsigned char)length,0);
}
//
bool cSocketUSBCanWin::sendData(const unsigned char *msg, const int length)
{
	if (_fInitCompleted == false)
	{
		return length;
	}
	return (bool)pWrite((unsigned char*)msg,(unsigned short)length);
}

