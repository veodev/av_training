#include <QApplication>
#include <QWindow>
#include <QPluginLoader>
#include <QLabel>
#include <QTimer>
#include <QTextStream>
#include <QtGlobal>
#include <QTextCodec>
#include <sys/resource.h>
#include <iostream>
#include <time.h>
#include <QScreen>

#include "mainwindow.h"
#include "settings.h"
#include "debug.h"
#include "styles.h"
#include "languageswitcher.h"
#include "splashscreen.h"
#include "core.h"

#ifndef TARGET_AVICON15
void fixBadSystemDate()
{
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    int year = tm->tm_year + 1900;
    if (year < 2000) {
        std::cerr << year << " BAD DATE! Resetting system clock..." << std::endl;
        struct tm time;
        time.tm_year = 2018 - 1900;
        time.tm_mon = 3;
        time.tm_mday = 1;
        time.tm_hour = 12;
        time.tm_min = 0;
        time.tm_sec = 0;
        time_t t = mktime(&time);
        if (t != (time_t) -1) {
            stime(&t);
        }
        system("hwclock -w");
    }
}
#endif

void setStyleSheets()
{
    qApp->setStyleSheet(Styles::button());
    qApp->setStyleSheet(qApp->styleSheet() + "QListWidget {border-radius: 2px; border: 1px solid #959391; background-color: #ffffff;}");
}

Q_COREAPP_STARTUP_FUNCTION(setApplicationSettingsProperties)

int main(int argc, char* argv[])
{
    int exitCode = 0;
    bool isElTest = false;

    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == QString("-v")) {
            QTextStream out(stdout);
            out << APP_VERSION << endl;
            return 0;
        }
        else if (QString(argv[i]) == QString("-ec")) {
            exitCode = QString(argv[i + 1]).toInt();
            ++i;
        }
        else if (QString(argv[i]) == QString("-eltest")) {
            isElTest = true;
            qDebug() << "Start app in eltest mode";
        }
    }
    qDebug() << "Avicon started!";
#ifndef TARGET_AVICON15
    fixBadSystemDate();
#endif


    id_t pid = getpid();
    setpriority(PRIO_PROCESS, pid, -19);

    QApplication::setStyle("fusion");
    QApplication::setEffectEnabled(Qt::UI_AnimateMenu, false);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, false);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateToolBox, false);
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

#ifdef ATOM_DEVICE
    qApp->setOverrideCursor(QCursor(Qt::BlankCursor));
#endif

#if !defined TARGET_AVICON15
    QString language = restoreLanguage();
    if (language.isEmpty() == false) {
        LanguageSwitcher::switchLanguage(language);
    }
#else
    LanguageSwitcher::switchLanguage(QString("Russian"));
#endif
    setStyleSheets();

    SplashScreen* splashScreen = new SplashScreen();
    splashScreen->setGeometry(QGuiApplication::screens().first()->availableGeometry());
#if defined(TARGET_AVICON31) || defined(TARGET_AVICON15) || defined(TARGET_AVICON31E)
    splashScreen->setLogo(QPixmap(":/logo"));
#endif
#if defined(TARGET_AVICONDB) || defined(TARGET_AVICONDBHS)
    splashScreen->hideLine();
#endif
    splashScreen->setOrganizationName(organisationName().toUpper());
    splashScreen->setApplicationName(applicationName().toUpper());
    splashScreen->setApplicationVersion(APP_VERSION);
    splashScreen->showFullScreen();
    splashScreen->changeProgress(5);
    qApp->processEvents();

    MainWindow* w = new MainWindow;
    splashScreen->changeProgress(7);
    QObject::connect(w, &MainWindow::messageChanged, splashScreen, &SplashScreen::showMessage, Qt::DirectConnection);
    QObject::connect(w, &MainWindow::progressChanged, splashScreen, &SplashScreen::changeProgress, Qt::DirectConnection);
    QObject::connect(w, &MainWindow::inited, splashScreen, &SplashScreen::close, Qt::DirectConnection);
    splashScreen->changeProgress(10);
    QTimer::singleShot(100, w, &MainWindow::init);
    w->readPreviousExitCode(exitCode);
    w->setElectricTest(isElTest);

    int rc = a.exec();
    qDebug() << "Exec stopped!";
    delete splashScreen;
    delete w;
    Core::instance().finit();
    qDebug() << "Main exited!";
    return rc;
}
