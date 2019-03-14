#include "trackmarksmetricselector.h"
#include "ui_trackmarksmetricselector.h"

TrackMarksMetricSelector::TrackMarksMetricSelector(QWidget* parent)
    : TrackMarksSelector(parent)
    , ui(new Ui::TrackMarksMetricSelector)
{
    ui->setupUi(this);
    ui->kmSpinBox->setMinimum(-99999);
    ui->kmSpinBox->setMaximum(99999);
    ui->kmSpinBox->setZeroPrefix(true);
    ui->kmSpinBox->setZeroPrefixSize(3);
    ui->kmSpinBox->setValue(0);
    ui->kmSpinBox->setEnableVirtualKeyboard(true);

    ui->mSpinBox->setMinimum(0);
    ui->mSpinBox->setMaximum(999);
    ui->mSpinBox->setZeroPrefix(true);
    ui->mSpinBox->setZeroPrefixSize(3);
    ui->mSpinBox->setValue(0);
    ui->mSpinBox->setEnableVirtualKeyboard(true);
}

TrackMarksMetricSelector::~TrackMarksMetricSelector()
{
    delete ui;
}

void TrackMarksMetricSelector::writeTrackMarks(TrackMarks* tm)
{
    tm->syncPole(static_cast<int>(ui->kmSpinBox->value()), static_cast<int>(ui->mSpinBox->value()));
}

void TrackMarksMetricSelector::readTrackMarks(TrackMarks* tm)
{
    TMMetric1KM* tmpTm = static_cast<TMMetric1KM*>(tm);
    ui->kmSpinBox->setValue(tmpTm->getKm());
    ui->mSpinBox->setValue(tmpTm->getM());
}

void TrackMarksMetricSelector::setInputsEnabled(bool value)
{
    ui->kmSpinBox->setEnabled(value);
    ui->mSpinBox->setEnabled(value);
}

void TrackMarksMetricSelector::restoreFromSettings()
{
}

bool TrackMarksMetricSelector::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}
