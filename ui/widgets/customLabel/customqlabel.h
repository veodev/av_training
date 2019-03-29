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

    void setIndexAndSide(int index, int side);
    int getIndex();
    int getSide();
    void blockControl(bool isBlock);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* me);

    int _index;
    int _side;
    bool _isBlock;
};

#endif  // CUSTOMQLABEL_H
