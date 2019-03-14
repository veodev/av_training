#include <QFile>

#include "playlistpage.h"

PlayListPage::PlayListPage(QWidget* parent)
    : QWidget(parent)
    , _currentFilename("")
    , _currentTrack(-1)
{
}

PlayListPage::~PlayListPage()
{
}

void PlayListPage::onSetPlaylist(const QString& tracksLocation, const QStringList& playlist, int currentTrack)
{
    _currentFilename = "";
    _tracksLocation = tracksLocation;
    _playlist = playlist;

    _currentTrack = -1;
    if (playlist.count() > 0) {
        if (currentTrack < playlist.count()) {
            _currentTrack = currentTrack;
        }
        else {
            _currentTrack = 0;
        }
        setPrevButtonEnabled(true);
        setNextButtonEnabled(true);
    }
    changeTrack();
}

void PlayListPage::onRemoveFile()
{
    if (QFile::remove(_currentFilename) == true) {
        _playlist.removeAt(_currentTrack);
        if (_playlist.count() > 0) {
            if (_currentTrack >= _playlist.count()) {
                _currentTrack = _playlist.count() - 1;
            }
        }
        else {
            _currentTrack = -1;
            setPrevButtonEnabled(false);
            setDeleteButtonEnabled(false);
            setNextButtonEnabled(false);
            emit noFiles();
        }
        changeTrack();
    }
}

void PlayListPage::stop()
{
}

void PlayListPage::setFilename(const QString& filename)
{
    Q_UNUSED(filename);
}

void PlayListPage::setPrevButtonEnabled(bool value)
{
    Q_UNUSED(value);
}

void PlayListPage::setPlayButtonEnabled(bool value)
{
    Q_UNUSED(value);
}

void PlayListPage::setStopButtonEnabled(bool value)
{
    Q_UNUSED(value);
}

void PlayListPage::setNextButtonEnabled(bool value)
{
    Q_UNUSED(value);
}

void PlayListPage::setDeleteButtonEnabled(bool value)
{
    Q_UNUSED(value);
}

void PlayListPage::deleteFile()
{
    stop();
    if (_currentTrack >= 0 && _playlist.count() > 0) {
        emit doRemoveFile(_playlist[_currentTrack] + tr(" will be deleted."));
    }
}

void PlayListPage::prevTrack()
{
    if (_playlist.count() > 0) {
        --_currentTrack;
        if (_currentTrack < 0) {
            _currentTrack = _playlist.count() - 1;
        }
        changeTrack();
    }
}

void PlayListPage::nextTrack()
{
    if (_playlist.count() > 0) {
        ++_currentTrack;
        if (_currentTrack >= _playlist.count()) {
            _currentTrack = 0;
        }
        changeTrack();
    }
}

void PlayListPage::changeTrack()
{
    QString filename;
    QString fullFilename;
    if (_currentTrack >= 0) {
        filename = _playlist[_currentTrack];
        fullFilename = _tracksLocation + '/' + filename;
    }
    if (_currentFilename != fullFilename) {
        _currentFilename = fullFilename;
        stop();
        setMedia(fullFilename);
        setFilename(filename);

        bool isEnabled = !_currentFilename.isEmpty();
        setPlayButtonEnabled(isEnabled);
        setStopButtonEnabled(isEnabled);
        setDeleteButtonEnabled(isEnabled);
    }
}
