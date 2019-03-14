#ifndef MEMORYPAGE_H
#define MEMORYPAGE_H

#include <QWidget>
#include <QDir>
#include <QTimer>
#include <QStorageInfo>
#include <QGestureEvent>
#include <QGesture>

#include "coredefinitions.h"

class FTPUploader;
class FileManager;

namespace Ui
{
class MemoryPage;
}

class MemoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit MemoryPage(QWidget* parent = 0);
    ~MemoryPage();

    void resetStateButtons();
    void browseFiles(QDir dir);
    void update();
    void startCheckTimer();
    void stopCheckTimer();
    void checkMemory();

    void removeSelectedFiles();
    void moveSelectedFiles();

    void configureUiForAviconDbAviconDbHs();
    void configureUiForAvicon15();
    void setVisibleLogButton(bool isVisible);

private:
    void blockButtons(bool isBlock);
    void getAviconFileInfo(const QString& fileName);
    void fileManagerFinishedOperation(ExitCode exitCode, QString& message);
    void operationImpossible(QString message);
    unsigned long long getUserDirectorySize();

public slots:
    void setVisible(bool visible);
    void startFileManager();
    void stopFileManager();

protected:
    bool event(QEvent* e);
    bool eventFilter(QObject* obj, QEvent* e);
    bool gestureEvent(QGestureEvent* e);
    void tapAndHoldTriggered(QTapAndHoldGesture* gesture);

private slots:
    void progressBarsHide();
    void onUploadDone();
    void attentionFlashLabelHide();
    void isFlashMount();
    void on_ultrasonicDataButton_released();
    void on_soundCommentsDataButton_released();
    void on_photoDataButton_released();
    void on_screenshotsDataButton_released();
    void on_videoDataButton_released();
    void on_logsDataButton_released();
    void on_selectAllButton_released();
    void on_unselectAllButton_released();
    void on_deleteButton_released();
    void on_itemSelectionChanged();
    void on_openButton_released();
    void on_umountButton_released();
    void on_copyToFlashButton_released();
    void on_moveToFlashButton_released();
    void checkAvailableMemory();

    void on_ftpUploadButton_released();
    void on_cancelButton_released();
    void onUploadProgressChanged(qint64 done, qint64 total, qint64 speed);
    void onUploadFinished();
    void onUploadError();

    void on_listWidget_currentRowChanged(int currentRow);

    void onFileProgress(int progress, QString message);
    void onTotalProgress(int progress, QString message);
    void onFinishedOperation(QString message);
    void onAbortedOperation(QString message);
    void onCancelOperation(QString message);

signals:
    void removeFiles(const QString&);
    void moveFiles(const QString&);
    void doOpenBScanFileViewer();
    void doOpenImageViewer(const QString&, const QString&);
    void doOpenAudioPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack);
    void doOpenVideoPlayer(const QString& tracksLocation, const QStringList& playlist, int currentTrack);
    void availableMemoryPercent(int availableMemoryPercent);
    void doMemoryInfo(unsigned long long total, unsigned long long user, unsigned long long system);

    void memoryIsOver();
    void lowMemory();
    void enoughMemory();

    void doReport(const QString& filePath);

    void doCopyFiles(QStringList fileNames, QString destPath, QString sourcePath);
    void doMoveFiles(QStringList fileNames, QString destPath, QString sourcePath);
    void doDeleteFiles(QStringList fileNames, QString sourcePath);

private:
    Ui::MemoryPage* ui;

    QDir _currentDir;
    QString _flashDirPath;
    QString _currentFlashSubDir;
    QTimer* _checkTimer;
    QTimer* _checkTimerForAvailableMemory;
    QString _cduNumber;
    double _availableFlashMemory;
    double _selectedFilesMemory;
    QString _homePath;
    QStringList _selectedFiles;
    FTPUploader* _ftpUploader;
    QStorageInfo _storage;
    bool _isFileManagerInProcess;
    FileManager* _fileManager;
};

#endif  // MEMORYPAGE_H
