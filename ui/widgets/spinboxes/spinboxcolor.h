#ifndef SPINBOXCOLOR_H
#define SPINBOXCOLOR_H

#include "spinboxbase.h"

class SpinBoxColor : public SpinBoxBase
{
    Q_OBJECT

    QVector<QColor> _colors;
    int _index;

public:
    explicit SpinBoxColor(QWidget* parent = 0);
    ~SpinBoxColor();
    void addColor(const QColor& color);
    void clearColors();
    void setIndex(int index);
signals:
    void colorChanged(QColor color, int index);

protected:
    virtual void displayValue();
    virtual void nextValue();
    virtual void prevValue();
    virtual bool isMinimumReached();
    virtual bool isMaximumReached();
};

#endif  // SPINBOXCOLOR_H
