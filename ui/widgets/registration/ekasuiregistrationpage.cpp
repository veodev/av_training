#include "ekasuiregistrationpage.h"
#include "ui_ekasuiregistrationpage.h"
#include "core.h"
#include "settings.h"
#include "trackmarks/trackmarksrussianselector.h"
#include "trackmarks/trackmarksmetricselector.h"
#include "misc/popupdialogwidget.h"
#include "debug.h"
#include "ekasui/EK_ASUIdata.h"
#include "ekasui/ekasuidirectionselectionwidget.h"
#include "ekasui/ekasuitracknumberselectionwidget.h"
#include "ekasui/ekasuistationselectionwidget.h"

EKASUIRegistrationPage::EKASUIRegistrationPage(QWidget* parent)
    : RegistrationPage(parent)
    , ui(new Ui::EKASUIRegistrationPage)
{
    ui->setupUi(this);
    ui->controlObjectSpinBox->addItem(tr("Main track"), 0);
    ui->controlObjectSpinBox->addItem(tr("Station track"), 1);

    ui->controlRailLabel->hide();
    ui->controlRailSpinBox->hide();
    ui->workSideSpinBox->hide();
    ui->LeftSideTrolleyLabel->hide();
    ui->selectOperator1Button->setEnabled(true);
    ui->selectOperator2Button->setDisabled(true);
    ui->selectOperator3Button->setDisabled(true);
    ui->unselectOperator2PushButton->setDisabled(true);
    ui->unselectOperator3PushButton->setDisabled(true);
    ui->regServiceOnOffButton->setVisible(false);

    _directionSelectionWidget = new EKASUIDirectionSelectionWidget(this);
    ASSERT(connect(_directionSelectionWidget, &EKASUIDirectionSelectionWidget::directionSelected, this, &EKASUIRegistrationPage::onDirectionSelected));
    ASSERT(connect(_directionSelectionWidget, &EKASUIDirectionSelectionWidget::closeWidget, this, &EKASUIRegistrationPage::onDirectionCancel));

    _trackNumberSelectionWidget = new EKASUITrackNumberSelectionWidget(this);
    ASSERT(connect(_trackNumberSelectionWidget, &EKASUITrackNumberSelectionWidget::trackNumberSelected, this, &EKASUIRegistrationPage::onTrackNumberSelected));
    ASSERT(connect(_trackNumberSelectionWidget, &EKASUITrackNumberSelectionWidget::closeWidget, this, &EKASUIRegistrationPage::onTrackNumberCancel));

    _stationSelectionWidget = new EKASUIStationSelectionWidget(this);
    ASSERT(connect(_stationSelectionWidget, &EKASUIStationSelectionWidget::stationSelected, this, &EKASUIRegistrationPage::onStationSelected));
    ASSERT(connect(_stationSelectionWidget, &EKASUIStationSelectionWidget::closeWidget, this, &EKASUIRegistrationPage::onStationCancel));

    ui->controlObjectSpinBox->setIndex(restoreEKASUIControlObject());
    switchRegistrationControlObject(restoreEKASUIControlObject());

    ASSERT(connect(ui->controlObjectSpinBox, &SpinBoxList::valueChanged, this, &EKASUIRegistrationPage::onControlObjectSpinBoxChanged));

    _operatorSelectionWidget = new OperatorSelectionWidget(this);
    ASSERT(connect(_operatorSelectionWidget, &OperatorSelectionWidget::operatorSelected, this, &EKASUIRegistrationPage::onOperatorSelected));
    ASSERT(connect(_operatorSelectionWidget, &OperatorSelectionWidget::operatorDeleted, this, &EKASUIRegistrationPage::onOperatorDeleted));

    _lineSelectionWidget = new LineSelectionWidget(this);
    ASSERT(connect(_lineSelectionWidget, &LineSelectionWidget::lineSelected, this, &EKASUIRegistrationPage::onLineSelected));
    ASSERT(connect(_lineSelectionWidget, &LineSelectionWidget::lineDeleted, this, &EKASUIRegistrationPage::onLineDeleted));
    setupTrackMarks();

    ui->ekasuiDirectionPushButton->setText(restoreEKASUIRegistrationLastDirection());
    ui->ekasuiStationPushButton->setText(restoreEKASUIRegistrationLastStation());
    ui->trackNumberSelectionPushButton->setText(restoreEKASUIRegistrationLastTrackNumber());

    retranslateRegistrationPage();
}

