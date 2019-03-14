#ifndef BATTERY_H
#define BATTERY_H

#include <QThread>
#include <QTimer>

class Powermanagement;

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject* parent = 0);

public slots:
    void init();
    void stop();

signals:
    void voltage(double value);
    void percents(double value);
    void temperatureSoc(double value);

private slots:
    void doWork();

private:
    QTimer* _timer;
    Powermanagement* _powermanagement;
};

class Battery : public QObject
{
    Q_OBJECT

public:
    static Battery& instance();
    static bool isSupported();

protected:
    explicit Battery(QObject* parent = 0);
    explicit Battery(const Battery& other);
    Battery& operator=(const Battery& object);
    virtual ~Battery();

signals:
    void voltage(double value);
    void percents(double value);
    void temperatureSoc(double value);
    void stopWorker();

private:
    Worker* _worker;
    QThread _workerThread;
};

#endif  // BATTERY_H
