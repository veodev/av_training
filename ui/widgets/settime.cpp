#include <QTime>
#include <QDateTime>
#include <QProcess>
#include <QMessageBox>

#include "settime.h"
#include "ui_settime.h"
#include "debug.h"
#include "notifier.h"
#include "settings.h"

const int degreesForTimeZone = 15;
const int maxTimeOffset = 12;
const int minTimeOffset = -12;

#define INVALID_TIME_OFFSET -100

SetTime::SetTime(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SetTime)
#if defined TARGET_AVICON31 && !defined ANDROID
    , _geoPosition(GeoPosition::instance("/dev/ttymxc1"))
#endif
    , _timeOffsetManual(restoreTimeOffset())
    , _timeOffsetAuto(0)
{
    ui->setupUi(this);

    ui->updateWithManualTimeZoneButton->setDisabled(_timeOffsetManual == INVALID_TIME_OFFSET);
    ui->updateWithManualTimeZoneButton->setDisabled(true);
    ui->updateWithAutoTimeZoneButton->setDisabled(true);
    ui->utcLabel->setVisible(false);

    ui->offsetSpinBox->enableCaption(true);
    ui->offsetSpinBox->setCaption(tr("Adjustment UTC"));
    ui->offsetSpinBox->setMaximum(maxTimeOffset);
    ui->offsetSpinBox->setMinimum(minTimeOffset);
    ui->offsetSpinBox->setVisible(false);
    ASSERT(connect(ui->offsetSpinBox, &SpinBoxNumber::valueChanged, this, &SetTime::onOffsetSpinboxValueChanged));

#if defined TARGET_AVICON31 && !defined ANDROID
    ASSERT(connect(_geoPosition, &GeoPosition::positionUpdated, this, &SetTime::onUpdateTimeAndCoordinate));
#endif
}

SetTime::~SetTime()
{
    delete ui;
}

void SetTime::configureUiForAviconDbAviconDbHs()
{
    ui->groupBox->hide();
    ui->updateWithAutoTimeZoneButton->hide();
    ui->updateWithManualTimeZoneButton->hide();
}

void SetTime::configureUiForAvicon15()
{
    configureUiForAviconDbAviconDbHs();
}

void SetTime::setVisible(bool visible)
{
    if (visible == true) {
        _time = QTime::currentTime();
        setHours(_time.hour());
        setMinutes(_time.minute());
        setSeconds(_time.second());
        setClock();
    }

    QWidget::setVisible(visible);
}

void SetTime::onUpdateTimeAndCoordinate(const QGeoPositionInfo& info)
{
    if (info.timestamp().time().isNull() == false && info.coordinate().latitude() != 0 && info.coordinate().longitude() != 0) {
        ui->updateWithAutoTimeZoneButton->setEnabled(true);
        ui->utcLabel->setVisible(true);
        ui->offsetLabel->setVisible(true);
        ui->offsetSpinBox->setVisible(true);
        ui->searchSatelliteLabel->setVisible(false);

        qreal longitude = info.coordinate().longitude();
        const QTime& curTime = info.timestamp().time();
        _timeFromSatellite.setHMS(curTime.hour(), curTime.minute(), curTime.second());
        _timeOffsetAuto = static_cast<int>(longitude) / degreesForTimeZone;
        int difference = abs(static_cast<int>((longitude - (_timeOffsetAuto * degreesForTimeZone)) * 10));
        int additive = (difference > 0) ? 1 : 0;
        if (_timeOffsetAuto >= 0) {
            _timeOffsetAuto += additive;
        }
        else {
            _timeOffsetAuto -= additive;
        }

        if (_timeOffsetManual == INVALID_TIME_OFFSET) {
            ui->offsetSpinBox->setValue(_timeOffsetAuto, true);
            ui->updateWithManualTimeZoneButton->setEnabled(false);
        }
        else {
            ui->offsetSpinBox->setValue(_timeOffsetManual, true);
            ui->updateWithManualTimeZoneButton->setEnabled(true);
        }
        ui->satelitteClockLabel->setText(curTime.toString("HH:mm:ss"));
    }
    else {
        ui->searchSatelliteLabel->setVisible(true);
        ui->utcLabel->setVisible(false);
        ui->offsetLabel->setVisible(false);
        ui->offsetSpinBox->setVisible(false);
        ui->updateWithAutoTimeZoneButton->setEnabled(false);
    }
}

bool SetTime::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

bool SetTime::runProcess(const QString& program)
{
    QProcess process;

    QDEBUG << "program:" << program;
    process.start(program);
    process.waitForFinished();
    if (process.exitCode()) {
        QByteArray error = process.readAllStandardError();
        qWarning() << error;
        QMessageBox::critical(this, tr("Error of setting time"), QString(error), QMessageBox::Ok);
        return false;
    }

    return true;
}

void SetTime::setHours(int value)
{
    if (value < 0) {
        value = 23;
    }
    else if (value > 23) {
        value = 0;
    }

    (value < 10) ? ui->setClockHoursLcdNumber->display("0" + QString::number(value)) : ui->setClockHoursLcdNumber->display(value);
}

