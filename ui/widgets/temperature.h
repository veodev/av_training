#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QWidget>


namespace Ui
{
class TemperatureWidget;
}

class QSensor;
class QSensorReading;

class Temperature : public QWidget
{
    Q_OBJECT

public:
    explicit Temperature(QWidget* parent = 0);
    virtual ~Temperature();

signals:

public slots:
    void onTemperatureChanged(qreal value);

private:
    Ui::TemperatureWidget* ui;
};

#endif  // TEMPERATURE_H
