#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QMessageBox>
#include <QThread>
#include "mxccamera.h"
#include "mxcmediarecorder.h"
#include "appdatapath.h"
#include "unistd.h"
#include "debug.h"

MxcMediaRecorder::MxcMediaRecorder(QObject* parent)
    : QObject(parent)
    , _outWidth(720)
    , _outHeight(576)
    , _quality(4)
    , _videoFilename(QString())
    , _videoDuraionMSec(10000)
    , _lastVideoDuration(0)
    , _captureVideoState(CaptureVideoNone)
    , _count(0)
    , _mxcCamera(0)
{
}

void MxcMediaRecorder::setCamera(MxcCamera* mxcCamera)
{
    _mxcCamera = mxcCamera;
    cameraConnect();
}

void MxcMediaRecorder::record(int seconds, int quality)
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (_captureVideoState == CaptureVideoNone) {
        _videoDuraionMSec = seconds * 1000;
        _captureVideoState = CaptureVideoStart;
        _quality = quality;
    }
}

void MxcMediaRecorder::stop()
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (_captureVideoState == CaptureVideoInProgress) {
        _captureVideoState = CaptureVideoStop;
    }
}

void MxcMediaRecorder::newImage(QSharedPointer<QImage> data)
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (_captureVideoState == CaptureVideoStart) {
        qDebug() << "Recording started...";
        if (_videoFile.isOpen()) {
            _videoFile.close();
        }

        QImage* image = data.data();
        _outWidth = image->width() / 2;
        _outHeight = image->height() / 2;

        _videoFilename = videosPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".avi");
        QDEBUG << Q_FUNC_INFO << "videoFilename:" << _videoFilename;

        _videoFile.setFileName(_videoFilename + ".raw");
        if (_videoFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            _videoElapsedTimer.start();
            _captureVideoState = CaptureVideoInProgress;
            emit recordStarted();
            _count = 0;
        }
        else {
            _captureVideoState = CaptureVideoNone;
        }
    }
    else if (_captureVideoState == CaptureVideoInProgress) {
        QImage* image = data.data();
        const QImage& scaledImage = image->scaled(_outWidth, _outHeight);
        _videoFile.write(reinterpret_cast<const char*>(scaledImage.constBits()), scaledImage.byteCount());

        ++_count;
        if (_videoElapsedTimer.elapsed() > _videoDuraionMSec) {
            _captureVideoState = CaptureVideoStop;
        }
    }
    else if (_captureVideoState == CaptureVideoStop) {
        emit encodingProgress(1);
        _lastVideoDuration = _videoElapsedTimer.elapsed();
        qDebug() << "Recording stopped!";
        _videoFile.close();
        emit encodingProgress(5);
        emit recordStopped();
        cameraDisconnect();
        convertVideo(_videoFilename + ".raw", _videoFilename);
        cameraConnect();
        emit encodingProgress(97);
        emit recordSaved(_videoFilename);
        qDebug() << "Video saved!";
        QFile::remove(_videoFilename + ".raw");
        emit encodingProgress(100);
        _captureVideoState = CaptureVideoNone;
    }
}

bool MxcMediaRecorder::runEncodingProcess(const QString& program)
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write(program.toStdString().c_str());
    process.closeWriteChannel();
    qDebug() << "Starting encoding process...";
    qDebug() << "Encoder string:" << program;
    QElapsedTimer encodingProgressTimer;
    encodingProgressTimer.start();
    while (!process.waitForFinished(1000)) {
        if (_lastVideoDuration != 0.0f) {
            float progress = ((encodingProgressTimer.elapsed()) / static_cast<float>(_lastVideoDuration * 1.3f));
            if (progress > 1.0f) {
                progress = 1.0f;
            }
            emit encodingProgress(static_cast<int>(progress * 60.0f + 20.0f));
        }
    }
    if (process.waitForFinished(120000)) {
        qDebug() << "Encoding process finished!";
    }
    else {
        qDebug() << "Encoding process timeout!";
    }

    if (process.exitCode()) {
        const QByteArray& error = process.readAllStandardError();
        qDebug() << "Encoding process error!";
        qDebug() << error;
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Cannot save video!"), QMessageBox::Ok);
        process.close();
        return false;
    }
    process.close();
    return true;
}

bool MxcMediaRecorder::convertVideo(const QString& src, const QString& dst)
{
    Q_ASSERT(thread() == QThread::currentThread());
    emit encodingProgress(12);
    qDebug() << "Encoding started...";
    sync();
    emit encodingProgress(19);
    QElapsedTimer encodingTimer;
    encodingTimer.start();
    // h264 profile: ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt rgb565 -r 15 -s %1x%2 -i %3 -f mp4 -vcodec h264 -s %4x%5 -r 15 -y -preset ultrafast -crf %6 %7
    bool status = runEncodingProcess(QString("ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt rgb565 -r 15 -s %1x%2 -i %3 -f avi -vcodec mjpeg -s %4x%5 -r 15 -y -qscale %6 %7").arg(_outWidth).arg(_outHeight).arg(src).arg(_outWidth).arg(_outHeight).arg(_quality).arg(dst));
    qint64 time = encodingTimer.elapsed();
    emit encodingProgress(81);
    qDebug() << "Encoding time:" << time << "ms (" << time / 1000 << "s )";
    qDebug() << "Last video duration:" << _lastVideoDuration << "ms (" << _lastVideoDuration / 1000 << "s )";
    sync();
    emit encodingProgress(89);
    qDebug() << "Encoding finished!";
    return status;
}

void MxcMediaRecorder::cameraConnect()
{
    ASSERT(connect(_mxcCamera, &MxcCamera::capturedImage, this, &MxcMediaRecorder::newImage));
}

void MxcMediaRecorder::cameraDisconnect()
{
    disconnect(_mxcCamera, &MxcCamera::capturedImage, this, &MxcMediaRecorder::newImage);
}
