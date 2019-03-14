#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>


class SplashScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SplashScreen(QWidget* parent = 0);
    ~SplashScreen();

    void setLogo(const QPixmap& pixmap = QPixmap());
    void setOrganizationName(const QString& orgName);
    void setApplicationName(const QString& appName);
    void setApplicationVersion(const QString& appVer);
    void setMessage(const QString& message = "");
    void hideProgressBar();
    void hideLine();

signals:

public slots:
    void showMessage(const QString& message = "");
    void changeProgress(qreal percent);

private:
    QLabel _logoLabel;
    QLabel _orgNameLabel;
    QLabel _lineLabel;
    QLabel _appNameLabel;
    QLabel _appVerLabel;
    QLabel _messageLabel;
    QProgressBar _progressBar;

    QPixmap _logo;
    QString _orgname;
    QString _appName;
    QString _appVer;
};

#endif  // SPLASHSCREEN_H
