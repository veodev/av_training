#include <unistd.h>
#include <string.h>
#include <time.h>

#include <iostream>
using namespace std;

#include "EventManager_Lin.h"

cEventManager_Lin::cEventManager_Lin()
    : _dataSize(0)
{
    pthread_mutex_init(&_eventMutex, NULL);
    pthread_cond_init(&_eventCond, NULL);
    pthread_mutex_init(&_dataMutex, NULL);
}

cEventManager_Lin::~cEventManager_Lin()
{
    pthread_mutex_destroy(&_eventMutex);
    pthread_cond_destroy(&_eventCond);
    pthread_mutex_destroy(&_dataMutex);
}

bool cEventManager_Lin::CreateNewEvent(unsigned long id)
{
    WriteEventData(&id, sizeof(id));

    return true;
}

bool cEventManager_Lin::WriteEventData(void *ptr, int size)
{
    DEFCORE_ASSERT(ptr != NULL);
    DEFCORE_ASSERT(size > 0);

    pthread_mutex_lock(&_dataMutex);
    _data.push_back(Data(ptr, size));
    _dataSize += size;
    pthread_mutex_unlock(&_dataMutex);

    return true;
}

void cEventManager_Lin::Post()
{
    pthread_mutex_lock(&_eventMutex);
    pthread_cond_broadcast(&_eventCond);
    pthread_mutex_unlock(&_eventMutex);
}

bool cEventManager_Lin::WaitForEvent()
{
    bool rc = false;

    pthread_mutex_lock(&_eventMutex);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    if (pthread_cond_timedwait(&_eventCond, &_eventMutex, &ts) == 0) {
        rc = true;
    }
    pthread_mutex_unlock(&_eventMutex);

    return rc;
}

int cEventManager_Lin::EventDataSize()
{
    return _dataSize;
}

bool cEventManager_Lin::ReadEventData(void *ptr, int size, unsigned long *read)
{
    if (ptr == NULL) {
        cout << endl;
    }
    DEFCORE_ASSERT(ptr != NULL);
    DEFCORE_ASSERT(size >= 0);

    if (read != NULL) {
        *read = 0;
    }
    if (size == 0) {
        return false;
    }

    int bytesRead = 0;
    char * iter = reinterpret_cast<char *>(ptr);
    if (_dataSize > 0) {
        pthread_mutex_lock(&_dataMutex);
        while (size > 0 && _dataSize > 0) {
            std::deque<Data>::iterator it = _data.begin();
            if (size >= it->_size) {
                ::memcpy(iter, it->_ptr.get() + it->_bytesRead, it->_size);
                iter       += it->_size;
                _dataSize  -= it->_size;
                bytesRead  += it->_size;
                size       -= it->_size;
                _data.pop_front();
            }
            else {
                ::memcpy(iter, it->_ptr.get() + it->_bytesRead, size);
                iter           += size;
                _dataSize      -= size;
                it->_bytesRead += size;
                it->_size      -= size;
                bytesRead      += size;
                size           =  0;
            }
        }
        pthread_mutex_unlock(&_dataMutex);
        if (read != NULL) {
            *read = bytesRead;
        }
        return true;
    }

    return false;
}

bool cEventManager_Lin::PeekEventData(
    void *ptr,
    int size,
    unsigned long *lpBytesRead,
    unsigned long *lpTotalBytesAvail,
    unsigned long *lpBytesLeftThisMessage)
{
    DEFCORE_ASSERT(ptr != NULL);
    DEFCORE_ASSERT(size >= 0);

    if (lpBytesRead != NULL) {
        *lpBytesRead = 0;
    }
    if (lpTotalBytesAvail) {
        *lpTotalBytesAvail = _dataSize;
    }
    if (lpBytesLeftThisMessage) {
        *lpBytesLeftThisMessage = 0;
    }
    if (size == 0) {
        return false;
    }

    int bytesReadTotal = 0;
    char * iter = reinterpret_cast<char *>(ptr);
    int dataSize = _dataSize;
    if (dataSize > 0) {
        pthread_mutex_lock(&_dataMutex);
        std::deque<Data>::iterator it = _data.begin();
        while (size > 0 && dataSize > 0 && it != _data.end()) {
            int itBytesRead = it->_bytesRead;
            int itSize      = it->_size;
            if (size >= it->_size) {
                ::memcpy(iter, it->_ptr.get() + itBytesRead, it->_size);
                iter            += it->_size;
                dataSize        -= it->_size;
                bytesReadTotal  += it->_size;
                size            -= it->_size;
                ++it;
            }
            else {
                ::memcpy(iter, it->_ptr.get() + itBytesRead, size);
                iter           += size;
                dataSize       -= size;
                itBytesRead    += size;
                itSize         -= size;
                bytesReadTotal += size;
                size           =  0;
            }
        }
        pthread_mutex_unlock(&_dataMutex);
        if (lpBytesRead != NULL) {
            *lpBytesRead = bytesReadTotal;
        }
        if (lpBytesLeftThisMessage) {
            *lpBytesLeftThisMessage = _dataSize - bytesReadTotal;
        }
        return true;
    }

    return false;
}

cEventManager_Lin::Data::Data(void *ptr, int size)
    : _ptr(new char[size])
    , _size(size)
    , _bytesRead(0)
{
    ::memcpy(_ptr.get(), ptr, size);
}

cEventManager_Lin::Data::Data(const cEventManager_Lin::Data &obj)
{
    *this = obj;
}

cEventManager_Lin::Data &cEventManager_Lin::Data::operator=(const cEventManager_Lin::Data &obj)
{
    _ptr = obj._ptr;
    _size = obj._size;
    _bytesRead = obj._bytesRead;

    return *this;
}

cEventManager_Lin::Data::~Data()
{
    _size = 0;
    _bytesRead = 0;
}
