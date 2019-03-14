#ifndef MXCCAMERAIMAGECAPTURE_H
#define MXCCAMERAIMAGECAPTURE_H

#include <QObject>
#include <QSharedPointer>
#include <QImage>

class MxcCamera;
class MxcCameraImageCapture : public QObject
{
    Q_OBJECT
public:
    explicit MxcCameraImageCapture(QObject* parent = 0);
    void setCamera(MxcCamera* mxcCamera);

    void capture(const QString& file = QString());

signals:
    void doImageCaptured(const QImage& preview);
    void doImageCaptured(const QString& imageFilename);

public slots:
    void newImage(QSharedPointer<QImage> image);

private:
    void cameraConnect();
    void cameraDisconnect();

private:
    bool _makePhoto;
    QString _photoFilename;
    MxcCamera* _mxcCamera;
};

#endif  // MXCCAMERAIMAGECAPTURE_H
