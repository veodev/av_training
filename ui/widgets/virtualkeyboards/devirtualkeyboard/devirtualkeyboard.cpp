
#include <QKeyEvent>

#include "devirtualkeyboard.h"
#include "ui_devirtualkeyboard.h"

#include "debug.h"

DeVirtualKeyboard::DeVirtualKeyboard(QWidget* parent)
    : VirtualKeyboardBase(parent)
    , ui(new Ui::DeVirtualKeyboard)
    , _inputWidget(nullptr)
{
    ui->setupUi(this);

    ASSERT(connect(ui->keyShift, &StickyPushButton::keyClicked, this, &DeVirtualKeyboard::onKeyShiftClicked));
}

DeVirtualKeyboard::~DeVirtualKeyboard()
{
    delete ui;
}

void DeVirtualKeyboard::setInputWidget(QWidget* inputWidget)
{
    _inputWidget = inputWidget;
}

QWidget* DeVirtualKeyboard::inputWidget()
{
    return _inputWidget;
}

QPushButton* DeVirtualKeyboard::prevVirtualKeyboardPushButton()
{
    return ui->keyPrevKeyboard;
}

QPushButton* DeVirtualKeyboard::nextVirtualKeyboardPushButton()
{
    return ui->keyNextKeyboard;
}

void DeVirtualKeyboard::on_keyPrevKeyboard_clicked()
{
    emit switchToPrevVirtualKeyboard();
}

void DeVirtualKeyboard::on_keyNextKeyboard_clicked()
{
    emit switchToNextVirtualKeyboard();
}

void DeVirtualKeyboard::on_keyBackspace_clicked()
{
    sendKeyEvent(Qt::Key_Backspace);
}

void DeVirtualKeyboard::on_keyLeft_clicked()
{
    sendKeyEvent(Qt::Key_Left);
}

void DeVirtualKeyboard::on_keyDown_clicked()
{
    sendKeyEvent(Qt::Key_Down);
}

void DeVirtualKeyboard::on_keyUp_clicked()
{
    sendKeyEvent(Qt::Key_Up);
}

void DeVirtualKeyboard::on_keyRight_clicked()
{
    sendKeyEvent(Qt::Key_Right);
}

void DeVirtualKeyboard::on_keySpace_clicked()
{
    sendKeyEvent(Qt::Key_Space, QString(" "));
}

void DeVirtualKeyboard::on_keyEscape_clicked()
{
    sendKeyEvent(Qt::Key_Escape);
}

void DeVirtualKeyboard::on_keyReturn_clicked()
{
    sendKeyEvent(Qt::Key_Return);
}

void DeVirtualKeyboard::onKeyShiftClicked()
{
    if (ui->keyShift->isActive()) {
        ui->keyU_2->setText("Ü");
        ui->keyA_2->setText("Ä");
        ui->keyO_2->setText("Ö");
        ui->keyHeis->setText("´");
        ui->keyQ->setText("Q");
        ui->keyW->setText("W");
        ui->keyE->setText("E");
        ui->keyR->setText("R");
        ui->keyT->setText("T");
        ui->keyY->setText("Y");
        ui->keyU->setText("U");
        ui->keyI->setText("I");
        ui->keyO->setText("O");
        ui->keyP->setText("P");
        ui->keyA->setText("A");
        ui->keyS->setText("S");
        ui->keyD->setText("D");
        ui->keyF->setText("F");
        ui->keyG->setText("G");
        ui->keyH->setText("H");
        ui->keyJ->setText("J");
        ui->keyK->setText("K");
        ui->keyL->setText("L");
        ui->keyZ->setText("Z");
        ui->keyX->setText("X");
        ui->keyC->setText("C");
        ui->keyV->setText("V");
        ui->keyB->setText("B");
        ui->keyN->setText("N");
        ui->keyM->setText("M");
        ui->key0->setText(")");
        ui->key1->setText("!");
        ui->key2->setText("\"");
        ui->key3->setText("№");
        ui->key4->setText(";");
        ui->key5->setText(":");
        ui->key6->setText("_");
        ui->key7->setText("+");
        ui->key8->setText("-");
        ui->key9->setText("(");
    }
    else {
        ui->keyU_2->setText("ü");
        ui->keyA_2->setText("ä");
        ui->keyO_2->setText("ö");
        ui->keyHeis->setText("ß");
        ui->keyQ->setText("q");
        ui->keyW->setText("w");
        ui->keyE->setText("e");
        ui->keyR->setText("r");
        ui->keyT->setText("t");
        ui->keyY->setText("y");
        ui->keyU->setText("u");
        ui->keyI->setText("i");
        ui->keyO->setText("o");
        ui->keyP->setText("p");
        ui->keyA->setText("a");
        ui->keyS->setText("s");
        ui->keyD->setText("d");
        ui->keyF->setText("f");
        ui->keyG->setText("g");
        ui->keyH->setText("h");
        ui->keyJ->setText("j");
        ui->keyK->setText("k");
        ui->keyL->setText("l");
        ui->keyZ->setText("z");
        ui->keyX->setText("x");
        ui->keyC->setText("c");
        ui->keyV->setText("v");
        ui->keyB->setText("b");
        ui->keyN->setText("n");
        ui->keyM->setText("m");
        ui->key0->setText("0");
        ui->key1->setText("1");
        ui->key2->setText("2");
        ui->key3->setText("3");
        ui->key4->setText("4");
        ui->key5->setText("5");
        ui->key6->setText("6");
        ui->key7->setText("7");
        ui->key8->setText("8");
        ui->key9->setText("9");
    }
}

bool DeVirtualKeyboard::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void DeVirtualKeyboard::sendKeyEvent(int keyCode, const QString& text)
{
    Q_ASSERT(_inputWidget);
    QKeyEvent key(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
    QCoreApplication::sendEvent(_inputWidget, &key);
    QKeyEvent key2(QEvent::KeyRelease, keyCode, Qt::NoModifier, text);
    QCoreApplication::sendEvent(_inputWidget, &key2);
}
