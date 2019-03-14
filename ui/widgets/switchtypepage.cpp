#include "switchtypepage.h"
#include "ui_switchtypepage.h"
#include "virtualkeyboards.h"
#include "dc_definitions.h"
#include "core.h"
#include "registration.h"
#include "settings.h"

#include <QDebug>

SwitchTypePage::SwitchTypePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SwitchTypePage)
    , _currentSwitchNumber(QString())
    , _currentControlVariant(QString())
    , _isStartControlSwitch(false)
    , _isSpecialRailTypeOn(false)
{
    _dict = {
        {static_cast<int>(SwitchType::SingleSwitchContiniousRollingSurface), QObject::tr("SCRS")},
        {static_cast<int>(SwitchType::CrossSwitchContiniousRollingSurface), QObject::tr("SCRS")},
        {static_cast<int>(SwitchType::SingleSwitch), QObject::tr("S")},
        {static_cast<int>(SwitchType::CrossSwitch), QObject::tr("S")},
        {static_cast<int>(SwitchType::DropSwitch), QObject::tr("S")},
        {static_cast<int>(SwitchType::OtherSwitch), QObject::tr("S")},
        {static_cast<int>(SwitchDirectionType::Left), QObject::tr("L")},
        {static_cast<int>(SwitchDirectionType::Right), QObject::tr("R")},
        {static_cast<int>(SwitchDirectionType::SingleThreadLeft), QObject::tr("1Th-L")},
        {static_cast<int>(SwitchDirectionType::SingleThreadRight), QObject::tr("1Th-R")},
        {static_cast<int>(SwitchDirectionType::DualThreadLeft), QObject::tr("2Th-L")},
        {static_cast<int>(SwitchDirectionType::DualThreadRight), QObject::tr("2Th-R")},
        {static_cast<int>(SwitchFixedDirection::Plus), "+"},
        {static_cast<int>(SwitchFixedDirection::Minus), "-"},
        {static_cast<int>(SwitchFixedDirection::LeftPlus), QObject::tr("L+")},
        {static_cast<int>(SwitchFixedDirection::LeftMinus), QObject::tr("L-")},
        {static_cast<int>(SwitchFixedDirection::RightPlus), QObject::tr("R+")},
        {static_cast<int>(SwitchFixedDirection::RightMinus), QObject::tr("R-")},
        {static_cast<int>(DeviceDirection::AgainstWool), QObject::tr("AW")},
        {static_cast<int>(DeviceDirection::ByWool), QObject::tr("BW")},
        {static_cast<int>(DeviceDirection::LeftPlus), QObject::tr("L+")},
        {static_cast<int>(DeviceDirection::LeftMinus), QObject::tr("L-")},
        {static_cast<int>(DeviceDirection::RightPlus), QObject::tr("R+")},
        {static_cast<int>(DeviceDirection::RightMinus), QObject::tr("R-")},
        {static_cast<int>(DeviceDirection::AgainstWoolPlus), QObject::tr("AW+")},
        {static_cast<int>(DeviceDirection::AgainstWoolMinus), QObject::tr("AW-")},
        {static_cast<int>(DeviceDirection::ByWoolPlus), QObject::tr("BW+")},
        {static_cast<int>(DeviceDirection::ByWoolMinus), QObject::tr("BW-")},
    };

    ui->setupUi(this);
    if (restoreDeviceType() == Avicon15) {
        configureUiForAvicon15();
    }
    resetPage();
}

SwitchTypePage::~SwitchTypePage()
{
    delete ui;
}

void SwitchTypePage::setDeviceLeftSide(bool isRight)
{
    isRight ? ui->leftSideLabel->setText(tr("Left Side - Right Rail")) : ui->leftSideLabel->setText(tr("Left Side - Left Rail"));
}

bool SwitchTypePage::switchControlStatus()
{
    return _isStartControlSwitch;
}

void SwitchTypePage::specialRailTypeOn()
{
    _isSpecialRailTypeOn = true;
}

void SwitchTypePage::specialRailTypeOff()
{
    _isSpecialRailTypeOn = false;
}

bool SwitchTypePage::getSpecialRailTypeStatus()
{
    return _isSpecialRailTypeOn;
}

