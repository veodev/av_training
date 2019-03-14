//---------------------------------------------------------------------------
#ifndef EventManager_WinH

#define EventManager_WinH

#include "EventManager.h"
#include "windows.h"
#include <queue>

// !!! Äîëæåí áûòü âûáðàí îäèí âàðèàíò !!!
// #define Use_Pipe // Íå ðàáîòàåò ñ*êà :(
// #define Use_Queue
#define Use_Array

//---------------------------------------------------------------------------

class cEventManagerWin: public cEventManager
{
private:

    HANDLE hDataEvent;
    #ifdef Use_Pipe
    HANDLE hReadData;
    HANDLE hWriteData;
    #endif

    #ifdef Use_Queue
    std::queue <unsigned long> DataQueue;
    #endif

    #ifdef Use_Array

    int SetIdx;
    int GetIdx;
    unsigned long Data[65536];

    int GetIdx_save;
    #endif

public:

    cEventManagerWin(void);
    ~cEventManagerWin(void);
    bool CreateNewEvent(unsigned long Id);
    bool WriteEventData(void* Ptr, unsigned int Size);
    void Post(void);
    bool WaitForEvent(void);
    int EventDataSize(void);
    bool ReadEventData(void* Ptr, unsigned int Size, unsigned long* Read);
    bool PeekEventData(void* Ptr, unsigned int Size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage);
    void WriteHeadBodyAndPost(unsigned long Id, void* headptr, unsigned int headsize, void* bodyptr, unsigned int bodysize);
    void WriteHeadAndPost(unsigned long Id, void* headptr, unsigned int headsize);
    void SaveIndex(void);
    void RestoreIndex(void);
};

#endif
