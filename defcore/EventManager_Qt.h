#ifndef EVENTMANAGER_QT_H
#define EVENTMANAGER_QT_H

#include <deque>
#include <pthread.h>

#include "EventManager.h"

class cEventManager_Qt : public cEventManager
{
public:
    cEventManager_Qt();
    ~cEventManager_Qt();

    bool CreateNewEvent(unsigned long id);
    bool WriteEventData(void* ptr, unsigned int size);
    void Post(void);
    bool WaitForEvent(void);
    int EventDataSize(void);
    bool ReadEventData(void* ptr, unsigned int size, unsigned long* read);
    bool PeekEventData(void* ptr, unsigned int size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage);
    void WriteHeadBodyAndPost(unsigned long Id, void* headptr, unsigned int headsize, void* bodyptr, unsigned int bodysize);
    void WriteHeadAndPost(unsigned long Id, void* headptr, unsigned int headsize);

private:
    pthread_mutex_t _eventMutex;
    pthread_cond_t _eventCond;
    pthread_mutex_t _dataMutex;
    std::deque<unsigned char> _queue;
};

#endif  // EVENTMANAGER_QT_H
