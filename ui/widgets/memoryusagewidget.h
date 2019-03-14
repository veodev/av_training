#ifndef MEMORYUSAGEWIDGET_HPP
#define MEMORYUSAGEWIDGET_HPP

#include <QWidget>

namespace Ui
{
class MemoryUsageWidget;
}

class MemoryUsageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MemoryUsageWidget(QWidget* parent = 0);
    ~MemoryUsageWidget();
    void setMemoryInfo(unsigned long long total, unsigned long long user, unsigned long long system);


private:
    unsigned long long _totalBytes;
    unsigned long long _systemBytes;
    unsigned long long _userBytes;
    Ui::MemoryUsageWidget* ui;
};

#endif  // MEMORYUSAGEWIDGET_HPP
