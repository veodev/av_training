#include <QDebug>
#include <QThread>

#include "camerapage.h"
#include "ui_camerapage.h"

#include "mxccamera.h"
#include "mxcmediarecorder.h"
#include "mxccameraimagecapture.h"

#include "debug.h"

static const int DURATION_LIMIT_SEC = 30;

CameraPage::CameraPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CameraPage)
    , _imageCaptureThread(Q_NULLPTR)
    , _mediaRecorderThread(Q_NULLPTR)
    , _mxcCamera(Q_NULLPTR)
    , _mxcMediaRecorder(Q_NULLPTR)
    , _mxcCameraImageCapture(Q_NULLPTR)
    , _videoRecordStarted(false)
{
    ui->setupUi(this);
    ui->encodingProgressBar->hide();

    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QImage>");

    _mediaRecorderThread = new QThread(this);
    _mediaRecorderThread->setObjectName("_mediaRecorderThread");
    _imageCaptureThread = new QThread(this);
    _imageCaptureThread->setObjectName("_imageCaptureThread");

    _mxcCamera = new MxcCamera(this);

    _mxcMediaRecorder = new MxcMediaRecorder();
    _mxcMediaRecorder->setCamera(_mxcCamera);
    _mxcMediaRecorder->moveToThread(_mediaRecorderThread);

    _mxcCameraImageCapture = new MxcCameraImageCapture();
    _mxcCameraImageCapture->setCamera(_mxcCamera);
    _mxcCameraImageCapture->moveToThread(_imageCaptureThread);

    ASSERT(connect(_mxcCamera, &MxcCamera::capturedImage, this, &CameraPage::updateImage));

    ASSERT(connect(_mxcMediaRecorder, &MxcMediaRecorder::recordStarted, this, &CameraPage::recordStarted));
    ASSERT(connect(_mxcMediaRecorder, &MxcMediaRecorder::recordStopped, this, &CameraPage::recordStopped));
    ASSERT(connect(_mxcMediaRecorder, &MxcMediaRecorder::recordSaved, this, &CameraPage::recordSaved));
    ASSERT(connect(_mxcMediaRecorder, &MxcMediaRecorder::recordSaved, this, &CameraPage::doVideoCaptured));

    ASSERT(connect(_mxcCameraImageCapture, static_cast<void (MxcCameraImageCapture::*)(const QImage&)>(&MxcCameraImageCapture::doImageCaptured), this, &CameraPage::imageCaptured));
    ASSERT(connect(_mxcCameraImageCapture, static_cast<void (MxcCameraImageCapture::*)(const QString&)>(&MxcCameraImageCapture::doImageCaptured), this, &CameraPage::doImageCaptured));

    ASSERT(connect(this, &CameraPage::doRecord, _mxcMediaRecorder, &MxcMediaRecorder::record, Qt::BlockingQueuedConnection));
    ASSERT(connect(this, &CameraPage::doStopRecord, _mxcMediaRecorder, &MxcMediaRecorder::stop, Qt::BlockingQueuedConnection));
    ASSERT(connect(_mxcMediaRecorder, &MxcMediaRecorder::encodingProgress, this, &CameraPage::onEncodingProgress));
}

CameraPage::~CameraPage()
{
    qDebug() << "Deleting camerapage...";

    Q_ASSERT(_mxcCamera);

    ASSERT(disconnect(_mxcCamera, &MxcCamera::capturedImage, this, &CameraPage::updateImage));

    ASSERT(disconnect(_mxcMediaRecorder, &MxcMediaRecorder::recordStarted, this, &CameraPage::recordStarted));
    ASSERT(disconnect(_mxcMediaRecorder, &MxcMediaRecorder::recordStopped, this, &CameraPage::recordStopped));
    ASSERT(disconnect(_mxcMediaRecorder, &MxcMediaRecorder::recordSaved, this, &CameraPage::recordSaved));
    ASSERT(disconnect(_mxcMediaRecorder, &MxcMediaRecorder::recordSaved, this, &CameraPage::doVideoCaptured));

    ASSERT(disconnect(_mxcCameraImageCapture, static_cast<void (MxcCameraImageCapture::*)(const QImage&)>(&MxcCameraImageCapture::doImageCaptured), this, &CameraPage::imageCaptured));
    ASSERT(disconnect(_mxcCameraImageCapture, static_cast<void (MxcCameraImageCapture::*)(const QString&)>(&MxcCameraImageCapture::doImageCaptured), this, &CameraPage::doImageCaptured));
    ASSERT(disconnect(_mxcMediaRecorder, &MxcMediaRecorder::encodingProgress, this, &CameraPage::onEncodingProgress));

    stopCamera();
    _imageCaptureThread->quit();
    _imageCaptureThread->wait();
    _mediaRecorderThread->quit();
    _mediaRecorderThread->wait();

    Q_ASSERT(_mxcMediaRecorder);
    delete _mxcMediaRecorder;

    Q_ASSERT(_mxcCameraImageCapture);
    delete _mxcCameraImageCapture;

    delete _mxcCamera;

    Q_ASSERT(ui);
    delete ui;
    qDebug() << "Camerapage deleted!";
}

