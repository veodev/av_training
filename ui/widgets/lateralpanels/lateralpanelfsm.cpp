#include "lateralpanelfsm.h"

LateralPanelFsm::LateralPanelFsm(QObject* parent)
    : QObject(parent)
    , _state(normalState)
{
}

void LateralPanelFsm::reset()
{
    setNormalState();
}

void LateralPanelFsm::select()
{
    if (_state == normalState) {
        setSelectedState();
    }
    else if (_state == alarmState) {
        setAlarmAndSelectedState();
    }
    else if (_state == normalHideBlinkState) {
        setSelectedHideBlinkState();
    }
    else if (_state == alarmHideBlinkState) {
        setAlarmAndSelectedHideBlinkState();
    }
}

void LateralPanelFsm::unselect()
{
    if (_state == selectedState) {
        setNormalState();
    }
    else if (_state == alarmAndSelectedState) {
        setAlarmState();
    }
    else if (_state == selectedHideBlinkState) {
        setNormalHideBlinkState();
    }
    else if (_state == alarmAndSelectedHideBlinkState) {
        setAlarmHideBlinkState();
    }
}

void LateralPanelFsm::alarm()
{
    if (_state == normalState) {
        setAlarmState();
    }
    else if (_state == selectedState) {
        setAlarmAndSelectedState();
    }
    else if (_state == normalHideBlinkState) {
        setAlarmHideBlinkState();
    }
    else if (_state == selectedHideBlinkState) {
        setAlarmAndSelectedHideBlinkState();
    }
}

void LateralPanelFsm::unalarm()
{
    if (_state == alarmState) {
        setNormalState();
    }
    else if (_state == alarmAndSelectedState) {
        setSelectedState();
    }
    else if (_state == alarmHideBlinkState) {
        setNormalHideBlinkState();
    }
    else if (_state == alarmAndSelectedHideBlinkState) {
        setSelectedHideBlinkState();
    }
}

void LateralPanelFsm::hideBlink()
{
    if (_state == normalState) {
        setNormalHideBlinkState();
    }
    else if (_state == selectedState) {
        setSelectedHideBlinkState();
    }
    else if (_state == alarmState) {
        setAlarmHideBlinkState();
    }
    else if (_state == alarmAndSelectedState) {
        setAlarmAndSelectedHideBlinkState();
    }
}

void LateralPanelFsm::showBlink()
{
    if (_state == normalHideBlinkState) {
        setNormalState();
    }
    else if (_state == selectedHideBlinkState) {
        setSelectedState();
    }
    else if (_state == alarmHideBlinkState) {
        setAlarmState();
    }
    else if (_state == alarmAndSelectedHideBlinkState) {
        setAlarmAndSelectedState();
    }
}

void LateralPanelFsm::updateFsm()
{
    switch (_state) {
    case normalState:
        emit doNormalState();
        break;
    case alarmState:
        emit doAlarmState();
        break;
    case selectedState:
        emit doSelectedState();
        break;
    case alarmAndSelectedState:
        emit doAlarmAndSelectedState();
        break;
    case normalHideBlinkState:
        emit doNormalHideBlinkState();
        break;
    case alarmHideBlinkState:
        emit doAlarmHideBlinkState();
        break;
    case selectedHideBlinkState:
        emit doSelectedHideBlinkState();
        break;
    case alarmAndSelectedHideBlinkState:
        emit doAlarmAndSelectedHideBlinkState();
        break;
    }
}

LateralPanelFsm::States LateralPanelFsm::state() const
{
    return _state;
}

void LateralPanelFsm::setNormalState()
{
    _state = normalState;
    emit doNormalState();
}

void LateralPanelFsm::setAlarmState()
{
    _state = alarmState;
    emit doAlarmState();
}

void LateralPanelFsm::setSelectedState()
{
    _state = selectedState;
    emit doSelectedState();
}

void LateralPanelFsm::setAlarmAndSelectedState()
{
    _state = alarmAndSelectedState;
    emit doAlarmAndSelectedState();
}

void LateralPanelFsm::setNormalHideBlinkState()
{
    _state = normalHideBlinkState;
    emit doNormalHideBlinkState();
}

void LateralPanelFsm::setAlarmHideBlinkState()
{
    _state = alarmHideBlinkState;
    emit doAlarmHideBlinkState();
}

void LateralPanelFsm::setSelectedHideBlinkState()
{
    _state = selectedHideBlinkState;
    emit doSelectedHideBlinkState();
}

void LateralPanelFsm::setAlarmAndSelectedHideBlinkState()
{
    _state = alarmAndSelectedHideBlinkState;
    emit doAlarmAndSelectedHideBlinkState();
}
