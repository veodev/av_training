#ifndef VIDEOVIEWERPAGE_H
#define VIDEOVIEWERPAGE_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui
{
class VideoViewerPage;
}

class VideoViewerPage : public QWidget
{
    Q_OBJECT

public:
    explicit VideoViewerPage(QWidget* parent = 0);
    ~VideoViewerPage();

    void setMedia(const QMediaContent& media);

public slots:
    void setVisible(bool visible);

signals:
    void leaveTheWidget();

private slots:
    void mediaStateChanged(QMediaPlayer::State newState);
    void handleError(QMediaPlayer::Error error);

private:
    Ui::VideoViewerPage* ui;

    QMediaPlayer* _mediaPlayer;
    QMediaContent _media;
};

#endif  // VIDEOVIEWERPAGE_H