EKASUIRegistrationPage::~EKASUIRegistrationPage()
{
    delete ui;
}

bool EKASUIRegistrationPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateRegistrationPage();
    }
    return QWidget::event(e);
}

void EKASUIRegistrationPage::switchRegistrationControlObject(int index)
{
    EK_ASUI* ekasui = Core::instance().getEkasui();
    if (index == 0) {
        ui->ekasuiDirectionPushButton->show();
        ui->ekasuiDirectionLabel->show();
        ui->ekasuiStationLabel->hide();
        ui->ekasuiStationPushButton->hide();
        ui->lineLabel->show();
        ui->selectLineButton->show();
        const QStringList& directions = ekasui->GetDIRNames_by_RR_PRED(restoreEKASUIRailroad(), restoreEKASUIPRED());
        _directionSelectionWidget->setDirectionsList(directions);
        if (!directions.isEmpty()) {
            ui->ekasuiDirectionPushButton->setText(directions[0]);
            saveEKASUIRegistrationLastDirection(directions[0]);
            ui->trackNumberSelectionPushButton->setEnabled(true);
            const QStringList& trackNumbers = ekasui->GetMAINPATH_NPUTs_by_RR_PRED_DIR(restoreEKASUIRailroad(), restoreEKASUIPRED(), ui->ekasuiDirectionPushButton->text());
            _trackNumberSelectionWidget->setTrackNumbersList(trackNumbers);
            if (!trackNumbers.isEmpty()) {
                ui->trackNumberSelectionPushButton->setText(trackNumbers[0]);
                saveEKASUIRegistrationLastTrackNumber(trackNumbers[0]);
            }
        }
        else {
            ui->trackNumberSelectionPushButton->setEnabled(false);
            ui->trackNumberSelectionPushButton->setText("");
            saveEKASUIRegistrationLastTrackNumber("");
            _trackNumberSelectionWidget->setTrackNumbersList(QStringList());
        }
    }
    else {
        ui->ekasuiDirectionPushButton->hide();
        ui->ekasuiDirectionLabel->hide();
        ui->ekasuiStationLabel->show();
        ui->ekasuiStationPushButton->show();
        ui->lineLabel->hide();
        ui->selectLineButton->hide();
        const QStringList& stations = ekasui->GetSTATIONSs_by_RR_PRED(restoreEKASUIRailroad(), restoreEKASUIPRED());
        _stationSelectionWidget->setStationList(stations);
        if (!stations.isEmpty()) {
            ui->ekasuiStationPushButton->setText(stations[0]);
            saveEKASUIRegistrationLastStation(stations[0]);
            ui->trackNumberSelectionPushButton->setEnabled(true);
            const QStringList& trackNumbers = ekasui->GetSTATIONSPATH_NPUTs_by_RR_PRED_STATION(restoreEKASUIRailroad(), restoreEKASUIPRED(), ui->ekasuiStationPushButton->text());
            _trackNumberSelectionWidget->setTrackNumbersList(trackNumbers);
            if (!trackNumbers.isEmpty()) {
                ui->trackNumberSelectionPushButton->setText(trackNumbers[0]);
                saveEKASUIRegistrationLastTrackNumber(trackNumbers[0]);
            }
        }
        else {
            ui->trackNumberSelectionPushButton->setEnabled(false);
            ui->trackNumberSelectionPushButton->setText("");
            saveEKASUIRegistrationLastTrackNumber("");
            _trackNumberSelectionWidget->setTrackNumbersList(QStringList());
        }
    }
}

int EKASUIRegistrationPage::getControlRail()
{
    return ui->controlRailSpinBox->index();
}

int EKASUIRegistrationPage::getDirection()
{
    return ui->movementDirectionSpinBox->index() == 0 ? 1 : -1;
}

