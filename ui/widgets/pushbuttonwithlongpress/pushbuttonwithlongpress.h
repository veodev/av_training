#ifndef PUSHBUTTONWITHLONGPRESS_H
#define PUSHBUTTONWITHLONGPRESS_H

#include <QPushButton>
#include <QTimer>

class PushButtonWithLongPress : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButtonWithLongPress(QWidget * parent = 0);

signals:
    void longPress();

protected:
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);

protected slots:
    void timeout();

private:
    QTimer _timer;
    bool _isTimerOut;
};

#endif // PUSHBUTTONWITHLONGPRESS_H
