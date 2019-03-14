#include "camerapage.h"
#include "ui_camerapage.h"
#include "appdatapath.h"
#include "cameraworker.h"
#include "core.h"
#include "accessories.h"

#include <QKeyEvent>
#include <QTimer>

const int MAX_VIDEO_FILE_DURATION_SEC = 30;

CameraPage::CameraPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CameraPage)
{
    ui->setupUi(this);
    qRegisterMetaType<Qt::ApplicationState>("Qt::ApplicationState");
    _cameraWorkerThread = new QThread(this);
    _cameraWorkerThread->setObjectName("cameraWorkerThread");
    _cameraWorker = new CameraWorker();
    connectSignals();
    _cameraWorker->moveToThread(_cameraWorkerThread);
    _cameraWorkerThread->start();
}

CameraPage::~CameraPage()
{
    qDebug() << "Deleting camerapage...";
    disconnectSignals();
    _cameraWorkerThread->quit();
    _cameraWorkerThread->wait(5000);
    if (_cameraWorkerThread->isRunning()) {
        _cameraWorkerThread->terminate();
        _cameraWorkerThread->wait(5000);
    }
    delete ui;
    qDebug() << "Camerapage deleted!";
}

void CameraPage::setVisible(bool visible)
{
    if (visible) {
        resetViewFinder();
        Core::instance().stopAudioOutput();
        emit doConstructCamera();
        emit doStart();
    }
    else {
        Core::instance().resumeAudioOutput();
        emit doStop();
        emit doDestroyCamera();
    }
    QWidget::setVisible(visible);
}

void CameraPage::unblockZoomControls()
{
    ui->zoomInButton->setDisabled(false);
    ui->zoomOutButton->setDisabled(false);
}

void CameraPage::blockZoomControls(bool isBlock)
{
    ui->zoomInButton->setDisabled(isBlock);
    ui->zoomOutButton->setDisabled(isBlock);
}

void CameraPage::connectSignals()
{
    connect(_cameraWorkerThread, &QThread::started, _cameraWorker, &CameraWorker::init);
    connect(_cameraWorker, &CameraWorker::doImageCaptured, this, &CameraPage::onImageCaptured);
    connect(_cameraWorker, &CameraWorker::doReadyForCaptureChanged, this, &CameraPage::onReadyForCaptureChanged);
    connect(_cameraWorker, &CameraWorker::doImageViewfinder, this, &CameraPage::onImageViewfinder);
    connect(_cameraWorker, &CameraWorker::doDigitalZoomChanged, this, &CameraPage::onDigitalZoomChanged);
    connect(_cameraWorker, &CameraWorker::doCaptureModeChanged, this, &CameraPage::onCaptureModeChanged);
    connect(_cameraWorker, &CameraWorker::doVideoDurationChanged, this, &CameraPage::onVideoDurationChanged);
    connect(_cameraWorker, &CameraWorker::doMediaRecorderStateChanged, this, &CameraPage::onMediaRecorderStateChanged);
    connect(_cameraWorker, &CameraWorker::doImageSaved, this, &CameraPage::doImageCaptured);
    connect(_cameraWorker, &CameraWorker::doVideoSaved, this, &CameraPage::doVideoCaptured);
    connect(this, &CameraPage::doCapture, _cameraWorker, &CameraWorker::capture);
    connect(this, &CameraPage::doStart, _cameraWorker, &CameraWorker::startCamera);
    connect(this, &CameraPage::doStop, _cameraWorker, &CameraWorker::stopCamera);
    connect(this, &CameraPage::doZoomIn, _cameraWorker, &CameraWorker::onZoomIn);
    connect(this, &CameraPage::doZoomOut, _cameraWorker, &CameraWorker::onZoomOut);
    connect(this, &CameraPage::doRecord, _cameraWorker, &CameraWorker::onRecord);
    connect(this, &CameraPage::doStopRecord, _cameraWorker, &CameraWorker::onStop);
    connect(this, &CameraPage::doPhotoMode, _cameraWorker, &CameraWorker::onPhotoMode);
    connect(this, &CameraPage::doVideoMode, _cameraWorker, &CameraWorker::onVideoMode);
    connect(this, &CameraPage::doConstructCamera, _cameraWorker, &CameraWorker::constructCamera);
    connect(this, &CameraPage::doDestroyCamera, _cameraWorker, &CameraWorker::destroyCamera);
}

