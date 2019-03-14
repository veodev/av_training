#ifndef ANDROIDJNI_H
#define ANDROIDJNI_H

#include <QString>

void setBrightnessJNI(int value);
void loggerJNI(const QString& tag, const QString& msg);
int getBatteryLevelJNI();
int getAvailableExternalMemoryPercentJNI();

#endif  // ANDROIDJNI_H
