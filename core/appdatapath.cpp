#include <QCoreApplication>
#include <QDir>

#include "appdatapath.h"

const static char* registrationsFolder = "/data/";
const static char* registrationServiceFolder = "/service/";
const static char* registrationScreenShootServiceFolder = "/serviceScreenShots/";
const static char* videosFolder = "/video/";
const static char* photosFolder = "/photo/";
const static char* commentsFolder = "/comments/";
const static char* screenshotsFolder = "/screenshots/";
const static char* logsFolder = "/log/";
const static char* marksLocalFolder = "/marks/";
const static char* soundsFolder = "/sounds/";
const static char* ekasuiFolder = "/ekasui/";

QString appDataPath(const QString& subdir)
{
    QString applicationName = qApp->applicationName();
    if (applicationName.isEmpty()) {
        applicationName = QCoreApplication::arguments().at(0);
    }
    QString rootDir;
#if defined ANDROID
    rootDir = "/sdcard";
#else
    rootDir = QDir::homePath();
#endif

#ifdef TARGET_AVICON31E
    QString path = rootDir + QString("/") + qApp->applicationName() + subdir;
#else
    QString path = rootDir + QString("/.") + qApp->applicationName() + subdir;
#endif
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }

    return path;
}

QString registrationsPath()
{
    return appDataPath(registrationsFolder);
}

QString registrationServicePath()
{
    return appDataPath(registrationServiceFolder);
}

QString videosPath()
{
    return appDataPath(videosFolder);
}

QString photosPath()
{
    return appDataPath(photosFolder);
}

QString commentsPath()
{
    return appDataPath(commentsFolder);
}

QString screenshotsPath()
{
    return appDataPath(screenshotsFolder);
}

QString logsPath()
{
    return appDataPath(logsFolder);
}

QString registrationServiceScreenShoots()
{
    return appDataPath(registrationScreenShootServiceFolder);
}

QString soundsPath()
{
    return appDataPath(soundsFolder);
}

QString ekasuiPath()
{
    return appDataPath(ekasuiFolder);
}

QString marksPath()
{
    return appDataPath(marksLocalFolder);
}
