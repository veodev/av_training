#include "virtualkeyboards.h"
#include "ui_virtualkeyboards.h"
#include "debug.h"
#include "settings.h"

#include <QRegExpValidator>

VirtualKeyboards* VirtualKeyboards::instance()
{
    static VirtualKeyboards* _instance = nullptr;
    if (_instance == nullptr) {
        _instance = new VirtualKeyboards;
    }

    return _instance;
}

VirtualKeyboards::VirtualKeyboards(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VirtualKeyboards)
    , _closure(nullptr)
{
    setupUi();
    setVisible(false);
    setInputWidgetType(LineEdit);

    ASSERT(connect(ui->fnVirtualKeyboard, &FnVirtualKeyboard::switchToPrevVirtualKeyboard, this, &VirtualKeyboards::switchToPrevVirtualKeyboard));
    ASSERT(connect(ui->fnVirtualKeyboard, &FnVirtualKeyboard::switchToNextVirtualKeyboard, this, &VirtualKeyboards::switchToNextVirtualKeyboard));

    ASSERT(connect(ui->enVirtualKeyboard, &EnVirtualKeyboard::switchToPrevVirtualKeyboard, this, &VirtualKeyboards::switchToPrevVirtualKeyboard));
    ASSERT(connect(ui->enVirtualKeyboard, &EnVirtualKeyboard::switchToNextVirtualKeyboard, this, &VirtualKeyboards::switchToNextVirtualKeyboard));

    ASSERT(connect(ui->ruVirtualKeyboard, &RuVirtualKeyboard::switchToPrevVirtualKeyboard, this, &VirtualKeyboards::switchToPrevVirtualKeyboard));
    ASSERT(connect(ui->ruVirtualKeyboard, &RuVirtualKeyboard::switchToNextVirtualKeyboard, this, &VirtualKeyboards::switchToNextVirtualKeyboard));

    ASSERT(connect(ui->deVirtualKeyboard, &DeVirtualKeyboard::switchToPrevVirtualKeyboard, this, &VirtualKeyboards::switchToPrevVirtualKeyboard));
    ASSERT(connect(ui->deVirtualKeyboard, &DeVirtualKeyboard::switchToNextVirtualKeyboard, this, &VirtualKeyboards::switchToNextVirtualKeyboard));

    int count = ui->virtualKeyboadsStack->count();
    int virtualKeybLangIdx = restoreVirtualKeybLanguageIdx();

    if (virtualKeybLangIdx >= 0 && virtualKeybLangIdx < count)
        ui->virtualKeyboadsStack->setCurrentIndex(virtualKeybLangIdx);
    else
        ui->virtualKeyboadsStack->setCurrentIndex(0);
}

VirtualKeyboards::~VirtualKeyboards()
{
    qDebug() << "Deleting virtualkeyboards...";
    delete ui;
}

void VirtualKeyboards::setInputWidgetType(VirtualKeyboards::InputWidgetType editorType)
{
    _inputWidgetType = editorType;
    switch (_inputWidgetType) {
    case LineEdit:
        ui->inputWidget->setCurrentWidget(ui->lineEditInputPage);
        break;
    case PlainTextEdit:
        ui->inputWidget->setCurrentWidget(ui->plainTextEditInputPage);
        break;
    }
    setInputWidget();
}

void VirtualKeyboards::clear()
{
    switch (_inputWidgetType) {
    case LineEdit:
        ui->lineEditInput->clear();
        break;
    case PlainTextEdit:
        ui->plainTextEditInput->clear();
        break;
    }
}

QString VirtualKeyboards::value() const
{
    switch (_inputWidgetType) {
    case LineEdit:
        return ui->lineEditInput->text();
    case PlainTextEdit:
        return ui->plainTextEditInput->toPlainText();
    }

    return QString();
}

void VirtualKeyboards::setValue(const QString& value)
{
    switch (_inputWidgetType) {
    case LineEdit:
        ui->lineEditInput->setText(value);
        break;
    case PlainTextEdit:
        ui->plainTextEditInput->setPlainText(value);
        break;
    }
}

