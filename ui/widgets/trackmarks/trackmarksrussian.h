#ifndef TRACKMARKSRUSSIAN_H
#define TRACKMARKSRUSSIAN_H
#include "tmrussian.h"
#include "trackmarks/trackmarkspage.h"
#include <QWidget>
#include "core.h"

namespace Ui
{
class TrackMarksRussian;
}

class TrackMarksRussian : public TrackMarksPage
{
    Q_OBJECT

    TMRussian* _trackMarks;

public:
    explicit TrackMarksRussian(QWidget* parent = 0);
    ~TrackMarksRussian();
    void setTrackMarks(TrackMarks* ptr);
    void updateLabels();
    void saveTempTrackMarks();
    void selectTrackMark();

protected:
    bool event(QEvent* e);

private slots:
    void on_nexTrackMarkButton_released();
    void on_prevTrackMarkButton_released();
    void on_selectTrackMarkButton_released();

private:
    Ui::TrackMarksRussian* ui;

    TMRussian _tempTrackMarks;
};

#endif  // TRACKMARKSRUSSIAN_H
