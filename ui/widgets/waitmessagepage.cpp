#include "waitmessagepage.h"
#include "ui_waitmessagepage.h"

const QString processAnimationSymbols = "|/-\\";
const int stepTimerInterval = 100;

WaitMessagePage::WaitMessagePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WaitMessagePage)
    , _processTimer(new QTimer(this))
    , _stepTimer(new QTimer(this))
    , _index(0)
    , _enableClick(false)
    , _message(QString())
{
    ui->setupUi(this);

    _stepTimer->setInterval(stepTimerInterval);
    connect(_stepTimer, &QTimer::timeout, this, &WaitMessagePage::onStepTimerTimeout);
    connect(_processTimer, &QTimer::timeout, this, &WaitMessagePage::onProcessTimerTimeout);
}

WaitMessagePage::~WaitMessagePage()
{
    delete _processTimer;
    delete _stepTimer;
    delete ui;
}

void WaitMessagePage::setText(const QString& text)
{
    _message = text;
    ui->messageLabel->setText(_message);
}

void WaitMessagePage::startProcessTimer(int msec)
{
    _index = 0;
    _processTimer->start(msec);
    _stepTimer->start();
}

void WaitMessagePage::stopProcessTimer()
{
    _processTimer->stop();
    _stepTimer->stop();
}

void WaitMessagePage::setClicked(bool enableClick)
{
    _enableClick = enableClick;
}

bool WaitMessagePage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void WaitMessagePage::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    if (_enableClick) {
        this->hide();
        emit clickWidget();
    }
}

void WaitMessagePage::onStepTimerTimeout()
{
    ui->messageLabel->setText(_message + QString(" ... ") + processAnimationSymbols[_index]);
    ++_index;
    if (_index == processAnimationSymbols.size()) {
        _index = 0;
    }
}

void WaitMessagePage::onProcessTimerTimeout()
{
    stopProcessTimer();
    emit timeIsOut();
}
