#ifndef UTSVUTEMPERATURESENSOR_H
#define UTSVUTEMPERATURESENSOR_H

#include <QTimer>
#include <QThread>
#include <qambienttemperaturesensor.h>

#include "utsvusensorbackend.h"
#include "ds18s20.h"

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doInit();
    void doWork();

signals:
    void initResult(bool isInited, const QString &error);
    void resultReady(qreal temperature, const QString &error);

private:
    Ds18s20 temperatureSensor;
};

class UtsvuTemperatureSensor : public UtsvuSensorBackend<QAmbientTemperatureReading>
{
    Q_OBJECT

public:
    explicit UtsvuTemperatureSensor(QSensor *sensor);
    ~UtsvuTemperatureSensor();

    void start();
    void stop();

public slots:
    void initResult(bool isInited, const QString &error);
    void handleResults(qreal temperature, const QString &error);

signals:
    void operate();

private:
    QTimer _timer;
    QThread _workerThread;
};

#endif // UTSVUTEMPERATURESENSOR_H
