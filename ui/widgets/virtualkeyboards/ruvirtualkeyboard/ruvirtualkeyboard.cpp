#include <QKeyEvent>

#include "ruvirtualkeyboard.h"
#include "ui_ruvirtualkeyboard.h"

#include "debug.h"

RuVirtualKeyboard::RuVirtualKeyboard(QWidget* parent)
    : VirtualKeyboardBase(parent)
    , ui(new Ui::RuVirtualKeyboard)
    , _inputWidget(nullptr)
{
    ui->setupUi(this);

    ASSERT(connect(ui->keyShift, &StickyPushButton::keyClicked, this, &RuVirtualKeyboard::onKeyShiftClicked));
}

RuVirtualKeyboard::~RuVirtualKeyboard()
{
    delete ui;
}

void RuVirtualKeyboard::setInputWidget(QWidget* inputWidget)
{
    _inputWidget = inputWidget;
}

QWidget* RuVirtualKeyboard::inputWidget()
{
    return _inputWidget;
}

QPushButton* RuVirtualKeyboard::prevVirtualKeyboardPushButton()
{
    return ui->keyPrevKeyboard;
}

QPushButton* RuVirtualKeyboard::nextVirtualKeyboardPushButton()
{
    return ui->keyNextKeyboard;
}

bool RuVirtualKeyboard::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void RuVirtualKeyboard::on_keyPrevKeyboard_clicked()
{
    emit switchToPrevVirtualKeyboard();
}

void RuVirtualKeyboard::on_keyNextKeyboard_clicked()
{
    emit switchToNextVirtualKeyboard();
}

void RuVirtualKeyboard::on_keySpace_clicked()
{
    sendKeyEvent(Qt::Key_Space, QString(" "));
}

void RuVirtualKeyboard::on_keyEscape_clicked()
{
    sendKeyEvent(Qt::Key_Escape);
}

void RuVirtualKeyboard::on_keyBackspace_clicked()
{
    sendKeyEvent(Qt::Key_Backspace);
}

void RuVirtualKeyboard::on_keyLeft_clicked()
{
    sendKeyEvent(Qt::Key_Left);
}

void RuVirtualKeyboard::on_keyDown_clicked()
{
    sendKeyEvent(Qt::Key_Down);
}

void RuVirtualKeyboard::on_keyUp_clicked()
{
    sendKeyEvent(Qt::Key_Up);
}

void RuVirtualKeyboard::on_keyRight_clicked()
{
    sendKeyEvent(Qt::Key_Right);
}

void RuVirtualKeyboard::on_keyReturn_clicked()
{
    sendKeyEvent(Qt::Key_Return);
}

void RuVirtualKeyboard::onKeyShiftClicked()
{
    if (ui->keyShift->isActive()) {
        ui->keyY->setText("Й");
        ui->keyC->setText("Ц");
        ui->keyU->setText("У");
        ui->keyK->setText("К");
        ui->keyE->setText("Е");
        ui->keyN->setText("Н");
        ui->keyG->setText("Г");
        ui->keySh->setText("Ш");
        ui->keySch->setText("Щ");
        ui->keyZ->setText("З");
        ui->keyH->setText("Х");
        ui->keyHardSign->setText("Ъ");
        ui->keyF->setText("Ф");
        ui->keyYy->setText("Ы");
        ui->keyV->setText("В");
        ui->keyA->setText("А");
        ui->keyP->setText("П");
        ui->keyR->setText("Р");
        ui->keyO->setText("О");
        ui->keyL->setText("Л");
        ui->keyD->setText("Д");
        ui->keyZh->setText("Ж");
        ui->keyEe->setText("Э");
        ui->keyYa->setText("Я");
        ui->keyCh->setText("Ч");
        ui->keyS->setText("С");
        ui->keyM->setText("М");
        ui->keyI->setText("И");
        ui->keyT->setText("Т");
        ui->keySoftSign->setText("Ь");
        ui->keyB->setText("Б");
        ui->keyYu->setText("Ю");
        ui->keyYo->setText("Ё");
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
        ui->keyY->setText("й");
        ui->keyC->setText("ц");
        ui->keyU->setText("у");
        ui->keyK->setText("к");
        ui->keyE->setText("е");
        ui->keyN->setText("н");
        ui->keyG->setText("г");
        ui->keySh->setText("ш");
        ui->keySch->setText("щ");
        ui->keyZ->setText("з");
        ui->keyH->setText("х");
        ui->keyHardSign->setText("ъ");
        ui->keyF->setText("ф");
        ui->keyYy->setText("ы");
        ui->keyV->setText("в");
        ui->keyA->setText("а");
        ui->keyP->setText("п");
        ui->keyR->setText("р");
        ui->keyO->setText("о");
        ui->keyL->setText("л");
        ui->keyD->setText("д");
        ui->keyZh->setText("ж");
        ui->keyEe->setText("э");
        ui->keyYa->setText("я");
        ui->keyCh->setText("ч");
        ui->keyS->setText("с");
        ui->keyM->setText("м");
        ui->keyI->setText("и");
        ui->keyT->setText("т");
        ui->keySoftSign->setText("ь");
        ui->keyB->setText("б");
        ui->keyYu->setText("ю");
        ui->keyYo->setText("ё");
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

void RuVirtualKeyboard::sendKeyEvent(int keyCode, const QString& text)
{
    Q_ASSERT(_inputWidget);
    QKeyEvent key(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
    QCoreApplication::sendEvent(_inputWidget, &key);
    QKeyEvent key2(QEvent::KeyRelease, keyCode, Qt::NoModifier, text);
    QCoreApplication::sendEvent(_inputWidget, &key2);
}
