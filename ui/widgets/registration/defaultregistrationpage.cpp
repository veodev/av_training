#include "defaultregistrationpage.h"
#include "ui_defaultregistrationpage.h"
#include "virtualkeyboards.h"
#include "debug.h"
#include "trackmarks/trackmarksrussianselector.h"
#include "trackmarks/trackmarksmetricselector.h"
#include "misc/popupdialogwidget.h"
#include "core.h"


DefaultRegistrationPage::DefaultRegistrationPage(QWidget* parent)
    : RegistrationPage(parent)
    , ui(new Ui::DefaultRegistrationPage)
{
    ui->setupUi(this);
    ui->controlRailSpinBox->hide();
    ui->controlRailLabel->hide();
    ui->workSideSpinBox->hide();
    ui->LeftSideTrolleyLabel->hide();
    ui->selectOperator1Button->setEnabled(true);
    ui->selectOperator2Button->setDisabled(true);
    ui->selectOperator3Button->setDisabled(true);
    ui->unselectOperator2PushButton->setDisabled(true);
    ui->unselectOperator3PushButton->setDisabled(true);
    ui->fileNameLabel->hide();
    ui->fileNameButton->hide();
    ui->defaultFileNameButton->hide();
    ui->correctionButton->hide();
    ui->correctionButton->setDisabled(true);

    ui->regServiceOnOffButton->setVisible(false);

    ASSERT(connect(ui->movementDirectionSpinBox, &SpinBoxList::valueChanged, this, &DefaultRegistrationPage::onDirectionSpinBoxChanged));
    ASSERT(connect(ui->controlRailSpinBox, &SpinBoxList::valueChanged, this, &DefaultRegistrationPage::onControlRailSpinBoxChanged));
    ASSERT(connect(ui->controlRailSpinBox, &SpinBoxList::valueChanged, this, &DefaultRegistrationPage::controlRailChanged));

    _operatorSelectionWidget = new OperatorSelectionWidget(this);
    ASSERT(connect(_operatorSelectionWidget, &OperatorSelectionWidget::operatorSelected, this, &DefaultRegistrationPage::onOperatorSelected));
    ASSERT(connect(_operatorSelectionWidget, &OperatorSelectionWidget::operatorDeleted, this, &DefaultRegistrationPage::onOperatorDeleted));

    _lineSelectionWidget = new LineSelectionWidget(this);
    ASSERT(connect(_lineSelectionWidget, &LineSelectionWidget::lineSelected, this, &DefaultRegistrationPage::onLineSelected));
    ASSERT(connect(_lineSelectionWidget, &LineSelectionWidget::lineDeleted, this, &DefaultRegistrationPage::onLineDeleted));

    _filePlacementWidget = new FilePlacementWidget(this);
    ASSERT(connect(_filePlacementWidget, &FilePlacementWidget::fileNameAndPlacementChanged, this, &DefaultRegistrationPage::onFileNameChanged));
    ASSERT(connect(_filePlacementWidget, &FilePlacementWidget::fileNameAndPlacementByDefault, this, &DefaultRegistrationPage::onFileNameByDefault));

    _notificationSelectionWidget = new NotificationSelectionWidget(this);

    setupTrackMarks();
    retranslateRegistrationPage();

#if defined(TARGET_AVICON15)
    ui->directionCodeButton->hide();
    ui->directionCodeLabel->hide();
#endif
    ui->selectOperator1Button->setText(restoreDefaultRegistrationLastOperator1());
    ui->selectOperator2Button->setText(restoreDefaultRegistrationLastOperator2());
    ui->selectOperator3Button->setText(restoreDefaultRegistrationLastOperator3());
    ui->selectLineButton->setText(restoreDefaultRegistrationLastLine());
    ui->trackNumberButton->setText(restoreDefaultRegistrationLastTrackNumber());
    ui->directionCodeButton->setText(restoreDefaultRegistrationLastDirectionCode());
}

DefaultRegistrationPage::~DefaultRegistrationPage()
{
    delete ui;
}

QString DefaultRegistrationPage::getOperator()
{
    return ui->selectOperator1Button->text();
}

QStringList DefaultRegistrationPage::getOperatorsList()
{
    return QStringList() << ui->selectOperator1Button->text() << ui->selectOperator2Button->text() << ui->selectOperator3Button->text();
}

QString DefaultRegistrationPage::getDirectionName()
{
    return ui->directionCodeButton->text();
}

