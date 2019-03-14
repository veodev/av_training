#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include <QWidget>
#include <QThread>
#include <QCamera>
#include <QMediaRecorder>

class CameraWorker;

namespace Ui
{
class CameraPage;
}

class CameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPage(QWidget* parent = 0);
    ~CameraPage();

    void setCapturedEnabled(bool value);

private:
    void unblockZoomControls();
    void blockZoomControls(bool isBlock);
    void connectSignals();
    void disconnectSignals();
    void resetViewFinder();

signals:
    void doCapture();
    void doStart();
    void doStop();
    void doImageCaptured(const QString& imageFilename);
    void doVideoCaptured(const QString& videoFilename);
    void doZoomIn();
    void doZoomOut();
    void doRecord();
    void doStopRecord();
    void doVideoMode();
    void doPhotoMode();

    void doConstructCamera();
    void doDestroyCamera();

public slots:
    void setVisible(bool visible);

private slots:
    void onReadyForCaptureChanged(bool ready);
    void onImageCaptured(int id, const QImage& preview);
    void onImageViewfinder(const QImage& preview);
    void onDigitalZoomChanged(qreal value);
    void onCaptureModeChanged(QCamera::CaptureModes mode);
    void onVideoDurationChanged(qint64 duration);
    void onMediaRecorderStateChanged(QMediaRecorder::State state);
    void on_captureButton_released();
    void on_zoomInButton_released();
    void on_zoomOutButton_released();
    void on_photoModeButton_released();
    void on_videoModeButton_released();
    void on_stopButton_released();

private:
    Ui::CameraPage* ui;

    QThread* _cameraWorkerThread;
    CameraWorker* _cameraWorker;
    QCamera::CaptureModes _currentMode;
};

#endif  // CAMERAPAGE_H
