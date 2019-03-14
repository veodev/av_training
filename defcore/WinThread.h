//---------------------------------------------------------------------------
//2
#ifndef WinThreadH

#define WinThreadH

#include "Windows.h"
#include "Classes.hpp"
#include "ThreadClassList.h"

class cWinThread : public TThread
{

  void __fastcall Execute(void);

public:

  int Tag;
  TickProcPtr Ptr;
  __fastcall cWinThread(TickProcPtr Tick);
  __fastcall ~cWinThread(void);

};


#endif

