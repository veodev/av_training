#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>

#include <QWidget>

namespace Ui {
    class CameraPage;
}

class CameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPage(QWidget *parent = 0);
    ~CameraPage();
    void setCapturedEnabled(bool value);

public slots:
    void setVisible(bool visible);

signals:
    void doImageCaptured(const QString & imageFilename);
    void doVideoCaptured(const QString & videoFilename);

protected:
    bool event(QEvent * e);

private slots:
    void on_takeAPhotoPushButton_released();
    void on_recordVideoPushButton_released();

    void displayRecorderError();
    void displayCameraError();

    void imageSaved(int id, const QString & fileName);

    void updateRecordTime();
    void updateRecorderState(QMediaRecorder::State state);

private:
    Ui::CameraPage *ui;

    QCamera * _camera;
    QCameraImageCapture * _imageCapture;
    QMediaRecorder * _mediaRecorder;
};

#endif // CAMERAPAGE_H
