#include "scannerencoderspage.h"
#include "ui_scannerencoderspage.h"
#include "settings.h"
#include "debug.h"
#include "core.h"
#include <cmath>
#include "direction.h"
#include "Device_definitions.h"

ScannerEncodersPage::ScannerEncodersPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScannerEncodersPage)
    , _horizontalDistance(0)
    , _verticalDistance(0)
    , _prevHorizontalSysCoordinat(0)
    , _prevVerticalSysCoordinat(0)
    , _horizontalStep(restoreHorizontalScannerEncoderStep())
    , _verticalStep(restoreVerticalScannerEncoderStep())
{
    ui->setupUi(this);
    setupUi();
    retranslateUi();
    on_resetHorizontalDistanceButton_released();
    on_resetVerticalDistanceButton_released();
    Core& core = Core::instance();
    ASSERT(connect(&core, &Core::doPathStepData, this, &ScannerEncodersPage::onPathStepData));
    ASSERT(connect(&core, &Core::doBScan2Data, this, &ScannerEncodersPage::onBScanData));
}

ScannerEncodersPage::~ScannerEncodersPage()
{
    delete ui;
}

void ScannerEncodersPage::resetEncodersDistances()
{
    on_resetHorizontalDistanceButton_released();
    on_resetVerticalDistanceButton_released();
}

bool ScannerEncodersPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void ScannerEncodersPage::setupUi()
{
    ui->horizontalStepSpinBox->setPrecision(3);
    ui->horizontalStepSpinBox->setMinimum(0.01f);
    ui->horizontalStepSpinBox->setMaximum(100);
    ui->horizontalStepSpinBox->setValue(static_cast<float>(_horizontalStep), false);
    ui->horizontalStepSpinBox->setStepBy(0.01f);
    ui->horizontalStepSpinBox->setValueFontSize(25);
    ASSERT(connect(ui->horizontalStepSpinBox, &SpinBoxNumber::valueChanged, this, &ScannerEncodersPage::onHorizontalSpinBoxChanged));

    ui->verticalStepSpinBox->setPrecision(3);
    ui->verticalStepSpinBox->setMinimum(0.01f);
    ui->verticalStepSpinBox->setMaximum(100);
    ui->verticalStepSpinBox->setValue(static_cast<float>(_verticalStep), false);
    ui->verticalStepSpinBox->setStepBy(0.01f);
    ui->verticalStepSpinBox->setValueFontSize(25);
    ASSERT(connect(ui->verticalStepSpinBox, &SpinBoxNumber::valueChanged, this, &ScannerEncodersPage::onVerticalSpinBoxChanged));
}

void ScannerEncodersPage::retranslateUi()
{
    ui->horizontalStepSpinBox->setSuffix(tr(" mm"));
    ui->verticalStepSpinBox->setSuffix(tr(" mm"));
    on_resetHorizontalDistanceButton_released();
    on_resetVerticalDistanceButton_released();
}

void ScannerEncodersPage::setHorizontalDistance()
{
    ui->horizontalDistanceLabel->setText(QString::number((_horizontalStep * _horizontalDistance), 'f', 1));
}

void ScannerEncodersPage::setVerticalDistance()
{
    ui->verticalDistanceLabel->setText(QString::number((_verticalStep * _verticalDistance), 'f', 1));
}

void ScannerEncodersPage::on_resetHorizontalDistanceButton_released()
{
    _horizontalDistance = 0;
    setHorizontalDistance();
}

void ScannerEncodersPage::on_resetVerticalDistanceButton_released()
{
    _verticalDistance = 0;
    setVerticalDistance();
}

void ScannerEncodersPage::onHorizontalSpinBoxChanged(qreal step)
{
    _horizontalStep = step;
    saveHorizontalScannerEncoderStep(_horizontalStep);
    setHorizontalDistance();
}

void ScannerEncodersPage::onVerticalSpinBoxChanged(qreal step)
{
    _verticalStep = step;
    saveVerticalScannerEncoderStep(_verticalStep);
    setVerticalDistance();
}

void ScannerEncodersPage::onPathStepData(QSharedPointer<tDEV_PathStepData> data)
{
    if (!(this->isVisible())) {
        return;
    }
    Q_ASSERT(data.data() != 0);
    long int currentVerticalSysCoordinat = data.data()->XSysCrd[2];
    if (Q_UNLIKELY(_prevVerticalSysCoordinat == 0)) {
        _prevVerticalSysCoordinat = currentVerticalSysCoordinat;
    }

    int differenceVertical = std::abs(currentVerticalSysCoordinat - _prevVerticalSysCoordinat);
    _prevVerticalSysCoordinat = currentVerticalSysCoordinat;
    Direction directionVertical = directionByProbe(data.data()->Dir[2]);
    switch (directionVertical) {
    case ForwardDirection:
        _verticalDistance += differenceVertical;
        break;
    case BackwardDirection:
        _verticalDistance -= differenceVertical;
        break;
    case UnknownDirection:
        break;
    }
    setVerticalDistance();
}

void ScannerEncodersPage::onBScanData(QSharedPointer<tDEV_BScan2Head> head)
{
    if (!(this->isVisible())) {
        return;
    }
    Q_ASSERT(head.data() != 0);
    if (head.data()->PathEncodersIndex == 0) {
        return;
    }
    if (head.data()->ChannelType == ctHandScan) {
        return;
    }
    Q_ASSERT(head.data() != 0);

    long int currentHorizontalSysCoordinat = head.data()->XSysCrd[1];

    if (Q_UNLIKELY(_prevHorizontalSysCoordinat == 0)) {
        _prevHorizontalSysCoordinat = currentHorizontalSysCoordinat;
    }

    int differenceHorizontal = std::abs(currentHorizontalSysCoordinat - _prevHorizontalSysCoordinat);
    _prevHorizontalSysCoordinat = currentHorizontalSysCoordinat;
    Direction directionHorizontal = directionByProbe(head.data()->Dir[1]);

    switch (directionHorizontal) {
    case ForwardDirection:
        _horizontalDistance -= differenceHorizontal;  // depend on construction
        break;
    case BackwardDirection:
        _horizontalDistance += differenceHorizontal;
        break;
    case UnknownDirection:
        break;
    }
    setHorizontalDistance();
}
