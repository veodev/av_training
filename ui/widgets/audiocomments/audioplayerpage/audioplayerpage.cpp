#include <QAudioProbe>
#include <QFile>
#include <QMediaPlaylist>
#include "audioplayerpage.h"
#include "ui_audioplayerpage.h"

#include "core.h"
#include "alarmtone.h"
#include "audiolevel.h"
#include "debug.h"

AudioPlayerPage::AudioPlayerPage(QWidget* parent)
    : PlayListPage(parent)
    , ui(new Ui::audioplayerpage)
    , _currentTrack(0)
    , _audioPlayer(new QMediaPlayer())
    , _audioProbe(new QAudioProbe())
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


    ASSERT(connect(_audioProbe, &QAudioProbe::audioBufferProbed, this, &AudioPlayerPage::processBuffer));
    _audioProbe->setSource(_audioPlayer);

    ASSERT(connect(_audioPlayer, &QMediaPlayer::positionChanged, this, &AudioPlayerPage::positionChanged));
    ASSERT(connect(_audioPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayerPage::stateChanged));

    _audioPlayer->setVolume(50);
}

AudioPlayerPage::~AudioPlayerPage()
{
    qDebug() << "Deleting AudioPlayerPage...";
    _audioPlayer->setPosition(0);
    _audioPlayer->setPlaylist(nullptr);
    _audioPlayer->stop();
    QMediaPlayer* nullPtrTemp = nullptr;
    _audioProbe->setSource(nullPtrTemp);
    disconnect(_audioProbe, &QAudioProbe::audioBufferProbed, this, &AudioPlayerPage::processBuffer);
    disconnect(_audioPlayer, &QMediaPlayer::positionChanged, this, &AudioPlayerPage::positionChanged);
    disconnect(_audioPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayerPage::stateChanged);
    delete _audioProbe;
    _audioProbe = nullptr;
    delete _audioPlayer;
    _audioPlayer = nullptr;
    _playlist.clear();
    for (auto& level : _audioLevels) {
        delete level;
    }
    _audioLevels.clear();

    delete ui;
}

void AudioPlayerPage::setVisible(bool visible)
{
    QDEBUG << "AudioPlayerPage visible:" << visible;
    if (visible) {
        Core::instance().stopAudioOutput();
    }
    else {
        stop();
        _audioPlayer->setPosition(0);
        _audioPlayer->setPlaylist(nullptr);
        _audioPlayer->stop();
        qDebug() << "Media state:" << _audioPlayer->state() << "Error:" << _audioPlayer->error();
        Core::instance().resumeAudioOutput();
    }

    QWidget::setVisible(visible);
}

void AudioPlayerPage::processBuffer(const QAudioBuffer& buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);

    if (_audioLevels.count() != levels.count()) {
        qDeleteAll(_audioLevels);
        _audioLevels.clear();
        for (int i = 0; i < levels.count(); ++i) {
            AudioLevel* level = new AudioLevel(this);
            _audioLevels.append(level);
            ui->levelsLayout->addWidget(level);
        }
    }

    for (int i = 0; i < levels.count(); ++i) {
        if (i < _audioLevels.count()) {
            _audioLevels.at(i)->setLevel(levels.at(i));
        }
    }

    positionChanged(_audioPlayer->position());
}

void AudioPlayerPage::positionChanged(qint64 position)
{
    ui->positionLabel->setText(tr("%1 / %2 ms").arg(position).arg(_audioPlayer->duration()));
    ui->positionSlider->setMaximum(_audioPlayer->duration());
    ui->positionSlider->setValue(position);
}

void AudioPlayerPage::stateChanged(QMediaPlayer::State state)
{
    QDEBUG << "Audio player state:" << state;
    switch (state) {
    case QMediaPlayer::StoppedState:
        _audioPlayer->setPosition(0);
    case QMediaPlayer::PausedState:
        ui->playPushButton->setIcon(QPixmap(":/media_play_black_27x32.png"));
        clearAudioLevels();
        break;
    case QMediaPlayer::PlayingState:
        ui->playPushButton->setIcon(QPixmap(":/media_pause_black_32x32.png"));
        break;
    }
}

void AudioPlayerPage::on_playPushButton_released()
{
    QMediaPlayer::State state = _audioPlayer->state();

    switch (state) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        _audioPlayer->play();
        break;
    case QMediaPlayer::PlayingState:
        _audioPlayer->pause();
        break;
    }
}

void AudioPlayerPage::on_stopPushButton_released()
{
    stop();
}

void AudioPlayerPage::on_deletePushButton_released()
{
    deleteFile();
}

void AudioPlayerPage::on_prevPushButton_released()
{
    prevTrack();
}

void AudioPlayerPage::on_nextPushButton_released()
{
    nextTrack();
}

void AudioPlayerPage::on_positionSlider_sliderReleased()
{
    _audioPlayer->setPosition(ui->positionSlider->value());
}

void AudioPlayerPage::stop()
{
    _audioPlayer->stop();
}

void AudioPlayerPage::setMedia(const QString& media)
{
    _audioPlayer->setMedia(QUrl::fromLocalFile(media));
    clearAudioLevels();
    on_playPushButton_released();
}

void AudioPlayerPage::setFilename(const QString& filename)
{
    ui->filename->setText(filename);
}

void AudioPlayerPage::setPrevButtonEnabled(bool value)
{
    ui->prevPushButton->setEnabled(value);
}

void AudioPlayerPage::setPlayButtonEnabled(bool value)
{
    ui->playPushButton->setEnabled(value);
}

void AudioPlayerPage::setStopButtonEnabled(bool value)
{
    ui->stopPushButton->setEnabled(value);
}

void AudioPlayerPage::setNextButtonEnabled(bool value)
{
    ui->nextPushButton->setEnabled(value);
}

void AudioPlayerPage::setDeleteButtonEnabled(bool value)
{
    ui->deletePushButton->setEnabled(value);
}

void AudioPlayerPage::clearAudioLevels()
{
    for (auto& level : _audioLevels) {
        level->setLevel(0);
    }
}
