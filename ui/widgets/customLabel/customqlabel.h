#ifndef CUSTOMQLABEL_H
#define CUSTOMQLABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>
#include <QWidget>

class CustomQLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomQLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CustomQLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* me);
};

#endif  // CUSTOMQLABEL_H