QString DefaultRegistrationPage::getTrackNumber()
{
    return ui->trackNumberButton->text();
}

int DefaultRegistrationPage::getLeftSide()
{
    return ui->LeftSideSpinBox->index();
}

int DefaultRegistrationPage::getControlRail()
{
    return ui->controlRailSpinBox->index();
}

int DefaultRegistrationPage::getDirection()
{
    return ui->movementDirectionSpinBox->index() == 0 ? 1 : -1;
}

QString DefaultRegistrationPage::getLine()
{
    return ui->selectLineButton->text();
}

void DefaultRegistrationPage::setRegistrationState(bool state)
{
    _isRegistrationStarted = state;
    if (state == false) {
        ui->regOnOffButton->setText(tr("Start registration"));
        ui->regServiceOnOffButton->setText(tr("Start registration"));
        ui->fileNameButton->setEnabled(true);
        ui->defaultFileNameButton->setEnabled(true);
        ui->selectOperator1Button->setEnabled(true);
        ui->correctionButton->setDisabled(true);
        if (ui->selectOperator1Button->text().isEmpty() == false) {
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
        }
        if (ui->selectOperator2Button->text().isEmpty() == false) {
            ui->selectOperator3Button->setEnabled(true);
            ui->unselectOperator3PushButton->setEnabled(true);
        }
        ui->selectLineButton->setEnabled(true);
        ui->movementDirectionSpinBox->setEnabled(true);
        ui->directionCodeButton->setEnabled(true);
        ui->trackNumberButton->setEnabled(true);
        ui->workSideSpinBox->setEnabled(true);
        _trackMarksSelector->setInputsEnabled(true);
        ui->LeftSideSpinBox->setEnabled(true);
        ui->controlRailSpinBox->setEnabled(true);
        onFileNameByDefault();
    }
    else {
        ui->regOnOffButton->setText(tr("Stop registration"));
        ui->regServiceOnOffButton->setText(tr("Stop registration"));
        ui->selectOperator1Button->setDisabled(true);
        ui->fileNameButton->setDisabled(true);
        ui->defaultFileNameButton->setDisabled(true);
        ui->correctionButton->setEnabled(true);
        ui->selectOperator2Button->setDisabled(true);
        ui->selectOperator3Button->setDisabled(true);
        ui->unselectOperator2PushButton->setDisabled(true);
        ui->unselectOperator3PushButton->setDisabled(true);
        ui->selectLineButton->setEnabled(false);
        ui->movementDirectionSpinBox->setEnabled(false);
        ui->directionCodeButton->setEnabled(false);
        ui->trackNumberButton->setEnabled(false);
        ui->workSideSpinBox->setEnabled(false);
        _trackMarksSelector->setInputsEnabled(false);
        ui->LeftSideSpinBox->setEnabled(false);
        ui->controlRailSpinBox->setEnabled(false);
        _trackMarksSelector->writeTrackMarks(Core::instance().getTrackMarks());
        Core::instance().getTrackMarks()->setDirection(ui->movementDirectionSpinBox->index() ? BackwardDirection : ForwardDirection);
        _trackMarksSelector->writeTrackMarks(Core::instance().getTrackMarks());
    }
    emit activeOperatorChanged(ui->selectOperator1Button->text());
}

void DefaultRegistrationPage::setDeviceType(DeviceType type)
{
    switch (type) {
    case AviconDB:
        ui->controlRailLabel->show();
        ui->controlRailSpinBox->show();
        ui->LeftSideSpinBox->hide();
        ui->leftSideLabel->hide();
        ui->workSideSpinBox->show();
        ui->LeftSideTrolleyLabel->show();
        break;
    case AviconDBHS:
        ui->operator2Label->hide();
        ui->selectOperator2Button->hide();
        ui->unselectOperator2PushButton->hide();
        ui->operator3Label->hide();
        ui->selectOperator3Button->hide();
        ui->unselectOperator3PushButton->hide();
        ui->controlRailLabel->show();
        ui->controlRailSpinBox->show();
        ui->LeftSideSpinBox->hide();
        ui->leftSideLabel->hide();
        ui->movementDirectionLabel->hide();
        ui->movementDirectionSpinBox->hide();
        break;
    case Avicon15:
        ui->operator3Label->hide();
        ui->selectOperator3Button->hide();
        ui->unselectOperator3PushButton->hide();
        ui->controlRailLabel->show();
        ui->controlRailSpinBox->show();
        ui->LeftSideSpinBox->hide();
        ui->leftSideLabel->hide();
        ui->notificationsButton->hide();
        ui->correctionButton->show();
        break;
    case Avicon31Estonia:
        ui->defaultFileNameButton->show();
        ui->fileNameLabel->show();
        ui->fileNameButton->show();
        ui->operator2Label->hide();
        ui->selectOperator2Button->hide();
        ui->unselectOperator2PushButton->hide();
        ui->operator3Label->hide();
        ui->selectOperator3Button->hide();
        ui->unselectOperator3PushButton->hide();
        ui->correctionButton->show();
        break;
    case Avicon31Default:
    default:

        break;
    }
}

