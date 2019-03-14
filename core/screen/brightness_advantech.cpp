#include <QDebug>
#include <QProcess>

//const static char* brightnessPath = "/sys/class/backlight/pkk/brightness";
//const static char* brightnessPath2 = "/sys/class/backlight/pkk_display0/brightness";

bool setScreenBrightness(double value)
{
    int exitCode = 1;
    if (value >= 0 && value <= 100) {
        double brightness = (value / 100.0) + 0.1;
        QString command = QString("xrandr --output eDP-1 --brightness ") +  QString::number(brightness);
        QProcess process;
        process.start(command);
        process.waitForStarted();
        process.waitForFinished();
        exitCode = process.exitCode();
        qDebug() << "Brightness ADVANTECH:" << value;
    }
    return exitCode;
}

double screenBrightness()
{    
    return 0.0;
}
