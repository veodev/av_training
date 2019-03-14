#include "defaultdefectpage.h"
#include "ui_defaultdefectpage.h"
#include "core.h"
#include "virtualkeyboards.h"

DefaultDefectPage::DefaultDefectPage(QWidget* parent)
    : DefectPage(parent)
    , ui(new Ui::DefaultDefectPage)
{
    ui->setupUi(this);

    ui->lengthSpinBox->setSuffix(tr(" mm"));
    ui->depthSpinBox->setSuffix(tr(" mm"));
    ui->widthSpinBox->setSuffix(tr(" mm"));

    ui->sideSpinBox->addItem(tr("None"), NoneDeviceSide);
    ui->sideSpinBox->addItem(tr("Left"), LeftDeviceSide);
    ui->sideSpinBox->addItem(tr("Right"), RightDeviceSide);
#if defined(TARGET_AVICON15)
    ui->sideTitle->hide();
    ui->sideSpinBox->hide();
#endif
}

DefaultDefectPage::~DefaultDefectPage()
{
    delete ui;
}

void DefaultDefectPage::on_codeEditButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultDefectPage::setDefectCode);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->defectCodeTextEdit->toPlainText());
    virtualKeyboards->show();
}

void DefaultDefectPage::on_linkEditButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultDefectPage::setLink);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->linkTextEdit->toPlainText());
    virtualKeyboards->show();
}

void DefaultDefectPage::on_serviceMarksDefectChangeCommentButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultDefectPage::setComment);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->serviceMarksDefectCommentPlainTextEdit->toPlainText());
    virtualKeyboards->show();
}

void DefaultDefectPage::on_serviceMarksDefectSetButton_released()
{
    emit defectCreated();
}

void DefaultDefectPage::setDefectCode()
{
    ui->defectCodeTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

void DefaultDefectPage::setLink()
{
    ui->linkTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

void DefaultDefectPage::setComment()
{
    ui->serviceMarksDefectCommentPlainTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

int DefaultDefectPage::getSide()
{
    return ui->sideSpinBox->index();
}

int DefaultDefectPage::getDefectLength()
{
    return ui->lengthSpinBox->value();
}

int DefaultDefectPage::getDefectWidth()
{
    return ui->widthSpinBox->value();
}

int DefaultDefectPage::getDefectDepth()
{
    return ui->depthSpinBox->value();
}

int DefaultDefectPage::getSpeedLimit()
{
    return 0;
}

QString DefaultDefectPage::getDefectCode()
{
    return ui->defectCodeTextEdit->toPlainText();
}

QString DefaultDefectPage::getLink()
{
    return ui->linkTextEdit->toPlainText();
}

QString DefaultDefectPage::getComment()
{
    return ui->serviceMarksDefectCommentPlainTextEdit->toPlainText();
}

void DefaultDefectPage::clearFields()
{
    ui->serviceMarksDefectCommentPlainTextEdit->clear();
    ui->defectCodeTextEdit->clear();
    ui->linkTextEdit->clear();
}
