#ifndef APPDATAPATH_H
#define APPDATAPATH_H

#include <QString>

QString appDataPath(const QString& subdir = QString("/"));
QString registrationsPath();
QString registrationServicePath();
QString registrationServiceScreenShoots();
QString videosPath();
QString photosPath();
QString commentsPath();
QString screenshotsPath();
QString logsPath();
QString soundsPath();
QString ekasuiPath();
QString marksPath();

#endif  // APPDATAPATH_H
