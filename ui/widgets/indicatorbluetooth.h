#ifndef INDICATORBLUETOOTH_H
#define INDICATORBLUETOOTH_H

#include <QWidget>

namespace Ui
{
class IndicatorBluetooth;
}

class IndicatorBluetooth : public QWidget
{
    Q_OBJECT

public:
    explicit IndicatorBluetooth(QWidget* parent = 0);
    ~IndicatorBluetooth();

    void changeBluetoothStatus(bool isOn = false);

private:
    Ui::IndicatorBluetooth* ui;
};

#endif  // INDICATORBLUETOOTH_H
