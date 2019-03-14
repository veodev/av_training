#ifndef EXTERNALTEMPERATUREOPTIONS_H
#define EXTERNALTEMPERATUREOPTIONS_H

#include <QWidget>

namespace Ui
{
class ExternalTemperatureOptions;
}

class ExternalTemperatureOptions : public QWidget
{
    Q_OBJECT

public:
    explicit ExternalTemperatureOptions(QWidget* parent = 0);
    ~ExternalTemperatureOptions();

private slots:
    void on_tempCheckBox_released();

private:
    Ui::ExternalTemperatureOptions* ui;
};

#endif  // EXTERNALTEMPERATUREOPTIONS_H
