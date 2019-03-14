#ifndef SPINBOXNUMBER_H
#define SPINBOXNUMBER_H

#include <QString>

#include "spinboxbase.h"

class SpinBoxNumberDelegateBase
{
public:
    virtual ~SpinBoxNumberDelegateBase() {}

    virtual QString value(qreal value) = 0;
};

class SpinBoxNumber : public SpinBoxBase
{
    Q_OBJECT

    Q_PROPERTY(qreal value READ value WRITE setValue)
    Q_PROPERTY(qreal stepBy READ stepBy WRITE setStepBy)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)

public:
    explicit SpinBoxNumber(QWidget* parent = 0);
    virtual ~SpinBoxNumber();

    void setValue(qreal value, bool isEmitSignal = true);
    qreal value() const;

    void setStepBy(qreal steps);
    qreal stepBy() const;

    void setMaximum(qreal max);
    qreal maximum() const;

    void setMinimum(qreal min);
    qreal minimum() const;

    void setMinMaxStepValue(qreal min, qreal max, qreal step, qreal value);

    void setSuffix(const QString& suffix);
    QString suffix() const;

    void setPrecision(int precision);

    void setEnableVirtualKeyboard(bool value);

    void setDelegate(SpinBoxNumberDelegateBase* delegate);

    void refresh();

    void setZeroPrefix(bool value);

    void setZeroPrefixSize(int size);

    void enableMultiStep();

signals:
    void valueChanged(qreal value);

protected:
    virtual void displayValue();
    virtual void nextValue();
    virtual void prevValue();
    virtual bool isMinimumReached();
    virtual bool isMaximumReached();

private slots:
    void onLabelsClicked();
    void onValueChanged();

private:
    bool _zeroPrefix;
    bool _isMultiStep;
    unsigned char _zeroPrefixSize;
    unsigned char _precision;
    int _precisionHelper;
    int _value;
    int _stepBy;
    int _minimum;
    int _maximum;

    int _firstStep;
    int _secondStep;
    int _thirdStep;
    int _maxValueForFirstStep;
    int _maxValueForSecondStep;

    SpinBoxNumberDelegateBase* _delegate;
};

#endif  // SPINBOXNUMBER_H
