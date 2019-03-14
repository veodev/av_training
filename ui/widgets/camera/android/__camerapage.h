#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui
{
class CameraPage;
}

class CameraPage : public QWidget
{
    Q_OBJECT

    enum CaptureMode
    {
        CaptureViewfinder = 0,
        CaptureStillImage = 0x01,
        CaptureVideo = 0x02
    };
    enum RecorderStatus
    {
        UnavailableStatus,
        UnloadedStatus,
        LoadingStatus,
        LoadedStatus,
        StartingStatus,
        RecordingStatus,
        PausedStatus,
        FinalizingStatus
    };

public:
    explicit CameraPage(QWidget* parent = 0);
    ~CameraPage();

    void setCapturedEnabled(bool value);

private:
    void cameraStart();
    void cameraStop();
    void zoomIn();
    void zoomOut();
    void setPhotoCaptureMode();
    void setVideoCaptureMode();
    void cameraCapture();
    void cameraRecordVideo();
    void cameraStopRecordVideo();
    void singleFocus();
    void restartCameraPage();

protected:
    void mousePressEvent(QMouseEvent* me);

signals:
    void doImageCaptured(const QString& imageFilename);  //
    void doVideoCaptured(const QString& videoFilename);  //

public slots:
    void setVisible(bool visible);

private slots:
    void onReadyForCaptureChanged(bool ready);
    void onDigitalZoomChanged(qreal value);
    void onCameraCaptureModeChanged(int mode);
    void onDurationChanged(int duration);
    void onVideoRecorderStatusChanged(int status);
    void on_captureButton_released();
    void on_zoomInButton_released();
    void on_zoomOutButton_released();
    void on_photoModeButton_released();
    void on_videoModeButton_released();
    void on_stopButton_released();
    void onImageSaved(QString fileName);
    void onRestartCameraPageForPhotoCapture();
    void onRestartCameraPageForVideoCapture();

private:
    Ui::CameraPage* ui;
    QObject* _rootObject;
    CaptureMode _currentCaptureMode = CaptureStillImage;
    QString _lastVideoFileName;
};

#endif  // CAMERAPAGE_H
