#include "encoderpage.h"
#include "ui_encoderpage.h"
#include "core.h"
#include "debug.h"
#include "settings.h"
#include "direction.h"
#include "registration.h"
#include "Device_definitions.h"
#include <qmath.h>
#include <QTimer>
#include <QVector>
#include <QElapsedTimer>

const static int SPEED_TIMER_INTERVAL = 500;
const static int RECORD_SPEED_TIMER = 10000;

const static float MIN_STEP_LENGTH_MM = 1.5f;
const static float MAX_STEP_LENGTH_MM = 10.0f;
const static float DEFAULT_STEP_LENGTH_MM = 1.89f;

EncoderPage::EncoderPage(QWidget* parent)
    : QWidget(parent)
    , _isInvert(invertEncoder())
    , _countStepsForOneMeter(0)
    , _countStepsForOneMeterConst(0)
    , _lastSpeed(0)
    , _distance(0)
    , _prevSysCoordinat(0)
    , _lastSession(0)
    , ui(new Ui::EncoderPage)
{
    ui->setupUi(this);
    restoreStepLength();
    setupUi();
    retranslateUi();
    Core& core = Core::instance();
    ASSERT(connect(&core, &Core::doBScan2Data, this, &EncoderPage::onBScanData));
    ASSERT(connect(&core, &Core::doDeviceSpeed, this, &EncoderPage::onDeviceSpeed));
    setDistance();
    resetCountStepsForOneMeter();
}

EncoderPage::~EncoderPage()
{
    qDebug() << "Deleting encoderpage...";
    Core& core = Core::instance();
    ASSERT(disconnect(&core, &Core::doBScan2Data, this, &EncoderPage::onBScanData));
    ASSERT(disconnect(&core, &Core::doDeviceSpeed, this, &EncoderPage::onDeviceSpeed));
    delete ui;
    qDebug() << "Encoderpage deleted!";
}

bool EncoderPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void EncoderPage::setupUi()
{
    ui->resetDistanceButton->setFocusPolicy(Qt::NoFocus);

    ui->stepSpinBox->setPrecision(3);
    ui->stepSpinBox->setMinimum(MIN_STEP_LENGTH_MM);
    ui->stepSpinBox->setMaximum(MAX_STEP_LENGTH_MM);
    ui->stepSpinBox->setValue(_stepLength, false);
    ui->stepSpinBox->setStepBy(0.01f);
    ui->stepSpinBox->setValueFontSize(25);
    ASSERT(connect(ui->stepSpinBox, &SpinBoxNumber::valueChanged, this, &EncoderPage::stepChanged));
}

void EncoderPage::retranslateUi()
{
    ui->stepSpinBox->setSuffix(tr(" mm"));
}

void EncoderPage::setSpeed(float value)
{
    ui->speedLabel->setText(QString::number(value, 'f', 2));
}

void EncoderPage::setDistance()
{
    ui->distanceLabel->setText(QString::number((_stepLength * _distance) / 1000, 'f', 3));
}

void EncoderPage::setCountStepsForOneMeter(float value)
{
    _countStepsForOneMeterConst = qRound(1000.0f / value);
}

void EncoderPage::restoreStepLength()
{
    float restoreStepLength = encoderCorrection();
    if (restoreStepLength < MIN_STEP_LENGTH_MM || restoreStepLength > MAX_STEP_LENGTH_MM) {
        _stepLength = DEFAULT_STEP_LENGTH_MM;
        saveEncoderCorrection(_stepLength);
        qDebug() << "Error! step = " << restoreStepLength << "! Default value was seted.";
    }
    else {
        _stepLength = restoreStepLength;
    }
}

void EncoderPage::resetCountStepsForOneMeter()
{
    _countStepsForOneMeter = 0;
    _prevSysCoordinat = 0;
    setCountStepsForOneMeter(encoderCorrection());
}

void EncoderPage::resetCoord()
{
    _prevSysCoordinat = 0;
}

void EncoderPage::onBScanData(QSharedPointer<tDEV_BScan2Head> head)
{
    if (head.data()->ChannelType == ctHandScan) {
        return;
    }
    if (head.data()->PathEncodersIndex != 0) {
        return;
    }
    long int currentSysCoordinat = head.data()->XSysCrd[0];
    if (Q_UNLIKELY(_prevSysCoordinat == 0)) {
        _prevSysCoordinat = currentSysCoordinat;
        return;
    }
    if (_lastSession != head.data()->BScanSessionID) {
        _prevSysCoordinat = currentSysCoordinat;
        _lastSession = head.data()->BScanSessionID;
    }

    long int difference = qAbs(currentSysCoordinat - _prevSysCoordinat);

    _prevSysCoordinat = currentSysCoordinat;

    Direction direction = directionByProbe(head.data()->Dir[0]);
    if (direction == ForwardDirection) {
        _countStepsForOneMeter += difference;
        _distance += difference;
    }
    else if (direction == BackwardDirection) {
        _countStepsForOneMeter -= difference;
        _distance -= difference;
    }
    setDistance();
    if (_countStepsForOneMeter >= _countStepsForOneMeterConst) {
        _countStepsForOneMeter = 0;
        emit changedDistance(1);
    }
    else if (_countStepsForOneMeter <= 0) {
        _countStepsForOneMeter = _countStepsForOneMeterConst;
        emit changedDistance(-1);
    }
}

void EncoderPage::onDeviceSpeed(unsigned int speedInStepsPerSecond)
{
    _lastSpeed = speedInStepsPerSecond * _stepLength * 3600 / 1000000;
    setSpeed(_lastSpeed);
    Core::instance().registration().changeLastSpeed(_lastSpeed);
    emit changedSpeed(_lastSpeed);
}

void EncoderPage::stepChanged(qreal value)
{
    _stepLength = value;
    saveEncoderCorrection(value);
    setDistance();
    setCountStepsForOneMeter(value);
}

void EncoderPage::on_resetDistanceButton_released()
{
    _distance = 0;
    _countStepsForOneMeter = 0;
    setDistance();
}
