#include "camerapage.h"
#include "ui_camerapage.h"
#include "appdatapath.h"
#include "cameraworker.h"
#include "core.h"
#include "accessories.h"

#include <QKeyEvent>
#include <QTimer>
#include <QQmlEngine>

const int MAX_VIDEO_FILE_DURATION_SEC = 30;

CameraPage::CameraPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CameraPage)
{
    ui->setupUi(this);
    ui->stopButton->hide();
    ui->durationWidget->hide();
    QSurfaceFormat format = ui->quickWidget->format();
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    ui->quickWidget->setFormat(format);
    ui->quickWidget->setSource(QUrl("qrc:/qml/Camera.qml"));
    ui->quickWidget->setFixedSize(640, 400);
    qDebug() << "====== OPENGL PROFILE" << format.profile();
    qDebug() << "====== RENDER TYPE " << format.renderableType();
    qDebug() << "====== VERSION " << format.majorVersion() << "." << format.minorVersion();
    qDebug() << "====== SWAP BEHAVIOR " << format.swapBehavior();
    qDebug() << "====== SWAP INTERVAL " << format.swapInterval();
    _rootObject = reinterpret_cast<QObject*>(ui->quickWidget->rootObject());
    connect(_rootObject, SIGNAL(doCameraCaptureModeChanged(int)), this, SLOT(onCameraCaptureModeChanged(int)));
    connect(_rootObject, SIGNAL(doDigitalZoomChanged(double)), this, SLOT(onDigitalZoomChanged(double)));
    connect(_rootObject, SIGNAL(doDurationChanged(int)), this, SLOT(onDurationChanged(int)));
    connect(_rootObject, SIGNAL(doVideoRecorderStatusChanged(int)), this, SLOT(onVideoRecorderStatusChanged(int)));
    connect(_rootObject, SIGNAL(doImageSaved(QString)), this, SLOT(onImageSaved(QString)));
    connect(_rootObject, SIGNAL(doReadyForCaptureChanged(bool)), this, SLOT(onReadyForCaptureChanged(bool)));
    connect(_rootObject, SIGNAL(doRestartCameraPageForPhotoCapture()), this, SLOT(onRestartCameraPageForPhotoCapture()));
    connect(_rootObject, SIGNAL(doRestartCameraPageForVideoCapture()), this, SLOT(onRestartCameraPageForVideoCapture()));
}

CameraPage::~CameraPage() {}

void CameraPage::setVisible(bool visible)
{
    visible ? onRestartCameraPageForPhotoCapture() : cameraStop();
    QWidget::setVisible(visible);
}

void CameraPage::cameraStart()
{
    Core::instance().stopAudioOutput();
    QMetaObject::invokeMethod(_rootObject, "cameraStart");
}

void CameraPage::cameraStop()
{
    Core::instance().resumeAudioOutput();
    QMetaObject::invokeMethod(_rootObject, "cameraStop");
}

void CameraPage::zoomIn()
{
    QMetaObject::invokeMethod(_rootObject, "zoomIn");
}

void CameraPage::zoomOut()
{
    QMetaObject::invokeMethod(_rootObject, "zoomOut");
}

void CameraPage::setPhotoCaptureMode()
{
    QMetaObject::invokeMethod(_rootObject, "setPhotoCaptureMode");
}

void CameraPage::setVideoCaptureMode()
{
    QMetaObject::invokeMethod(_rootObject, "setVideoCaptureMode");
}

void CameraPage::cameraCapture()
{
    QVariant fileName = photosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".jpg");
    QMetaObject::invokeMethod(_rootObject, "captureImage", Q_ARG(QVariant, fileName));
}

void CameraPage::cameraRecordVideo()
{
    _lastVideoFileName = videosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".mp4");
    QVariant fileName = _lastVideoFileName;
    QMetaObject::invokeMethod(_rootObject, "captureVideo", Q_ARG(QVariant, fileName));
}

void CameraPage::cameraStopRecordVideo()
{
    QMetaObject::invokeMethod(_rootObject, "stopCaptureVideo");
}

void CameraPage::singleFocus()
{
    ui->captureButton->setDisabled(true);
    ui->stopButton->setDisabled(true);
    QMetaObject::invokeMethod(_rootObject, "singleFocus");
    delay(2000);
    ui->captureButton->setDisabled(false);
    ui->stopButton->setDisabled(false);
}

