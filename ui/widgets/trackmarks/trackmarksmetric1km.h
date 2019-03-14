#ifndef TRACKMARKSMETRIC1KM_H
#define TRACKMARKSMETRIC1KM_H

#include "trackmarks/trackmarkspage.h"
#include "trackmarks/tmmetric1km.h"
#include <QWidget>
#include "core.h"

namespace Ui
{
class TrackMarksMetric1KM;
}

class TrackMarksMetric1KM : public TrackMarksPage
{
    Q_OBJECT

    TMMetric1KM* _trackMarks;

public:
    explicit TrackMarksMetric1KM(QWidget* parent = 0);
    ~TrackMarksMetric1KM();
    void setTrackMarks(TrackMarks* ptr);
    void saveTempTrackMarks();
    void selectTrackMark();
    void updateLabels();

protected:
    bool event(QEvent* e);

private slots:
    void on_selectButton_released();
    void onKmChanged(qreal value);
    void onMeterChanged(qreal value);

private:
    Ui::TrackMarksMetric1KM* ui;
};

#endif  // TRACKMARKSMETRIC1KM_H
