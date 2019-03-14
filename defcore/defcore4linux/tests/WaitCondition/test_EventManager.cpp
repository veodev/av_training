#include <QTest>
#include <QDebug>

#include "test_EventManager.h"
#include "EventManager_Qt.h"
#include "ThreadClassList_Lin.h"

static const char * srcStr1 = "Hello ";
static const char * srcStr2 = "world!";

class Sender
{
public:
    Sender(cEventManager & eventManager)
        : _eventManager(&eventManager)
    {
    }

    ~Sender()
    {
    }

    bool run()
    {
        _eventManager->CreateNewEvent(edAlarmData);
        _eventManager->WriteEventData(&srcStr1, SIZE_OF_TYPE(&srcStr1));
        _eventManager->WriteEventData(&srcStr2, SIZE_OF_TYPE(&srcStr2));
        _eventManager->Post();

        return false;
    }

private:
    cEventManager * _eventManager;
};

class Receiver
{
public:
    Receiver(cEventManager &eventManager)
        : _eventManager(&eventManager)
        , _result(false)
        , _checkpuints(0)
    {
    }

    ~Receiver()
    {
    }

    bool run()
    {
        for (int i = 0; i < 5; ++i) {
            if (_eventManager->WaitForEvent()) {
                _result = true;
                unsigned long eventID;
                if (_eventManager->ReadEventData(&eventID, sizeof(typeof(&eventID)), NULL) == false) {
                    return false;
                }
                ++_checkpuints; // 1
                if (eventID != edAlarmData) {
                    return false;
                }
                ++_checkpuints; // 2
                char * dstStr;
                if (_eventManager->ReadEventData(&dstStr, SIZE_OF_TYPE(&dstStr), NULL) == false) {
                    return false;
                }
                ++_checkpuints; // 3
                if (dstStr != srcStr1) {
                    return false;
                }
                ++_checkpuints; // 5
                if (_eventManager->ReadEventData(&dstStr, SIZE_OF_TYPE(&dstStr), NULL) == false) {
                    return false;
                }
                ++_checkpuints; // 4
                if (dstStr != srcStr2) {
                    return false;
                }
                ++_checkpuints; // 6
                break;
            }
        }

        return true;
    }

    bool result() const
    {
        return _result;
    }

    int checkpuints() {
        return _checkpuints;
    }

private:
    cEventManager * _eventManager;
    bool _result;
    int _checkpuints;
};

Test_EventManager::Test_EventManager(QObject *parent) :
    QObject(parent)
{
}

Test_EventManager::~Test_EventManager()
{
}

void Test_EventManager::test_WaitCondition()
{
    cEventManager *eventManager = new cEventManager_Qt;

    Receiver receiver(*eventManager);
    Sender sender(*eventManager);

    cThreadClassList * threads = new cThreadClassList_Lin;
    int receiverThread = threads->AddTick(DEFCORE_THREAD_FUNCTION(Receiver, &receiver, run));
    int senderThread = threads->AddTick(DEFCORE_THREAD_FUNCTION(Sender, &sender, run));

    threads->Resume(receiverThread);
    threads->Resume(senderThread);

    int i;
    for (i = 0; i < 10; ++i) {
        if (receiver.result() == true) {
            break;
        }
        ::sleep(1);
    }
    if (i > 3) {
        QFAIL("Timeout");
    }

    QCOMPARE(receiver.checkpuints(), 6);

    delete threads;
    delete eventManager;
}

void Test_EventManager::test_EventManager()
{
    cEventManager_Qt * eventManager = new cEventManager_Qt;
    eventManager->CreateNewEvent(edAlarmData);
    eventManager->WriteEventData(&srcStr1, SIZE_OF_TYPE(&srcStr1));
    eventManager->WriteEventData(&srcStr2, SIZE_OF_TYPE(&srcStr2));
    eventManager->Post();
    QCOMPARE(eventManager->EventDataSize(), (int)(sizeof(unsigned long) + SIZE_OF_TYPE(&srcStr1) + SIZE_OF_TYPE(&srcStr2)));

    EventDataType id;
    eventManager->ReadEventData(&id, sizeof(unsigned long), NULL);
    QCOMPARE(id, edAlarmData);
    char * dstStr;
    eventManager->ReadEventData(&dstStr, SIZE_OF_TYPE(&dstStr), NULL);
    QCOMPARE(srcStr1, dstStr);
    eventManager->ReadEventData(&dstStr, SIZE_OF_TYPE(&dstStr), NULL);
    QCOMPARE(srcStr2, dstStr);
    QCOMPARE(eventManager->EventDataSize(), 0);
}
