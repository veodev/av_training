#ifndef ENVIRTUALKEYBOARD_H
#define ENVIRTUALKEYBOARD_H

#include <QWidget>

#include "virtualkeyboardbase.h"

namespace Ui
{
class EnVirtualKeyboard;
}

class EnVirtualKeyboard : public VirtualKeyboardBase
{
    Q_OBJECT

public:
    explicit EnVirtualKeyboard(QWidget* parent = 0);
    ~EnVirtualKeyboard();

    void setInputWidget(QWidget* inputWidget);
    QWidget* inputWidget();
    QPushButton* prevVirtualKeyboardPushButton();
    QPushButton* nextVirtualKeyboardPushButton();

signals:
    void switchToNextVirtualKeyboard();
    void switchToPrevVirtualKeyboard();

private slots:
    void on_keyPrevKeyboard_clicked();
    void on_keyNextKeyboard_clicked();
    void on_keyBackspace_clicked();
    void on_keyLeft_clicked();
    void on_keyDown_clicked();
    void on_keyUp_clicked();
    void on_keyRight_clicked();
    void on_keySpace_clicked();
    void on_keyEscape_clicked();
    void on_keyReturn_clicked();
    void onKeyShiftClicked();

protected:
    bool event(QEvent* e);

private:
    void sendKeyEvent(int keyCode, const QString& text = QString());

private:
    Ui::EnVirtualKeyboard* ui;
    QWidget* _inputWidget;
};

#endif  // ENVIRTUALKEYBOARD_H
