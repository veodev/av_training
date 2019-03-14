#ifndef RUVIRTUALKEYBOARD_H
#define RUVIRTUALKEYBOARD_H

#include <QWidget>

#include "virtualkeyboardbase.h"

namespace Ui
{
class RuVirtualKeyboard;
}

class RuVirtualKeyboard : public VirtualKeyboardBase
{
    Q_OBJECT

public:
    explicit RuVirtualKeyboard(QWidget* parent = 0);
    ~RuVirtualKeyboard();

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
    void on_keySpace_clicked();
    void on_keyEscape_clicked();
    void on_keyBackspace_clicked();
    void on_keyLeft_clicked();
    void on_keyDown_clicked();
    void on_keyUp_clicked();
    void on_keyRight_clicked();
    void on_keyReturn_clicked();
    void onKeyShiftClicked();

private:
    void sendKeyEvent(int keyCode, const QString& text = QString());

private:
    Ui::RuVirtualKeyboard* ui;
    QWidget* _inputWidget;
};

#endif  // RUVIRTUALKEYBOARD_H
