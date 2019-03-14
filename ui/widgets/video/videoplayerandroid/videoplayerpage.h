#ifndef VIDEOPLAYERPAGE_H
#define VIDEOPLAYERPAGE_H

#include <QMediaPlayer>

#include "playlistpage.h"

namespace Ui
{
class VideoPlayerPage;
}

enum VideoPlayerState
{
    StoppedState,
    PlayingState,
    PausedState
};

class VideoPlayerPage : public PlayListPage
{
    Q_OBJECT

public:
    explicit VideoPlayerPage(QWidget* parent = 0);
    ~VideoPlayerPage();

private:
    void playVideo();
    void stopVideo();
    void pauseVideo();
    void setVideoFile(QString fileName);
    VideoPlayerState getVideoPlayerState();

public slots:
    void setVisible(bool visible);

private slots:
    void mediaStateChanged(QMediaPlayer::State newState);
    void onVideoPositionChanged(int position);
    void onVideoDurationChanged(int duration);
    void on_prevPushButton_released();
    void on_playPushButton_released();
    void on_stopPushButton_released();
    void on_nextPushButton_released();
    void on_deletePushButton_released();

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
    Ui::VideoPlayerPage* ui;
    QObject* _rootObject;
    int _currentVideoFileDuration = 0;
};

#endif  // VIDEOPLAYERPAGE_H
