#include "videoviewerpage.h"
#include "ui_videoviewerpage.h"

#include "debug.h"

VideoViewerPage::VideoViewerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoViewerPage)
    , _mediaPlayer(0)
{
    ui->setupUi(this);
}

VideoViewerPage::~VideoViewerPage()
{
    if (_mediaPlayer != 0) {
        _mediaPlayer->stop();
    }

    delete ui;
}

void VideoViewerPage::setMedia(const QMediaContent& media)
{
    _media = media;
}

void VideoViewerPage::setVisible(bool visible)
{
    if (visible == true) {
        if (_mediaPlayer == 0) {
            _mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
            Q_ASSERT(_mediaPlayer != 0);
            _mediaPlayer->setVideoOutput(ui->videoWidget);

            ASSERT(connect(_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State))));
            ASSERT(connect(_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError(QMediaPlayer::Error))));

            _mediaPlayer->setMedia(_media);
            _mediaPlayer->play();
        }
    }
    else {
        if (_mediaPlayer != 0) {
            disconnect(_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
            disconnect(_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError(QMediaPlayer::Error)));

            _mediaPlayer->stop();

            _mediaPlayer->deleteLater();
            _mediaPlayer = 0;
        }
    }

    QWidget::setVisible(visible);
}

void VideoViewerPage::mediaStateChanged(QMediaPlayer::State newState)
{
    if (newState == QMediaPlayer::StoppedState) {
        emit leaveTheWidget();
    }
}

void VideoViewerPage::handleError(QMediaPlayer::Error error)
{
    QDEBUG << "Error:" << QString::number(error) << _mediaPlayer->errorString();
    emit leaveTheWidget();
}
