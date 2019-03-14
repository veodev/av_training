#include "defaultscannerdefectregistrationpage.h"
#include "ui_defaultscannerdefectregistrationpage.h"
#include "virtualkeyboards.h"
#include "settings.h"
#include "debug.h"

DefaultScannerDefectRegistrationPage::DefaultScannerDefectRegistrationPage(QWidget* parent)
    : ScannerDefectRegistrationPage(parent)
    , ui(new Ui::DefaultScannerDefectRegistrationPage)
{
    ui->setupUi(this);

    bool isBehind = restoreRuLocation();
    int locationIndex = isBehind ? 0 : 1;
    ui->locationRuSpinBox->addItem(tr("Behind"), true);
    ui->locationRuSpinBox->addItem(tr("In base"), false);
    ui->locationRuSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->locationRuSpinBox, &SpinBoxList::valueChanged, this, &DefaultScannerDefectRegistrationPage::onRuLocationSpinBoxChanged));

    retranslateRegistrationPage();
}

DefaultScannerDefectRegistrationPage::~DefaultScannerDefectRegistrationPage()
{
    delete ui;
}

void DefaultScannerDefectRegistrationPage::retranslateRegistrationPage()
{
    ui->controledRailSpinBox->clear();
    *ui->controledRailSpinBox << tr("Left rail") << tr("Right rail");
    ui->controledRailSpinBox->setIndex(restoreHeadScannerWorkRail());

    ui->workEdgeSpinBox->clear();
    *ui->workEdgeSpinBox << "1"
                         << "2";
    ui->workEdgeSpinBox->setIndex(restoreHeadScannerOrientation());

    ui->locationRuSpinBox->clear();
    ui->locationRuSpinBox->addItem(tr("Behind"), true);
    ui->locationRuSpinBox->addItem(tr("In base"), false);
    ui->locationRuSpinBox->setIndex(!restoreRuLocation());
}

void DefaultScannerDefectRegistrationPage::changeComment()
{
    ui->commentPlainTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

void DefaultScannerDefectRegistrationPage::setLink()
{
    ui->linkLineEdit->setText(VirtualKeyboards::instance()->value());
}

void DefaultScannerDefectRegistrationPage::setDefectCode()
{
    ui->defectCodeLineEdit->setText(VirtualKeyboards::instance()->value());
}

void DefaultScannerDefectRegistrationPage::switchState(int state)
{
    if (state == 1) {
        ui->infoLabel->setVisible(true);
        ui->deltaHSpinBox->setEnabled(false);
        ui->deltaLSpinBox->setEnabled(false);
        ui->deltaXSpinBox->setEnabled(false);

        ui->locationRuSpinBox->setEnabled(true);
        ui->commentHeadScannerGroupBox->setVisible(false);
        ui->saveHeadScanResultButton->setVisible(false);
        ui->startWorkHeadScanButton->setVisible(false);
        ui->nextHeadScanButton->setVisible(true);
        ui->infoLabel->setText(tr("Install the scanner on the rail head, leaning to the rear wheel of his truck facedown on the scanner (without connector), and then click \"Next\"."));
    }
    else if (state == 0) {
        ui->infoLabel->setVisible(false);
        ui->deltaHSpinBox->setEnabled(true);
        ui->deltaLSpinBox->setEnabled(true);
        ui->deltaXSpinBox->setEnabled(true);

        ui->locationRuSpinBox->setEnabled(false);
        ui->commentHeadScannerGroupBox->setVisible(true);
        ui->saveHeadScanResultButton->setVisible(true);
        ui->startWorkHeadScanButton->setVisible(false);
    }
    else if (state == 2) {
        ui->infoLabel->setText(tr("Install the scanner in the center of the study area and click the \"Start\" button."));
        ui->startWorkHeadScanButton->setVisible(true);
        ui->nextHeadScanButton->setVisible(false);
        ui->locationRuSpinBox->setEnabled(false);
    }
}

int DefaultScannerDefectRegistrationPage::getWorkRail()
{
    return ui->controledRailSpinBox->index();
}

int DefaultScannerDefectRegistrationPage::getEdge()
{
    return ui->workEdgeSpinBox->index();
}

int DefaultScannerDefectRegistrationPage::getHSize()
{
    return ui->deltaHSpinBox->value();
}

int DefaultScannerDefectRegistrationPage::getLSize()
{
    return ui->deltaLSpinBox->value();
}

int DefaultScannerDefectRegistrationPage::getDSize()
{
    return ui->deltaXSpinBox->value();
}

int DefaultScannerDefectRegistrationPage::getSpeedLimit()
{
    return 0;
}

QString DefaultScannerDefectRegistrationPage::getDefectCode()
{
    return ui->defectCodeLineEdit->text();
}

QString DefaultScannerDefectRegistrationPage::getLink()
{
    return ui->linkLineEdit->text();
}

QString DefaultScannerDefectRegistrationPage::getComment()
{
    return ui->commentPlainTextEdit->toPlainText();
}

void DefaultScannerDefectRegistrationPage::resetPage()
{
    ui->deltaLSpinBox->setValue(ui->deltaLSpinBox->minimum());
    ui->deltaHSpinBox->setValue(ui->deltaHSpinBox->minimum());
    ui->deltaXSpinBox->setValue(ui->deltaXSpinBox->minimum());
    ui->linkLineEdit->clear();
    ui->defectCodeLineEdit->clear();
    ui->commentPlainTextEdit->clear();
}

void DefaultScannerDefectRegistrationPage::on_saveHeadScanResultButton_released()
{
    emit saveResults();
}

void DefaultScannerDefectRegistrationPage::on_nextHeadScanButton_released()
{
    emit nextHead();
}

void DefaultScannerDefectRegistrationPage::on_startWorkHeadScanButton_released()
{
    emit startWork();
}

void DefaultScannerDefectRegistrationPage::on_defectCodeEditButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultScannerDefectRegistrationPage::setDefectCode);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->defectCodeLineEdit->text());
    virtualKeyboards->show();
}

void DefaultScannerDefectRegistrationPage::on_changeCommentButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultScannerDefectRegistrationPage::changeComment);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->commentPlainTextEdit->toPlainText());
    virtualKeyboards->show();
}

void DefaultScannerDefectRegistrationPage::on_changeLinkButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &DefaultScannerDefectRegistrationPage::setLink);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->linkLineEdit->text());
    virtualKeyboards->show();
}

void DefaultScannerDefectRegistrationPage::onRuLocationSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    bool isBehind = userData.toBool();
    saveRuLocation(isBehind);
}

void DefaultScannerDefectRegistrationPage::on_controledRailSpinBox_valueChanged(int value)
{
    saveHeadScannerWorkRail(value);
}
