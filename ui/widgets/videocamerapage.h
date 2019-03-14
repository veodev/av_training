#ifndef VIDEOCAMERAPAGE_H
#define VIDEOCAMERAPAGE_H

#include <QWidget>

namespace Ui
{
class VideoCameraPage;
}

class VideoCameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit VideoCameraPage(QWidget* parent = 0);
    ~VideoCameraPage();

    void setCapturedEnabled(bool value);

signals:
    void doBackFromVideoCameraPage();

public slots:
    void setVisible(bool visible);
private slots:
    void onImageCaptured(const QString& imageFilename);
    void onVideoCaptured(const QString& videoFilename);

    void on_backButton_released();

private:
    Ui::VideoCameraPage* ui;
};

#endif  // VIDEOCAMERAPAGE_H
