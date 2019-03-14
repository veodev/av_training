#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QCamera>
#include <QCameraFocus>
#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <QCameraExposure>
#include <QMediaRecorder>
#include <QVideoProbe>
#include <atomic>
#include <QElapsedTimer>
#include <QTimer>

class CameraWorker : public QObject
{
    Q_OBJECT
public:
    explicit CameraWorker(QObject* parent = nullptr);

    void init();
    void startCamera();
    void stopCamera();
    void capture();
    void constructCamera();
    void destroyCamera();

public slots:
    void onZoomIn();
    void onZoomOut();
    void onRecord();
    void onStop();
    void onPhotoMode();
    void onVideoMode();

private:
    void initCamera();
    void deinitCamera();
    void restartCamera();

signals:
    void doImageCaptured(int id, const QImage& preview);
    void doReadyForCaptureChanged(bool ready);
    void doImageViewfinder(const QImage& preview);
    void doVideoDurationChanged(qint64 duration);
    void doDigitalZoomChanged(qreal value);
    void doCaptureModeChanged(QCamera::CaptureModes mode);
    void doMediaRecorderStateChanged(QMediaRecorder::State state);
    void doImageSaved(QString& fileName);
    void doVideoSaved(QString& fileName);

private slots:
    void onReadyForCaptureChanged(bool ready);
    void onImageCaptured(int id, const QImage& preview);
    void onVideoFrameProbed(const QVideoFrame& frame);
    void onImageSaved(int id, const QString& fileName);
    void onWatchdogTimeout();

private:
    QCamera* _camera;
    QCameraFocus* _cameraFocus;
    QCameraImageCapture* _cameraImageCapture;
    QVideoProbe* _videoProbe;
    QMediaRecorder* _mediaRecorder;
    QCameraExposure* _cameraExposure;
    QString _videoFileName;
    QElapsedTimer _elapsedTimer;
    QCameraViewfinder* _viewFinder;
    QCamera::CaptureMode _currentMode;
    QTimer* _watchdog;
};

#endif  // CAMERAWORKER_H
