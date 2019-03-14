#include "trackmarksrussianselector.h"
#include "ui_trackmarksrussianselector.h"

TrackMarksRussianSelector::TrackMarksRussianSelector(QWidget* parent)
    : TrackMarksSelector(parent)
    , ui(new Ui::TrackMarksRussianSelector)
{
    ui->setupUi(this);
    ui->kmSpinBox->setMinimum(0);
    ui->kmSpinBox->setMaximum(99999);
    ui->kmSpinBox->setEnableVirtualKeyboard(true);

    ui->picketSpinBox->setMinimum(1);
    ui->picketSpinBox->setMaximum(10);
    ui->picketSpinBox->setEnableVirtualKeyboard(true);

    ui->mSpinBox->setMinimum(0);
    ui->mSpinBox->setMaximum(999);
    ui->mSpinBox->setEnableVirtualKeyboard(true);
}

TrackMarksRussianSelector::~TrackMarksRussianSelector()
{
    delete ui;
}

void TrackMarksRussianSelector::setInputsEnabled(bool value)
{
    ui->kmSpinBox->setEnabled(value);
    ui->picketSpinBox->setEnabled(value);
    ui->mSpinBox->setEnabled(value);
}

void TrackMarksRussianSelector::readTrackMarks(TrackMarks* tm)
{
    TMRussian* tmpTm = static_cast<TMRussian*>(tm);
    ui->kmSpinBox->setValue(tmpTm->getKm());
    ui->picketSpinBox->setValue(tmpTm->getPk());
    ui->mSpinBox->setValue(tmpTm->getM());
}

void TrackMarksRussianSelector::writeTrackMarks(TrackMarks* tm)
{
    TMRussian* tmpTm = static_cast<TMRussian*>(tm);
    tmpTm->setKm(static_cast<int>(ui->kmSpinBox->value()));
    tmpTm->setPk(static_cast<int>(ui->picketSpinBox->value()));
    tmpTm->setM(static_cast<int>(ui->mSpinBox->value()));
}

void TrackMarksRussianSelector::restoreFromSettings()
{
    ui->kmSpinBox->setValue(restoreLastKm());
    ui->picketSpinBox->setValue(restoreLastPk());
    ui->mSpinBox->setValue(restoreLastM());
}

bool TrackMarksRussianSelector::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}
