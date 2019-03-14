#ifndef TRACKMARKSMETRICSELECTOR_H
#define TRACKMARKSMETRICSELECTOR_H

#include "trackmarksselector.h"
#include "trackmarks/tmmetric1km.h"

namespace Ui
{
class TrackMarksMetricSelector;
}

class TrackMarksMetricSelector : public TrackMarksSelector
{
    Q_OBJECT

public:
    explicit TrackMarksMetricSelector(QWidget* parent = 0);
    ~TrackMarksMetricSelector();
    void writeTrackMarks(TrackMarks* tm);
    void readTrackMarks(TrackMarks* tm);
    void setInputsEnabled(bool value);
    void restoreFromSettings();

protected:
    bool event(QEvent* e);

private:
    Ui::TrackMarksMetricSelector* ui;
};

#endif  // TRACKMARKSMETRICSELECTOR_H
