#ifndef LATERALPANELFSM_H
#define LATERALPANELFSM_H

#include <QObject>

class LateralPanelFsm : public QObject
{
    Q_OBJECT

public:
    explicit LateralPanelFsm(QObject* parent = 0);

    enum States
    {
        normalState,
        alarmState,
        selectedState,
        alarmAndSelectedState,
        normalHideBlinkState,
        alarmHideBlinkState,
        selectedHideBlinkState,
        alarmAndSelectedHideBlinkState,
    };

    void reset();
    void select();
    void unselect();
    void alarm();
    void unalarm();
    void hideBlink();
    void showBlink();
    void updateFsm();

    States state() const;

signals:
    void doNormalState();
    void doAlarmState();
    void doSelectedState();
    void doAlarmAndSelectedState();

    void doNormalHideBlinkState();
    void doAlarmHideBlinkState();
    void doSelectedHideBlinkState();
    void doAlarmAndSelectedHideBlinkState();

protected:
    void setNormalState();
    void setAlarmState();
    void setSelectedState();
    void setAlarmAndSelectedState();

    void setNormalHideBlinkState();
    void setAlarmHideBlinkState();
    void setSelectedHideBlinkState();
    void setAlarmAndSelectedHideBlinkState();

private:
    States _state;
};

#endif  // LATERALPANELFSM_H
