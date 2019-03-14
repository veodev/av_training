#ifndef DEVICEOPTIONS_H
#define DEVICEOPTIONS_H

#include <QWidget>

namespace Ui
{
class DeviceOptions;
}

class DeviceOptions : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceOptions(QWidget* parent = 0);
    ~DeviceOptions();

private slots:
    void on_saveButton_released();

private:
    Ui::DeviceOptions* ui;
};

#endif  // DEVICEOPTIONS_H
