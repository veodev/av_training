#include "cameraworker.h"
#include <private/qvideoframe_p.h>
#include <QKeyEvent>
#include <QVideoFrame>
#include <QDateTime>
#include <QThread>
#include "appdatapath.h"
#include "accessories.h"

const int WATCHDOG_INTERVAL_MS = 5000;
const bool RESTART = true;

CameraWorker::CameraWorker(QObject* parent)
    : QObject(parent)
{
}

void CameraWorker::init()
{
    initCamera();
}

void CameraWorker::capture()
{
    QString photoFileName = photosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".jpg");
    _cameraImageCapture->capture(photoFileName);
}

void CameraWorker::startCamera()
{
    qDebug() << "CAMERA START";
    _elapsedTimer.start();
    _camera->start();
    _watchdog->start();
}

void CameraWorker::stopCamera()
{
    qDebug() << "CAMERA STOP";
    _watchdog->stop();
    _camera->stop();
    _camera->unload();
    _camera->unlock();
}

void CameraWorker::constructCamera()
{
    initCamera();
}

void CameraWorker::destroyCamera()
{
    deinitCamera();
}

void CameraWorker::onReadyForCaptureChanged(bool ready)
{
    emit doReadyForCaptureChanged(ready);
}

void CameraWorker::onImageCaptured(int id, const QImage& preview)
{
    Q_UNUSED(id);
    QImage image(preview);
    emit doImageCaptured(id, image);
}

void CameraWorker::onVideoFrameProbed(const QVideoFrame& frame)
{
    if (_elapsedTimer.elapsed() <= 50) {
        return;
    }
    if (frame.isValid()) {
        _watchdog->start();
        QImage image = qt_imageFromVideoFrame(frame);
        emit doImageViewfinder(image);
        _elapsedTimer.restart();
    }
}

void CameraWorker::onImageSaved(int id, const QString& fileName)
{
    Q_UNUSED(id);
    QString imageFileName = fileName;
    emit doImageSaved(imageFileName);
}

void CameraWorker::onWatchdogTimeout()
{
    if (_currentMode == QCamera::CaptureStillImage) {
        onPhotoMode();
    }
    else if (_currentMode == QCamera::CaptureVideo) {
        onVideoMode();
    }
}

void CameraWorker::onZoomIn()
{
    if (_cameraFocus->digitalZoom() != _cameraFocus->maximumDigitalZoom()) {
        _cameraFocus->zoomTo(1.0, _cameraFocus->digitalZoom() + 1.0);
    }
}

void CameraWorker::onZoomOut()
{
    if (_cameraFocus->digitalZoom() != 1.0) {
        _cameraFocus->zoomTo(1.0, _cameraFocus->digitalZoom() - 1.0);
    }
}

void CameraWorker::onRecord()
{
    if (_mediaRecorder->state() == QMediaRecorder::StoppedState || _mediaRecorder->state() == QMediaRecorder::PausedState) {
        _videoFileName = videosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".mp4");
        _mediaRecorder->setOutputLocation(QUrl::fromLocalFile(_videoFileName));
        _mediaRecorder->record();
    }
    else {
        _mediaRecorder->stop();
    }
}

void CameraWorker::onStop()
{
    if (_mediaRecorder->state() == QMediaRecorder::RecordingState) {
        _mediaRecorder->stop();
        emit doVideoSaved(_videoFileName);
    }
}

void CameraWorker::onPhotoMode()
{
    disconnect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);
    restartCamera();
    _currentMode = QCamera::CaptureStillImage;
    _camera->setCaptureMode(QCamera::CaptureStillImage);
    connect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);
    qDebug() << "=== PHOTO MODE";
}

void CameraWorker::onVideoMode()
{
    disconnect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);
    restartCamera();
    _currentMode = QCamera::CaptureVideo;
    _camera->setCaptureMode(QCamera::CaptureVideo);
    connect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);
    qDebug() << "=== VIDEO MODE";
}

