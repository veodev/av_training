#ifndef VIDEOPLAYERPAGE_H
#define VIDEOPLAYERPAGE_H

#include <QMediaPlayer>

#include "playlistpage.h"

namespace Ui
{
class VideoPlayerPage;
}

class VideoPlayerPage : public PlayListPage
{
    Q_OBJECT

public:
    explicit VideoPlayerPage(QWidget* parent = 0);
    ~VideoPlayerPage();

public slots:
    void setVisible(bool visible);

private slots:
    void mediaStateChanged(QMediaPlayer::State newState);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void handleError(QMediaPlayer::Error error);

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

    QMediaPlayer* _mediaPlayer;
};

#endif  // VIDEOPLAYERPAGE_H
