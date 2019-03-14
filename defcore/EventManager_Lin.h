#ifndef EVENTMANAGER_LIN_H
#define EVENTMANAGER_LIN_H

#include "platforms.h"

#include <pthread.h>
#include <deque>

#if defined(DEFCORE_CC_GNU)
#  include <tr1/memory>
using namespace std;
using namespace std::tr1;
#else
#  include <memory>>
using namespace std;
#endif

#include "EventManager.h"

class cEventManager_Lin : public cEventManager
{
public:
    cEventManager_Lin();
    ~cEventManager_Lin();

    bool CreateNewEvent(unsigned long id);
    bool WriteEventData(void* ptr, int size);
    void Post(void);
    bool WaitForEvent(void);
    int EventDataSize(void);
    bool ReadEventData(void* ptr, int size, unsigned long* read);
    bool PeekEventData(void* ptr, int size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage);

private:
    pthread_mutex_t _eventMutex;
    pthread_cond_t  _eventCond;

    pthread_mutex_t      _dataMutex;
    struct Data {
        Data(void *ptr, int size);
        explicit Data(const Data & obj);
        Data & operator=(const Data & obj);
        ~Data();

        std::tr1::shared_ptr<char>_ptr;
        int   _size;
        int   _bytesRead;
    };
    std::deque<Data> _data;
    int _dataSize;
};

#endif // EVENTMANAGER_LIN_H
