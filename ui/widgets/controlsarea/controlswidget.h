#ifndef CONTROLSWIDGET_H
#define CONTROLSWIDGET_H

#include <QWidget>

class ControlsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlsWidget(QWidget * parent = 0);
    ~ControlsWidget();

signals:
    void widthChanged(int width);

protected:
    void resizeEvent(QResizeEvent * event);
};

#endif // CONTROLSWIDGET_H