void EKASUIRegistrationPage::setRegistrationState(bool state)
{
    if (!state) {
        ui->regOnOffButton->setText(tr("Start registration"));
        ui->regServiceOnOffButton->setText(tr("Start registration"));
        ui->selectOperator1Button->setEnabled(true);
        if (ui->selectOperator1Button->text().isEmpty() == false) {
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
        }
        if (ui->selectOperator2Button->text().isEmpty() == false) {
            ui->selectOperator3Button->setEnabled(true);
            ui->unselectOperator3PushButton->setEnabled(true);
        }
        ui->controlObjectSpinBox->setEnabled(true);
        ui->selectLineButton->setEnabled(true);
        ui->movementDirectionSpinBox->setEnabled(true);
        ui->workSideSpinBox->setEnabled(true);
        ui->ekasuiDirectionPushButton->setEnabled(true);
        ui->ekasuiStationPushButton->setEnabled(true);
        ui->trackNumberSelectionPushButton->setEnabled(true);
        _trackMarksSelector->setInputsEnabled(true);
        ui->LeftSideSpinBox->setEnabled(true);
        ui->controlRailSpinBox->setEnabled(true);
    }
    else {
        ui->regOnOffButton->setText(tr("Stop registration"));
        ui->regServiceOnOffButton->setText(tr("Stop registration"));
        ui->selectOperator1Button->setDisabled(true);
        ui->selectOperator2Button->setDisabled(true);
        ui->selectOperator3Button->setDisabled(true);
        ui->unselectOperator2PushButton->setDisabled(true);
        ui->unselectOperator3PushButton->setDisabled(true);
        ui->controlObjectSpinBox->setEnabled(false);
        ui->selectLineButton->setEnabled(false);
        ui->movementDirectionSpinBox->setEnabled(false);
        ui->workSideSpinBox->setEnabled(false);
        ui->ekasuiDirectionPushButton->setEnabled(false);
        ui->ekasuiStationPushButton->setEnabled(false);
        ui->trackNumberSelectionPushButton->setEnabled(false);
        _trackMarksSelector->setInputsEnabled(false);
        ui->LeftSideSpinBox->setEnabled(false);
        ui->controlRailSpinBox->setEnabled(false);
        _trackMarksSelector->writeTrackMarks(Core::instance().getTrackMarks());
        Core::instance().getTrackMarks()->setDirection(ui->movementDirectionSpinBox->index() ? BackwardDirection : ForwardDirection);
        _trackMarksSelector->writeTrackMarks(Core::instance().getTrackMarks());
    }
    emit activeOperatorChanged(ui->selectOperator1Button->text());
}

QString EKASUIRegistrationPage::getLine()
{
    return ui->selectLineButton->text();
}

void EKASUIRegistrationPage::setDeviceType(DeviceType type)
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
        break;
    case Avicon31Default:
    default:

        break;
    }
}

QString EKASUIRegistrationPage::getOperator()
{
    return ui->selectOperator1Button->text();
}

QStringList EKASUIRegistrationPage::getOperatorsList()
{
    return QStringList() << ui->selectOperator1Button->text() << ui->selectOperator2Button->text() << ui->selectOperator3Button->text();
}

QString EKASUIRegistrationPage::getDirectionName()
{
    return ui->ekasuiDirectionPushButton->text();
}

QString EKASUIRegistrationPage::getTrackNumber()
{
    return ui->trackNumberSelectionPushButton->text();
}

int EKASUIRegistrationPage::getLeftSide()
{
    return ui->LeftSideSpinBox->index();
}

int EKASUIRegistrationPage::getControlObject()
{
    return ui->controlObjectSpinBox->index();
}

void EKASUIRegistrationPage::updateTrackMarks()
{
    Core& core = Core::instance();
    TrackMarks* tm = core.getTrackMarks();
    _trackMarksSelector->readTrackMarks(tm);
}

QString EKASUIRegistrationPage::getStation()
{
    return ui->ekasuiStationPushButton->text();
}

QString EKASUIRegistrationPage::getFileName()
{
    return _fileName;
}

void EKASUIRegistrationPage::onControlObjectSpinBoxChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    saveEKASUIControlObject(index);
    switchRegistrationControlObject(index);
}