void CameraWorker::initCamera()
{
    _camera = new QCamera(QCamera::BackFace);
    connect(_camera, &QCamera::captureModeChanged, this, &CameraWorker::doCaptureModeChanged);
    _camera->setCaptureMode(QCamera::CaptureStillImage);

    _cameraExposure = _camera->exposure();
    _cameraExposure->setExposureMode(QCameraExposure::ExposureAuto);

    QCameraViewfinderSettings settings;
    settings.setResolution(352, 288);
    settings.setMinimumFrameRate(16);
    settings.setMaximumFrameRate(16);
    settings.setPixelFormat(QVideoFrame::Format_ARGB32_Premultiplied);
    _camera->setViewfinderSettings(settings);

    _cameraFocus = _camera->focus();
    connect(_cameraFocus, &QCameraFocus::digitalZoomChanged, this, &CameraWorker::doDigitalZoomChanged);
    _cameraFocus->setFocusMode(QCameraFocus::ContinuousFocus);
    emit doDigitalZoomChanged(_cameraFocus->digitalZoom());

    _viewFinder = new QCameraViewfinder;
    _camera->setViewfinder(_viewFinder);

    _cameraImageCapture = new QCameraImageCapture(_camera);
    _cameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    connect(_cameraImageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &CameraWorker::onReadyForCaptureChanged);
    connect(_cameraImageCapture, &QCameraImageCapture::imageCaptured, this, &CameraWorker::onImageCaptured);
    connect(_cameraImageCapture, &QCameraImageCapture::imageSaved, this, &CameraWorker::onImageSaved);

    _videoProbe = new QVideoProbe;
    _videoProbe->setSource(_camera);
    connect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);

    QVideoEncoderSettings videoSettings;
    videoSettings.setCodec("h264");
    videoSettings.setResolution(352, 288);
    videoSettings.setQuality(QMultimedia::HighQuality);
    videoSettings.setFrameRate(30);

    _mediaRecorder = new QMediaRecorder(_camera);
    _mediaRecorder->setVideoSettings(videoSettings);
    _mediaRecorder->setContainerFormat("mp4");
    connect(_mediaRecorder, &QMediaRecorder::durationChanged, this, &CameraWorker::doVideoDurationChanged);
    connect(_mediaRecorder, &QMediaRecorder::stateChanged, this, &CameraWorker::doMediaRecorderStateChanged);

    _watchdog = new QTimer;
    _watchdog->setInterval(WATCHDOG_INTERVAL_MS);
    connect(_watchdog, &QTimer::timeout, this, &CameraWorker::onWatchdogTimeout);

    emit doReadyForCaptureChanged(true);
    qDebug() << "=== INIT CAMERA";
}

void CameraWorker::deinitCamera()
{
    disconnect(_camera, &QCamera::captureModeChanged, this, &CameraWorker::doCaptureModeChanged);
    _camera->deleteLater();
    _camera = nullptr;

    disconnect(_cameraFocus, &QCameraFocus::digitalZoomChanged, this, &CameraWorker::doDigitalZoomChanged);
    _viewFinder->deleteLater();
    _viewFinder = nullptr;

    disconnect(_cameraImageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &CameraWorker::onReadyForCaptureChanged);
    disconnect(_cameraImageCapture, &QCameraImageCapture::imageCaptured, this, &CameraWorker::onImageCaptured);
    disconnect(_cameraImageCapture, &QCameraImageCapture::imageSaved, this, &CameraWorker::onImageSaved);
    _cameraImageCapture->deleteLater();
    _cameraImageCapture = nullptr;

    disconnect(_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraWorker::onVideoFrameProbed);
    _videoProbe->deleteLater();
    _videoProbe = nullptr;

    disconnect(_mediaRecorder, &QMediaRecorder::durationChanged, this, &CameraWorker::doVideoDurationChanged);
    disconnect(_mediaRecorder, &QMediaRecorder::stateChanged, this, &CameraWorker::doMediaRecorderStateChanged);
    _mediaRecorder->deleteLater();
    _mediaRecorder = nullptr;

    disconnect(_watchdog, &QTimer::timeout, this, &CameraWorker::onWatchdogTimeout);
    _watchdog->deleteLater();
    _watchdog = nullptr;
    qDebug() << "=== DEINIT CAMERA";
}

void CameraWorker::restartCamera()
{
    stopCamera();
    delay(200);
    deinitCamera();
    delay(200);
    initCamera();
    delay(200);
    startCamera();
}
