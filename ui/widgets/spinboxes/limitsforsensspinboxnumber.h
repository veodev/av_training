#ifndef LIMITSFORSENSSPINBOXNUMBER_H
#define LIMITSFORSENSSPINBOXNUMBER_H

#include "spinboxnumber.h"
#include "Definitions.h"

class LimitsForSensSpinBoxNumber : public SpinBoxNumber
{
    Q_OBJECT

public:
    LimitsForSensSpinBoxNumber(quint16 angle, eInspectionMethod method, QWidget* parent = 0);
    virtual ~LimitsForSensSpinBoxNumber();

signals:
    void valueChanged(quint16 angle, eInspectionMethod method, int value);

private slots:
    void onValueChanged(qreal value);

private:
    int _angle;
    eInspectionMethod _method;
};

#endif  // LIMITSFORSENSSPINBOXNUMBER_H
