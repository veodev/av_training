#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include "androidJNI.h"

void setBrightnessJNI(int value)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();

    QAndroidJniObject::callStaticMethod<void>("com/radioavionica/avicon15/System", "setSystemBrightness", "(ILorg/qtproject/qt5/android/bindings/QtActivity;)V", (int) ((value * 255) / 100), activity.object<jobject>());
}

void loggerJNI(const QString& tag, const QString& msg)
{
    QAndroidJniObject androidMsg = QAndroidJniObject::fromString(msg);
    QAndroidJniObject androidTag = QAndroidJniObject::fromString(tag);


    QAndroidJniObject::callStaticMethod<jint>("android/util/Log", "d", "(Ljava/lang/String;Ljava/lang/String;)I", androidTag.object<jstring>(), androidMsg.object<jstring>());
}

int getBatteryLevelJNI()
{
    QAndroidJniObject activity = QtAndroid::androidActivity();

    return QAndroidJniObject::callStaticMethod<jint>("com/radioavionica/avicon15/System", "getBatteryLevel", "(Lorg/qtproject/qt5/android/bindings/QtActivity;)I", activity.object<jobject>());
}

int getAvailableExternalMemoryPercentJNI()
{
    return QAndroidJniObject::callStaticMethod<jint>("com/radioavionica/avicon15/System", "getAvailableExternalMemoryPercent", "()I");
}
