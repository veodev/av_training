#ifndef FNVIRTUALKEYBOARD_H
#define FNVIRTUALKEYBOARD_H

#include <QWidget>

#include "virtualkeyboardbase.h"

namespace Ui
{
class FnVirtualKeyboard;
}

class FnVirtualKeyboard : public VirtualKeyboardBase
{
    Q_OBJECT

public:
    explicit FnVirtualKeyboard(QWidget* parent = 0);
    ~FnVirtualKeyboard();

    void setInputWidget(QWidget* inputWidget);
    QWidget* inputWidget();
    QPushButton* prevVirtualKeyboardPushButton();
    QPushButton* nextVirtualKeyboardPushButton();

signals:
    void switchToNextVirtualKeyboard();
    void switchToPrevVirtualKeyboard();

protected:
    bool event(QEvent* e);

private slots:
    void on_keyPrevKeyboard_clicked();
    void on_keyNextKeyboard_clicked();
    void on_keyBackspace_clicked();
    void on_keyDel_clicked();
    void on_keyInsert_clicked();
    void on_keyHome_clicked();
    void on_keyPgUp_clicked();
    void on_keyPgDn_clicked();
    void on_keyEnd_clicked();
    void on_keyLeft_clicked();
    void on_keyDown_clicked();
    void on_keyUp_clicked();
    void on_keyRight_clicked();
    void on_keySpace_clicked();
    void on_keyCopy_clicked();
    void on_keyCut_clicked();
    void on_keyPaste_clicked();

private:
    void sendKeyEvent(int keyCode, const QString& text = QString());

private:
    Ui::FnVirtualKeyboard* ui;
    QWidget* _inputWidget;
};

#endif  // FNVIRTUALKEYBOARD_H
