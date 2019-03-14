#ifndef AUDIOPLAYERPAGE_H
#define AUDIOPLAYERPAGE_H

#include <QMediaPlayer>

#include "playlistpage.h"

namespace Ui
{
class audioplayerpage;
}

class QAudioProbe;
class QAudioBuffer;

class AudioLevel;

class AudioPlayerPage : public PlayListPage
{
    Q_OBJECT

public:
    explicit AudioPlayerPage(QWidget* parent = nullptr);
    ~AudioPlayerPage();

public slots:
    void setVisible(bool visible);

private slots:
    void processBuffer(const QAudioBuffer& buffer);

    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);

    void on_playPushButton_released();
    void on_stopPushButton_released();
    void on_deletePushButton_released();
    void on_prevPushButton_released();
    void on_nextPushButton_released();
    void on_positionSlider_sliderReleased();

protected:
    virtual void stop();
    virtual void setMedia(const QString& media);
    virtual void setFilename(const QString& filename);

    virtual void setPrevButtonEnabled(bool value);
    virtual void setPlayButtonEnabled(bool value);
    virtual void setStopButtonEnabled(bool value);
    virtual void setNextButtonEnabled(bool value);
    virtual void setDeleteButtonEnabled(bool value);

private:
    void clearAudioLevels();

private:
    Ui::audioplayerpage* ui;

    QString _currentFilename;
    QString _tracksLocation;
    QStringList _playlist;
    int _currentTrack;

    QMediaPlayer* _audioPlayer;
    QAudioProbe* _audioProbe;
    QList<AudioLevel*> _audioLevels;
};

#endif  // AUDIOPLAYERPAGE_H
