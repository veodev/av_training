#ifndef TEST_EVENTMANAGER_H
#define TEST_EVENTMANAGER_H

#include <QObject>

class Test_EventManager : public QObject
{
    Q_OBJECT
public:
    explicit Test_EventManager(QObject * parent = 0);
    ~Test_EventManager();

signals:

private slots:
    void test_WaitCondition();
    void test_EventManager();
};

#endif // TEST_EVENTMANAGER_H
