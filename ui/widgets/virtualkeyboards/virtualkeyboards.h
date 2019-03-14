#ifndef VIRTUALKEYBOARDS_H
#define VIRTUALKEYBOARDS_H

#include <QWidget>

#include "closure.h"

namespace Ui
{
class VirtualKeyboards;
}

typedef void (*FunctionPtr)(void*);
typedef void* ObjectPtr;

class VirtualKeyboards : public QWidget
{
    Q_OBJECT

public:
    static VirtualKeyboards* instance();

    explicit VirtualKeyboards(QWidget* parent = 0);
    ~VirtualKeyboards();

    template <typename Object, typename Method>
    void setCallback(Object* object, Method method)
    {
        if (_closure != 0) {
            delete _closure;
        }
        _closure = new Closure<Object>(object, method);
    }

    enum InputWidgetType
    {
        LineEdit,
        PlainTextEdit
    };

    enum Validators
    {
        None,
        Numbers,
        EnglishLetters,
        IpAddress,
        NumbersLimiting,
        Date,
    };

    void setInputWidgetType(InputWidgetType editorType);

    void clear();
    QString value() const;
    void setValue(const QString& value);
    void setValidator(Validators validator, int maxValue = 0);
    void leave();

public slots:
    void setVisible(bool visible);

protected:
    bool event(QEvent* e);

private:
    void setupUi();
    void retranslateUi();
    void setInputWidget();
    void setupLanguageSwichButtons();

private slots:
    void switchToPrevVirtualKeyboard();
    void switchToNextVirtualKeyboard();
    void on_doneButton_released();

private:
    Ui::VirtualKeyboards* ui;

    ClosureBase* _closure;

    InputWidgetType _inputWidgetType;
};

#endif  // VIRTUALKEYBOARDS_H
