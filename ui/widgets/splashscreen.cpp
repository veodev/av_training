
#include <QLayout>
#include <QDebug>
#include <QCoreApplication>

#include "splashscreen.h"

SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent, Qt::SplashScreen | Qt::FramelessWindowHint)
    , _logoLabel(this)
    , _orgNameLabel(this)
    , _appNameLabel(this)
    , _appVerLabel(this)
    , _messageLabel(this)
    , _progressBar(this)
    , _orgname("")
    , _appName("")
    , _appVer("")
{
    QPalette Pal(palette());

    // устанавливаем цвет фона
    Pal.setColor(QPalette::Background, Qt::white);
    setPalette(Pal);

    setAutoFillBackground(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(50);

#if defined(ANDROID)
    _progressBar.setMaximumHeight(5);
    _progressBar.setTextVisible(false);
#endif

    _logoLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _logoLabel.setFixedSize(80, 80);

    _orgNameLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
#if !defined ANDROID
    _orgNameLabel.setStyleSheet("color: rgba(50, 40, 134, 255); font: bold 28px; margin-top: 24px");
#else
    _orgNameLabel.setStyleSheet("color: rgba(50, 40, 134, 255); font: bold 30px;");
#endif

    _lineLabel.setStyleSheet("background-color: rgba(1, 157, 220, 255)");
    _lineLabel.setFixedHeight(5);
    _lineLabel.setAutoFillBackground(true);

    _appNameLabel.setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    _appNameLabel.setStyleSheet("font: bold 40px;");

    _appVerLabel.setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    _appVerLabel.setStyleSheet("font: bold 16px;");

    _messageLabel.setAlignment(Qt::AlignLeft);
    _messageLabel.setStyleSheet("color: rgba(50, 40, 134, 255); font: bold 16px;");

    _progressBar.setValue(0);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->setAlignment(Qt::AlignCenter);
    hBoxLayout->addWidget(&_logoLabel);
    hBoxLayout->addWidget(&_orgNameLabel);
    mainLayout->addLayout(hBoxLayout);
    mainLayout->addWidget(&_lineLabel);
    mainLayout->addStretch(2);
    mainLayout->addWidget(&_appNameLabel);
    mainLayout->addWidget(&_appVerLabel);
    mainLayout->addStretch(2);
    mainLayout->addWidget(&_messageLabel);
    mainLayout->addWidget(&_progressBar);
}

SplashScreen::~SplashScreen() {}

void SplashScreen::setLogo(const QPixmap& pixmap)
{
    _logoLabel.setPixmap(pixmap);
}

void SplashScreen::setOrganizationName(const QString& orgName)
{
    _orgname = orgName;
    _orgNameLabel.setText(_orgname);
}

void SplashScreen::setApplicationName(const QString& appName)
{
    _appName = appName;
    _appNameLabel.setText(_appName);
}

void SplashScreen::setApplicationVersion(const QString& appVer)
{
    _appVer = appVer;
    _appVerLabel.setText(_appVer);
}

void SplashScreen::setMessage(const QString& message)
{
    _messageLabel.setText(message);
}

void SplashScreen::hideProgressBar()
{
    _progressBar.hide();
    _messageLabel.setAlignment(Qt::AlignCenter);
}

void SplashScreen::hideLine()
{
    _lineLabel.hide();
}

void SplashScreen::showMessage(const QString& message)
{
    _messageLabel.setText(message);
    repaint();
    QCoreApplication::processEvents();
}

void SplashScreen::changeProgress(qreal percent)
{
    _progressBar.setValue(percent);
    repaint();
    QCoreApplication::processEvents();
}
