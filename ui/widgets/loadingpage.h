#ifndef LOADINGPAGE_H
#define LOADINGPAGE_H

#include <QWidget>
#include "netsettingpage.h"

namespace Ui
{
class LoadingPage;
}

class LoadingPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingPage(QWidget* parent = nullptr);
    ~LoadingPage();

private slots:
    void onIndicatorPressedAndHold();

private:
    Ui::LoadingPage* ui;
    NetSettingPage* _netSettingsPage;
};

#endif  // LOADINGPAGE_H