void SwitchTypePage::stopSwitchControl()
{
    if (_isStartControlSwitch) {
        on_finishButton_released();
        resetPage();
    }
}

void SwitchTypePage::resetSingleSwitchTab()
{
    ui->singleSwitchCoreCrossingButton->setChecked(false);
    ui->singleSwitchLeftButton->setChecked(false);
    ui->singleSwitchRightButton->setChecked(false);
    ui->singleSwitchMinusDirectionButton->setChecked(false);
    ui->singleSwitchPlusDirectionButton->setChecked(false);
    ui->singleSwitchAgainstWoolPlusButton->setChecked(false);
    ui->singleSwitchAgainstWoolMinusButton->setChecked(false);
    ui->singleSwitchByWoolPlusButton->setChecked(false);
    ui->singleSwitchByWoolMinusButton->setChecked(false);
    ui->switchInfoLabel->clear();
}

void SwitchTypePage::resetCrossSwitchTab()
{
    ui->crossSwitchCoreCrossingButton->setChecked(false);
    ui->crossSwitchFixedLeftPlusButton->setChecked(false);
    ui->crossSwitchFixedLeftMinusButton->setChecked(false);
    ui->crossSwitchFixedRightPlusButton->setChecked(false);
    ui->crossSwitchFixedRightMinusButton->setChecked(false);
    ui->crossSwitchDeviceDirectionLeftMinusButton->setChecked(false);
    ui->crossSwitchDeviceDirectionLeftPlusButton->setChecked(false);
    ui->crossSwitchDeviceDirectionRightMinusButton->setChecked(false);
    ui->crossSwitchDeviceDirectionRightPlusButton->setChecked(false);
    ui->switchInfoLabel->clear();
}

void SwitchTypePage::resetDropSwitchTab()
{
    ui->dropSwitchSingleLeftButton->setChecked(false);
    ui->dropSwitchSingleRightButton->setChecked(false);
    ui->dropSwitchDualLeftButton->setChecked(false);
    ui->dropSwitchDualRightButton->setChecked(false);
    ui->dropSwitchFixedDirectButton->setChecked(false);
    ui->dropSwitchFixedLateralButton->setChecked(false);
    ui->dropSwitchDeviceDirectionByWoolButton->setChecked(false);
    ui->dropSwitchDeviceDirectionAgainstWoolButton->setChecked(false);
    ui->switchInfoLabel->clear();
}

void SwitchTypePage::resetOtherSwitchTab()
{
    ui->switchInfoLabel->clear();
}

void SwitchTypePage::resetSwitchVariables()
{
    _currentSwitchType = SwitchType::Undefined;
    _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    _currentDeviceDirection = DeviceDirection::Undefined;
    _currentControlVariant.clear();
}

void SwitchTypePage::blockSingleSwitchControls(bool isBlock)
{
    ui->singleSwitchCoreCrossingButton->setDisabled(isBlock);
    ui->singleSwitchLeftButton->setDisabled(isBlock);
    ui->singleSwitchRightButton->setDisabled(isBlock);
    ui->singleSwitchMinusDirectionButton->setDisabled(isBlock);
    ui->singleSwitchPlusDirectionButton->setDisabled(isBlock);
    ui->singleSwitchAgainstWoolPlusButton->setDisabled(isBlock);
    ui->singleSwitchAgainstWoolMinusButton->setDisabled(isBlock);
    ui->singleSwitchByWoolPlusButton->setDisabled(isBlock);
    ui->singleSwitchByWoolMinusButton->setDisabled(isBlock);
}

void SwitchTypePage::blockCrossSwitchControls(bool isBlock)
{
    ui->crossSwitchCoreCrossingButton->setDisabled(isBlock);
    ui->crossSwitchFixedLeftPlusButton->setDisabled(isBlock);
    ui->crossSwitchFixedLeftMinusButton->setDisabled(isBlock);
    ui->crossSwitchFixedRightPlusButton->setDisabled(isBlock);
    ui->crossSwitchFixedRightMinusButton->setDisabled(isBlock);
    ui->crossSwitchDeviceDirectionLeftMinusButton->setDisabled(isBlock);
    ui->crossSwitchDeviceDirectionLeftPlusButton->setDisabled(isBlock);
    ui->crossSwitchDeviceDirectionRightMinusButton->setDisabled(isBlock);
    ui->crossSwitchDeviceDirectionRightPlusButton->setDisabled(isBlock);
}

