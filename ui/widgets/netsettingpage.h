#ifndef NETSETTINGPAGE_H
#define NETSETTINGPAGE_H

#include <QWidget>

namespace Ui
{
class NetSettingPage;
}

class NetSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit NetSettingPage(QWidget* parent = nullptr);
    ~NetSettingPage();

private slots:
    void on_backButton_released();

private:
    Ui::NetSettingPage* ui;
};

#endif  // NETSETTINGPAGE_H
