#ifndef SWITCHTYPEPAGE_H
#define SWITCHTYPEPAGE_H

#include <QWidget>
#include "dc_definitions.h"

namespace Ui
{
class SwitchTypePage;
}

class SwitchTypePage : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchTypePage(QWidget* parent = nullptr);
    ~SwitchTypePage();

    void setDeviceLeftSide(bool isLeft);
    bool switchControlStatus();

    void specialRailTypeOn();
    void specialRailTypeOff();
    bool getSpecialRailTypeStatus();
    void stopSwitchControl();

private:
    void resetSingleSwitchTab();
    void resetCrossSwitchTab();
    void resetDropSwitchTab();
    void resetOtherSwitchTab();
    void resetSwitchVariables();
    void blockSingleSwitchControls(bool isBlock);
    void blockCrossSwitchControls(bool isBlock);
    void blockDropSwitchControls(bool isBlock);
    void blockTopControls(bool isBlock);
    void blockSpecialControls(bool isBlock);
    void blockBottomControls(bool isBlock);

    void setSwitchNumber();
    void parseSwitchVariant();
    void resetPage();
    void configureUiForAvicon15();

signals:
    void doStartSwitchControl();
    void doStopSwitchControl();

private slots:
    void on_singleSwitchCoreCrossingButton_toggled(bool checked);
    void on_singleSwitchLeftButton_toggled(bool checked);
    void on_singleSwitchRightButton_toggled(bool checked);
    void on_singleSwitchPlusDirectionButton_toggled(bool checked);
    void on_singleSwitchMinusDirectionButton_toggled(bool checked);
    void on_singleSwitchAgainstWoolPlusButton_toggled(bool checked);
    void on_singleSwitchAgainstWoolMinusButton_toggled(bool checked);
    void on_singleSwitchByWoolPlusButton_toggled(bool checked);
    void on_singleSwitchByWoolMinusButton_toggled(bool checked);
    void on_crossSwitchCoreCrossingButton_toggled(bool checked);
    void on_crossSwitchFixedLeftPlusButton_toggled(bool checked);
    void on_crossSwitchFixedRightPlusButton_toggled(bool checked);
    void on_crossSwitchFixedLeftMinusButton_toggled(bool checked);
    void on_crossSwitchFixedRightMinusButton_toggled(bool checked);
    void on_crossSwitchDeviceDirectionLeftPlusButton_toggled(bool checked);
    void on_crossSwitchDeviceDirectionRightPlusButton_toggled(bool checked);
    void on_crossSwitchDeviceDirectionLeftMinusButton_toggled(bool checked);
    void on_crossSwitchDeviceDirectionRightMinusButton_toggled(bool checked);
    void on_dropSwitchSingleLeftButton_toggled(bool checked);
    void on_dropSwitchSingleRightButton_toggled(bool checked);
    void on_dropSwitchDualLeftButton_toggled(bool checked);
    void on_dropSwitchDualRightButton_toggled(bool checked);
    void on_dropSwitchFixedDirectButton_toggled(bool checked);
    void on_dropSwitchFixedLateralButton_toggled(bool checked);
    void on_dropSwitchDeviceDirectionByWoolButton_toggled(bool checked);
    void on_dropSwitchDeviceDirectionAgainstWoolButton_toggled(bool checked);
    void on_editNoButton_released();
    void on_singleSwitchButton_released();
    void on_crossSwitchButton_released();
    void on_dropSwitchButton_released();
    void on_otherSwitchButton_released();
    void on_finishButton_released();
    void on_continueButton_released();
    void on_startButton_released();

private:
    Ui::SwitchTypePage* ui;
    QString _currentSwitchNumber;
    QString _currentControlVariant;
    SwitchType _currentSwitchType = SwitchType::Undefined;
    SwitchDirectionType _currentSwitchDirectionType = SwitchDirectionType::Undefined;
    SwitchFixedDirection _currentSwitchFixedDirection = SwitchFixedDirection::Undefined;
    DeviceDirection _currentDeviceDirection = DeviceDirection::Undefined;
    std::map<int, QString> _dict;
    bool _isStartControlSwitch;
    bool _isSpecialRailTypeOn;
};

#endif  // SWITCHTYPEPAGE_H
