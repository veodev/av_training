#include "videocamerapage.h"
#include "ui_videocamerapage.h"

#include "core.h"
#include "debug.h"
#include "registration.h"

VideoCameraPage::VideoCameraPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoCameraPage)
{
    ui->setupUi(this);

    ASSERT(connect(ui->cameraPage, &CameraPage::doImageCaptured, this, &VideoCameraPage::onImageCaptured));
    ASSERT(connect(ui->cameraPage, &CameraPage::doVideoCaptured, this, &VideoCameraPage::onVideoCaptured));
}

VideoCameraPage::~VideoCameraPage()
{
    qDebug() << "Deleting videocamerapage...";
    ASSERT(disconnect(ui->cameraPage, &CameraPage::doImageCaptured, this, &VideoCameraPage::onImageCaptured));
    ASSERT(disconnect(ui->cameraPage, &CameraPage::doVideoCaptured, this, &VideoCameraPage::onVideoCaptured));
    delete ui;
    qDebug() << "Videocamerapage deleted!";
}

void VideoCameraPage::setCapturedEnabled(bool value)
{
    ui->cameraPage->setCapturedEnabled(value);
}

void VideoCameraPage::setVisible(bool visible)
{
    QDEBUG << "VideoCameraPage set visible:" << visible;
#if defined IMX_DEVICE || defined ANDROID
    ui->cameraPage->setVisible(visible);
#endif
    QWidget::setVisible(visible);
}

void VideoCameraPage::onImageCaptured(const QString& imageFilename)
{
    Core::instance().registration().addImage(imageFilename);
    qDebug() << "ADD PHOTO TO REGISTRATION: " << imageFilename;
}

void VideoCameraPage::onVideoCaptured(const QString& videoFilename)
{
    Core::instance().registration().addVideo(videoFilename);
    qDebug() << "ADD VIDEO TO REGISTRATION: " << videoFilename;
}

void VideoCameraPage::on_backButton_released()
{
    emit doBackFromVideoCameraPage();
}