void EKASUIRegistrationPage::onOperatorSelected(const QString& operatorName)
{
    if (_selectOperatorNumber == 1) {
        if (operatorName != ui->selectOperator2Button->text() && operatorName != ui->selectOperator3Button->text()) {
            ui->selectOperator1Button->setText(operatorName);
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
            emit activeOperatorChanged(operatorName);
        }
    }
    else if (_selectOperatorNumber == 2) {
        if (operatorName != ui->selectOperator1Button->text() && operatorName != ui->selectOperator3Button->text()) {
            ui->selectOperator2Button->setText(operatorName);
            ui->selectOperator3Button->setEnabled(true);
            ui->unselectOperator3PushButton->setEnabled(true);
        }
    }
    else if (_selectOperatorNumber == 3) {
        if (operatorName != ui->selectOperator2Button->text() && operatorName != ui->selectOperator1Button->text()) {
            ui->selectOperator3Button->setText(operatorName);
        }
    }
    _operatorSelectionWidget->hide();
    updateOperatorsList();
}

void EKASUIRegistrationPage::updateOperatorsList()
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

void EKASUIRegistrationPage::onOperatorDeleted(const QString& operatorName)
{
    if (operatorName == ui->selectOperator1Button->text()) {
        ui->selectOperator1Button->setText(ui->selectOperator2Button->text());
        ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
        ui->selectOperator3Button->setText("");
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
    }
    else if (operatorName == ui->selectOperator2Button->text()) {
        ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
        ui->selectOperator3Button->setText("");
    }
    else if (operatorName == ui->selectOperator3Button->text()) {
        ui->selectOperator3Button->setText("");
    }

    if (_operatorSelectionWidget->getOperatorsCount() == 0) {
        ui->selectOperator2Button->setEnabled(false);
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator2PushButton->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
    }

    updateOperatorsList();
}

void EKASUIRegistrationPage::onLineSelected(const QString& line, bool isControlSection)
{
#if defined TARGET_AVICON31
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
    }
    else {
        if (ui->selectOperator1Button->text().isEmpty() == false) {
            ui->selectOperator2Button->setEnabled(true);
            ui->unselectOperator2PushButton->setEnabled(true);
        }
#endif
#if !defined TARGET_AVICONDBHS
        ui->operator2Label->setHidden(false);
        ui->selectOperator2Button->setHidden(false);
        ui->unselectOperator2PushButton->setHidden(false);
#endif
#if defined TARGET_AVICON31
        ui->operator3Label->setHidden(false);
        ui->selectOperator3Button->setHidden(false);
        ui->unselectOperator3PushButton->setHidden(false);
        ui->leftSideLabel->setHidden(false);
        ui->LeftSideSpinBox->setHidden(false);
        ui->movementDirectionLabel->setHidden(false);
        ui->movementDirectionSpinBox->setHidden(false);
        _trackMarksSelector->setHidden(false);
        ui->regOnOffButton->setHidden(false);
        ui->regServiceOnOffButton->setHidden(true);
    }
#endif
    ui->selectLineButton->setText(line);
    _lineSelectionWidget->hide();
}

void EKASUIRegistrationPage::onLineDeleted(const QString& line)
{
    Q_UNUSED(line);
    ui->selectLineButton->setText("");
}

void EKASUIRegistrationPage::setupTrackMarks()
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

void EKASUIRegistrationPage::retranslateRegistrationPage()
{
    ui->movementDirectionSpinBox->clear();
    *ui->movementDirectionSpinBox << tr("Increase") << tr("Decrease");

    ui->LeftSideSpinBox->clear();
    *ui->LeftSideSpinBox << tr("Left rail") << tr("Right rail");

    ui->controlRailSpinBox->clear();
    *ui->controlRailSpinBox << tr("Left rail") << tr("Right rail");

    ui->workSideSpinBox->clear();
    *ui->workSideSpinBox << tr("Gauge") << tr("Field");

#ifdef TARGET_AVICONDBHS
    ui->operator1Label->setText(tr("Operator"));
#endif
}

void EKASUIRegistrationPage::on_regServiceOnOffButton_released()
{
    PopupDialogWidget* popup = new PopupDialogWidget(this);
    popup->setMessage(tr("Remove old, and record a new control section of track?"));
    connect(popup, &PopupDialogWidget::dialogExited, this, &EKASUIRegistrationPage::onServiceDialogExited);
    popup->popup();
}

void EKASUIRegistrationPage::on_regOnOffButton_released()
{
    emit startStopRegistration();
}

