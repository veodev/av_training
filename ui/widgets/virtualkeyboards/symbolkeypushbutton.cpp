#include <QCoreApplication>
#include <QKeyEvent>

#include "symbolkeypushbutton.h"
#include "virtualkeyboardbase.h"
#include "debug.h"

SymbolKeyPushButton::SymbolKeyPushButton(QWidget* parent)
    : KeyPushButton(parent)
{
    ASSERT(connect(this, &SymbolKeyPushButton::pressed, this, &SymbolKeyPushButton::getKeyPress));
}

void SymbolKeyPushButton::getKeyPress()
{
    VirtualKeyboardBase* keyboard = reinterpret_cast<VirtualKeyboardBase*>(this->parent());
    if (keyboard != 0) {
        QWidget* inputWidget = keyboard->inputWidget();
        if (inputWidget != 0) {
            const QString& text = this->text();
            int keyCode = text.toLocal8Bit()[0];
            keyCode += 32;
            QKeyEvent key(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
            QCoreApplication::sendEvent(inputWidget, &key);
        }
    }
}
