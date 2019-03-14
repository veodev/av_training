#ifndef PLAYLISTPAGE_H
#define PLAYLISTPAGE_H

#include <QWidget>
#include <QString>

class PlayListPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayListPage(QWidget* parent = 0);
    ~PlayListPage();

public slots:
    void onSetPlaylist(const QString& tracksLocation, const QStringList& playlist, int currentTrack);
    void onRemoveFile();

signals:
    void doRemoveFile(const QString&);
    void noFiles();

protected:
    virtual void stop();
    virtual void setMedia(const QString& media) = 0;
    virtual void setFilename(const QString& filename);

    virtual void setPrevButtonEnabled(bool value);
    virtual void setPlayButtonEnabled(bool value);
    virtual void setStopButtonEnabled(bool value);
    virtual void setNextButtonEnabled(bool value);
    virtual void setDeleteButtonEnabled(bool value);

    void deleteFile();
    void prevTrack();
    void nextTrack();

private:
    void changeTrack();

private:
    QString _currentFilename;
    QString _tracksLocation;
    QStringList _playlist;
    int _currentTrack;
};

#endif  // PLAYLISTPAGE_H
