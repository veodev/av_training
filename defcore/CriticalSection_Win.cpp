
#include "CriticalSection_Win.h"


cCriticalSectionWin::cCriticalSectionWin(void)
{
   	cs = new TCriticalSection();
}


cCriticalSectionWin::~cCriticalSectionWin(void)
{
	delete cs;
	cs = NULL;
}

void cCriticalSectionWin::Enter(void)
{
	cs->Enter();
}

void cCriticalSectionWin::Release(void)
{
	cs->Release();  
}
