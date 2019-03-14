#include "limitsforsensspinboxnumber.h"
#include "debug.h"

LimitsForSensSpinBoxNumber::LimitsForSensSpinBoxNumber(quint16 angle, eInspectionMethod method, QWidget* parent)
    : SpinBoxNumber(parent)
    , _angle(angle)
    , _method(method)
{
    ASSERT(connect(this, &LimitsForSensSpinBoxNumber::valueChanged, this, &LimitsForSensSpinBoxNumber::onValueChanged));
}

LimitsForSensSpinBoxNumber::~LimitsForSensSpinBoxNumber()
{
}

void LimitsForSensSpinBoxNumber::onValueChanged(qreal value)
{
    emit valueChanged(_angle, _method, value);
}
