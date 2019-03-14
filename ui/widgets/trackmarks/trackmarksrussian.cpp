#include "trackmarksrussian.h"
#include "ui_trackmarksrussian.h"
#include "settings.h"
#include "registration.h"

TrackMarksRussian::TrackMarksRussian(QWidget* parent)
    : TrackMarksPage(parent)
    , ui(new Ui::TrackMarksRussian)
{
    ui->setupUi(this);
    _trackMarks = 0;
}

TrackMarksRussian::~TrackMarksRussian()
{
    delete ui;
}

void TrackMarksRussian::setTrackMarks(TrackMarks* ptr)
{
    _trackMarks = static_cast<TMRussian*>(ptr);
    updateLabels();
}

void TrackMarksRussian::updateLabels()
{
    _tempTrackMarks.updatePost();
    if (_tempTrackMarks.getPostKm(0) == _tempTrackMarks.getPostKm(1)) {
        ui->kmLabel->setText(QString::number(_tempTrackMarks.getPostKm(0)));
    }
    else {
        ui->kmLabel->setText(QString::number(_tempTrackMarks.getPostKm(0)) + " / " + QString::number(_tempTrackMarks.getPostKm(1)));
    }
    ui->picketLabel->setText(QString::number(_tempTrackMarks.getPostPk(0)) + " / " + QString::number(_tempTrackMarks.getPostPk(1)));
}

void TrackMarksRussian::saveTempTrackMarks()
{
    if (_trackMarks != 0) {
        _tempTrackMarks = *_trackMarks;
        _tempTrackMarks.next();
        updateLabels();
    }
}

void TrackMarksRussian::selectTrackMark()
{
    on_selectTrackMarkButton_released();
}

bool TrackMarksRussian::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void TrackMarksRussian::on_nexTrackMarkButton_released()
{
    _tempTrackMarks.next();
    updateLabels();
}

void TrackMarksRussian::on_prevTrackMarkButton_released()
{
    _tempTrackMarks.prev();
    updateLabels();
}

void TrackMarksRussian::on_selectTrackMarkButton_released()
{
    *_trackMarks = _tempTrackMarks;
    _trackMarks->resetMeter();
    emit selected();
    if (_trackMarks != 0) {
        Core::instance().registration().setMrfPost(_trackMarks);
        saveLastKm(_trackMarks->getKm());
        saveLastPk(_trackMarks->getPk());
    }
}
