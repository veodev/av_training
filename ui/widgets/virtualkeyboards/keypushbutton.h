#ifndef KEYPUSHBUTTON_H
#define KEYPUSHBUTTON_H

#include <QPushButton>

class KeyPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit KeyPushButton(QWidget* parent = 0);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif  // KEYPUSHBUTTON_H
