#ifndef TRACKMARKSRUSSIANSELECTOR_H
#define TRACKMARKSRUSSIANSELECTOR_H

#include "trackmarksselector.h"
#include "trackmarks/tmrussian.h"

namespace Ui
{
class TrackMarksRussianSelector;
}

class TrackMarksRussianSelector : public TrackMarksSelector
{
    Q_OBJECT

public:
    explicit TrackMarksRussianSelector(QWidget* parent = 0);
    ~TrackMarksRussianSelector();
    void setInputsEnabled(bool value);
    void readTrackMarks(TrackMarks* tm);
    void writeTrackMarks(TrackMarks* tm);
    void restoreFromSettings();

protected:
    bool event(QEvent* e);

private:
    Ui::TrackMarksRussianSelector* ui;
};

#endif  // TRACKMARKSRUSSIANSELECTOR_H
