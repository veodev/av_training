#include "trackmarksmetric1km.h"
#include "ui_trackmarksmetric1km.h"
#include "debug.h"
#include "registration.h"

TrackMarksMetric1KM::TrackMarksMetric1KM(QWidget* parent)
    : TrackMarksPage(parent)
    , ui(new Ui::TrackMarksMetric1KM)
{
    ui->setupUi(this);
    ASSERT(connect(ui->kmSpinBox, &SpinBoxNumber::valueChanged, this, &TrackMarksMetric1KM::onKmChanged));
    ASSERT(connect(ui->mSpinBox, &SpinBoxNumber::valueChanged, this, &TrackMarksMetric1KM::onMeterChanged));
    ui->kmSpinBox->setMinimum(-99999);
    ui->kmSpinBox->setMaximum(99999);
    ui->kmSpinBox->setZeroPrefix(true);
    ui->kmSpinBox->setZeroPrefixSize(3);
    ui->kmSpinBox->setEnableVirtualKeyboard(true);

    ui->mSpinBox->setMinimum(0);
    ui->mSpinBox->setMaximum(999);
    ui->mSpinBox->setZeroPrefix(true);
    ui->mSpinBox->setZeroPrefixSize(3);
    ui->mSpinBox->setEnableVirtualKeyboard(true);
}

TrackMarksMetric1KM::~TrackMarksMetric1KM()
{
    delete ui;
}

void TrackMarksMetric1KM::setTrackMarks(TrackMarks* ptr)
{
    _trackMarks = static_cast<TMMetric1KM*>(ptr);
    updateLabels();
}

void TrackMarksMetric1KM::saveTempTrackMarks()
{
}

void TrackMarksMetric1KM::selectTrackMark()
{
}

void TrackMarksMetric1KM::updateLabels()
{
    ui->kmSpinBox->setValue(_trackMarks->getKm());
    ui->mSpinBox->setValue(_trackMarks->getM());
}

bool TrackMarksMetric1KM::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void TrackMarksMetric1KM::on_selectButton_released()
{
    emit selected();
    _trackMarks->syncPole(static_cast<int>(ui->kmSpinBox->value()), static_cast<int>(ui->mSpinBox->value()));
    Core::instance().registration().setCaPost(_trackMarks);
}

void TrackMarksMetric1KM::onKmChanged(qreal value)
{
    _trackMarks->syncPole(static_cast<int>(value), static_cast<int>(ui->mSpinBox->value()));
}

void TrackMarksMetric1KM::onMeterChanged(qreal value)
{
    _trackMarks->syncPole(static_cast<int>(ui->kmSpinBox->value()), static_cast<int>(value));
}
