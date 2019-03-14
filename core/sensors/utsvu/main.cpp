#include "utsvusensorbackend.h"
#include "utsvutemperaturesensor.h"

#include <qsensormanager.h>
#include <qsensorbackend.h>
#include <qsensorplugin.h>

#include <QDebug>

#include "debug.h"

static const char *utsvuTemperatureSensorId = "utsvuTemperatureSensor";

class UtsvuSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.qt-project.Qt.QSensorPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(QSensorPluginInterface)

public:
    void registerSensors()
    {
        QDEBUG << "loaded the UTSVU sensors plugin";
        QSensorManager::registerBackend(QAmbientTemperatureSensor::type, utsvuTemperatureSensorId, this);
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        qDebug() << Q_FUNC_INFO;
        UtsvuSensorBackendBase *backend = 0;
        if (sensor->identifier() == utsvuTemperatureSensorId)
            backend = new UtsvuTemperatureSensor(sensor);
        return backend;
    }
};

#include "main.moc"
