#include <QPixmap>
#include <QDebug>
#include <QDateTime>

#include "mxccamera.h"
#include "mxccameraimagecapture.h"
#include "appdatapath.h"

#include "debug.h"

MxcCameraImageCapture::MxcCameraImageCapture(QObject* parent)
    : QObject(parent)
    , _makePhoto(false)
    , _photoFilename(QString())
    , _mxcCamera(0)
{
}

void MxcCameraImageCapture::setCamera(MxcCamera* mxcCamera)
{
    _mxcCamera = mxcCamera;
    cameraConnect();
}

void MxcCameraImageCapture::capture(const QString& file)
{
    _photoFilename = file;
    _makePhoto = true;
}

void MxcCameraImageCapture::newImage(QSharedPointer<QImage> image)
{
    if (_makePhoto == false) {
        return;
    }

    const QPixmap& pixmap = QPixmap::fromImage(*(image.data()));
    _photoFilename = photosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".png");
    QDEBUG << "photoFilename:" << _photoFilename;
    cameraDisconnect();
    pixmap.save(_photoFilename, "PNG");
    cameraConnect();
    _makePhoto = false;
    emit doImageCaptured(*(image.data()));
    emit doImageCaptured(_photoFilename);
}

void MxcCameraImageCapture::cameraConnect()
{
    ASSERT(connect(_mxcCamera, &MxcCamera::capturedImage, this, &MxcCameraImageCapture::newImage));
}

void MxcCameraImageCapture::cameraDisconnect()
{
    disconnect(_mxcCamera, &MxcCamera::capturedImage, this, &MxcCameraImageCapture::newImage);
}
