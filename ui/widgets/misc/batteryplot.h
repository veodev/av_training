#ifndef BATTERYPLOT_H
#define BATTERYPLOT_H

#include <QWidget>
#include <QElapsedTimer>

#pragma pack(push, 1)
struct VoltagePoint
{
    float voltage;
    unsigned int time;
};
#pragma pack(pop)

namespace Ui
{
class BatteryPlot;
}

class BatteryPlot : public QWidget
{
    Q_OBJECT

    std::vector<VoltagePoint> _voltageValues;
    bool _loggingEnabled;

public:
    explicit BatteryPlot(QWidget* parent = 0);
    ~BatteryPlot();
    void paintEvent(QPaintEvent*);
    bool loggingEnabled() const;
    void setLoggingEnabled(bool loggingEnabled);

public slots:
    void onVoltage(double voltage);

private:
    QElapsedTimer _timer;
    Ui::BatteryPlot* ui;
};

#endif  // BATTERYPLOT_H
