#include "memoryusagewidget.h"
#include "ui_memoryusagewidget.h"
#include <QPainter>
#include <QDebug>

MemoryUsageWidget::MemoryUsageWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MemoryUsageWidget)
{
    ui->setupUi(this);
    setMemoryInfo(1000, 100, 100);
}

MemoryUsageWidget::~MemoryUsageWidget()
{
    delete ui;
}

void MemoryUsageWidget::setMemoryInfo(unsigned long long total, unsigned long long user, unsigned long long system)
{
    if (isVisible()) {
        _totalBytes = total;
        _userBytes = user;
        _systemBytes = system;
        // ui->systemLabel->setText(tr("System") + " " + QString::number(system / (1024)) + " " + tr("Mb"));
        ui->userLabel->setText(tr("User") + " " + QString::number(user / (1024)) + " " + tr("Mb"));
        ui->freeLabel->setText(tr("Free") + " " + QString::number((total - (user + system)) / (1024)) + " " + tr("Mb"));
#ifdef ANDROID
        ui->usageProgressBar->setValue((static_cast<float>(user / 1024) / static_cast<float>((total) / 1024)) * 100.0f);
#else
        ui->usageProgressBar->setValue((static_cast<float>(user / 1024) / static_cast<float>((total - (user + system)) / 1024)) * 100.0f);
#endif
        update();
    }
}
