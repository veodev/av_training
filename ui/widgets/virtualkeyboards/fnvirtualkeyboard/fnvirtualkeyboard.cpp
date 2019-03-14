#include <QKeyEvent>
#include <QLineEdit>

#include "fnvirtualkeyboard.h"
#include "ui_fnvirtualkeyboard.h"

FnVirtualKeyboard::FnVirtualKeyboard(QWidget* parent)
    : VirtualKeyboardBase(parent)
    , ui(new Ui::FnVirtualKeyboard)
    , _inputWidget(nullptr)
{
    ui->setupUi(this);
}

FnVirtualKeyboard::~FnVirtualKeyboard()
{
    delete ui;
}

void FnVirtualKeyboard::setInputWidget(QWidget* inputWidget)
{
    _inputWidget = inputWidget;
}

QWidget* FnVirtualKeyboard::inputWidget()
{
    return _inputWidget;
}

QPushButton* FnVirtualKeyboard::prevVirtualKeyboardPushButton()
{
    return ui->keyPrevKeyboard;
}

QPushButton* FnVirtualKeyboard::nextVirtualKeyboardPushButton()
{
    return ui->keyNextKeyboard;
}

bool FnVirtualKeyboard::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void FnVirtualKeyboard::on_keyPrevKeyboard_clicked()
{
    emit switchToPrevVirtualKeyboard();
}

void FnVirtualKeyboard::on_keyNextKeyboard_clicked()
{
    emit switchToNextVirtualKeyboard();
}

void FnVirtualKeyboard::on_keyBackspace_clicked()
{
    sendKeyEvent(Qt::Key_Backspace);
}

void FnVirtualKeyboard::on_keyDel_clicked()
{
    sendKeyEvent(Qt::Key_Delete);
}

void FnVirtualKeyboard::on_keyInsert_clicked()
{
    sendKeyEvent(Qt::Key_Insert);
}

void FnVirtualKeyboard::on_keyHome_clicked()
{
    sendKeyEvent(Qt::Key_Home);
}

void FnVirtualKeyboard::on_keyPgUp_clicked()
{
    sendKeyEvent(Qt::Key_PageUp);
}

void FnVirtualKeyboard::on_keyPgDn_clicked()
{
    sendKeyEvent(Qt::Key_PageDown);
}

void FnVirtualKeyboard::on_keyEnd_clicked()
{
    sendKeyEvent(Qt::Key_End);
}

void FnVirtualKeyboard::on_keyLeft_clicked()
{
    sendKeyEvent(Qt::Key_Left);
}

void FnVirtualKeyboard::on_keyDown_clicked()
{
    sendKeyEvent(Qt::Key_Down);
}

void FnVirtualKeyboard::on_keyUp_clicked()
{
    sendKeyEvent(Qt::Key_Up);
}

void FnVirtualKeyboard::on_keyRight_clicked()
{
    sendKeyEvent(Qt::Key_Right);
}

void FnVirtualKeyboard::on_keySpace_clicked()
{
    sendKeyEvent(Qt::Key_Space, QString(" "));
}

void FnVirtualKeyboard::on_keyCopy_clicked()
{
    QLineEdit* inputWidget = qobject_cast<QLineEdit*>(_inputWidget);
    Q_ASSERT(inputWidget);
    if (inputWidget) {
        inputWidget->copy();
    }
}

void FnVirtualKeyboard::on_keyCut_clicked()
{
    QLineEdit* inputWidget = qobject_cast<QLineEdit*>(_inputWidget);
    Q_ASSERT(inputWidget);
    if (inputWidget) {
        inputWidget->cut();
    }
}

void FnVirtualKeyboard::on_keyPaste_clicked()
{
    QLineEdit* inputWidget = qobject_cast<QLineEdit*>(_inputWidget);
    Q_ASSERT(inputWidget);
    if (inputWidget) {
        inputWidget->paste();
    }
}

void FnVirtualKeyboard::sendKeyEvent(int keyCode, const QString& text)
{
    QKeyEvent key(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
    Q_ASSERT(_inputWidget);
    QCoreApplication::sendEvent(_inputWidget, &key);
}
