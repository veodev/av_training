#ifndef BATTERYOPTIONS_H
#define BATTERYOPTIONS_H

#include <QWidget>

namespace Ui
{
class BatteryOptions;
}

class BatteryOptions : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryOptions(QWidget* parent = 0);
    ~BatteryOptions();
signals:
    void minimalLevelBatteryChanged(qreal value);

private slots:
    void on_enableLogButton_released();

private:
    Ui::BatteryOptions* ui;
};

#endif  // BATTERYOPTIONS_H
