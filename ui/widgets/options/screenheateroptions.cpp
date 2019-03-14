#include "screenheateroptions.h"
#include "ui_screenheateroptions.h"
#include <debug.h>
#include "settings.h"
#include "core.h"

ScreenHeaterOptions::ScreenHeaterOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScreenHeaterOptions)
{
    ui->setupUi(this);
    ui->screenHeaterStatusSpinBox->addItem(tr("Off"), false);
    ui->screenHeaterStatusSpinBox->addItem(tr("On"), true);
    ASSERT(connect(ui->screenHeaterStatusSpinBox, &SpinBoxList::valueChanged, this, &ScreenHeaterOptions::onScreenHeaterStatusChanged));
    ui->screenHeaterStatusSpinBox->setIndex(restoreScreenHeaterStatus());


    int temp = restoreScreenHeaterTemp();
    ui->screenHeaterTempSpinBox->setMinimum(-50);
    ui->screenHeaterTempSpinBox->setMaximum(5);
    ASSERT(connect(ui->screenHeaterTempSpinBox, &SpinBoxNumber::valueChanged, this, &ScreenHeaterOptions::onScreenHeaterTempChanged));
    ui->screenHeaterTempSpinBox->setValue(temp, true);
}

ScreenHeaterOptions::~ScreenHeaterOptions()
{
    delete ui;
}

void ScreenHeaterOptions::onScreenTemperatureChanged(qreal value)
{
    ui->screenTemperatureLabel->setText(QString::number(value, 'f', 1) + " ℃");
}

void ScreenHeaterOptions::onScreenHeaterStatusChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    Core::instance().setScreenHeaterStatus(index == 1);
    Core::instance().resetTemperatureCache();
    saveScreenHeaterStatus(userData.toBool());
    qDebug() << "Screen heater status:" << (index == 1);
}

void ScreenHeaterOptions::onScreenHeaterTempChanged(qreal value)
{
    Core::instance().setScreenHeaterTargetTemperature(static_cast<int>(value));
    Core::instance().resetTemperatureCache();
    saveScreenHeaterTemp(static_cast<int>(value));
    qDebug() << "Screen heater temp changed:" << value;
}
