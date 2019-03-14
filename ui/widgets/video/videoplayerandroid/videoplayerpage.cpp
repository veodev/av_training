#include <QMediaPlayer>
#include "videoplayerpage.h"
#include "ui_videoplayerpage.h"

#include "debug.h"

VideoPlayerPage::VideoPlayerPage(QWidget* parent)
    : PlayListPage(parent)
    , ui(new Ui::VideoPlayerPage)
{
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl("qrc:/qml/VideoPlayer.qml"));
    _rootObject = reinterpret_cast<QObject*>(ui->quickWidget->rootObject());
    connect(_rootObject, SIGNAL(doVideoPositionChanged(int)), this, SLOT(onVideoPositionChanged(int)));
    connect(_rootObject, SIGNAL(doVideoDurationChanged(int)), this, SLOT(onVideoDurationChanged(int)));

    ui->prevPushButton->setIconSize(QSize(32, 32));
    ui->prevPushButton->setIcon(QPixmap(":/media_previous_black_19x32.png"));
    ui->prevPushButton->setText("");

    ui->playPushButton->setIconSize(QSize(32, 32));
    ui->playPushButton->setIcon(QPixmap(":/media_play_black_27x32.png"));
    ui->playPushButton->setText("");

    ui->stopPushButton->setIconSize(QSize(32, 32));
    ui->stopPushButton->setIcon(QPixmap(":/media_stop_black_32x32.png"));
    ui->stopPushButton->setText("");

    ui->nextPushButton->setIconSize(QSize(32, 32));
    ui->nextPushButton->setIcon(QPixmap(":/media_next_black_32_19.png"));
    ui->nextPushButton->setText("");

    ui->deletePushButton->setIconSize(QSize(32, 32));
    ui->deletePushButton->setIcon(QPixmap(":/bin_black_26x32.png"));
    ui->deletePushButton->setText("");
}

VideoPlayerPage::~VideoPlayerPage()
{
    stopVideo();
    delete ui;
}

void VideoPlayerPage::playVideo()
{
    QMetaObject::invokeMethod(_rootObject, "playVideo");
}

void VideoPlayerPage::stopVideo()
{
    QMetaObject::invokeMethod(_rootObject, "stopVideo");
}

void VideoPlayerPage::pauseVideo()
{
    QMetaObject::invokeMethod(_rootObject, "pauseVideo");
}

void VideoPlayerPage::setVideoFile(QString fileName)
{
    QVariant videoFileName = fileName;
    QMetaObject::invokeMethod(_rootObject, "setMedia", Q_ARG(QVariant, videoFileName));
}

VideoPlayerState VideoPlayerPage::getVideoPlayerState()
{
    QVariant retval;
    QMetaObject::invokeMethod(_rootObject, "getVideoPlayerState", Q_RETURN_ARG(QVariant, retval));
    return static_cast<VideoPlayerState>(retval.toInt());
}

void VideoPlayerPage::setVisible(bool visible)
{
    if (visible == false) {
        stopVideo();
    }
    QWidget::setVisible(visible);
}

void VideoPlayerPage::mediaStateChanged(QMediaPlayer::State newState)
{
    switch (newState) {
    case QMediaPlayer::PlayingState:
        ui->playPushButton->setIcon(QPixmap(":/media_pause_black_32x32.png"));
        break;
    default:
        ui->playPushButton->setIcon(QPixmap(":/media_play_black_27x32.png"));
        break;
    }
}

void VideoPlayerPage::onVideoPositionChanged(int position)
{
    ui->positionLabel->setText(tr("%1 / %2 ms").arg(position).arg(_currentVideoFileDuration));
}

void VideoPlayerPage::onVideoDurationChanged(int duration)
{
    _currentVideoFileDuration = duration;
    onVideoPositionChanged(0);
}

void VideoPlayerPage::on_prevPushButton_released()
{
    prevTrack();
}

void VideoPlayerPage::on_playPushButton_released()
{
    VideoPlayerState state = getVideoPlayerState();
    switch (state) {
    case StoppedState:
    case PausedState:
        playVideo();
        break;
    case PlayingState:
        pauseVideo();
        break;
    default:
        break;
    }
}

void VideoPlayerPage::on_stopPushButton_released()
{
    stopVideo();
}

void VideoPlayerPage::on_nextPushButton_released()
{
    nextTrack();
}

void VideoPlayerPage::on_deletePushButton_released()
{
    deleteFile();
}

void VideoPlayerPage::stop()
{
    stopVideo();
}

void VideoPlayerPage::setMedia(const QString& media)
{
    setVideoFile(media);
    on_playPushButton_released();
}

void VideoPlayerPage::setFilename(const QString& filename)
{
    ui->filename->setText(filename);
}

void VideoPlayerPage::setPrevButtonEnabled(bool value)
{
    ui->prevPushButton->setEnabled(value);
}

void VideoPlayerPage::setPlayButtonEnabled(bool value)
{
    ui->playPushButton->setEnabled(value);
}

void VideoPlayerPage::setStopButtonEnabled(bool value)
{
    ui->stopPushButton->setEnabled(value);
}

void VideoPlayerPage::setNextButtonEnabled(bool value)
{
    ui->nextPushButton->setEnabled(value);
}

void VideoPlayerPage::setDeleteButtonEnabled(bool value)
{
    ui->deletePushButton->setEnabled(value);
}