void CameraPage::setCapturedEnabled(bool value)
{
    if (value == true) {
        ui->recordVideoPushButton->setEnabled(true);
        ui->takeAPhotoPushButton->setEnabled(true);
    }
    else {
        ui->recordVideoPushButton->setDisabled(true);
        ui->takeAPhotoPushButton->setDisabled(true);
        emit doStopRecord();
    }
}

void CameraPage::startCamera()
{
    QDEBUG << "Starting camera...";
    Q_ASSERT(_mxcCamera);
    _mxcCamera->start();
}

void CameraPage::stopCamera()
{
    QDEBUG << "Stopping camera...";
    Q_ASSERT(_mxcCamera);
    _mxcCamera->stop();
}

void CameraPage::setVisible(bool visible)
{
    QDEBUG << "CameraPage set visible:" << visible;
    if (visible) {
        _mediaRecorderThread->start();
        _imageCaptureThread->start();
        startCamera();
    }
    else {
        stopCamera();
        _imageCaptureThread->quit();
        _imageCaptureThread->wait();
        _mediaRecorderThread->quit();
        _mediaRecorderThread->wait();
    }

    QWidget::setVisible(visible);
}

bool CameraPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void CameraPage::updateImage(QSharedPointer<QImage> image)
{
    ui->videoWidget->setPixmap(QPixmap::fromImage(*(image.data())));
    if (_videoRecordStarted == true) {
        ui->recordVideoPushButton->setText(QString(tr("Stop [%1/%2 sec]")).arg(static_cast<int>(_elapsedTimer.elapsed() / 1000)).arg(DURATION_LIMIT_SEC));
    }
}

void CameraPage::recordStarted()
{
    ui->recordVideoPushButton->setText(tr("Stop"));
    _videoRecordStarted = true;
    _elapsedTimer.start();
}

void CameraPage::recordStopped()
{
    ui->encodingProgressBar->show();
    stopCamera();
    ui->recordVideoPushButton->setText(tr("Saving..."));
    ui->recordVideoPushButton->setEnabled(false);
    _videoRecordStarted = false;
}

void CameraPage::recordSaved(const QString& filename)
{
    startCamera();
    Q_UNUSED(filename);
    ui->recordVideoPushButton->setText(tr("Record video"));
    ui->recordVideoPushButton->setEnabled(true);
    ui->encodingProgressBar->hide();
}

void CameraPage::imageCaptured(const QImage& preview)
{
    Q_UNUSED(preview);
    ui->takeAPhotoPushButton->setEnabled(true);
    ui->takeAPhotoPushButton->setText(tr("Take a photo"));
}

void CameraPage::onEncodingProgress(int progress)
{
    ui->encodingProgressBar->setValue(progress);
}

void CameraPage::on_takeAPhotoPushButton_released()
{
    ui->takeAPhotoPushButton->setEnabled(false);
    ui->takeAPhotoPushButton->setText(tr("Saving"));
    _mxcCameraImageCapture->capture();
}

void CameraPage::on_recordVideoPushButton_released()
{
    if (_videoRecordStarted == false) {
        emit doRecord(DURATION_LIMIT_SEC, 4);
    }
    else {
        emit doStopRecord();
    }
}
