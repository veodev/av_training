#include "camera_advanced.h"

void checkSupportedImageSettings(QCameraImageCapture* cameraImageCapture)
{
    qDebug() << "IMAGE SETTINGS:";
    const QStringList supportedImageCodecs = cameraImageCapture->supportedImageCodecs();
    for (const QString& codecName : supportedImageCodecs) {
        QString description = cameraImageCapture->imageCodecDescription(codecName);
        qDebug() << "CODEC: " << description;
    }

    const QList<QSize> supportedResolutions = cameraImageCapture->supportedResolutions();
    for (const QSize& resolution : supportedResolutions) {
        qDebug() << "RESOLUTION: " << resolution;
    }
}

void checkSupportedVideoSettings(QMediaRecorder* mediaRecorder)
{
    qDebug() << "VIDEO SETTINGS:";
    const QStringList supportedAudioCodecs = mediaRecorder->supportedAudioCodecs();
    for (const QString& codecName : supportedAudioCodecs) {
        QString description = mediaRecorder->audioCodecDescription(codecName);
        qDebug() << "AUDIO CODEC: " << description << codecName;
    }

    const QList<int> supportedAudioSampleRates = mediaRecorder->supportedAudioSampleRates();
    for (int sampleRate : supportedAudioSampleRates) {
        qDebug() << "RATE: " << sampleRate;
    }

    const QStringList supportedVideoCodecs = mediaRecorder->supportedVideoCodecs();
    for (const QString& codecName : supportedVideoCodecs) {
        QString description = mediaRecorder->videoCodecDescription(codecName);
        qDebug() << "VIDEO CODEC: " << description << codecName;
    }

    const QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
    for (const QSize& resolution : supportedResolutions) {
        qDebug() << "RESOLUTION: " << resolution;
    }

    const QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
    for (qreal rate : supportedFrameRates) {
        qDebug() << "RATE: " << rate;
    }

    const QStringList containers = mediaRecorder->supportedContainers();
    for (const QString& container : containers) {
        qDebug() << "FORMAT: " << container << mediaRecorder->containerDescription(container);
    }
}

void checkSupportedViewfinderResolutions(QCamera* camera)
{
    const QList<QSize> supportedResolutions = camera->supportedViewfinderResolutions();
    for (const QSize& resolution : supportedResolutions) {
        qDebug() << "VIEWFINDER RESOLUTION: " << resolution;
    }
}