int DefaultRegistrationPage::getControlObject()
{
    return 0;
}

void DefaultRegistrationPage::updateTrackMarks()
{
    Core& core = Core::instance();
    TrackMarks* tm = core.getTrackMarks();
    _trackMarksSelector->readTrackMarks(tm);
}

QString DefaultRegistrationPage::getStation()
{
    return "";
}

QString DefaultRegistrationPage::getFileName()
{
    return _fileName;
}

bool DefaultRegistrationPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateRegistrationPage();
    }
    return QWidget::event(e);
}

void DefaultRegistrationPage::setVisible(bool visible)
{
    if (ui->correctionButton->isChecked()) {
        ui->correctionButton->setChecked(false);
    }
    QWidget::setVisible(visible);
}

void DefaultRegistrationPage::onOperatorSelected(const QString& operatorName)
{
    if (_selectOperatorNumber == 1) {
        if (operatorName != ui->selectOperator2Button->text() && operatorName != ui->selectOperator3Button->text()) {
            ui->selectOperator1Button->setText(operatorName);
            saveDefaultRegistrationLastOperator1(operatorName);
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
        }
    }
    else if (_selectOperatorNumber == 2) {
        if (operatorName != ui->selectOperator1Button->text() && operatorName != ui->selectOperator3Button->text()) {
            ui->selectOperator2Button->setText(operatorName);
            saveDefaultRegistrationLastOperator2(operatorName);
            ui->selectOperator3Button->setEnabled(true);
            ui->unselectOperator3PushButton->setEnabled(true);
        }
    }
    else if (_selectOperatorNumber == 3) {
        if (operatorName != ui->selectOperator2Button->text() && operatorName != ui->selectOperator1Button->text()) {
            ui->selectOperator3Button->setText(operatorName);
            saveDefaultRegistrationLastOperator3(operatorName);
        }
    }
    _operatorSelectionWidget->hide();
    updateOperatorsList();
}


void DefaultRegistrationPage::onOperatorDeleted(const QString& operatorName)
{
    if (operatorName == ui->selectOperator1Button->text()) {
        ui->selectOperator1Button->setText(ui->selectOperator2Button->text());
        ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
        ui->selectOperator3Button->setText("");
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
        saveDefaultRegistrationLastOperator1("");
    }
    else if (operatorName == ui->selectOperator2Button->text()) {
        ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
        ui->selectOperator3Button->setText("");
        saveDefaultRegistrationLastOperator2("");
    }
    else if (operatorName == ui->selectOperator3Button->text()) {
        ui->selectOperator3Button->setText("");
        saveDefaultRegistrationLastOperator3("");
    }

    if (_operatorSelectionWidget->getOperatorsCount() == 0) {
        ui->selectOperator2Button->setEnabled(false);
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator2PushButton->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
    }
    updateOperatorsList();
}

