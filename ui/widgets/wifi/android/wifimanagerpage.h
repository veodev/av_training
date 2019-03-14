#ifndef WIFIMANAGERPAGE_H
#define WIFIMANAGERPAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui
{
class WifiManagerPage;
}

class WifiManagerPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiManagerPage(QWidget* parent = nullptr);
    ~WifiManagerPage();

    QString& getDefaultGetaway();

signals:
    void connectToWifi();
    void disconnectToWifi();
    void qualityPercent(qreal percent);

public slots:
    void setVisible(bool visible);

private slots:
    void on_wifiOnButton_released();
    void on_wifiOffButton_released();
    void on_connectButton_released();
    void on_scanButton_released();
    void onCheckWifiTimerTimeout();

private:
    Ui::WifiManagerPage* ui;
    QTimer _checkWifiTimer;
    bool _isWifiOn;
    QString _defaultGateway;
};

#endif  // WIFIMANAGERPAGE_H
