#ifndef TRACKMARKSSELECTOR_H
#define TRACKMARKSSELECTOR_H
#include "trackmarks.h"
#include "settings.h"
#include <QWidget>

class TrackMarksSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TrackMarksSelector(QWidget* parent = 0)
        : QWidget(parent){};
    virtual void setInputsEnabled(bool value) = 0;
    virtual void writeTrackMarks(TrackMarks* tm) = 0;
    virtual void readTrackMarks(TrackMarks* tm) = 0;
    virtual void restoreFromSettings() = 0;
};

#endif  // TRACKMARKSSELECTOR_H