void EKASUIRegistrationPage::on_selectOperator1Button_released()
{
    _selectOperatorNumber = 1;
    _operatorSelectionWidget->popup();
}

void EKASUIRegistrationPage::on_selectOperator2Button_released()
{
    _selectOperatorNumber = 2;
    _operatorSelectionWidget->show();
}

void EKASUIRegistrationPage::on_selectOperator3Button_released()
{
    _selectOperatorNumber = 3;
    _operatorSelectionWidget->show();
}

void EKASUIRegistrationPage::on_unselectOperator2PushButton_released()
{
    ui->selectOperator2Button->setText(ui->selectOperator3Button->text());
    if (ui->selectOperator2Button->text() == "") {
        ui->selectOperator3Button->setEnabled(false);
        ui->unselectOperator3PushButton->setEnabled(false);
    }
    ui->selectOperator3Button->setText("");
    // TODO: ui->changeOperatorButton->setVisible(false);
}

void EKASUIRegistrationPage::on_unselectOperator3PushButton_released()
{
    ui->selectOperator3Button->setText("");
    // TODO:  ui->changeOperatorButton->setVisible(false);
}

void EKASUIRegistrationPage::on_selectLineButton_released()
{
    _lineSelectionWidget->popup();
}

void EKASUIRegistrationPage::onServiceDialogExited(bool status)
{
    if (status) {
        emit startStopService();
    }
}

void EKASUIRegistrationPage::on_ekasuiDirectionPushButton_released()
{
    _directionSelectionWidget->popup();
}

void EKASUIRegistrationPage::onDirectionSelected(QString direction)
{
    ui->ekasuiDirectionPushButton->setText(direction);
    saveEKASUIRegistrationLastDirection(direction);
    _directionSelectionWidget->hide();
    if (!direction.isEmpty()) {
        EK_ASUI* ekasui = Core::instance().getEkasui();
        const QStringList& list = ekasui->GetMAINPATH_NPUTs_by_RR_PRED_DIR(restoreEKASUIRailroad(), restoreEKASUIPRED(), direction);
        _trackNumberSelectionWidget->setTrackNumbersList(list);
        if (!list.isEmpty()) {
            ui->trackNumberSelectionPushButton->setText(list[0]);
            saveEKASUIRegistrationLastTrackNumber(list[0]);
        }
        else {
            ui->trackNumberSelectionPushButton->setText("");
            saveEKASUIRegistrationLastTrackNumber("");
        }
    }
}

void EKASUIRegistrationPage::onDirectionCancel()
{
    _directionSelectionWidget->hide();
}

void EKASUIRegistrationPage::onTrackNumberSelected(QString tracknumber)
{
    ui->trackNumberSelectionPushButton->setText(tracknumber);
    saveEKASUIRegistrationLastTrackNumber(tracknumber);
    _trackNumberSelectionWidget->hide();
}

void EKASUIRegistrationPage::onTrackNumberCancel()
{
    _trackNumberSelectionWidget->hide();
}

void EKASUIRegistrationPage::onStationSelected(QString station)
{
    ui->ekasuiStationPushButton->setText(station);
    saveEKASUIRegistrationLastStation(station);
    _stationSelectionWidget->hide();
    if (!station.isEmpty()) {
        EK_ASUI* ekasui = Core::instance().getEkasui();
        const QStringList& list = ekasui->GetSTATIONSPATH_NPUTs_by_RR_PRED_STATION(restoreEKASUIRailroad(), restoreEKASUIPRED(), station);
        _trackNumberSelectionWidget->setTrackNumbersList(list);
        if (!list.isEmpty()) {
            ui->trackNumberSelectionPushButton->setText(list[0]);
            saveEKASUIRegistrationLastTrackNumber(list[0]);
        }
        else {
            ui->trackNumberSelectionPushButton->setText("");
            saveEKASUIRegistrationLastTrackNumber("");
        }
    }
}

void EKASUIRegistrationPage::onStationCancel()
{
    _stationSelectionWidget->hide();
}

void EKASUIRegistrationPage::on_trackNumberSelectionPushButton_released()
{
    _trackNumberSelectionWidget->popup();
}

void EKASUIRegistrationPage::on_ekasuiStationPushButton_released()
{
    _stationSelectionWidget->popup();
}
