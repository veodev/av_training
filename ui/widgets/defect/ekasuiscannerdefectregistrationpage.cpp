#include "ekasuiscannerdefectregistrationpage.h"
#include "ui_ekasuiscannerdefectregistrationpage.h"
#include "core.h"
#include "ekasui/EK_ASUIdata.h"
#include "settings.h"
#include "virtualkeyboards.h"
#include "debug.h"

#include "selectors/ekasui/ekasuidefectselectionwidget.h"

EKASUIScannerDefectRegistrationPage::EKASUIScannerDefectRegistrationPage(QWidget* parent)
    : ScannerDefectRegistrationPage(parent)
    , ui(new Ui::EKASUIScannerDefectRegistrationPage)
{
    ui->setupUi(this);
    EK_ASUI* ekasui = Core::instance().getEkasui();

    _defectSelectionWidget = new EKASUIDefectSelectionWidget(this);
    _defectSelectionWidget->setDefectsList(ekasui->GetDEFECTshortNames());
    ASSERT(connect(_defectSelectionWidget, &EKASUIDefectSelectionWidget::defectSelected, this, &EKASUIScannerDefectRegistrationPage::onDefectSeleced));
    ASSERT(connect(_defectSelectionWidget, &EKASUIDefectSelectionWidget::closeWidget, this, &EKASUIScannerDefectRegistrationPage::onDefectSelectorClosed));

    ui->ekasuiHeadSpeedLimitSpinBox->addItem("140", 140);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("120", 120);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("100", 100);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("70", 70);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("40", 40);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("25", 25);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("15", 15);
    ui->ekasuiHeadSpeedLimitSpinBox->addItem("0", 0);

    bool isBehind = restoreRuLocation();
    int locationIndex = isBehind ? 0 : 1;
    ui->locationRuSpinBox->addItem(tr("Behind"), true);
    ui->locationRuSpinBox->addItem(tr("In base"), false);
    ui->locationRuSpinBox->setIndex(locationIndex);
    ASSERT(connect(ui->locationRuSpinBox, &SpinBoxList::valueChanged, this, &EKASUIScannerDefectRegistrationPage::onRuLocationSpinBoxChanged));

    retranslateRegistrationPage();
}

EKASUIScannerDefectRegistrationPage::~EKASUIScannerDefectRegistrationPage()
{
    delete ui;
}

void EKASUIScannerDefectRegistrationPage::switchState(int state)
{
    if (state == 1) {
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

void EKASUIScannerDefectRegistrationPage::retranslateRegistrationPage()
{
    ui->controledRailHeadScannerSpinBox->clear();
    *ui->controledRailHeadScannerSpinBox << tr("Left rail") << tr("Right rail");
    ui->controledRailHeadScannerSpinBox->setIndex(restoreHeadScannerWorkRail());

    ui->workEdgeHeadScannerSpinBox->clear();
    *ui->workEdgeHeadScannerSpinBox << "1"
                                    << "2";
    ui->workEdgeHeadScannerSpinBox->setIndex(restoreHeadScannerOrientation());

    ui->locationRuSpinBox->clear();
    ui->locationRuSpinBox->addItem(tr("Behind"), true);
    ui->locationRuSpinBox->addItem(tr("In base"), false);
    ui->locationRuSpinBox->setIndex(!restoreRuLocation());
}

void EKASUIScannerDefectRegistrationPage::onRuLocationSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    bool isBehind = userData.toBool();
    saveRuLocation(isBehind);
}

void EKASUIScannerDefectRegistrationPage::changeHeadScannerDefectComment()
{
    ui->headScannerDefectCommentPlainTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

void EKASUIScannerDefectRegistrationPage::setEKASUIHeadLink()
{
    ui->ekasuiHeadLinkLineEdit->setText(VirtualKeyboards::instance()->value());
}

int EKASUIScannerDefectRegistrationPage::getWorkRail()
{
    return ui->controledRailHeadScannerSpinBox->index();
}

int EKASUIScannerDefectRegistrationPage::getEdge()
{
    return ui->workEdgeHeadScannerSpinBox->index();
}

int EKASUIScannerDefectRegistrationPage::getHSize()
{
    return ui->deltaHSpinBox->value();
}

int EKASUIScannerDefectRegistrationPage::getLSize()
{
    return ui->deltaLSpinBox->value();
}

int EKASUIScannerDefectRegistrationPage::getDSize()
{
    return ui->deltaXSpinBox->value();
}

int EKASUIScannerDefectRegistrationPage::getSpeedLimit()
{
    return ui->ekasuiHeadSpeedLimitSpinBox->userData().toInt();
}

QString EKASUIScannerDefectRegistrationPage::getDefectCode()
{
    return ui->ekasuiDefectCodeSelectionButton->text();
}

QString EKASUIScannerDefectRegistrationPage::getLink()
{
    return ui->ekasuiHeadLinkLineEdit->text();
}

QString EKASUIScannerDefectRegistrationPage::getComment()
{
    return ui->headScannerDefectCommentPlainTextEdit->toPlainText();
}

void EKASUIScannerDefectRegistrationPage::resetPage()
{
    ui->deltaLSpinBox->setValue(ui->deltaLSpinBox->minimum());
    ui->deltaHSpinBox->setValue(ui->deltaHSpinBox->minimum());
    ui->deltaXSpinBox->setValue(ui->deltaXSpinBox->minimum());
    ui->ekasuiDefectCodeSelectionButton->setText(QString());
    ui->ekasuiHeadLinkLineEdit->clear();
    ui->ekasuiHeadSpeedLimitSpinBox->clear();
    ui->headScannerDefectCommentPlainTextEdit->clear();
}

void EKASUIScannerDefectRegistrationPage::on_headScannerDefectChangeCommentButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &EKASUIScannerDefectRegistrationPage::changeHeadScannerDefectComment);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->headScannerDefectCommentPlainTextEdit->toPlainText());
    virtualKeyboards->show();
}

void EKASUIScannerDefectRegistrationPage::on_ekasuiChangeHeadLinkButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &EKASUIScannerDefectRegistrationPage::setEKASUIHeadLink);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->ekasuiHeadLinkLineEdit->text());
    virtualKeyboards->show();
}

void EKASUIScannerDefectRegistrationPage::on_controledRailHeadScannerSpinBox_valueChanged(int value)
{
    saveHeadScannerWorkRail(value);
}

void EKASUIScannerDefectRegistrationPage::on_workEdgeHeadScannerSpinBox_valueChanged(int value)
{
    saveHeadScannerOrientation(value);
}

void EKASUIScannerDefectRegistrationPage::on_saveHeadScanResultButton_released()
{
    emit saveResults();
}

void EKASUIScannerDefectRegistrationPage::on_nextHeadScanButton_released()
{
    emit nextHead();
}

void EKASUIScannerDefectRegistrationPage::on_startWorkHeadScanButton_released()
{
    emit startWork();
}

void EKASUIScannerDefectRegistrationPage::onDefectSeleced(QString defect)
{
    ui->ekasuiDefectCodeSelectionButton->setText(defect);
    _defectSelectionWidget->hide();
}

void EKASUIScannerDefectRegistrationPage::onDefectSelectorClosed()
{
    _defectSelectionWidget->hide();
}

void EKASUIScannerDefectRegistrationPage::on_ekasuiDefectCodeSelectionButton_released()
{
    _defectSelectionWidget->popup();
}
