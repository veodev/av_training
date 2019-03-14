#ifndef STICKYPUSHBUTTON_H
#define STICKYPUSHBUTTON_H

#include <QPushButton>

class StickyPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit StickyPushButton(QWidget* parent = 0);
    bool isActive();

signals:
    void keyClicked();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private slots:
    virtual void getKeyPress();

private:
    bool _isActive;
};

#endif  // STICKYPUSHBUTTON_H