void VirtualKeyboards::setValidator(VirtualKeyboards::Validators validator, int maxValue)
{
    ui->lineEditInput->clear();
    switch (validator) {
    case Numbers:
        ui->lineEditInput->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), ui->lineEditInput));
        break;
    case EnglishLetters:
        ui->lineEditInput->setValidator(new QRegExpValidator(QRegExp("[A-Z]*"), ui->lineEditInput));
        break;
    case IpAddress:
        ui->lineEditInput->setValidator(new QRegExpValidator(QRegExp("\\b((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|$)){4}\\b"), ui->lineEditInput));
        break;
    case NumbersLimiting:
        ui->lineEditInput->setValidator(new QRegExpValidator(QRegExp("^[0-9]{1," + QString::number(QString::number(maxValue).length()) + "}"), ui->lineEditInput));
        break;
    case Date:
        ui->lineEditInput->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,2}\\.[0-9]{1,2}\\.[0-9]{1,4}"), ui->lineEditInput));
        break;
    case None:
    default:
        ui->lineEditInput->setValidator(nullptr);
        break;
    };
}

void VirtualKeyboards::leave()
{
    this->hide();
}

void VirtualKeyboards::setVisible(bool visible)
{
    if (visible) {
        ui->lineEditInput->setFocus();
    }
    QWidget::setVisible(visible);
#if defined ANDROID
    this->clearFocus();
    ui->lineEditInput->clearFocus();
    ui->lineEditInputPage->clearFocus();
#endif
}

bool VirtualKeyboards::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    return QWidget::event(e);
}

void VirtualKeyboards::setupUi()
{
    ui->setupUi(this);
    setupLanguageSwichButtons();
}

void VirtualKeyboards::retranslateUi()
{
    ui->retranslateUi(this);
    setupLanguageSwichButtons();
}

void VirtualKeyboards::setInputWidget()
{
    QWidget* inputWidget = nullptr;

    switch (_inputWidgetType) {
    case LineEdit:
        inputWidget = ui->lineEditInput;
        break;
    case PlainTextEdit:
        inputWidget = ui->plainTextEditInput;
        break;
    default:
        return;
    }

    ui->fnVirtualKeyboard->setInputWidget(inputWidget);
    ui->enVirtualKeyboard->setInputWidget(inputWidget);
    ui->ruVirtualKeyboard->setInputWidget(inputWidget);
    ui->deVirtualKeyboard->setInputWidget(inputWidget);
}

void VirtualKeyboards::setupLanguageSwichButtons()
{
    ui->fnVirtualKeyboard->prevVirtualKeyboardPushButton()->setText("RU");
    ui->fnVirtualKeyboard->nextVirtualKeyboardPushButton()->setText("EN");

    ui->enVirtualKeyboard->prevVirtualKeyboardPushButton()->setText("FN");
    ui->enVirtualKeyboard->nextVirtualKeyboardPushButton()->setText("DE");

    ui->ruVirtualKeyboard->prevVirtualKeyboardPushButton()->setText("DE");
    ui->ruVirtualKeyboard->nextVirtualKeyboardPushButton()->setText("FN");

    ui->deVirtualKeyboard->prevVirtualKeyboardPushButton()->setText("EN");
    ui->deVirtualKeyboard->nextVirtualKeyboardPushButton()->setText("RU");
}

void VirtualKeyboards::switchToPrevVirtualKeyboard()
{
    int virtualKeyboardsCount = ui->virtualKeyboadsStack->count();
    if (virtualKeyboardsCount == 0) {
        return;
    }

    int prevIndex = ui->virtualKeyboadsStack->currentIndex();
    if (prevIndex > 0) {
        --prevIndex;
    }
    else {
        prevIndex = virtualKeyboardsCount - 1;
    }
    ui->virtualKeyboadsStack->setCurrentIndex(prevIndex);
    saveVirtualKeybLanguageIdx(prevIndex);
}

void VirtualKeyboards::switchToNextVirtualKeyboard()
{
    int virtualKeyboardsCount = ui->virtualKeyboadsStack->count();
    if (virtualKeyboardsCount == 0) {
        return;
    }

    int nextIndex = (ui->virtualKeyboadsStack->currentIndex() + 1) % virtualKeyboardsCount;
    ui->virtualKeyboadsStack->setCurrentIndex(nextIndex);
    saveVirtualKeybLanguageIdx(nextIndex);
}

void VirtualKeyboards::on_doneButton_released()
{
    if (_closure != nullptr) {
        _closure->execute();
        delete _closure;
        _closure = nullptr;
    }
    leave();
}
