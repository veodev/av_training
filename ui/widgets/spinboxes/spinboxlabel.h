#ifndef SPINBOXLABEL_H
#define SPINBOXLABEL_H

#include <QLabel>

class SpinBoxLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SpinBoxLabel(QWidget* parent = 0);
    ~SpinBoxLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif  // SPINBOXLABEL_H
