//---------------------------------------------------------------------------


// # pragma hdrstop

#include "WinThread.h"

__fastcall cWinThread::cWinThread(TickProcPtr Tick) : TThread(true)
{
    Ptr = Tick;
//    FreeOnTerminate = true;
}

__fastcall cWinThread::~cWinThread(void)
{
    //
}

void __fastcall cWinThread::Execute(void)
{
     while (true)
     {
          if (!Ptr()) break;
     };
}

