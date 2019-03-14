#ifndef MESSAGEPAGE_H
#define MESSAGEPAGE_H

#include <QWidget>

#include "closure.h"

namespace Ui
{
class MessagePage;
}

class MessagePage : public QWidget
{
    Q_OBJECT

public:
    explicit MessagePage(QWidget* parent = 0);
    ~MessagePage();

    void setInfoLabel(const QString& info);
    void clearInfoLabel();

    template <typename Object, typename Method>
    void setCallback(Object* object, Method method)
    {
        if (_closure != 0) {
            delete _closure;
        }
        _closure = new Closure<Object>(object, method);
    }

signals:
    void leaveTheWidget();

protected:
    bool event(QEvent* e);

private slots:
    void on_okButton_released();
    void on_cancelButton_released();

private:
    Ui::MessagePage* ui;

    ClosureBase* _closure;
};

#endif  // MESSAGEPAGE_H
