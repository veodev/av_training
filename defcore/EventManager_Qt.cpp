#include <ctime>
#include <QDebug>
#include "EventManager_Qt.h"

cEventManager_Qt::cEventManager_Qt()
{
    pthread_mutex_init(&_eventMutex, nullptr);
    pthread_cond_init(&_eventCond, nullptr);
    pthread_mutex_init(&_dataMutex, nullptr);
}

cEventManager_Qt::~cEventManager_Qt()
{
    qDebug() << "Deleting EventManager_Qt...";
}

bool cEventManager_Qt::CreateNewEvent(unsigned long id)
{
    return WriteEventData(&id, sizeof(id));
}

bool cEventManager_Qt::WriteEventData(void* ptr, unsigned int size)
{
    Q_ASSERT(ptr != nullptr);
    pthread_mutex_lock(&_dataMutex);
    unsigned char* iter = reinterpret_cast<unsigned char*>(ptr);
    std::copy_n(&iter[0], size, std::back_inserter(_queue));
    pthread_mutex_unlock(&_dataMutex);
    return true;
}

void cEventManager_Qt::Post()
{
    pthread_mutex_lock(&_eventMutex);
    pthread_cond_broadcast(&_eventCond);
    pthread_mutex_unlock(&_eventMutex);
}

bool cEventManager_Qt::WaitForEvent()
{
    bool rc = false;
    struct timespec ts;
    ts.tv_nsec = 0;
    ts.tv_sec = 0;

    pthread_mutex_lock(&_eventMutex);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    rc = (pthread_cond_timedwait(&_eventCond, &_eventMutex, &ts) == 0);
    pthread_mutex_unlock(&_eventMutex);

    return rc;
}

int cEventManager_Qt::EventDataSize()
{
    pthread_mutex_lock(&_dataMutex);
    size_t temp = _queue.size();
    pthread_mutex_unlock(&_dataMutex);
    return static_cast<int>(temp);
}

bool cEventManager_Qt::ReadEventData(void* ptr, unsigned int size, unsigned long* read)
{
    Q_ASSERT(ptr != nullptr);    
    pthread_mutex_lock(&_dataMutex);
    size_t length = _queue.size();

    if (size > length) {
        pthread_mutex_unlock(&_dataMutex);
        return false;
    }

    std::deque<unsigned char>::iterator endIt = _queue.begin() + static_cast<int>(size);
    std::move(_queue.begin(), endIt, reinterpret_cast<unsigned char*>(ptr));
    _queue.erase(_queue.begin(), endIt);
    pthread_mutex_unlock(&_dataMutex);

    if (read != nullptr) {
        *read = length;
    }

    return true;
}

bool cEventManager_Qt::PeekEventData(void* ptr, unsigned int size, unsigned long* lpBytesRead, unsigned long* lpTotalBytesAvail, unsigned long* lpBytesLeftThisMessage)
{
    Q_UNUSED(ptr);
    Q_UNUSED(size);
    Q_UNUSED(lpBytesRead);
    Q_UNUSED(lpTotalBytesAvail);
    Q_UNUSED(lpBytesLeftThisMessage);

    return false;
}

void cEventManager_Qt::WriteHeadBodyAndPost(unsigned long Id, void* headptr, unsigned int headsize, void* bodyptr, unsigned int bodysize)
{
    Q_ASSERT(headptr != nullptr);
    Q_ASSERT(bodyptr != nullptr);

    pthread_mutex_lock(&_dataMutex);
    std::copy_n(reinterpret_cast<unsigned char*>(&Id), sizeof(Id), std::back_inserter(_queue));
    std::copy_n(reinterpret_cast<unsigned char*>(headptr), headsize, std::back_inserter(_queue));
    std::copy_n(reinterpret_cast<unsigned char*>(bodyptr), bodysize, std::back_inserter(_queue));
    pthread_mutex_unlock(&_dataMutex);
    Post();
}

void cEventManager_Qt::WriteHeadAndPost(unsigned long Id, void* headptr, unsigned int headsize)
{
    Q_ASSERT(headptr != nullptr);
    pthread_mutex_lock(&_dataMutex);
    std::copy_n(reinterpret_cast<unsigned char*>(&Id), sizeof(Id), std::back_inserter(_queue));
    std::copy_n(reinterpret_cast<unsigned char*>(headptr), headsize, std::back_inserter(_queue));
    pthread_mutex_unlock(&_dataMutex);
    Post();
}