void DefaultRegistrationPage::onLineSelected(const QString& line, bool isControlSection)
{
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    if (isControlSection) {
        ui->operator2Label->setHidden(true);
        ui->selectOperator2Button->setText("");
        ui->selectOperator2Button->setHidden(true);
        ui->unselectOperator2PushButton->setHidden(true);
        ui->operator3Label->setHidden(true);
        ui->selectOperator3Button->setHidden(true);
        ui->selectOperator3Button->setText("");
        ui->unselectOperator3PushButton->setHidden(true);
        ui->movementDirectionLabel->setHidden(true);
        ui->movementDirectionSpinBox->setHidden(true);
        ui->leftSideLabel->setHidden(true);
        ui->LeftSideSpinBox->setHidden(true);
        _trackMarksSelector->setHidden(true);
        ui->regOnOffButton->setHidden(true);
        ui->regServiceOnOffButton->setHidden(false);
        ui->trackNumberLabel->setHidden(true);
        ui->trackNumberButton->setHidden(true);
        ui->directionCodeLabel->setHidden(true);
        ui->directionCodeButton->setHidden(true);
        saveDefaultRegistrationLastLine(QString());
    }
    else {
        if (ui->selectOperator1Button->text().isEmpty() == false) {
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
        }
#endif
#if defined TARGET_AVICONDB || defined TARGET_AVICON31 || defined TARGET_AVICON15
        ui->operator2Label->setHidden(false);
        ui->selectOperator2Button->setHidden(false);
        ui->unselectOperator2PushButton->setHidden(false);
#endif
#if defined TARGET_AVICON31
        ui->operator3Label->setHidden(false);
        ui->selectOperator3Button->setHidden(false);
        ui->unselectOperator3PushButton->setHidden(false);
#endif
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
        ui->leftSideLabel->setHidden(false);
        ui->LeftSideSpinBox->setHidden(false);
        ui->movementDirectionLabel->setHidden(false);
        ui->movementDirectionSpinBox->setHidden(false);
        _trackMarksSelector->setHidden(false);
        ui->regOnOffButton->setHidden(false);
        ui->regServiceOnOffButton->setHidden(true);
        ui->trackNumberLabel->setHidden(false);
        ui->trackNumberButton->setHidden(false);
        ui->directionCodeLabel->setHidden(false);
        ui->directionCodeButton->setHidden(false);
        saveDefaultRegistrationLastLine(line);
    }
#endif
    ui->selectLineButton->setText(line);
    _lineSelectionWidget->hide();
}

void DefaultRegistrationPage::onLineDeleted(const QString& line)
{
    Q_UNUSED(line);
    ui->selectLineButton->setText("");
    saveDefaultRegistrationLastLine("");
}

void DefaultRegistrationPage::onServiceDialogExited(bool status)
{
    if (status) {
        emit startStopService();
    }
}

void DefaultRegistrationPage::retranslateRegistrationPage()
{
    ui->movementDirectionSpinBox->clear();
    *ui->movementDirectionSpinBox << tr("Increase") << tr("Decrease");

    ui->LeftSideSpinBox->clear();
    *ui->LeftSideSpinBox << tr("Left rail") << tr("Right rail");

    ui->controlRailSpinBox->clear();
    *ui->controlRailSpinBox << tr("Left rail") << tr("Right rail");

    ui->workSideSpinBox->clear();
    *ui->workSideSpinBox << tr("Gauge") << tr("Field");
}

void DefaultRegistrationPage::changeDirectionCode()
{
    const QString& directionCode = VirtualKeyboards::instance()->value();
    ui->directionCodeButton->setText(directionCode);
    saveDefaultRegistrationLastDirectionCode(directionCode);
}

void DefaultRegistrationPage::setTrackNumber()
{
    const QString& trackNumber = VirtualKeyboards::instance()->value();
    ui->trackNumberButton->setText(trackNumber);
    saveDefaultRegistrationLastTrackNumber(trackNumber);
}

void DefaultRegistrationPage::on_regServiceOnOffButton_released()
{
    if (_isRegistrationStarted == false) {
        PopupDialogWidget* popup = new PopupDialogWidget(this);
        popup->setMessage(tr("Remove old, and record a new control section of track?"));
        connect(popup, &PopupDialogWidget::dialogExited, this, &DefaultRegistrationPage::onServiceDialogExited);
        popup->popup();
    }
    else {
        emit startStopService();
    }
}

void DefaultRegistrationPage::on_regOnOffButton_released()
{
    emit startStopRegistration();
}

void DefaultRegistrationPage::setupTrackMarks()
{
    Core& core = Core::instance();
    switch (core.getSystemCoordType()) {
    case csMetricRF:
        _trackMarksSelector = new TrackMarksRussianSelector();
        ui->trackMarksRegistrationWidget->addWidget(_trackMarksSelector);
        ui->trackMarksRegistrationWidget->setCurrentWidget(_trackMarksSelector);

        break;
    case csMetric1km:
        _trackMarksSelector = new TrackMarksMetricSelector();
        ui->trackMarksRegistrationWidget->addWidget(_trackMarksSelector);
        ui->trackMarksRegistrationWidget->setCurrentWidget(_trackMarksSelector);

        break;
    case csImperial:
    case csMetric0_1km:
    case csMetric1kmTurkish:
        break;
    }

    if (_trackMarksSelector != 0) {
        _trackMarksSelector->readTrackMarks(core.getTrackMarks());
        _trackMarksSelector->restoreFromSettings();
    }
}

