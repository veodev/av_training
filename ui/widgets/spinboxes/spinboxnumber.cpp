#include "spinboxnumber.h"
#include "ui_spinboxbase.h"

#include "virtualkeyboards.h"
#include "debug.h"

const float FIRST_STEP = 0.5;
const float SECOND_STEP = 2.0;
const float THIRD_STEP = 5.0;
const int MAX_VALUE_FOR_FIRST_STEP = 10;
const int MAX_VALUE_FOR_SECOND_STEP = 20;

static int pow10(int n)
{
    Q_ASSERT(n < 9);
    int rez = 1;
    while ((n--) > 0) {
        rez *= 10;
    }
    return rez;
}

SpinBoxNumber::SpinBoxNumber(QWidget* parent)
    : SpinBoxBase(parent)
    , _zeroPrefix(false)
    , _isMultiStep(false)
    , _zeroPrefixSize(0)
    , _precision(0)
    , _precisionHelper(::pow10(_precision))
    , _value(1 * _precisionHelper)
    , _stepBy(1 * _precisionHelper)
    , _minimum(1 * _precisionHelper)
    , _maximum(100 * _precisionHelper)
    , _firstStep(0)
    , _secondStep(0)
    , _thirdStep(0)
    , _maxValueForFirstStep(0)
    , _maxValueForSecondStep(0)
    , _delegate(Q_NULLPTR)
{
    displayValue();
}

SpinBoxNumber::~SpinBoxNumber() {}

void SpinBoxNumber::setValue(qreal value, bool isEmitSignal)
{
    int newValue = value * _precisionHelper;
    int prevValue = _value;
    if (newValue <= _minimum) {
        _value = _minimum;
    }
    else if (newValue >= _maximum) {
        _value = _maximum;
    }
    else {
        _value = newValue;
    }
    if (prevValue != _value) {
        displayValue();
        if (isEmitSignal) {
            emit valueChanged(static_cast<qreal>(_value) / _precisionHelper);
        }
    }
}

qreal SpinBoxNumber::value() const
{
    return static_cast<qreal>(_value);
}

void SpinBoxNumber::setStepBy(qreal steps)
{
    _stepBy = steps * _precisionHelper;
}

qreal SpinBoxNumber::stepBy() const
{
    return (_stepBy / _precisionHelper);
}

void SpinBoxNumber::setMaximum(qreal max)
{
    _maximum = max * _precisionHelper;
    displayValue();
}

qreal SpinBoxNumber::maximum() const
{
    return (static_cast<qreal>(_maximum) / _precisionHelper);
}

void SpinBoxNumber::setMinimum(qreal min)
{
    _minimum = min * _precisionHelper;
    displayValue();
}

qreal SpinBoxNumber::minimum() const
{
    return (static_cast<qreal>(_minimum) / _precisionHelper);
}

void SpinBoxNumber::setMinMaxStepValue(qreal min, qreal max, qreal step, qreal value)
{
    setMinimum(min);
    setMaximum(max);
    setStepBy(step);
    setValue(value);
}

void SpinBoxNumber::setSuffix(const QString& suffix)
{
    _suffix = suffix;
    displayValue();
}

QString SpinBoxNumber::suffix() const
{
    return _suffix;
}

void SpinBoxNumber::setPrecision(int precision)
{
    Q_ASSERT(precision < 9);
    int precisionHelper = ::pow10(precision);
    _value = (static_cast<qreal>(_value) / _precisionHelper) * precisionHelper;
    _stepBy = (static_cast<qreal>(_stepBy) / _precisionHelper) * precisionHelper;
    _minimum = (static_cast<qreal>(_minimum) / _precisionHelper) * precisionHelper;
    _maximum = (static_cast<qreal>(_maximum) / _precisionHelper) * precisionHelper;
    _precision = precision;
    _precisionHelper = precisionHelper;
}

void SpinBoxNumber::setEnableVirtualKeyboard(bool value)
{
    if (value == true) {
        ASSERT(connect(ui->caption, &SpinBoxLabel::clicked, this, &SpinBoxNumber::onLabelsClicked, Qt::UniqueConnection));
        ASSERT(connect(ui->value, &SpinBoxLabel::clicked, this, &SpinBoxNumber::onLabelsClicked, Qt::UniqueConnection));
    }
    else {
        disconnect(ui->caption, &SpinBoxLabel::clicked, this, &SpinBoxNumber::onLabelsClicked);
        disconnect(ui->value, &SpinBoxLabel::clicked, this, &SpinBoxNumber::onLabelsClicked);
    }
}

void SpinBoxNumber::setDelegate(SpinBoxNumberDelegateBase* delegate)
{
    _delegate = delegate;
}

