#ifndef WIFIMANAGERPAGE_H
#define WIFIMANAGERPAGE_H

#include <QWidget>

namespace Ui
{
class WifiManagerPage;
}

class WifiManagerPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiManagerPage(QWidget* parent = 0);
    ~WifiManagerPage();

    bool isFaked();

signals:
    void qualityPercent(qreal percent);
    void connectToWifi();
    void disconnectToWifi();

private:
    Ui::WifiManagerPage* ui;
};

#endif  // WIFIMANAGERPAGE_H