void SwitchTypePage::blockDropSwitchControls(bool isBlock)
{
    ui->dropSwitchSingleLeftButton->setDisabled(isBlock);
    ui->dropSwitchSingleRightButton->setDisabled(isBlock);
    ui->dropSwitchDualLeftButton->setDisabled(isBlock);
    ui->dropSwitchDualRightButton->setDisabled(isBlock);
    ui->dropSwitchFixedDirectButton->setDisabled(isBlock);
    ui->dropSwitchFixedLateralButton->setDisabled(isBlock);
    ui->dropSwitchDeviceDirectionByWoolButton->setDisabled(isBlock);
    ui->dropSwitchDeviceDirectionAgainstWoolButton->setDisabled(isBlock);
}

void SwitchTypePage::blockTopControls(bool isBlock)
{
    ui->singleSwitchButton->setDisabled(isBlock);
    ui->crossSwitchButton->setDisabled(isBlock);
    ui->dropSwitchButton->setDisabled(isBlock);
    ui->otherSwitchButton->setDisabled(isBlock);
}

void SwitchTypePage::blockSpecialControls(bool isBlock)
{
    ui->singleSwitchCoreCrossingButton->setDisabled(isBlock);
    ui->singleSwitchLeftButton->setDisabled(isBlock);
    ui->singleSwitchRightButton->setDisabled(isBlock);
    ui->crossSwitchCoreCrossingButton->setDisabled(isBlock);
    ui->dropSwitchSingleLeftButton->setDisabled(isBlock);
    ui->dropSwitchSingleRightButton->setDisabled(isBlock);
    ui->dropSwitchDualLeftButton->setDisabled(isBlock);
    ui->dropSwitchDualRightButton->setDisabled(isBlock);
}

void SwitchTypePage::blockBottomControls(bool isBlock)
{
    ui->finishButton->setDisabled(isBlock);
    ui->startButton->setDisabled(isBlock);
}

void SwitchTypePage::setSwitchNumber()
{
    ui->switchNumberLineEdit->setText(VirtualKeyboards::instance()->value());
    _currentSwitchNumber = VirtualKeyboards::instance()->value();
    blockSingleSwitchControls(_currentSwitchNumber.isEmpty());
    blockCrossSwitchControls(_currentSwitchNumber.isEmpty());
    blockDropSwitchControls(_currentSwitchNumber.isEmpty());
    parseSwitchVariant();
}

