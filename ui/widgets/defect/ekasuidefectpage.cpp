#include "ekasuidefectpage.h"
#include "ui_ekasuidefectpage.h"
#include "core.h"
#include "debug.h"
#include "ekasui/EK_ASUIdata.h"
#include "virtualkeyboards.h"
#include "selectors/ekasui/ekasuidefectselectionwidget.h"

void EKASUIDefectPage::init()
{
    EK_ASUI* ekasui = Core::instance().getEkasui();
    _defectSelectionWidget = new EKASUIDefectSelectionWidget(this);
    _defectSelectionWidget->setDefectsList(ekasui->GetDEFECTshortNames());

    Q_ASSERT(_defectSelectionWidget != nullptr);
    ASSERT(connect(_defectSelectionWidget, &EKASUIDefectSelectionWidget::defectSelected, this, &EKASUIDefectPage::onDefectSelected));
    ASSERT(connect(_defectSelectionWidget, &EKASUIDefectSelectionWidget::closeWidget, this, &EKASUIDefectPage::onDefectSelectorClosed));
}

EKASUIDefectPage::EKASUIDefectPage(QWidget* parent)
    : DefectPage(parent)
    , _defectSelectionWidget(nullptr)
    , ui(new Ui::EKASUIDefectPage)
{
    _inited = false;
    ui->setupUi(this);

    ui->ekasuiSpeedLimitSpinBox->addItem("-", -1);
    ui->ekasuiSpeedLimitSpinBox->addItem("140", 140);
    ui->ekasuiSpeedLimitSpinBox->addItem("120", 120);
    ui->ekasuiSpeedLimitSpinBox->addItem("100", 100);
    ui->ekasuiSpeedLimitSpinBox->addItem("70", 70);
    ui->ekasuiSpeedLimitSpinBox->addItem("40", 40);
    ui->ekasuiSpeedLimitSpinBox->addItem("25", 25);
    ui->ekasuiSpeedLimitSpinBox->addItem("15", 15);
    ui->ekasuiSpeedLimitSpinBox->addItem("0", 0);

    ui->ekasuiLength->setSuffix(tr(" mm"));
    ui->ekasuiDepth->setSuffix(tr(" mm"));
    ui->ekasuiWidth->setSuffix(tr(" mm"));

    ui->sideSpinBox->addItem(tr("None"), NoneDeviceSide);
    ui->sideSpinBox->addItem(tr("Left"), LeftDeviceSide);
    ui->sideSpinBox->addItem(tr("Right"), RightDeviceSide);

#ifdef TARGET_AVICONDB
    ui->sideSpinBox->hide();
#endif
}

void EKASUIDefectPage::changeServiceMarksDefectComment()
{
    ui->serviceMarksDefectCommentPlainTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

void EKASUIDefectPage::setEKASUILink()
{
    ui->ekasuiLinkTextEdit->setPlainText(VirtualKeyboards::instance()->value());
}

EKASUIDefectPage::~EKASUIDefectPage()
{
    delete ui;
}

void EKASUIDefectPage::on_serviceMarksDefectChangeCommentButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &EKASUIDefectPage::changeServiceMarksDefectComment);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->serviceMarksDefectCommentPlainTextEdit->toPlainText());
    virtualKeyboards->show();
}

void EKASUIDefectPage::clearFields()
{
    ui->serviceMarksDefectCommentPlainTextEdit->clear();
    ui->ekasuiLinkTextEdit->clear();
}

void EKASUIDefectPage::setVisible(bool visible)
{
    if (visible) {
        if (!_inited) {
            init();
        }
    }

    QWidget::setVisible(visible);
}

void EKASUIDefectPage::on_linkEditButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    Q_ASSERT(virtualKeyboards != nullptr);
    virtualKeyboards->setCallback(this, &EKASUIDefectPage::setEKASUILink);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->ekasuiLinkTextEdit->toPlainText());
    virtualKeyboards->show();
}

void EKASUIDefectPage::on_serviceMarksDefectSetButton_released()
{
    emit defectCreated();
}

int EKASUIDefectPage::getSide()
{
    return ui->sideSpinBox->index();
}

int EKASUIDefectPage::getDefectLength()
{
    return ui->ekasuiLength->value();
}

int EKASUIDefectPage::getDefectWidth()
{
    return ui->ekasuiWidth->value();
}

int EKASUIDefectPage::getDefectDepth()
{
    return ui->ekasuiDepth->value();
}

int EKASUIDefectPage::getSpeedLimit()
{
    return ui->ekasuiSpeedLimitSpinBox->userData().toInt();
}

QString EKASUIDefectPage::getDefectCode()
{
    return ui->defectCodePushButton->text();
}

QString EKASUIDefectPage::getLink()
{
    return ui->ekasuiLinkTextEdit->toPlainText();
}

QString EKASUIDefectPage::getComment()
{
    return ui->serviceMarksDefectCommentPlainTextEdit->toPlainText();
}

void EKASUIDefectPage::on_defectCodePushButton_released()
{
    _defectSelectionWidget->popup();
}

void EKASUIDefectPage::onDefectSelected(QString defect)
{
    ui->defectCodePushButton->setText(defect);
    _defectSelectionWidget->hide();
}

void EKASUIDefectPage::onDefectSelectorClosed()
{
    _defectSelectionWidget->hide();
}
