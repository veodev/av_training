#ifndef WRONGCALIBRATIONMSG
#define WRONGCALIBRATIONMSG

#include <QWidget>
#include "dc_definitions.h"

namespace Ui
{
class WrongCalibrationMsg;
}

enum SignalFromCheckCalibration
{
    LeftSideWrongCalibration,
    RightSideWrongCalibration,
    LeftSideNo70Signal,
    RightSideNo70Signal
};

class WrongCalibrationMsg : public QWidget
{
    Q_OBJECT

public:
    explicit WrongCalibrationMsg(QWidget* parent = 0);
    ~WrongCalibrationMsg();

    void resetCountBoltJoints();

signals:
    void leaveTheWidget();

protected:
    bool event(QEvent* e);

private:
    void registrateWrongCalibration(eDeviceSide side, tQualityCalibrationRecType recType);
    bool isViewMsg();
    void checkCalibrationSignalsHandler(SignalFromCheckCalibration signal);

private slots:
    void onStartBoltJoint();
    void onLeftSideWrongCalibration();
    void onRightSideWrongCalibration();
    void onLeftSideNo70Signal();
    void onRightSideNo70Signal();
    void buttonsHandler();

private:
    Ui::WrongCalibrationMsg* ui;

    int _countOfBoltJoints;
};

#endif  // WRONGCALIBRATIONMSG
