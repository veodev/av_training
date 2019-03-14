

#include "battery.h"
#include "powermanagement.h"
#include "debug.h"

#ifdef USE_UTSVU_POWERMANAGEMENT
#include "utsvupowermanagement.h"
#endif

#ifdef USE_ADVANTECH_POWERMANAGEMENT
#include "advantechpowermanagement.h"
#endif

#include <QApplication>

const static int updateInterval = 2000;

Worker::Worker(QObject* parent)
    : QObject(parent)
    , _timer(nullptr)
    , _powermanagement(nullptr)
{
#ifdef USE_UTSVU_POWERMANAGEMENT
    _powermanagement = new UtsvuPowermanagement();
#endif

#ifdef USE_ADVANTECH_POWERMANAGEMENT
    _powermanagement = new AdvantechPowermanagement();
#endif
}

void Worker::doWork()
{
    Q_ASSERT(_powermanagement != nullptr);

    if (_powermanagement->update() == true) {
        double percentsValue = _powermanagement->batteryPercent();
        emit voltage(_powermanagement->batteryVoltage());
        emit percents(percentsValue);
        emit temperatureSoc(_powermanagement->temperatureSoc());
    }
}

void Worker::init()
{
    if (_powermanagement != nullptr) {
        _timer = new QTimer(this);
        ASSERT(connect(_timer, &QTimer::timeout, this, &Worker::doWork));
        _timer->setInterval(updateInterval);
        _timer->setSingleShot(false);
        _timer->start(updateInterval);
    }
}

void Worker::stop()
{
    qDebug() << "Stopping battery worker...";
    if (_timer != nullptr) {
        _timer->stop();
        ASSERT(disconnect(_timer, &QTimer::timeout, this, &Worker::doWork));
    }
}

Battery::Battery(QObject* parent)
    : QObject(parent)
    , _worker(nullptr)
{
    if (isSupported()) {
#if defined TARGET_AVICON31 || defined TARGET_AVICONDBHS || defined TARGET_AVICONDB
        _worker = new Worker();
        _worker->setObjectName("batteryWorker");
        _workerThread.setObjectName("batteryWorkerThread");
        _worker->moveToThread(&_workerThread);
        ASSERT(connect(&_workerThread, &QThread::started, _worker, &Worker::init));
        ASSERT(connect(_worker, &Worker::voltage, this, &Battery::voltage));
        ASSERT(connect(_worker, &Worker::percents, this, &Battery::percents));
        ASSERT(connect(_worker, &Worker::temperatureSoc, this, &Battery::temperatureSoc));
        ASSERT(connect(this, &Battery::stopWorker, _worker, &Worker::stop, Qt::BlockingQueuedConnection));
        _workerThread.start();
#endif
    }
}

Battery::~Battery()
{
    qDebug() << "Deleting battery...";
    if (isSupported()) {
#if defined TARGET_AVICON31 || defined TARGET_AVICONDBHS || defined TARGET_AVICONDB
        Q_ASSERT(_worker);
        Q_ASSERT(_workerThread.isRunning());
        emit stopWorker();

        ASSERT(disconnect(&_workerThread, &QThread::started, _worker, &Worker::init));
        ASSERT(disconnect(_worker, &Worker::voltage, this, &Battery::voltage));
        ASSERT(disconnect(_worker, &Worker::percents, this, &Battery::percents));
        ASSERT(disconnect(_worker, &Worker::temperatureSoc, this, &Battery::temperatureSoc));
        ASSERT(disconnect(this, &Battery::stopWorker, _worker, &Worker::stop));
        _workerThread.exit(0);
        _workerThread.wait();
#endif
    }
    qDebug() << "Battery deleted!";
}

Battery& Battery::instance()
{
    static Battery _instance;

    return _instance;
}

bool Battery::isSupported()
{
#ifdef USE_UTSVU_POWERMANAGEMENT
    return UtsvuPowermanagement::isSupported();
#endif
#ifdef USE_ADVANTECH_POWERMANAGEMENT
    return AdvantechPowermanagement::isSupported();
#endif
    return false;
}