void DefaultRegistrationPage::on_selectOperator1Button_released()
{
    _selectOperatorNumber = 1;
    _operatorSelectionWidget->popup();
}

void DefaultRegistrationPage::on_selectOperator2Button_released()
{
    _selectOperatorNumber = 2;
    _operatorSelectionWidget->show();
}

void DefaultRegistrationPage::on_selectOperator3Button_released()
{
    _selectOperatorNumber = 3;
    _operatorSelectionWidget->show();
}

void DefaultRegistrationPage::on_unselectOperator2PushButton_released()
{
    ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
    if (ui->selectOperator2Button->text() == "") {
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
    }
    ui->selectOperator3Button->setText("");
    // TODO: ui->changeOperatorButton->setVisible(false);
}

void DefaultRegistrationPage::on_unselectOperator3PushButton_released()
{
    ui->selectOperator3Button->setText("");
    // TODO:  ui->changeOperatorButton->setVisible(false);
}

void DefaultRegistrationPage::on_selectLineButton_released()
{
    _lineSelectionWidget->popup();
}

void DefaultRegistrationPage::on_directionCodeButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::NumbersLimiting, 9999999);
    virtualKeyboards->setValue(ui->directionCodeButton->text());
    virtualKeyboards->setCallback(this, &DefaultRegistrationPage::changeDirectionCode);
    virtualKeyboards->show();
}

void DefaultRegistrationPage::onDirectionSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);

    ui->LeftSideSpinBox->setIndex(index);
    Core::instance().setCurrentDirection(index == 0 ? 1 : -1);
}

void DefaultRegistrationPage::onControlRailSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);

    ui->controlRailSpinBox->setIndex(index);
}

void DefaultRegistrationPage::onWorkSideSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);

    ui->workSideSpinBox->setIndex(index);
    Core::instance().setleftSideTrolleyPosition(index == 0 ? LeftSideTrolley_WorkSide : LeftSideTrolley_UnWorkSide);
    emit workSideChanged(index, value, userData);
}

void DefaultRegistrationPage::on_trackNumberButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &DefaultRegistrationPage::setTrackNumber);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->trackNumberButton->text());
    virtualKeyboards->show();
}


void DefaultRegistrationPage::updateOperatorsList()
{
    QStringList operators;
    if (!ui->selectOperator1Button->text().isEmpty()) {
        operators << ui->selectOperator1Button->text();
    }
    if (!ui->selectOperator2Button->text().isEmpty()) {
        operators << ui->selectOperator2Button->text();
    }
    if (!ui->selectOperator3Button->text().isEmpty()) {
        operators << ui->selectOperator3Button->text();
    }

    emit operatorsListChanged(operators);
}

void DefaultRegistrationPage::on_notificationsButton_released()
{
    _notificationSelectionWidget->setValues(restoreOrganizationName(), restoreDepartmentName(), ui->selectLineButton->text(), ui->trackNumberButton->text());
    _notificationSelectionWidget->popup();
}

void DefaultRegistrationPage::on_defaultFileNameButton_released()
{
    ui->fileNameButton->setText(QString(tr("will be set and placed by default")));
}

void DefaultRegistrationPage::on_fileNameButton_released()
{
    _filePlacementWidget->popup();
}

void DefaultRegistrationPage::onFileNameChanged(const QString& viewFilePath, const QString& fullFilePath)
{
    ui->fileNameButton->setText(fullFilePath);
    _fileName = fullFilePath;
}

void DefaultRegistrationPage::onFileNameByDefault()
{
    _fileName = QString();
    on_defaultFileNameButton_released();
}

void DefaultRegistrationPage::on_correctionButton_toggled(bool checked)
{
    if (checked) {
        ui->registrationPageLabel->setText(ui->registrationPageLabel->text() + QString(tr(" Correction! ")));
    }
    else {
        ui->registrationPageLabel->setText(QString(tr("Registration:")));
        emit correctRegistration();
    }
    ui->selectOperator1Button->setEnabled(checked);
    ui->selectLineButton->setEnabled(checked);
    ui->movementDirectionSpinBox->setEnabled(checked);
    ui->directionCodeButton->setEnabled(checked);
    ui->trackNumberButton->setEnabled(checked);
    ui->workSideSpinBox->setEnabled(checked);
    _trackMarksSelector->setInputsEnabled(checked);
    ui->LeftSideSpinBox->setEnabled(checked);
    ui->controlRailSpinBox->setEnabled(checked);
}
