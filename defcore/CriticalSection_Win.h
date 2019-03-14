//---------------------------------------------------------------------------

#ifndef cCriticalSection_WinH

#define cCriticalSection_WinH

#include "CriticalSection.h"
#include <AppEvnts.hpp>

//---------------------------------------------------------------------------

class cCriticalSectionWin: public cCriticalSection
{

private:

    TCriticalSection * cs;

public:

    cCriticalSectionWin(void);
    ~cCriticalSectionWin(void);
    virtual void Enter(void);
    virtual void Release(void);

};

#endif
