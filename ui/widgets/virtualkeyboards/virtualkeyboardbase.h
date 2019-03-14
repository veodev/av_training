#ifndef VIRTUALKEYBOARDBASE_H
#define VIRTUALKEYBOARDBASE_H
#include <QWidget>
// class QWidget;
class QPushButton;

class VirtualKeyboardBase : public QWidget
{
public:
    VirtualKeyboardBase(QWidget* parent = 0)
        : QWidget(parent)
    {
    }
    virtual void setInputWidget(QWidget* inputWidget) = 0;
    virtual QWidget* inputWidget() = 0;
    virtual QPushButton* prevVirtualKeyboardPushButton() = 0;
    virtual QPushButton* nextVirtualKeyboardPushButton() = 0;
};

#endif  // VIRTUALKEYBOARDBASE_H