void SpinBoxNumber::refresh()
{
    displayValue();
}

void SpinBoxNumber::setZeroPrefix(bool value)
{
    _zeroPrefix = value;
}

void SpinBoxNumber::setZeroPrefixSize(int size)
{
    Q_ASSERT(size > 0);
    Q_ASSERT(size <= 255);
    _zeroPrefixSize = size;
}

void SpinBoxNumber::enableMultiStep()
{
    _isMultiStep = true;
    _firstStep = FIRST_STEP * _precisionHelper;
    _secondStep = SECOND_STEP * _precisionHelper;
    _thirdStep = THIRD_STEP * _precisionHelper;
    _maxValueForFirstStep = MAX_VALUE_FOR_FIRST_STEP * _precisionHelper;
    _maxValueForSecondStep = MAX_VALUE_FOR_SECOND_STEP * _precisionHelper;
}

void SpinBoxNumber::displayValue()
{
    if (_delegate != Q_NULLPTR) {
        ui->value->setText(_delegate->value(static_cast<qreal>(_value) / _precisionHelper) + _suffix);
    }
    else {
        QString num;

        if (_precision == 0) {
            num = QString::number(static_cast<qreal>(_value) / _precisionHelper) + _suffix;
        }
        else {
            _value = (qRound(static_cast<qreal>(_value) / 10)) * 10;
            num = QString::number(static_cast<qreal>(_value) / _precisionHelper, 'g', _precision) + _suffix;
        }
        if (_zeroPrefix) {
            int numSize = num.length();
            if (numSize >= _zeroPrefixSize) {
                ui->value->setText(num);
            }
            else {
                int zeroNum = _zeroPrefixSize - numSize;
                if (num.at(0) == '-') {
                    num.remove(0, 1);
                    for (int i = 0; i < zeroNum + 1; ++i) {
                        num.prepend('0');
                    }
                    num.prepend('-');
                }
                else {
                    for (int i = 0; i < zeroNum; ++i) {
                        num.prepend('0');
                    }
                }
                ui->value->setText(num);
            }
        }
        else {
            ui->value->setText(num);
        }
    }

    _value == _minimum ? ui->leftButton->setEnabled(false) : ui->leftButton->setEnabled(true);
    _value == _maximum ? ui->rightButton->setEnabled(false) : ui->rightButton->setEnabled(true);
}

void SpinBoxNumber::nextValue()
{
    int prevValue = _value;
    int newValue = _value + _stepBy;
    if (_isMultiStep) {
        if (newValue <= _maxValueForFirstStep && _stepBy != _firstStep) {
            _stepBy = _firstStep;
        }
        else if (newValue > _maxValueForFirstStep && newValue <= _maxValueForSecondStep && _stepBy != _secondStep) {
            _stepBy = _secondStep;
        }
        else if (newValue > _maxValueForSecondStep && _stepBy != _thirdStep) {
            _stepBy = _thirdStep;
        }
    }
    newValue <= _maximum ? _value = _value + _stepBy : _value = _maximum;
    if (prevValue != _value) {
        displayValue();
        emit valueChanged(static_cast<qreal>(_value) / _precisionHelper);
    }
}

void SpinBoxNumber::prevValue()
{
    int prevValue = _value;
    int newValue = _value - _stepBy;
    if (_isMultiStep) {
        if (newValue < _maxValueForFirstStep && _stepBy != _firstStep) {
            _stepBy = _firstStep;
        }
        else if (newValue >= _maxValueForFirstStep && newValue < _maxValueForSecondStep && _stepBy != _secondStep) {
            _stepBy = _secondStep;
        }
        else if (newValue >= _maxValueForSecondStep && _stepBy != _thirdStep) {
            _stepBy = _thirdStep;
        }
    }
    newValue >= _minimum ? _value = _value - _stepBy : _value = _minimum;
    if (prevValue != _value) {
        displayValue();
        emit valueChanged(static_cast<qreal>(_value) / _precisionHelper);
    }
}

bool SpinBoxNumber::isMinimumReached()
{
    return (_value == _minimum);
}

bool SpinBoxNumber::isMaximumReached()
{
    return (_value == _maximum);
}

void SpinBoxNumber::onLabelsClicked()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &SpinBoxNumber::onValueChanged);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::NumbersLimiting, _maximum);
    virtualKeyboards->clear();
    virtualKeyboards->setValue(QString::number(static_cast<qreal>(_value) / _precisionHelper));
    virtualKeyboards->show();
}

void SpinBoxNumber::onValueChanged()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    setValue(virtualKeyboards->value().toDouble());
    virtualKeyboards->setValidator(VirtualKeyboards::None);
}
