#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui
{
class BatteryWidget;
}

class BatteryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryWidget(QWidget* parent = 0);
    ~BatteryWidget();

    void setVoltageVisible(bool isVisible);
    void setPercentVisible(bool isVisible);
    void setTextColor(QColor color);
    void setMinimalPercent(int minimalPercent);

protected:
    bool event(QEvent* e);

signals:
    void lowBattery();
    void batteryIsOver();

public slots:
    void voltage(double value);
    void percents(double value);
#ifdef TARGET_AVICON15
    void onConnectionStatusConnected(void);
    void onConnectionStatusDisconnected(void);
#endif

private slots:
    void checkLevelOfBattery();
    void startCheckTimer();

private:
    Ui::BatteryWidget* ui;

    int _minimalPercent;
    int _currentPercent;
    int _countOfNotes;
    bool _umu_Connected;

    QTimer* _checkBatteryTimer;
};

#endif  // BATTERYWIDGET_H
