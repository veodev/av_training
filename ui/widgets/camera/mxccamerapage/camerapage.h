#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include <QWidget>
#include <QPixmap>
#include <QElapsedTimer>

namespace Ui
{
class CameraPage;
}

class MxcCamera;
class MxcMediaRecorder;
class MxcCameraImageCapture;
class CameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPage(QWidget* parent = 0);
    ~CameraPage();
    void setCapturedEnabled(bool value);

    void startCamera();
    void stopCamera();

public slots:
    void setVisible(bool visible);

signals:
    void doImageCaptured(const QString& imageFilename);
    void doVideoCaptured(const QString& videoFilename);
    void doRecord(int time, int quality);
    void doStopRecord();

protected:
    bool event(QEvent* e);

private slots:
    void updateImage(QSharedPointer<QImage> image);

    void recordStarted();
    void recordStopped();
    void recordSaved(const QString& filename);
    void imageCaptured(const QImage& preview);

    void onEncodingProgress(int progress);

    void on_takeAPhotoPushButton_released();
    void on_recordVideoPushButton_released();

private:
    Ui::CameraPage* ui;

    QThread* _imageCaptureThread;
    QThread* _mediaRecorderThread;
    MxcCamera* _mxcCamera;
    MxcMediaRecorder* _mxcMediaRecorder;
    MxcCameraImageCapture* _mxcCameraImageCapture;
    QElapsedTimer _elapsedTimer;
    bool _videoRecordStarted;
};

#endif  // CAMERAPAGE_H
