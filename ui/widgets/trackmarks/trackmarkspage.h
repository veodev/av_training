#ifndef TRACKMARKSPAGE_H
#define TRACKMARKSPAGE_H

#include "trackmarks/trackmarks.h"
#include <QWidget>

class TrackMarksPage : public QWidget
{
    Q_OBJECT
public:
    explicit TrackMarksPage(QWidget* parent = 0)
        : QWidget(parent){};
    virtual void setTrackMarks(TrackMarks* ptr) = 0;
    virtual void updateLabels() = 0;
    virtual void saveTempTrackMarks() = 0;
    virtual void selectTrackMark() = 0;

protected:
    virtual bool event(QEvent* e) = 0;

signals:
    void selected();
};

#endif  // TRACKMARKSPAGE_H