void SwitchTypePage::parseSwitchVariant()
{
    _currentControlVariant.clear();
    switch (_currentSwitchType) {
    case SwitchType::SingleSwitch:
    case SwitchType::SingleSwitchContiniousRollingSurface:
        if (_currentSwitchNumber.isEmpty() || _currentSwitchDirectionType == SwitchDirectionType::Undefined || _currentSwitchFixedDirection == SwitchFixedDirection::Undefined || _currentDeviceDirection == DeviceDirection::Undefined) {
            ui->switchInfoLabel->clear();
            _isStartControlSwitch ? ui->continueButton->setEnabled(false) : ui->startButton->setEnabled(false);
            return;
        }
        else {
            _currentControlVariant =
                _dict.at(static_cast<int>(_currentSwitchType)) + tr(" #") + _currentSwitchNumber + "_" + _dict.at(static_cast<int>(_currentSwitchDirectionType)) + _dict.at(static_cast<int>(_currentSwitchFixedDirection)) + tr("/UT:") + _dict.at(static_cast<int>(_currentDeviceDirection));
            _isStartControlSwitch ? ui->continueButton->setEnabled(true) : ui->startButton->setEnabled(true);
        }
        break;
    case SwitchType::CrossSwitch:
    case SwitchType::CrossSwitchContiniousRollingSurface:
        if (_currentSwitchNumber.isEmpty() || _currentSwitchFixedDirection == SwitchFixedDirection::Undefined || _currentDeviceDirection == DeviceDirection::Undefined) {
            ui->switchInfoLabel->clear();
            _isStartControlSwitch ? ui->continueButton->setEnabled(false) : ui->startButton->setEnabled(false);
            return;
        }
        else {
            _currentControlVariant = _dict.at(static_cast<int>(_currentSwitchType)) + tr(" #") + _currentSwitchNumber + "_" + _dict.at(static_cast<int>(_currentSwitchFixedDirection)) + tr("/UT:") + _dict.at(static_cast<int>(_currentDeviceDirection));
            _isStartControlSwitch ? ui->continueButton->setEnabled(true) : ui->startButton->setEnabled(true);
        }
        break;
    case SwitchType::DropSwitch:
        if (_currentSwitchNumber.isEmpty() || _currentSwitchDirectionType == SwitchDirectionType::Undefined || _currentDeviceDirection == DeviceDirection::Undefined || _currentSwitchFixedDirection == SwitchFixedDirection::Undefined) {
            ui->switchInfoLabel->clear();
            _isStartControlSwitch ? ui->continueButton->setEnabled(false) : ui->startButton->setEnabled(false);
            return;
        }
        else {
            _currentControlVariant =
                _dict.at(static_cast<int>(_currentSwitchType)) + tr(" #") + _currentSwitchNumber + "_" + _dict.at(static_cast<int>(_currentSwitchDirectionType)) + _dict.at(static_cast<int>(_currentSwitchFixedDirection)) + tr("/UT:") + _dict.at(static_cast<int>(_currentDeviceDirection));
            _isStartControlSwitch ? ui->continueButton->setEnabled(true) : ui->startButton->setEnabled(true);
        }
        break;
    case SwitchType::OtherSwitch:
        if (_currentSwitchNumber.isEmpty()) {
            ui->switchInfoLabel->clear();
            _isStartControlSwitch ? ui->continueButton->setEnabled(false) : ui->startButton->setEnabled(false);
            return;
        }
        else {
            _currentControlVariant = _dict.at(static_cast<int>(_currentSwitchType)) + tr(" #") + _currentSwitchNumber;
            _isStartControlSwitch ? ui->continueButton->setEnabled(true) : ui->startButton->setEnabled(true);
        }
        break;
    default:
        break;
    }
    ui->switchInfoLabel->setText(_currentControlVariant);
}

void SwitchTypePage::resetPage()
{
    on_singleSwitchButton_released();
    blockSingleSwitchControls(true);
    blockCrossSwitchControls(true);
    blockDropSwitchControls(true);
    blockBottomControls(true);
    ui->continueButton->setVisible(false);
    ui->switchNumberLineEdit->clear();
    _currentSwitchNumber.clear();
    _currentControlVariant.clear();
    _isStartControlSwitch = false;
    _isSpecialRailTypeOn = false;
}

void SwitchTypePage::configureUiForAvicon15()
{
    QString style = "font: 10pt";
    ui->leftSideLabel->setStyleSheet(style);
    ui->switchInfoLabel->setStyleSheet(style);
    style = "font: bold 13pt";
    ui->label_5->setStyleSheet(style);
    ui->label->setStyleSheet(style);
    ui->label_3->setStyleSheet(style);
    ui->label_4->setStyleSheet(style);
    style = "font: bold 15pt";
    ui->label_2->setStyleSheet(style);
    style = "font: 15pt";
    ui->editNoButton->setStyleSheet(style);
    style = "font: 18pt";
    ui->switchNumberLineEdit->setMinimumHeight(30);
    ui->switchNumberLineEdit->setStyleSheet(style);
    style = "font: 12pt";
    ui->singleSwitchAgainstWoolPlusButton->setStyleSheet(style);
    ui->singleSwitchAgainstWoolMinusButton->setStyleSheet(style);
    ui->singleSwitchByWoolPlusButton->setStyleSheet(style);
    ui->singleSwitchByWoolMinusButton->setStyleSheet(style);
}