void CameraPage::restartCameraPage()
{
    disconnect(_rootObject, SIGNAL(doCameraCaptureModeChanged(int)), this, SLOT(onCameraCaptureModeChanged(int)));
    disconnect(_rootObject, SIGNAL(doDigitalZoomChanged(double)), this, SLOT(onDigitalZoomChanged(double)));
    disconnect(_rootObject, SIGNAL(doDurationChanged(int)), this, SLOT(onDurationChanged(int)));
    disconnect(_rootObject, SIGNAL(doVideoRecorderStatusChanged(int)), this, SLOT(onVideoRecorderStatusChanged(int)));
    disconnect(_rootObject, SIGNAL(doImageSaved(QString)), this, SLOT(onImageSaved(QString)));
    disconnect(_rootObject, SIGNAL(doReadyForCaptureChanged(bool)), this, SLOT(onReadyForCaptureChanged(bool)));
    disconnect(_rootObject, SIGNAL(doRestartCameraPageForPhotoCapture()), this, SLOT(onRestartCameraPageForPhotoCapture()));
    disconnect(_rootObject, SIGNAL(doRestartCameraPageForVideoCapture()), this, SLOT(onRestartCameraPageForVideoCapture()));
    ui->quickWidget->setSource(QUrl(""));
    ui->quickWidget->engine()->clearComponentCache();
    ui->quickWidget->setSource(QUrl("qrc:/qml/Camera.qml"));
    _rootObject = reinterpret_cast<QObject*>(ui->quickWidget->rootObject());
    connect(_rootObject, SIGNAL(doCameraCaptureModeChanged(int)), this, SLOT(onCameraCaptureModeChanged(int)));
    connect(_rootObject, SIGNAL(doDigitalZoomChanged(double)), this, SLOT(onDigitalZoomChanged(double)));
    connect(_rootObject, SIGNAL(doDurationChanged(int)), this, SLOT(onDurationChanged(int)));
    connect(_rootObject, SIGNAL(doVideoRecorderStatusChanged(int)), this, SLOT(onVideoRecorderStatusChanged(int)));
    connect(_rootObject, SIGNAL(doImageSaved(QString)), this, SLOT(onImageSaved(QString)));
    connect(_rootObject, SIGNAL(doReadyForCaptureChanged(bool)), this, SLOT(onReadyForCaptureChanged(bool)));
    connect(_rootObject, SIGNAL(doRestartCameraPageForPhotoCapture()), this, SLOT(onRestartCameraPageForPhotoCapture()));
    connect(_rootObject, SIGNAL(doRestartCameraPageForVideoCapture()), this, SLOT(onRestartCameraPageForVideoCapture()));
}

void CameraPage::mousePressEvent(QMouseEvent* me)
{
    Q_UNUSED(me);
    singleFocus();
}

void CameraPage::onReadyForCaptureChanged(bool ready)
{
    ui->captureButton->setEnabled(ready);
}

void CameraPage::onDigitalZoomChanged(qreal value)
{
    ui->zoomValueLabel->setText(QString::number(qRound(value), 'f', 1));
}

void CameraPage::onCameraCaptureModeChanged(int mode)
{
    if (mode == CaptureStillImage) {
        ui->modeValueLabel->setText(tr("PHOTO"));
        ui->photoModeButton->setEnabled(false);
        ui->videoModeButton->setEnabled(true);
        ui->stopButton->hide();
        ui->durationWidget->hide();
    }
    else if (mode == CaptureVideo) {
        ui->modeValueLabel->setText(tr("VIDEO"));
        ui->photoModeButton->setEnabled(true);
        ui->videoModeButton->setEnabled(false);
        ui->stopButton->hide();
        ui->durationWidget->show();
    }
    _currentCaptureMode = static_cast<CaptureMode>(mode);
}

void CameraPage::onDurationChanged(int duration)
{
    int seconds = duration / 1000;
    ui->durationValueLabel->setText(QString::number(seconds));
    if (seconds >= MAX_VIDEO_FILE_DURATION_SEC) {
        on_stopButton_released();
    }
}

void CameraPage::onVideoRecorderStatusChanged(int status)
{
    if (status == RecordingStatus) {
        ui->captureButton->hide();
        ui->stopButton->show();
        ui->recorderStateValueLabel->setText(tr("Rec"));
    }
    else if (status == LoadedStatus) {
        ui->captureButton->show();
        ui->stopButton->hide();
        ui->recorderStateValueLabel->setText(tr("Stop"));
    }
}

void CameraPage::setCapturedEnabled(bool value) {}


void CameraPage::on_captureButton_released()
{
    if (_currentCaptureMode == CaptureStillImage) {
        cameraCapture();
    }
    else if (_currentCaptureMode == CaptureVideo) {
        cameraRecordVideo();
    }
}

void CameraPage::on_zoomInButton_released()
{
    zoomIn();
}

void CameraPage::on_zoomOutButton_released()
{
    zoomOut();
}

void CameraPage::on_stopButton_released()
{
    cameraStopRecordVideo();
    emit doVideoCaptured(_lastVideoFileName);
}

void CameraPage::onImageSaved(QString fileName)
{
    doImageCaptured(fileName);
}

void CameraPage::onRestartCameraPageForPhotoCapture()
{
    restartCameraPage();
    QMetaObject::invokeMethod(_rootObject, "setPhotoCaptureModeAfterRestart");
}

void CameraPage::onRestartCameraPageForVideoCapture()
{
    restartCameraPage();
    QMetaObject::invokeMethod(_rootObject, "setVideoCaptureModeAfterRestart");
}

void CameraPage::on_photoModeButton_released()
{
    setPhotoCaptureMode();
}

void CameraPage::on_videoModeButton_released()
{
    setVideoCaptureMode();
}
