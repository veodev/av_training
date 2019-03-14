#include <QTimer>

#include "temperature.h"
#include "ui_temperature.h"
#include "debug.h"
#include "core.h"


Temperature::Temperature(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TemperatureWidget)
{
    ui->setupUi(this);
}

Temperature::~Temperature()
{
    delete ui;
}

void Temperature::onTemperatureChanged(qreal value)
{
    setEnabled(true);
    ui->temperature->setText(QString::number(value, 'f', 1) + " " + QString(0x2103));
}
