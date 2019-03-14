#include "wrongcalibrationmsg.h"
#include "ui_wrongcalibrationmsg.h"
#include "core.h"
#include "checkcalibration.h"
#include "debug.h"

const int COUNT_BOLT_JOINTS_FOR_VIEW_MSG = 3;

WrongCalibrationMsg::WrongCalibrationMsg(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WrongCalibrationMsg)
    , _countOfBoltJoints(0)
{
    ui->setupUi(this);
    ui->leftCheckCalibrationWidget->hide();
    ui->rightCheckCalibrationWidget->hide();

    Core& core = Core::instance();
    CheckCalibration& checkcalibration = core.checkcalibration();

    ASSERT(connect(&checkcalibration, &CheckCalibration::doLeftSideWrongCalibration, this, &WrongCalibrationMsg::onLeftSideWrongCalibration));
    ASSERT(connect(&checkcalibration, &CheckCalibration::doRightSideWrongCalibration, this, &WrongCalibrationMsg::onRightSideWrongCalibration));
    ASSERT(connect(&checkcalibration, &CheckCalibration::doLeftSideNo70Signal, this, &WrongCalibrationMsg::onLeftSideNo70Signal));
    ASSERT(connect(&checkcalibration, &CheckCalibration::doRightSideNo70Signal, this, &WrongCalibrationMsg::onRightSideNo70Signal));

    ASSERT(connect(&core, &Core::doStartBoltJoint, this, &WrongCalibrationMsg::onStartBoltJoint, Qt::UniqueConnection));
    ASSERT(connect(ui->leftSideBoltJointBtn, &QPushButton::released, this, &WrongCalibrationMsg::buttonsHandler));
    ASSERT(connect(ui->rightSideBoltJointBtn, &QPushButton::released, this, &WrongCalibrationMsg::buttonsHandler));
    ASSERT(connect(ui->leftSideALTBtn, &QPushButton::released, this, &WrongCalibrationMsg::buttonsHandler));
    ASSERT(connect(ui->rightSideALTBtn, &QPushButton::released, this, &WrongCalibrationMsg::buttonsHandler));
}

WrongCalibrationMsg::~WrongCalibrationMsg()
{
    delete ui;
}

void WrongCalibrationMsg::resetCountBoltJoints()
{
    _countOfBoltJoints = 0;
}

// -----------------------------------------------------------------------------------------
bool WrongCalibrationMsg::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

// ------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::onStartBoltJoint()
{
    this->hide();
}

// ------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::registrateWrongCalibration(eDeviceSide side, tQualityCalibrationRecType recType)
{
    CheckCalibration& checkcalibration = Core::instance().checkcalibration();
    Registration& registration = Core::instance().registration();
    tBadChannelList badChannelList;
    badChannelList = checkcalibration.getBadChannelList(side);

    if (recType == qrtFailByAluminothermicJoint) {
        registration.addQualityCalibrationRec(side, tBadChannelList{}, qrtAluminothermicJoint);
    }

    if (recType == qrtFailByBoltedJoint) {
        registration.addQualityCalibrationRec(side, badChannelList, recType);
    }
}

bool WrongCalibrationMsg::isViewMsg()
{
    ++_countOfBoltJoints;
    if (_countOfBoltJoints == COUNT_BOLT_JOINTS_FOR_VIEW_MSG) {
        resetCountBoltJoints();
        return true;
    }
    return false;
}

void WrongCalibrationMsg::checkCalibrationSignalsHandler(SignalFromCheckCalibration signal)
{
    if (isViewMsg() == false) {
        return;
    }
    tBadChannelList badChannelList;
    QString cidStr;
    QString badChannelsStr = "";
    eDeviceSide side = dsNone;

    Core& core = Core::instance();
    CheckCalibration& checkcalibration = core.checkcalibration();
    if (signal == LeftSideNo70Signal || signal == LeftSideWrongCalibration) {
        ui->leftSideBoltJointBtn->setText(tr("Bolt joint\n"));
        ui->leftSideBoltJointBtn->show();
        side = dsLeft;
    }
    else if (signal == RightSideNo70Signal || signal == RightSideWrongCalibration) {
        ui->rightSideBoltJointBtn->setText(tr("Bolt joint\n"));
        ui->rightSideBoltJointBtn->show();
        side = dsRight;
    }

    if (signal == LeftSideNo70Signal) {
        ui->leftSideALTBtn->setText(tr("ALT joint\n"));
        ui->leftSideALTBtn->show();
    }
    else if (signal == RightSideNo70Signal) {
        ui->rightSideALTBtn->setText(tr("ALT joint\n"));
        ui->rightSideALTBtn->show();
    }

    badChannelList.clear();
    badChannelList = checkcalibration.getBadChannelList(side);

    for (tBadChannelListItem& currentItem : badChannelList) {
        badChannelsStr += "\n ";
        if (currentItem.ChId == N0EMS && currentItem.GateIndex == 0) {
            cidStr = tr("0° Echo");  //"0° ЭХО";
        }
        else {
            checkcalibration.cidToNameString(currentItem.ChId, &cidStr);
        }

        badChannelsStr += cidStr;
        if (side == dsLeft) {
            ui->leftSideBoltJointBtn->setText(ui->leftSideBoltJointBtn->text() + "\n" + cidStr);
            ui->leftCheckCalibrationWidget->show();
        }
        else if (side == dsRight) {
            ui->rightSideBoltJointBtn->setText(ui->rightSideBoltJointBtn->text() + "\n" + cidStr);
            ui->rightCheckCalibrationWidget->show();
        }
    }

    if (this->isHidden()) {
        this->show();
    }
}

// ------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::onLeftSideWrongCalibration()
{
    checkCalibrationSignalsHandler(LeftSideWrongCalibration);
}

// ------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::onRightSideWrongCalibration()
{
    checkCalibrationSignalsHandler(RightSideWrongCalibration);
}

// ----------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::onLeftSideNo70Signal()
{
    checkCalibrationSignalsHandler(LeftSideNo70Signal);
}

// ----------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::onRightSideNo70Signal()
{
    checkCalibrationSignalsHandler(RightSideNo70Signal);
}

// ----------------------------------------------------------------------------------------------------------
void WrongCalibrationMsg::buttonsHandler()
{
    if (QObject::sender() == ui->leftSideBoltJointBtn) {
        registrateWrongCalibration(dsLeft, qrtFailByBoltedJoint);
        ui->leftCheckCalibrationWidget->hide();
        if (ui->rightCheckCalibrationWidget->isHidden()) {
            this->hide();
        }
    }

    if (QObject::sender() == ui->leftSideALTBtn) {
        registrateWrongCalibration(dsLeft, qrtFailByAluminothermicJoint);
        ui->leftCheckCalibrationWidget->hide();
        if (ui->rightCheckCalibrationWidget->isHidden()) {
            this->hide();
        }
    }

    if (QObject::sender() == ui->rightSideBoltJointBtn) {
        registrateWrongCalibration(dsRight, qrtFailByBoltedJoint);
        ui->rightCheckCalibrationWidget->hide();
        if (ui->leftCheckCalibrationWidget->isHidden()) {
            this->hide();
        }
    }

    if (QObject::sender() == ui->rightSideALTBtn) {
        registrateWrongCalibration(dsRight, qrtFailByAluminothermicJoint);
        ui->rightCheckCalibrationWidget->hide();
        if (ui->leftCheckCalibrationWidget->isHidden()) {
            this->hide();
        }
    }
}