void SwitchTypePage::on_singleSwitchCoreCrossingButton_toggled(bool checked)
{
    if (checked) {
        _currentSwitchType = SwitchType::SingleSwitchContiniousRollingSurface;
        ui->singleSwitchCoreCrossingButton->setText(tr("Yes"));
    }
    else {
        _currentSwitchType = SwitchType::SingleSwitch;
        ui->singleSwitchCoreCrossingButton->setText(tr("No"));
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchLeftButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchRightButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::Left;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchRightButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchLeftButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::Right;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchPlusDirectionButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchMinusDirectionButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::Plus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchMinusDirectionButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchPlusDirectionButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::Minus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchAgainstWoolPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchAgainstWoolMinusButton->setChecked(false);
        ui->singleSwitchByWoolPlusButton->setChecked(false);
        ui->singleSwitchByWoolMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::AgainstWoolPlus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchAgainstWoolMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchAgainstWoolPlusButton->setChecked(false);
        ui->singleSwitchByWoolPlusButton->setChecked(false);
        ui->singleSwitchByWoolMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::AgainstWoolMinus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchByWoolPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchAgainstWoolPlusButton->setChecked(false);
        ui->singleSwitchAgainstWoolMinusButton->setChecked(false);
        ui->singleSwitchByWoolMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::ByWoolPlus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_singleSwitchByWoolMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->singleSwitchAgainstWoolPlusButton->setChecked(false);
        ui->singleSwitchAgainstWoolMinusButton->setChecked(false);
        ui->singleSwitchByWoolPlusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::ByWoolMinus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchCoreCrossingButton_toggled(bool checked)
{
    if (checked) {
        _currentSwitchType = SwitchType::CrossSwitchContiniousRollingSurface;
        ui->crossSwitchCoreCrossingButton->setText(tr("Yes"));
    }
    else {
        _currentSwitchType = SwitchType::CrossSwitch;
        ui->crossSwitchCoreCrossingButton->setText(tr("No"));
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchFixedLeftPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchFixedRightPlusButton->setChecked(false);
        ui->crossSwitchFixedLeftMinusButton->setChecked(false);
        ui->crossSwitchFixedRightMinusButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::LeftPlus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchFixedRightPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchFixedLeftPlusButton->setChecked(false);
        ui->crossSwitchFixedLeftMinusButton->setChecked(false);
        ui->crossSwitchFixedRightMinusButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::RightPlus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchFixedLeftMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchFixedLeftPlusButton->setChecked(false);
        ui->crossSwitchFixedRightPlusButton->setChecked(false);
        ui->crossSwitchFixedRightMinusButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::LeftMinus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchFixedRightMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchFixedLeftPlusButton->setChecked(false);
        ui->crossSwitchFixedRightPlusButton->setChecked(false);
        ui->crossSwitchFixedLeftMinusButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::RightMinus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchDeviceDirectionLeftPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchDeviceDirectionRightPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionLeftMinusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionRightMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::LeftPlus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchDeviceDirectionRightPlusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchDeviceDirectionLeftPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionLeftMinusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionRightMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::RightPlus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchDeviceDirectionLeftMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchDeviceDirectionLeftPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionRightPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionRightMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::LeftMinus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_crossSwitchDeviceDirectionRightMinusButton_toggled(bool checked)
{
    if (checked) {
        ui->crossSwitchDeviceDirectionLeftPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionRightPlusButton->setChecked(false);
        ui->crossSwitchDeviceDirectionLeftMinusButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::RightMinus;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchSingleLeftButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchSingleRightButton->setChecked(false);
        ui->dropSwitchDualLeftButton->setChecked(false);
        ui->dropSwitchDualRightButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::SingleThreadLeft;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchSingleRightButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchSingleLeftButton->setChecked(false);
        ui->dropSwitchDualLeftButton->setChecked(false);
        ui->dropSwitchDualRightButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::SingleThreadRight;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchDualLeftButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchSingleLeftButton->setChecked(false);
        ui->dropSwitchSingleRightButton->setChecked(false);
        ui->dropSwitchDualRightButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::DualThreadLeft;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchDualRightButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchSingleLeftButton->setChecked(false);
        ui->dropSwitchSingleRightButton->setChecked(false);
        ui->dropSwitchDualLeftButton->setChecked(false);
        _currentSwitchDirectionType = SwitchDirectionType::DualThreadRight;
    }
    else {
        _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    }
    parseSwitchVariant();
}


void SwitchTypePage::on_dropSwitchFixedDirectButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchFixedLateralButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::Plus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchFixedLateralButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchFixedDirectButton->setChecked(false);
        _currentSwitchFixedDirection = SwitchFixedDirection::Minus;
    }
    else {
        _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchDeviceDirectionByWoolButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchDeviceDirectionAgainstWoolButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::ByWool;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_dropSwitchDeviceDirectionAgainstWoolButton_toggled(bool checked)
{
    if (checked) {
        ui->dropSwitchDeviceDirectionByWoolButton->setChecked(false);
        _currentDeviceDirection = DeviceDirection::AgainstWool;
    }
    else {
        _currentDeviceDirection = DeviceDirection::Undefined;
    }
    parseSwitchVariant();
}

void SwitchTypePage::on_editNoButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &SwitchTypePage::setSwitchNumber);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->switchNumberLineEdit->text());
    virtualKeyboards->show();
}

void SwitchTypePage::on_singleSwitchButton_released()
{
    resetSingleSwitchTab();
    resetSwitchVariables();
    ui->singleSwitchButton->setChecked(true);
    ui->crossSwitchButton->setChecked(false);
    ui->dropSwitchButton->setChecked(false);
    ui->otherSwitchButton->setChecked(false);
    ui->stackedWidget->setCurrentWidget(ui->singleSwitchPage);
    _currentSwitchType = SwitchType::SingleSwitch;
}

void SwitchTypePage::on_crossSwitchButton_released()
{
    resetCrossSwitchTab();
    resetSwitchVariables();
    ui->singleSwitchButton->setChecked(false);
    ui->dropSwitchButton->setChecked(false);
    ui->otherSwitchButton->setChecked(false);
    ui->stackedWidget->setCurrentWidget(ui->crossSwitchPage);
    _currentSwitchType = SwitchType::CrossSwitch;
}

void SwitchTypePage::on_dropSwitchButton_released()
{
    resetDropSwitchTab();
    resetSwitchVariables();
    ui->singleSwitchButton->setChecked(false);
    ui->crossSwitchButton->setChecked(false);
    ui->otherSwitchButton->setChecked(false);
    ui->stackedWidget->setCurrentWidget(ui->dropSwitchPage);
    _currentSwitchType = SwitchType::DropSwitch;
}

void SwitchTypePage::on_otherSwitchButton_released()
{
    ui->crossSwitchButton->setChecked(false);
    ui->singleSwitchButton->setChecked(false);
    ui->dropSwitchButton->setChecked(false);
    ui->stackedWidget->setCurrentWidget(ui->otherSwitchPage);
    _currentSwitchType = SwitchType::OtherSwitch;
    parseSwitchVariant();
}

void SwitchTypePage::on_finishButton_released()
{
    if (_isStartControlSwitch) {
        ui->startButton->setVisible(true);
        ui->continueButton->setVisible(false);
        ui->editNoButton->setEnabled(true);
        blockTopControls(false);
        blockSpecialControls(false);
        _isStartControlSwitch = false;
        parseSwitchVariant();
        QString finishControlMessage = QString(tr("Switch #%1 finish control").arg(_currentSwitchNumber));
        Core::instance().registration().addEndSwitchControlLabel(_currentSwitchNumber, finishControlMessage);
        emit doStopSwitchControl();
    }
}

void SwitchTypePage::on_continueButton_released()
{
    Core::instance().registration().addStartSwitchControlLabel(_currentSwitchNumber, _currentSwitchType, _currentSwitchDirectionType, _currentSwitchFixedDirection, _currentDeviceDirection, eDeviceSide::dsNone, _currentControlVariant);
    emit doStartSwitchControl();
}

void SwitchTypePage::on_startButton_released()
{
    if (_isStartControlSwitch == false) {
        ui->finishButton->setEnabled(true);
        ui->startButton->setVisible(false);
        ui->continueButton->setVisible(true);
        ui->editNoButton->setEnabled(false);
        blockTopControls(true);
        blockSpecialControls(true);
        _isStartControlSwitch = true;
        Core::instance().registration().addStartSwitchControlLabel(_currentSwitchNumber, _currentSwitchType, _currentSwitchDirectionType, _currentSwitchFixedDirection, _currentDeviceDirection, eDeviceSide::dsNone, _currentControlVariant);
        emit doStartSwitchControl();
    }
}