void CameraPage::disconnectSignals()
{
    disconnect(_cameraWorkerThread, &QThread::started, _cameraWorker, &CameraWorker::init);
    disconnect(_cameraWorker, &CameraWorker::doImageCaptured, this, &CameraPage::onImageCaptured);
    disconnect(_cameraWorker, &CameraWorker::doReadyForCaptureChanged, this, &CameraPage::onReadyForCaptureChanged);
    disconnect(_cameraWorker, &CameraWorker::doImageViewfinder, this, &CameraPage::onImageViewfinder);
    disconnect(_cameraWorker, &CameraWorker::doDigitalZoomChanged, this, &CameraPage::onDigitalZoomChanged);
    disconnect(_cameraWorker, &CameraWorker::doCaptureModeChanged, this, &CameraPage::onCaptureModeChanged);
    disconnect(_cameraWorker, &CameraWorker::doVideoDurationChanged, this, &CameraPage::onVideoDurationChanged);
    disconnect(_cameraWorker, &CameraWorker::doMediaRecorderStateChanged, this, &CameraPage::onMediaRecorderStateChanged);
    disconnect(_cameraWorker, &CameraWorker::doImageSaved, this, &CameraPage::doImageCaptured);
    disconnect(_cameraWorker, &CameraWorker::doVideoSaved, this, &CameraPage::doVideoCaptured);
    disconnect(this, &CameraPage::doCapture, _cameraWorker, &CameraWorker::capture);
    disconnect(this, &CameraPage::doStart, _cameraWorker, &CameraWorker::startCamera);
    disconnect(this, &CameraPage::doStop, _cameraWorker, &CameraWorker::stopCamera);
    disconnect(this, &CameraPage::doZoomIn, _cameraWorker, &CameraWorker::onZoomIn);
    disconnect(this, &CameraPage::doZoomOut, _cameraWorker, &CameraWorker::onZoomOut);
    disconnect(this, &CameraPage::doRecord, _cameraWorker, &CameraWorker::onRecord);
    disconnect(this, &CameraPage::doStopRecord, _cameraWorker, &CameraWorker::onStop);
    disconnect(this, &CameraPage::doPhotoMode, _cameraWorker, &CameraWorker::onPhotoMode);
    disconnect(this, &CameraPage::doVideoMode, _cameraWorker, &CameraWorker::onVideoMode);
    disconnect(this, &CameraPage::doConstructCamera, _cameraWorker, &CameraWorker::constructCamera);
    disconnect(this, &CameraPage::doDestroyCamera, _cameraWorker, &CameraWorker::destroyCamera);
}

void CameraPage::resetViewFinder()
{
    ui->viewFinderLabel->clear();
    ui->viewFinderLabel->setText(tr("Loading ..."));
}

void CameraPage::onReadyForCaptureChanged(bool ready)
{
    ui->captureButton->setEnabled(ready);
}

void CameraPage::onImageCaptured(int id, const QImage& preview)
{
    Q_UNUSED(id);
    Q_UNUSED(preview);
    delay(1000);
    ui->captureButton->setEnabled(true);
}

void CameraPage::onImageViewfinder(const QImage& preview)
{
    ui->viewFinderLabel->setPixmap(QPixmap::fromImage(preview).scaled(ui->viewFinderLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void CameraPage::onDigitalZoomChanged(qreal value)
{
    ui->zoomValueLabel->setText(QString::number(qRound(value), 'f', 1));
}

void CameraPage::onCaptureModeChanged(QCamera::CaptureModes mode)
{
    if (mode == QCamera::CaptureStillImage) {
        ui->modeValueLabel->setText(tr("PHOTO"));
        ui->photoModeButton->setEnabled(false);
        ui->videoModeButton->setEnabled(true);
        ui->stopButton->hide();
        ui->durationWidget->hide();
    }
    else if (mode == QCamera::CaptureVideo) {
        ui->modeValueLabel->setText(tr("VIDEO"));
        ui->photoModeButton->setEnabled(true);
        ui->videoModeButton->setEnabled(false);
        ui->stopButton->hide();
        ui->durationWidget->show();
    }
    _currentMode = mode;
}

void CameraPage::onVideoDurationChanged(qint64 duration)
{
    int seconds = duration / 1000;
    if (seconds > MAX_VIDEO_FILE_DURATION_SEC) {
        emit doStopRecord();
        return;
    }
    ui->durationValueLabel->setText(QString::number(seconds));
}

void CameraPage::onMediaRecorderStateChanged(QMediaRecorder::State state)
{
    if (state == QMediaRecorder::RecordingState) {
        ui->captureButton->hide();
        ui->stopButton->show();
    }
    else {
        ui->captureButton->show();
        ui->stopButton->hide();
    }
}

void CameraPage::setCapturedEnabled(bool value)
{
    Q_UNUSED(value);
}

void CameraPage::on_captureButton_released()
{
    if (_currentMode == QCamera::CaptureStillImage) {
        ui->captureButton->setDisabled(true);
        emit doCapture();
    }
    else if (_currentMode == QCamera::CaptureVideo) {
        emit doRecord();
    }
}

void CameraPage::on_zoomInButton_released()
{
    blockZoomControls(true);
    emit doZoomIn();
    delay(1000);
    blockZoomControls(false);
}

void CameraPage::on_zoomOutButton_released()
{
    blockZoomControls(true);
    emit doZoomOut();
    delay(1000);
    blockZoomControls(false);
}

void CameraPage::on_stopButton_released()
{
    emit doStopRecord();
}

void CameraPage::on_photoModeButton_released()
{
    emit doPhotoMode();
    delay(1500);
}

void CameraPage::on_videoModeButton_released()
{
    emit doVideoMode();
    delay(1500);
}
