//---------------------------------------------------------------------------


#pragma hdrstop

#include "EventManager_Win.h"

//---------------------------------------------------------------------------

//#pragma package(smart_init)



cEventManagerWin::cEventManagerWin(void)
{
    hDataEvent = CreateEvent(   NULL,               // default security attributes
								FALSE,               // auto-reset event
                                FALSE,              // initial state is nonsignaled
                                TEXT("UMUAScan")    // object name
                                );

    #ifdef Use_Pipe
    SECURITY_ATTRIBUTES S_ATTR = {sizeof(S_ATTR)};
    S_ATTR.bInheritHandle = TRUE; //хэндлы наследуемые
    S_ATTR.lpSecurityDescriptor = NULL;
    if(!CreatePipe(&hReadData, &hWriteData, &S_ATTR, 0))
    {
      // Error
    }
    #endif

    #ifdef Use_Array
    SetIdx = 0;
    GetIdx = 0;
    #endif
}

cEventManagerWin::~cEventManagerWin(void)
{
    hDataEvent = NULL;
    #ifdef Use_Pipe
    hReadData = NULL;
    hWriteData = NULL;
    #endif
}

bool cEventManagerWin::CreateNewEvent(unsigned long Id)
{
    DWORD dwWritten;
    #ifdef Use_Pipe
    WriteFile(hWriteData, &Id, 4, &dwWritten, NULL);
    return (dwWritten == 4);
    #endif

    #ifdef Use_Queue
    DataQueue.push(Id);
    #endif

    #ifdef Use_Array
    Data[SetIdx] = Id;
    SetIdx++;
    if (SetIdx == 65536) SetIdx = 0;
    #endif
    return true;
}

bool cEventManagerWin::WriteEventData(void* Ptr,unsigned int Size)
{
    #ifdef Use_Pipe
    DWORD dwWritten;
    WriteFile(hWriteData, Ptr, Size, &dwWritten, NULL);
    return (dwWritten == Size);
    #endif

    #ifdef Use_Queue
    while (Size != 0)
    {
        DataQueue.push(*((unsigned long*)Ptr));
        Size = Size - 4;
    }
    return (Size == 0);
    #endif

    #ifdef Use_Array
    while (Size != 0)
    {
        Data[SetIdx] = *((unsigned long*)Ptr);
        SetIdx++;
        if (SetIdx == 65536) SetIdx = 0;
        Size = Size - 4;
    }
    return (Size == 0);
    #endif
}

void cEventManagerWin::Post(void)
{
    SetEvent(hDataEvent);
}

bool cEventManagerWin::WaitForEvent(void)
{
    DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(hDataEvent, 50 /* INFINITE */ );
    switch(dwWaitResult)
    {
        case WAIT_OBJECT_0: return TRUE;
                   default: return FALSE;
    }
}

bool cEventManagerWin::ReadEventData(void* Ptr, unsigned int Size, unsigned long* Read)
{
    #ifdef Use_Pipe
    DWORD Read_;
    return ReadFile(hReadData, Ptr, Size, &Read_, NULL);
    #endif

	#ifdef Use_Queue
    if (DataQueue.empty()) return false;

    while ((Size != 0) && (!DataQueue.empty()))
    {
        unsigned long tmp;
        tmp = DataQueue.front();
        memcpy(Ptr, &tmp, 4);
        *((unsigned long*)(Ptr)) = tmp;
        DataQueue.pop();
        Size = Size - 4;
    }
    return (Size == 0);
    #endif

    #ifdef Use_Array
    if (SetIdx == GetIdx) return false;

    while ((Size != 0) && (GetIdx != SetIdx))
    {
        unsigned long tmp;
        tmp = Data[GetIdx];
        GetIdx++;
        if (GetIdx == 65536) GetIdx = 0;
        memcpy(Ptr, &tmp, 4);
        Size = Size - 4;
    }
    return (Size == 0);
    #endif

}

int cEventManagerWin::EventDataSize(void)
{

    #ifdef Use_Pipe
    char Buff;
    DWORD lpBytesRead;
    DWORD lpTotalBytesAvail;
    DWORD lpBytesLeftThisMessage;
    DWORD dwRead, dwWritten;
    PeekNamedPipe(hReadData, &Buff, 1, &lpBytesRead, &lpTotalBytesAvail, &lpBytesLeftThisMessage);
    return lpTotalBytesAvail;
    #endif
    #ifdef Use_Queue
	return DataQueue.size() * 4;
    #endif
    #ifdef Use_Array
    if (GetIdx == SetIdx) return 0;
    if (GetIdx < SetIdx) return abs(SetIdx - GetIdx + 1) * 4;
    if (GetIdx > SetIdx) return abs(65536 - GetIdx + SetIdx - 1) * 4;
    #endif
    return 0;
}

bool cEventManagerWin::PeekEventData(void* Ptr, unsigned int Size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage)
{
//    DWORD lpBytesRead;
//    DWORD lpTotalBytesAvail;
//    DWORD lpBytesLeftThisMessage;
//    DWORD dwRead, dwWritten;


    #ifdef Use_Pipe
    return PeekNamedPipe(hReadData, Ptr, Size, lpBytesRead, lpTotalBytesAvail, lpBytesLeftThisMessage);
    #endif
    #ifdef Use_Pipe
    #endif
    #ifdef Use_Array
    #endif
    return 0;
}

void cEventManagerWin::WriteHeadBodyAndPost(unsigned long Id, void *headptr, unsigned int headsize, void *bodyptr, unsigned int bodysize)
{
    CreateNewEvent(Id);
    WriteEventData(headptr, headsize);
    WriteEventData(bodyptr, bodysize);
    Post();
}

void cEventManagerWin::WriteHeadAndPost(unsigned long Id, void *headptr, unsigned int headsize)
{
    WriteEventData(&Id, sizeof(Id));
    WriteEventData(headptr, headsize);
    Post();
}

void cEventManagerWin::SaveIndex(void)
{
   GetIdx_save = GetIdx;
}

void cEventManagerWin::RestoreIndex(void)
{
   GetIdx = GetIdx_save;
}



                /*
                const BUFSIZE = 1;
                CHAR chReadBuf[BUFSIZE];
                DWORD lpBytesRead;
                DWORD lpTotalBytesAvail;
                DWORD lpBytesLeftThisMessage;
                DWORD dwRead, dwWritten;

                PeekNamedPipe(hRead, chReadBuf, BUFSIZE, &lpBytesRead,
                    &lpTotalBytesAvail, &lpBytesLeftThisMessage);
                if (lpTotalBytesAvail == 0)
                    break;
                bool bSuccess = ReadFile(hRead, &DataID, 4, &dwRead, NULL);
                if (!bSuccess || dwRead == 0)
                    break;

                switch(DataID) {
                case 1: // AScan
                    {
                        PeekNamedPipe(hRead, chReadBuf, BUFSIZE, &lpBytesRead,
                            &lpTotalBytesAvail, &lpBytesLeftThisMessage);
                        if (lpTotalBytesAvail == 0) break;
                        bSuccess = ReadFile(hRead, &AScanData_ptr, 4, &dwRead, NULL);
                        if (!bSuccess || dwRead == 0) break;
                        bSuccess = ReadFile(hRead, &TVGData_ptr, 4, &dwRead, NULL);
                        if (!bSuccess || dwRead == 0) break;
                        Synchronize(ValueToScreen);
                        delete AScanData_ptr;
                        AScanData_ptr = NULL;
                        delete TVGData_ptr;
                        TVGData_ptr = NULL;
                        break;
                    }
                case 2: // BScan
*/
