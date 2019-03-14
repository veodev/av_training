#ifndef CONTROLSAREA_H
#define CONTROLSAREA_H

#include <QWidget>

namespace Ui {
class ControlsArea;
}

class QScrollBar;

class ControlsArea : public QWidget
{
    Q_OBJECT

public:
    explicit ControlsArea(QWidget * parent = 0);
    ~ControlsArea();
    void addWidget(QWidget * widget);
    void reset();

public slots:
    void setVisible(bool visible);

private:
    void addSpacer();

private slots:
    void scrollBarValueChanged(int value);
    void scrollBarRangeChanged(int min, int max);
    void controlsWidgedWidthChanged(int width);

    void on_scrollUpButton_released();
    void on_scrollDownButton_released();

private:
    Ui::ControlsArea * ui;
};

#endif // CONTROLSAREA_H
