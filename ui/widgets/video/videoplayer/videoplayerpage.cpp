#include <QMediaPlayer>

#include "videoplayerpage.h"
#include "ui_videoplayerpage.h"

#include "debug.h"

VideoPlayerPage::VideoPlayerPage(QWidget* parent)
    : PlayListPage(parent)
    , ui(new Ui::VideoPlayerPage)
    , _mediaPlayer(Q_NULLPTR)
{
    ui->setupUi(this);

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
    if (_mediaPlayer != Q_NULLPTR) {
        _mediaPlayer->stop();
    }

    delete ui;
}

void VideoPlayerPage::setVisible(bool visible)
{
    if (visible == true) {
        if (_mediaPlayer == Q_NULLPTR) {
            _mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
            Q_ASSERT(_mediaPlayer != Q_NULLPTR);
            _mediaPlayer->setVideoOutput(ui->videoWidget);

            ASSERT(connect(_mediaPlayer, &QMediaPlayer::stateChanged, this, &VideoPlayerPage::mediaStateChanged));
            ASSERT(connect(_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayerPage::positionChanged));
            ASSERT(connect(_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayerPage::durationChanged));
            ASSERT(connect(_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &VideoPlayerPage::handleError));
        }
    }
    else {
        stop();
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

void VideoPlayerPage::positionChanged(qint64 position)
{
    ui->positionLabel->setText(tr("%1 / %2 ms").arg(position).arg(_mediaPlayer->duration()));
}

void VideoPlayerPage::durationChanged(qint64 duration)
{
    Q_UNUSED(duration);
    positionChanged(0);
}

void VideoPlayerPage::handleError(QMediaPlayer::Error error)
{
    qDebug() << "Error:" << QString::number(error) << _mediaPlayer->errorString();
}

void VideoPlayerPage::on_prevPushButton_released()
{
    prevTrack();
}

void VideoPlayerPage::on_playPushButton_released()
{
    Q_ASSERT(_mediaPlayer != Q_NULLPTR);

    QMediaPlayer::State state = _mediaPlayer->state();
    switch (state) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        _mediaPlayer->play();
        break;
    case QMediaPlayer::PlayingState:
        _mediaPlayer->pause();
        break;
    default:
        break;
    }
}

void VideoPlayerPage::on_stopPushButton_released()
{
    stop();
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
    if (_mediaPlayer != Q_NULLPTR) {
        _mediaPlayer->stop();
    }
}

void VideoPlayerPage::setMedia(const QString& media)
{
    _mediaPlayer->setMedia(QUrl::fromLocalFile(media));
    on_playPushButton_released();

#ifdef DEBUG
    QStringList list = _mediaPlayer->availableMetaData();
    for (const QString& item : list) {
        qDebug() << item << _mediaPlayer->metaData(item);
    }
#endif
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
