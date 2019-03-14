#ifndef WIFIPAGE_H
#define WIFIPAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui
{
class WifiPage;
}

class WifiManager;

class WifiPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiPage(QWidget * parent = 0);
    ~WifiPage();

public slots:
    void setVisible(bool visible);

signals:
    void qualityPercent(qreal percent);
    void connectToWifi();
    void disconnectToWifi();

private slots:
    void on_interfacesUpdatePushButton_released();
    void on_scanPushButton_released();
    void on_setIpAddressPushButton_released();
    void on_connectPushButton_released();
    void on_disconnectPushButton_released();
    void on_changeIpAddressPushButton_released();
    void onQualityTimerTimeout();

private:
    void changeIpAddrees();

private:
    Ui::WifiPage * ui;

    WifiManager * _wirelessInterface;
    QTimer * _qualityTimer;
};

#endif // WIFIPAGE_H
