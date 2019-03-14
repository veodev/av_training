#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QMediaMetaData>
#include <QDateTime>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>

#include "camerapage.h"
#include "ui_camerapage.h"

#include "appdatapath.h"
#include "debug.h"

CameraPage::CameraPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CameraPage)
{
    ui->setupUi(this);

    _camera = new QCamera;
    _camera->setViewfinder(ui->viewfinder);
    ASSERT(connect(_camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &CameraPage::displayCameraError));
    _mediaRecorder = new QMediaRecorder(_camera);
    _mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("Test Title")));
    ASSERT(connect(_mediaRecorder, &QMediaRecorder::stateChanged, this, &CameraPage::updateRecorderState));
    ASSERT(connect(_mediaRecorder, &QMediaRecorder::durationChanged, this, &CameraPage::updateRecordTime));
    ASSERT(connect(_mediaRecorder, QOverload<QMediaRecorder::Error>::of(&QMediaRecorder::error), this, &CameraPage::displayRecorderError));
    updateRecorderState(_mediaRecorder->state());

    _imageCapture = new QCameraImageCapture(_camera);
    ASSERT(connect(_imageCapture, &QCameraImageCapture::imageSaved, this, &CameraPage::imageSaved));
}

CameraPage::~CameraPage()
{
    qDebug() << "Deleting camerapage...";
    disconnect(_imageCapture, &QCameraImageCapture::imageSaved, this, &CameraPage::imageSaved);
    disconnect(_mediaRecorder, &QMediaRecorder::stateChanged, this, &CameraPage::updateRecorderState);
    disconnect(_mediaRecorder, &QMediaRecorder::durationChanged, this, &CameraPage::updateRecordTime);
    disconnect(_mediaRecorder, QOverload<QMediaRecorder::Error>::of(&QMediaRecorder::error), this, &CameraPage::displayRecorderError);
    disconnect(_camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &CameraPage::displayCameraError);

    _camera->stop();
    _mediaRecorder->stop();

    delete _imageCapture;
    _imageCapture = 0;
    delete _mediaRecorder;
    _mediaRecorder = 0;
    delete _camera;
    _camera = 0;
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
        _camera->stop();
        ui->recordVideoPushButton->setDisabled(true);
        ui->takeAPhotoPushButton->setDisabled(true);
        _mediaRecorder->stop();
    }
}

void CameraPage::setVisible(bool visible)
{
    if (visible == true) {
        _camera->start();
    }
    else {
        _camera->stop();
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

void CameraPage::on_takeAPhotoPushButton_released()
{
    ui->takeAPhotoPushButton->setEnabled(false);
    ui->takeAPhotoPushButton->setText(tr("Saving"));
    _imageCapture->capture(photosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".jpg"));
}

void CameraPage::on_recordVideoPushButton_released()
{
    QString fileName = videosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".avi");
    QDEBUG << "photo fileName:" << fileName;

    _mediaRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    _mediaRecorder->record();
    updateRecordTime();
}

void CameraPage::displayRecorderError()
{
    QMessageBox::warning(this, tr("Capture error"), _mediaRecorder->errorString());
}

void CameraPage::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera error"), _camera->errorString());
}

void CameraPage::imageSaved(int id, const QString& fileName)
{
    Q_UNUSED(id);
    QDEBUG << "video fileName:" << fileName;
    ui->takeAPhotoPushButton->setEnabled(true);
    ui->takeAPhotoPushButton->setText(tr("Take a photo"));
    emit doImageCaptured(fileName);
}

void CameraPage::updateRecordTime()
{
    qint64 duration = _mediaRecorder->duration() / 1000;
    //    if (duration > 30) {
    //        _mediaRecorder->stop();
    //    }
    ui->recordVideoPushButton->setText(QString(tr("Stop [%1/30 sec]")).arg(duration));
}

void CameraPage::updateRecorderState(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::StoppedState:
        ui->recordVideoPushButton->setText(tr("Record video"));
        emit doVideoCaptured(_mediaRecorder->outputLocation().toLocalFile());
        break;
    case QMediaRecorder::PausedState:
        break;
    case QMediaRecorder::RecordingState:
        ui->recordVideoPushButton->setText(tr("Stop"));
        break;
    }
}
