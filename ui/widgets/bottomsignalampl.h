#ifndef BOTTOMSIGNALAMPL_H
#define BOTTOMSIGNALAMPL_H

#include <QWidget>
#include <QPainter>

class BottomSignalAmplView : public QWidget
{
    Q_OBJECT

public:
    explicit BottomSignalAmplView(QWidget* parent = 0);
    ~BottomSignalAmplView();

    void setSignalAmplValue(int value);

protected:
    void paintEvent(QPaintEvent* event);

private:
    int _amplValue;
    QRect _rect;
};

#endif  // BOTTOMSIGNALAMPL_H
