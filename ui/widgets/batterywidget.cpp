#include <QTimer>

#include "battery.h"
#include "batterywidget.h"
#include "ui_batterywidget.h"
#include "debug.h"
#include "core.h"
#include "androidJNI.h"
#include "settings.h"

#define COUNT_OF_NOTES 3

#define BATTERY_OVER_PERCENT 2
#define MINIMAL_PERCENT 5
#define MAXIMUM_PERCENT 50

BatteryWidget::BatteryWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BatteryWidget)
    , _minimalPercent(restoreMinimalBatteryPercent())
    , _currentPercent(MINIMAL_PERCENT)
    , _countOfNotes(COUNT_OF_NOTES)
    , _umu_Connected(false)
{
    ui->setupUi(this);
    _checkBatteryTimer = new QTimer(this);
    ASSERT(connect(_checkBatteryTimer, &QTimer::timeout, this, &BatteryWidget::checkLevelOfBattery));
    if (restoreDeviceType() != Avicon31Estonia) {
        QTimer::singleShot(20000, this, &BatteryWidget::startCheckTimer);
    }

#ifdef TARGET_AVICON15
    ui->voltage->setText("---");
    ui->percent->setText("---");
#else
    if (Battery::isSupported() == true) {
        setVisible(true);

        Battery& battery = Battery::instance();
        ASSERT(connect(&battery, &Battery::percents, this, &BatteryWidget::percents));
        ASSERT(connect(&battery, &Battery::voltage, this, &BatteryWidget::voltage));
    }
    else {
        setVisible(false);
    }
#endif
}

BatteryWidget::~BatteryWidget()
{
#ifndef TARGET_AVICON15
    if (Battery::isSupported() == true) {
        Battery& battery = Battery::instance();
        ASSERT(disconnect(&battery, &Battery::percents, this, &BatteryWidget::percents));
        ASSERT(disconnect(&battery, &Battery::voltage, this, &BatteryWidget::voltage));
    }
#endif
    _checkBatteryTimer->stop();
    ASSERT(disconnect(_checkBatteryTimer, &QTimer::timeout, this, &BatteryWidget::checkLevelOfBattery));
    delete ui;
}

void BatteryWidget::setVoltageVisible(bool isVisible)
{
    ui->voltage->setVisible(isVisible);
}

void BatteryWidget::setPercentVisible(bool isVisible)
{
    ui->percent->setVisible(isVisible);
}

void BatteryWidget::setTextColor(QColor color)
{
    QString style = "color: " + color.name();
    ui->voltage->setStyleSheet(style);
    ui->percent->setStyleSheet(style);
}

void BatteryWidget::setMinimalPercent(int minimalPercent)
{
    _minimalPercent = minimalPercent;
}

bool BatteryWidget::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void BatteryWidget::voltage(double value)
{
    ui->voltage->setEnabled(true);
    ui->voltage->setText(QString::number(value, 'f', 1) + tr(" V"));
}

void BatteryWidget::percents(double value)
{
    ui->percent->setEnabled(true);
    ui->percent->setText(QString::number(value, 'f', 0) + "%");
    _currentPercent = value;

#ifdef TARGET_AVICON15
    if (_currentPercent <= BATTERY_OVER_PERCENT && _umu_Connected) {
#else
    if (_currentPercent <= BATTERY_OVER_PERCENT) {
#endif
        _checkBatteryTimer->stop();
        emit batteryIsOver();
    }
}

void BatteryWidget::checkLevelOfBattery()
{
    if (_countOfNotes == 0) {
        _checkBatteryTimer->stop();
        return;
    }
#ifdef TARGET_AVICON15
    else if (_currentPercent <= _minimalPercent && _umu_Connected) {
#else
    else if (_currentPercent <= _minimalPercent) {
#endif

        emit lowBattery();
        --_countOfNotes;
    }
}

void BatteryWidget::startCheckTimer()
{
    checkLevelOfBattery();
    _checkBatteryTimer->start(60000);
}


#ifdef TARGET_AVICON15
void BatteryWidget::onConnectionStatusConnected(void)
{
    _umu_Connected = true;
}

void BatteryWidget::onConnectionStatusDisconnected(void)
{
    _umu_Connected = false;

    ui->percent->setText("---");
    ui->voltage->setText("---");
}
#endif