void SetTime::setMinutes(int value)
{
    if (value < 0) {
        value = 59;
        setHours(ui->setClockHoursLcdNumber->value() - 1);
    }
    else if (value > 59) {
        value = 0;
        setHours(ui->setClockHoursLcdNumber->value() + 1);
    }

    (value < 10) ? ui->setClockMinutesLcdNumber->display("0" + QString::number(value)) : ui->setClockMinutesLcdNumber->display(value);
}

void SetTime::setSeconds(int value)
{
    if (value < 0) {
        value = 59;
        setMinutes(ui->setClockMinutesLcdNumber->value() - 1);
    }
    else if (value > 59) {
        setMinutes(ui->setClockMinutesLcdNumber->value() + 1);
        value = 0;
    }

    (value < 10) ? ui->setClockSecondsLcdNumber->display("0" + QString::number(value)) : ui->setClockSecondsLcdNumber->display(value);
}

void SetTime::setClock()
{
    _time.setHMS(ui->setClockHoursLcdNumber->value(), ui->setClockMinutesLcdNumber->value(), ui->setClockSecondsLcdNumber->value());
}


int SetTime::convertToCurrentHours(const int& timeOffset, const int& hours)
{
    int tmpCurrentHours = timeOffset + hours;
    if (tmpCurrentHours > 23) {
        tmpCurrentHours = tmpCurrentHours - 24;
    }
    else if (tmpCurrentHours < 0) {
        tmpCurrentHours = 24 + tmpCurrentHours;
    }
    return tmpCurrentHours;
}

void SetTime::on_okButton_released()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime.setTime(_time);
    QString sudoCommand;
#if defined TARGET_AVICONDB || defined TARGET_AVICONDBHS
    sudoCommand = "sudo ";
#endif
    if (runProcess(sudoCommand + QString("/bin/date -s \"%1\"").arg(dateTime.toUTC().toString("yyyy-MM-dd hh:mm:ss"))) == true) {
        // Set the Hardware Clock to the current System Time.
        runProcess(sudoCommand + "hwclock -w");
    }
    saveTimeOffset(_timeOffsetManual);
    emit leaveTheWidget();
}

void SetTime::on_setHoursDownPushButton_released()
{
    setHours(ui->setClockHoursLcdNumber->value() - 1);
    setClock();
}

void SetTime::on_setHoursUpPushButton_released()
{
    setHours(ui->setClockHoursLcdNumber->value() + 1);
    setClock();
}

void SetTime::on_setMinutesDownPushButton_released()
{
    setMinutes(ui->setClockMinutesLcdNumber->value() - 1);
    setClock();
}

void SetTime::on_setMinutesUpPushButton_released()
{
    setMinutes(ui->setClockMinutesLcdNumber->value() + 1);
    setClock();
}

void SetTime::on_setSecondsDownPushButton_released()
{
    setSeconds(ui->setClockSecondsLcdNumber->value() - 1);
    setClock();
}

void SetTime::on_setSecondsUpPushButton_released()
{
    setSeconds(ui->setClockSecondsLcdNumber->value() + 1);
    setClock();
}

void SetTime::on_updateWithAutoTimeZoneButton_released()
{
    if (ui->searchSatelliteLabel->isVisible() == false) {
        _timeOffsetManual = INVALID_TIME_OFFSET;
        setHours(convertToCurrentHours(_timeOffsetAuto, _timeFromSatellite.hour()));
        setMinutes(_timeFromSatellite.minute());
        setSeconds(_timeFromSatellite.second());
        setClock();
        ui->offsetSpinBox->setValue(_timeOffsetAuto, true);
        _timeOffsetManual = _timeOffsetAuto;
        ui->updateWithManualTimeZoneButton->setEnabled(true);
        Notifier::instance().addNote(Qt::red, tr("Time has been updated from the satellite. Check accuracy!"));
    }
    else {
        Notifier::instance().addNote(Qt::red, tr("No visible satellites!"));
    }
}

void SetTime::on_updateWithManualTimeZoneButton_released()
{
    if (ui->searchSatelliteLabel->isVisible() == false) {
        setHours(convertToCurrentHours(_timeOffsetManual, _timeFromSatellite.hour()));
        setMinutes(_timeFromSatellite.minute());
        setSeconds(_timeFromSatellite.second());
        setClock();
        Notifier::instance().addNote(Qt::green, tr("Time synchronized with the satellite. Check accuracy!"));
    }
    else {
        Notifier::instance().addNote(Qt::red, tr("No visible satellites!"));
    }
}

void SetTime::onOffsetSpinboxValueChanged(qreal value)
{
    _timeOffsetManual = value;
    if (value >= 0) {
        ui->offsetLabel->setText("+" + QString::number(value));
    }
    else {
        ui->offsetLabel->setText(QString::number(value));
    }
    saveTimeOffset(_timeOffsetManual);
}
