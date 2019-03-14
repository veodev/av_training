#include "spinboxbase.h"
#include "ui_spinboxbase.h"

#include "styles.h"

SpinBoxBase::SpinBoxBase(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SpinBoxBase)
{
    ui->setupUi(this);
    ui->caption->hide();
    ui->leftButton->setStyleSheet(Styles::spinBoxButton());
    ui->rightButton->setStyleSheet(Styles::spinBoxButton());
}

SpinBoxBase::~SpinBoxBase()
{
    delete ui;
}

void SpinBoxBase::setCaption(const QString& caption)
{
    ui->caption->setText(caption);
}

QString SpinBoxBase::caption() const
{
    return ui->caption->text();
}

void SpinBoxBase::enableCaption(bool enable)
{
    if (enable) {
        ui->caption->show();
    }
    else {
        ui->caption->hide();
    }
}

bool SpinBoxBase::isCaptionEnabled() const
{
    return !(ui->caption->isHidden());
}

void SpinBoxBase::setCaptionFontSize(int value)
{
    QFont font(ui->caption->font());
    font.setPointSize(value);
    ui->caption->setFont(font);
}

void SpinBoxBase::setValueFontSize(int value)
{
    QFont font(ui->value->font());
    font.setPointSize(value);
    ui->value->setFont(font);
}

void SpinBoxBase::setColor(const QColor& color)
{
    int r, g, b, a;
    color.getRgb(&r, &g, &b, &a);
    ui->value->setStyleSheet("background-color: rgb(" + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + ")");
}

bool SpinBoxBase::isMinimumReached()
{
    return false;
}

bool SpinBoxBase::isMaximumReached()
{
    return false;
}

void SpinBoxBase::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    this->focusPreviousChild();
}

void SpinBoxBase::timerStop()
{
    _timer.stop();
    disconnect(&_timer, &QTimer::timeout, this, 0);
}

void SpinBoxBase::onLeft()
{
    if (_timerInterval > 30) {
        --_timerInterval;
    }
    _timer.setInterval(_timerInterval);
    prevValue();
    if (isMinimumReached()) {
        timerStop();
    }
}

void SpinBoxBase::onRight()
{
    if (_timerInterval > 30) {
        --_timerInterval;
    }
    _timer.setInterval(_timerInterval);
    nextValue();
    if (isMaximumReached()) {
        timerStop();
    }
}


void SpinBoxBase::on_leftButton_pressed()
{
    prevValue();
    _timerInterval = 100;
    connect(&_timer, &QTimer::timeout, this, &SpinBoxBase::onLeft);
    _timer.start(300);
}

void SpinBoxBase::on_leftButton_released()
{
    timerStop();
}

void SpinBoxBase::on_rightButton_pressed()
{
    nextValue();
    _timerInterval = 100;
    connect(&_timer, &QTimer::timeout, this, &SpinBoxBase::onRight);
    _timer.start(300);
}

void SpinBoxBase::on_rightButton_released()
{
    timerStop();
}
