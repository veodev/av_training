#ifndef MXCMEDIARECORDER_H
#define MXCMEDIARECORDER_H

#include <QObject>
#include <QFile>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QImage>

class MxcCamera;
class MxcMediaRecorder : public QObject
{
    Q_OBJECT
public:
    explicit MxcMediaRecorder(QObject* parent = 0);
    void setCamera(MxcCamera* mxcCamera);

signals:
    void recordStarted();
    void recordStopped();
    void recordSaved(const QString& filename);
    void encodingProgress(int progress);

public slots:
    void newImage(QSharedPointer<QImage> data);
    void record(int seconds, int quality);
    void stop();

private:
    bool convertVideo(const QString& src, const QString& dst);
    bool runEncodingProcess(const QString& program);
    void cameraConnect();
    void cameraDisconnect();

private:
    int _outWidth;
    int _outHeight;
    int _quality;  // [2..32]

    QString _videoFilename;
    qint64 _videoDuraionMSec;
    qint64 _lastVideoDuration;
    enum CaptureVideoStates
    {
        CaptureVideoNone,
        CaptureVideoStart,
        CaptureVideoInProgress,
        CaptureVideoStop
    };
    CaptureVideoStates _captureVideoState;
    QElapsedTimer _videoElapsedTimer;
    QFile _videoFile;

    int _count;


    MxcCamera* _mxcCamera;
};

#endif  // MXCMEDIARECORDER_H
