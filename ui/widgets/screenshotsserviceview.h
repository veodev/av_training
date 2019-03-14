#ifndef SCREENSHOTSSERVICEVIEW_H
#define SCREENSHOTSSERVICEVIEW_H

#include <QWidget>

namespace Ui
{
class ScreenShotsServiceView;
}

class QLabel;

class ScreenShotsServiceView : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenShotsServiceView(QWidget* parent = 0);
    ~ScreenShotsServiceView();

    void showScreen(QString& path) const;

protected:
    void mousePressEvent(QMouseEvent* e);

private:
    Ui::ScreenShotsServiceView* ui;
    QLabel* _dateLabel;
};

#endif  // SCREENSHOTSSERVICEVIEW_H
