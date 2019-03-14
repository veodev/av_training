#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>

class QMouseEvent;

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = 0);
    ~StatusBar();

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif  // STATUSBAR_H
