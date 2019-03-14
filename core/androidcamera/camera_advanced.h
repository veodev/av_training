#ifndef CAMERA_ADVANCED_H
#define CAMERA_ADVANCED_H

#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QCamera>

void checkSupportedImageSettings(QCameraImageCapture* cameraImageCapture);
void checkSupportedVideoSettings(QMediaRecorder* mediaRecorder);
void checkSupportedViewfinderResolutions(QCamera* camera);

#endif  // CAMERA_ADVANCED_H
