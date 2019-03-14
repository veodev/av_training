#ifndef INDICATORWIFI_H
#define INDICATORWIFI_H

#include <QWidget>

namespace Ui
{
class IndicatorWifi;
}

class IndicatorWifi : public QWidget
{
    Q_OBJECT

public:
    explicit IndicatorWifi(QWidget* parent = 0);
    ~IndicatorWifi();

    void setIndicatorPercent(qreal percent);

private:
    Ui::IndicatorWifi* ui;
    QPixmap _wifi25;
    QPixmap _wifi50;
    QPixmap _wifi75;
    QPixmap _wifi100;
    QPixmap _wifiOff;
};

#endif  // INDICATORWIFI_H
