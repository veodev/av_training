#include <QProcess>
#include <QFile>
#include <QScrollBar>

#include "memorypage.h"
#include "ui_memorypage.h"
#include "core.h"
#include "debug.h"
#include "messagepage.h"
#include "settings.h"
#include "appdatapath.h"
#include "accessories.h"
#include "notifier.h"
#include "ftp/ftpuploader.h"
#include "filemanager.h"

#ifdef ANDROID
#include "androidJNI.h"
#endif

#if defined(Q_OS_LINUX)
#include "sys/mount.h"
#include <unistd.h>
#endif

#define MINIMAL_MEMORY_PERCENTS 5
#define LOW_MEMORY_PERCENTS 20

MemoryPage::MemoryPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MemoryPage)
    , _currentFlashSubDir("")
    , _checkTimer(new QTimer(this))
    , _checkTimerForAvailableMemory(new QTimer(this))
    , _cduNumber(cduSerialNumber())
    , _availableFlashMemory(0)
    , _selectedFilesMemory(0)
    , _homePath(QDir::homePath())
    , _isFileManagerInProcess(false)
    , _fileManager(nullptr)
{
    ui->setupUi(this);
    ui->attentionFlashLabel->setStyleSheet("color: red; border: 2px solid red;");
    ui->attentionLabel->setStyleSheet("color: green; border: 2px solid green;");
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->grabGesture(Qt::TapAndHoldGesture);
    ui->listWidget->installEventFilter(this);
    ui->attentionFlashLabel->setVisible(false);
    ui->attentionLabel->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->progressBarLabel->setVisible(false);
    ui->groupBox->setVisible(false);

    ui->cancelButton->setEnabled(false);
    ui->fileProgressBar->setVisible(false);
    ui->fileProgressBarLabel->setVisible(false);
    ui->totalProgressBar->setVisible(false);
    ui->totalProgressBarLabel->setVisible(false);

    _checkTimer->setInterval(1000);
    ASSERT(connect(_checkTimer, &QTimer::timeout, this, &MemoryPage::isFlashMount));
    ASSERT(connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MemoryPage::on_itemSelectionChanged));

    _storage = QStorageInfo::root();

    _checkTimerForAvailableMemory->setInterval(10000);
    ASSERT(connect(_checkTimerForAvailableMemory, &QTimer::timeout, this, &MemoryPage::checkAvailableMemory));
    _checkTimerForAvailableMemory->start();

    registrationsPath();
    videosPath();
    photosPath();
    commentsPath();
    screenshotsPath();
#if defined TARGET_AVICON31
    _ftpUploader = Core::instance().getFtpUploader();
    ASSERT(_ftpUploader);
    ASSERT(connect(_ftpUploader, &FTPUploader::progress, this, &MemoryPage::onUploadProgressChanged));
    ASSERT(connect(_ftpUploader, &FTPUploader::finished, this, &MemoryPage::onUploadFinished));
    ASSERT(connect(_ftpUploader, &FTPUploader::uploadError, this, &MemoryPage::onUploadError));
#elif defined(TARGET_AVICON15)
    ui->copyToFlashButton->hide();
    ui->moveToFlashButton->hide();
    ui->ftpUploadButton->hide();
    ui->umountButton->hide();
    ui->cancelButton->hide();
#else
    ui->ftpUploadButton->hide();
#endif
    on_ultrasonicDataButton_released();
    startFileManager();
}

MemoryPage::~MemoryPage()
{
    qDebug() << "Deleting memorypage...";

    stopFileManager();
    disconnect(_checkTimer, &QTimer::timeout, this, &MemoryPage::isFlashMount);
    disconnect(_checkTimerForAvailableMemory, &QTimer::timeout, this, &MemoryPage::checkAvailableMemory);

#if defined TARGET_AVICON31
    _ftpUploader = Core::instance().getFtpUploader();
    ASSERT(_ftpUploader);
    disconnect(_ftpUploader, &FTPUploader::progress, this, &MemoryPage::onUploadProgressChanged);
    disconnect(_ftpUploader, &FTPUploader::finished, this, &MemoryPage::onUploadFinished);
    disconnect(_ftpUploader, &FTPUploader::uploadError, this, &MemoryPage::onUploadError);
#endif
    delete ui;
}

void MemoryPage::resetStateButtons()
{
    ui->ultrasonicDataButton->setChecked(false);
    ui->screenshotsDataButton->setChecked(false);
    ui->soundCommentsDataButton->setChecked(false);
    ui->videoDataButton->setChecked(false);
    ui->photoDataButton->setChecked(false);
    ui->logsDataButton->setChecked(false);
}

void MemoryPage::browseFiles(QDir dir)
{
    ui->listWidget->clear();
    dir.refresh();
    int size = 0;
    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& fileInfo : dirList) {
        QFileInfoList fileList = QDir(fileInfo.absoluteFilePath()).entryInfoList(QDir::Files);
        size += fileList.size();
        for (const QFileInfo& fileInfo : fileList) {
            QListWidgetItem* newItem = new QListWidgetItem();
            int position = registrationsPath().size() - 1;
            newItem->setText(fileInfo.absoluteFilePath().remove(0, position));
            ui->listWidget->addItem(newItem);
        }
    }
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    size += fileList.size();
    for (const QFileInfo& fileInfo : fileList) {
        QListWidgetItem* newItem = new QListWidgetItem();
        newItem->setText(fileInfo.fileName());
        ui->listWidget->addItem(newItem);
    }
    size == 0 ? ui->countFilesLabel->setText(tr("Files: No files")) : ui->countFilesLabel->setText(tr("Files: ") + QString::number(size));
}

void MemoryPage::update()
{
    browseFiles(_currentDir);
}

void MemoryPage::startCheckTimer()
{
    if (_isFileManagerInProcess == false) {
        _checkTimer->start();
    }
}

void MemoryPage::stopCheckTimer()
{
    _checkTimer->stop();
}

void MemoryPage::checkMemory()
{
    checkAvailableMemory();
}

void MemoryPage::removeSelectedFiles()
{
    blockButtons(true);
    ui->cancelButton->setEnabled(true);
    ui->totalProgressBar->reset();
    ui->totalProgressBar->setVisible(true);
    ui->totalProgressBarLabel->setVisible(true);
    ui->progressBarLabel->setVisible(true);
    ui->progressBarLabel->setText(tr("Deleting files..."));
    emit doDeleteFiles(_selectedFiles, _currentDir.absolutePath());
}

void MemoryPage::moveSelectedFiles()
{
    blockButtons(true);
    _isFileManagerInProcess = true;
#if !defined TARGET_AVICON15
    _checkTimer->stop();
    _checkTimerForAvailableMemory->stop();
#endif
    ui->cancelButton->setEnabled(true);
    ui->fileProgressBar->reset();
    ui->totalProgressBar->reset();
    ui->fileProgressBar->setVisible(true);
    ui->fileProgressBarLabel->setVisible(true);
    ui->totalProgressBar->setVisible(true);
    ui->totalProgressBarLabel->setVisible(true);
    ui->progressBarLabel->setVisible(true);
    ui->progressBarLabel->setText(tr("Moving files..."));
    QDir().mkdir(_flashDirPath + "/" + _cduNumber + "/");
    QDir().mkdir(_flashDirPath + "/" + _cduNumber + "/" + _currentFlashSubDir);
    QString destinationPath = _flashDirPath + "/" + _cduNumber + "/" + _currentFlashSubDir;

    doMoveFiles(_selectedFiles, destinationPath, _currentDir.absolutePath());
}

void MemoryPage::configureUiForAviconDbAviconDbHs()
{
    ui->soundCommentsDataButton->hide();
    ui->photoDataButton->hide();
    ui->videoDataButton->hide();
}

void MemoryPage::configureUiForAvicon15()
{
    configureUiForAviconDbAviconDbHs();
}

void MemoryPage::setVisibleLogButton(bool isVisible)
{
    ui->logsDataButton->setVisible(isVisible);
}

void MemoryPage::blockButtons(bool isBlock)
{
    ui->umountButton->setDisabled(isBlock);
    ui->deleteButton->setDisabled(isBlock);
    ui->copyToFlashButton->setDisabled(isBlock);
    ui->moveToFlashButton->setDisabled(isBlock);
    ui->ftpUploadButton->setDisabled(isBlock);
    ui->umountButton->setDisabled(isBlock);
    ui->selectAllButton->setDisabled(isBlock);
    ui->unselectAllButton->setDisabled(isBlock);
    ui->listWidget->setDisabled(isBlock);
    ui->ultrasonicDataButton->setDisabled(isBlock);
    ui->soundCommentsDataButton->setDisabled(isBlock);
    ui->screenshotsDataButton->setDisabled(isBlock);
    ui->photoDataButton->setDisabled(isBlock);
    ui->videoDataButton->setDisabled(isBlock);
    if ((QString(_currentDir.absolutePath() + "/") == registrationsPath()) == false) {
        ui->openButton->setDisabled(isBlock);  // TODO Workaround. Need viewer for registration files.
    }
}

void MemoryPage::getAviconFileInfo(const QString& fileName)
{
    if (fileName.contains(".a31", Qt::CaseInsensitive)) {
        qDebug() << "Parsing avicon file for stats:" << fileName;
        QFile file(registrationsPath() + fileName);
        if (file.open(QIODevice::ReadOnly) == false) {
            return;
        }
        ui->lastSelectFileNameLabel->setText("");
        ui->selectedFileLineNameLabel->setText("");
        ui->selectedFileDateLabel->setText("");
        ui->groupBox->setVisible(true);
        ui->lastSelectFileNameLabel->setText(QString(QObject::tr("File: ")) + fileName);
        if (file.size() < 3246) {
            qDebug() << "File is not full:" << fileName;
            return;
        }
        QByteArray data;
        file.seek(2970);
        data = file.read(130);
        ui->selectedFileLineNameLabel->setText(QString(QObject::tr("Line: ")) + QString::fromUtf16(reinterpret_cast<const ushort*>(data.constData())));
        file.seek(3236);
        quint16 year = 0;
        quint16 month = 0;
        quint16 day = 0;
        data = file.read(2);
        year = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
        data = file.read(2);
        month = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
        data = file.read(2);
        day = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
        quint16 hours = 0;
        quint16 minutes = 0;
        file.seek(3242);
        data = file.read(2);
        hours = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
        file.seek(3244);
        data = file.read(2);
        minutes = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
        QDateTime dateTime(QDate(year, month, day), QTime(hours, minutes, 0));
        ui->selectedFileDateLabel->setText(QString(QObject::tr("Date: ")) + dateTime.toString(QString("dd.MM.yyyy") + QString(" ") + QString("(hh:mm)")));
        file.close();
    }
}

void MemoryPage::fileManagerFinishedOperation(ExitCode exitCode, QString& message)
{
    switch (exitCode) {
    case ExitCode::Abort:
        ui->attentionLabel->setStyleSheet("color: red; border: 2px solid red;");
        break;
    case ExitCode::Cancel:
    case ExitCode::Success:
        ui->attentionLabel->setStyleSheet("color: green; border: 2px solid green;");
        break;
    }

    _isFileManagerInProcess = false;
    ui->attentionLabel->setText(message);
    ui->attentionLabel->setVisible(true);
    QTimer::singleShot(3000, this, &MemoryPage::progressBarsHide);
    browseFiles(_currentDir);
#if !defined TARGET_AVICON15
    startCheckTimer();
    _checkTimerForAvailableMemory->start();
#endif
    blockButtons(false);
    ui->cancelButton->setEnabled(false);
}

void MemoryPage::operationImpossible(QString message)
{
    ui->attentionLabel->setStyleSheet("color: red; border: 2px solid red;");
    ui->attentionLabel->setText(message);
    ui->attentionLabel->setVisible(true);
}

void MemoryPage::startFileManager()
{
    _fileManager = Core::instance().getFileManager();
    connect(_fileManager, &FileManager::fileProgress, this, &MemoryPage::onFileProgress);
    connect(_fileManager, &FileManager::totalProgress, this, &MemoryPage::onTotalProgress);
    connect(_fileManager, &FileManager::finishedOperation, this, &MemoryPage::onFinishedOperation);
    connect(_fileManager, &FileManager::abortOperation, this, &MemoryPage::onAbortedOperation);
    connect(_fileManager, &FileManager::cancelOperation, this, &MemoryPage::onCancelOperation);
    connect(this, &MemoryPage::doCopyFiles, _fileManager, &FileManager::onCopyFiles);
    connect(this, &MemoryPage::doMoveFiles, _fileManager, &FileManager::onMoveFiles);
    connect(this, &MemoryPage::doDeleteFiles, _fileManager, &FileManager::onDeleteFiles);
}

void MemoryPage::stopFileManager()
{
    _fileManager->cancel();
    disconnect(_fileManager, &FileManager::fileProgress, this, &MemoryPage::onFileProgress);
    disconnect(_fileManager, &FileManager::totalProgress, this, &MemoryPage::onTotalProgress);
    disconnect(_fileManager, &FileManager::finishedOperation, this, &MemoryPage::onFinishedOperation);
    disconnect(_fileManager, &FileManager::abortOperation, this, &MemoryPage::onAbortedOperation);
    disconnect(_fileManager, &FileManager::cancelOperation, this, &MemoryPage::onCancelOperation);
    disconnect(this, &MemoryPage::doCopyFiles, _fileManager, &FileManager::onCopyFiles);
    disconnect(this, &MemoryPage::doMoveFiles, _fileManager, &FileManager::onMoveFiles);
    disconnect(this, &MemoryPage::doDeleteFiles, _fileManager, &FileManager::onDeleteFiles);
}

void MemoryPage::setVisible(bool visible)
{
    if (visible == true) {
        update();
        startCheckTimer();
    }
    else {
        stopCheckTimer();
    }

    QWidget::setVisible(visible);
}

bool MemoryPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    if (e->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent*>(e));
    }
    return QWidget::event(e);
}

bool MemoryPage::eventFilter(QObject* obj, QEvent* e)
{
    Q_UNUSED(obj);
    if (ui->listWidget->verticalScrollBar()->isSliderDown() || _isFileManagerInProcess) {  // workaround!
        return false;
    }

    if (e->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent*>(e));
    }
    return false;
}

bool MemoryPage::gestureEvent(QGestureEvent* e)
{
    if (QGesture* tapAndHold = e->gesture(Qt::TapAndHoldGesture)) {
        tapAndHoldTriggered(static_cast<QTapAndHoldGesture*>(tapAndHold));
    }
    return true;
}

void MemoryPage::tapAndHoldTriggered(QTapAndHoldGesture* gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        if (_currentDir == registrationsPath()) {
            int row = ui->listWidget->currentRow();
            if (row != -1) {
                on_unselectAllButton_released();
                ui->listWidget->setCurrentRow(row);
                emit doReport(registrationsPath() + ui->listWidget->currentItem()->text());
            }
        }
    }
}

void MemoryPage::on_ultrasonicDataButton_released()
{
    ui->deleteButton->setEnabled(true);
    ui->openButton->setEnabled(true);
    ui->typeFilesLabel->setText(tr("Registration files."));
    _currentDir.setPath(registrationsPath());
    _currentFlashSubDir = "data";
    resetStateButtons();
    ui->ultrasonicDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_soundCommentsDataButton_released()
{
    ui->deleteButton->setEnabled(true);
    ui->openButton->setDisabled(false);
    ui->typeFilesLabel->setText(tr("Comments files."));
    _currentDir.setPath(commentsPath());
    _currentFlashSubDir = "comments";
    resetStateButtons();
    ui->soundCommentsDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_photoDataButton_released()
{
    ui->deleteButton->setEnabled(true);
    ui->openButton->setDisabled(false);
    ui->typeFilesLabel->setText(tr("Photo files."));
    _currentDir.setPath(photosPath());
    _currentFlashSubDir = "photo";
    resetStateButtons();
    ui->photoDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_screenshotsDataButton_released()
{
    ui->deleteButton->setEnabled(true);
    ui->openButton->setDisabled(false);
    ui->typeFilesLabel->setText(tr("Screenshots files."));
    _currentDir.setPath(screenshotsPath());
    _currentFlashSubDir = "screenshots";
    resetStateButtons();
    ui->screenshotsDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_videoDataButton_released()
{
    ui->deleteButton->setEnabled(true);
    ui->openButton->setDisabled(false);
    ui->typeFilesLabel->setText(tr("Video files."));
    _currentDir.setPath(videosPath());
    _currentFlashSubDir = "video";
    resetStateButtons();
    ui->videoDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_logsDataButton_released()
{
    ui->openButton->setDisabled(true);
    ui->deleteButton->setDisabled(true);
    ui->moveToFlashButton->setDisabled(true);
    ui->ftpUploadButton->setDisabled(true);
    ui->cancelButton->setDisabled(true);
    ui->typeFilesLabel->setText(tr("Log files."));
    _currentDir.setPath(logsPath());
    _currentFlashSubDir = "logs";
    resetStateButtons();
    ui->logsDataButton->setChecked(true);
    browseFiles(_currentDir);
}

void MemoryPage::on_selectAllButton_released()
{
    ui->attentionLabel->hide();
    ui->listWidget->selectAll();
    ui->groupBox->setVisible(false);
}

void MemoryPage::on_unselectAllButton_released()
{
    ui->attentionLabel->hide();
    ui->listWidget->clearSelection();
    ui->groupBox->setVisible(false);
}

void MemoryPage::on_deleteButton_released()
{
    ui->attentionLabel->hide();
    _selectedFiles = convertToQStringList(ui->listWidget->selectedItems());
    int count = _selectedFiles.size();
    QString message = "";
    if (count == 0) {
        message = tr("No selected files for delete!");
        operationImpossible(message);
    }
    else {
        _selectedFiles = convertToQStringList(ui->listWidget->selectedItems());
        message = tr("Will be deleted files: ") + QString::number(count);
        emit removeFiles(message);
    }
}

void MemoryPage::on_itemSelectionChanged()
{
    _selectedFilesMemory = 0;
    ui->countOfSelectedFilesLabel->setText(tr("Selected files: ") + QString::number(ui->listWidget->selectedItems().size()));
    for (const QListWidgetItem* item : ui->listWidget->selectedItems()) {
        QFileInfo fileInfo(_currentDir, item->text());
        _selectedFilesMemory = _selectedFilesMemory + fileInfo.size();
    }
    ui->countOfSelectedFilesLabel->setText(ui->countOfSelectedFilesLabel->text() + QString(" (") + convertBytesToView(_selectedFilesMemory) + QString(")"));
    if (ui->listWidget->selectedItems().size() == 0) {
        ui->groupBox->setVisible(false);
    }
    else if (ui->listWidget->selectedItems().size() == 1) {
        on_listWidget_currentRowChanged(ui->listWidget->currentRow());
        ui->openButton->setEnabled(true);
    }
    else{
        ui->openButton->setEnabled(false);
    }
}

void MemoryPage::on_openButton_released()
{
    ui->attentionLabel->hide();
    QString dirPath = _currentDir.absolutePath() + "/";
    int currentRow = 0;

    if (ui->listWidget->selectedItems().size() == 0){
        operationImpossible(QString(tr("No selected files for open.")));
        return;
    }

    if (dirPath == registrationsPath()) {
        if (ui->listWidget->count() > 0) {
            QString fileName = "";
            QString message = "";
            fileName = ui->listWidget->selectedItems().at(0)->text();
            switch (Core::instance().openFile(dirPath, fileName)) {
            case 0:
                emit doOpenBScanFileViewer();
                break;
            case 1:
                message = tr("Another scheme");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            case 2:
                message = tr("Broken file");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            case 3:
                message = tr("Can't open file");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            case 4:
                message = tr("Invalid format file");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            case 5:
                message = tr("Identifier is empty");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            default:
                message = tr("Uknown error");
                Notifier::instance().addNote(Qt::red, QString(message));
                break;
            }
        }
        return;
    }

    if (dirPath == commentsPath()) {
        QStringList playlist;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            playlist.append(ui->listWidget->item(i)->text());
        }

        if (playlist.count() > 0) {
            currentRow = ui->listWidget->currentRow();
            if (currentRow == -1) {
                currentRow = 0;
            }
            emit doOpenAudioPlayer(commentsPath(), playlist, currentRow);
        }
    }
    else if (dirPath == videosPath()) {
        QStringList playlist;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            playlist.append(ui->listWidget->item(i)->text());
        }

        if (playlist.count() > 0) {
            currentRow = ui->listWidget->currentRow();
            if (currentRow == -1) {
                currentRow = 0;
            }
            emit doOpenVideoPlayer(videosPath(), playlist, currentRow);
        }
    }
    else if (dirPath == photosPath() || dirPath == screenshotsPath()) {
        if (ui->listWidget->count() > 0) {
            QString fileName = "";
            if (ui->listWidget->selectedItems().size() == 1) {
                fileName = ui->listWidget->currentItem()->text();
            }
            emit doOpenImageViewer(dirPath, fileName);
        }
    }
}

void MemoryPage::progressBarsHide()
{
    ui->fileProgressBar->setVisible(false);
    ui->fileProgressBarLabel->setVisible(false);
    ui->totalProgressBar->setVisible(false);
    ui->totalProgressBarLabel->setVisible(false);
    ui->progressBarLabel->setVisible(false);
    ui->progressBar->setVisible(false);
}

void MemoryPage::onUploadDone()
{
    ui->progressBarLabel->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->ftpUploadButton->setEnabled(true);
    ui->cancelButton->setEnabled(false);
}

void MemoryPage::attentionFlashLabelHide()
{
    ui->attentionLabel->hide();
    ui->attentionFlashLabel->setVisible(false);
}

void MemoryPage::on_umountButton_released()
{
    ui->attentionLabel->hide();
    int isUmount = -1;
#if defined(Q_OS_LINUX)
    sync();
    isUmount = umount(_flashDirPath.toStdString().c_str());
#endif
    if (isUmount == 0) {
        QString message = tr("You can remove the USB drive.");
        ui->attentionFlashLabel->setVisible(true);
        ui->attentionFlashLabel->setText(message);
        QTimer::singleShot(5000, this, &MemoryPage::attentionFlashLabelHide);
    }
}

void MemoryPage::on_copyToFlashButton_released()
{
    ui->attentionLabel->hide();
    blockButtons(true);
    _isFileManagerInProcess = true;
    _selectedFiles = convertToQStringList(ui->listWidget->selectedItems());
#if !defined TARGET_AVICON15
    _checkTimer->stop();
    _checkTimerForAvailableMemory->stop();
#endif
    int count = _selectedFiles.size();
    if (count == 0) {
        operationImpossible(QString(tr("No selected files for copy!")));
        blockButtons(false);
    }
    else if ((_availableFlashMemory * 1024) < _selectedFilesMemory) {
        operationImpossible(QString(tr("There is no space on USB drive!")));
        blockButtons(false);
    }
    else {
        ui->cancelButton->setEnabled(true);
        ui->fileProgressBar->reset();
        ui->totalProgressBar->reset();
        ui->fileProgressBar->setVisible(true);
        ui->fileProgressBarLabel->setVisible(true);
        ui->totalProgressBar->setVisible(true);
        ui->totalProgressBarLabel->setVisible(true);
        ui->progressBarLabel->setVisible(true);
        ui->progressBarLabel->setText(tr("Copying files..."));
        QDir().mkdir(_flashDirPath + "/" + _cduNumber + "/");
        QDir().mkdir(_flashDirPath + "/" + _cduNumber + "/" + _currentFlashSubDir);
        QString destinationPath = _flashDirPath + "/" + _cduNumber + "/" + _currentFlashSubDir;
        emit doCopyFiles(_selectedFiles, destinationPath, _currentDir.absolutePath());
    }
}

void MemoryPage::on_moveToFlashButton_released()
{
    ui->attentionLabel->hide();
    _selectedFiles = convertToQStringList(ui->listWidget->selectedItems());
    int count = _selectedFiles.size();
    QString message = "";
    if (count == 0) {
        message = tr("No selected files for move!");
        operationImpossible(message);
    }
    else if ((_availableFlashMemory * 1024) < _selectedFilesMemory) {
        message = tr("There is no space on USB drive!");
        operationImpossible(message);
    }
    else {
        message = tr("Will be moved files: ") + QString::number(count);
        emit moveFiles(message);
    }
}

unsigned long long MemoryPage::getUserDirectorySize()
{
    QProcess process;
    process.start("du -s " + appDataPath(""));
    process.waitForStarted();
    process.waitForFinished();
    const QStringList& list = QString(process.readAllStandardOutput()).split('\t');
    process.close();
    if (!list.empty()) {
        return list.at(0).toInt();
    }

    return 0;
}

void MemoryPage::checkAvailableMemory()
{
    unsigned long long totalBytes = 0;
    unsigned long long availableBytes = 0;
    unsigned long long usedBytes = 0;
    unsigned long long userKBytes = 0;
#if defined TARGET_AVICON31 && defined ANDROID
    int availableMemory = getAvailableExternalMemoryPercentJNI();
    if (availableMemory < 0) return;
    totalBytes = getTotalBytes();
    availableBytes = getAvailableBytes();
    usedBytes = totalBytes - availableBytes;
    userKBytes = usedBytes;
    qWarning() << "Total bytes: " << convertBytesToView(totalBytes);
    qWarning() << "Available bytes: " << convertBytesToView(availableBytes);
    qWarning() << "Used bytes: " << convertBytesToView(usedBytes);

    emit doMemoryInfo(totalBytes / 1024, userKBytes / 1024, 0);
#else
    _storage.refresh();
    totalBytes = _storage.bytesTotal();
    availableBytes = _storage.bytesAvailable();
    usedBytes = totalBytes - availableBytes;
    userKBytes = getUserDirectorySize();
    emit doMemoryInfo(totalBytes / 1024, userKBytes, (usedBytes / 1024) - userKBytes);
    int availableMemory = qRound((availableBytes / static_cast<double>(totalBytes)) * 100.0);
#endif
    _checkTimerForAvailableMemory->stop();
    if (availableMemory > LOW_MEMORY_PERCENTS) {
        _checkTimerForAvailableMemory->setInterval(120 * 1000);
    }
    else {
        _checkTimerForAvailableMemory->setInterval(10 * 1000);
    }
    _checkTimerForAvailableMemory->start();
    emit availableMemoryPercent(availableMemory);
    if (availableMemory < MINIMAL_MEMORY_PERCENTS) {
        emit memoryIsOver();
    }
    else if (availableMemory < LOW_MEMORY_PERCENTS) {
        emit lowMemory();
    }
    else {
        emit enoughMemory();
    }
}

void MemoryPage::isFlashMount()
{
    QString message = "";
    QProcess process;
    process.start("df -T");
    process.waitForStarted();
    QString dfCommandFileSystemPattern;
#if defined TARGET_AVICON31
    dfCommandFileSystemPattern = "sda";
#elif defined(TARGET_AVICONDBHS) || defined(TARGET_AVICONDB)
    dfCommandFileSystemPattern = "sdb";
#endif
    process.waitForFinished();
    QStringList list = QString(process.readAllStandardOutput()).split('\n');
    QString srtFromDfCommand;
    if (list.isEmpty() == false) {
        for (const QString& str : list) {
            if (str.contains(dfCommandFileSystemPattern)) {
                srtFromDfCommand = str;
            }
        }
    }
    QString flashPath;
    QString flashFileSystem;
    QString flashAvailableMemoryKb;
    QStringList list1 = srtFromDfCommand.split(QChar(' '));
    list1.removeAll(QString());
    if (list1.isEmpty() == false && list1.count() == 7) {
        flashFileSystem = list1[1];
        flashAvailableMemoryKb = list1[4];
        flashPath = list1[6];
    }
    if (flashPath.isEmpty()) {
        ui->availableFlashMemoryLabel->setText(tr("Available memory: 0"));
        _availableFlashMemory = 0;
        ui->umountButton->setEnabled(false);
        ui->copyToFlashButton->setEnabled(false);
        ui->moveToFlashButton->setEnabled(false);
        QTimer::singleShot(3000, this, &MemoryPage::attentionFlashLabelHide);
    }
    else {
        if (flashFileSystem == QString("vfat")) {
            _availableFlashMemory = flashAvailableMemoryKb.toDouble();
            ui->availableFlashMemoryLabel->setText(QString(tr("Available memory: ")) + convertBytesToView(_availableFlashMemory * 1024));
            _flashDirPath = flashPath;
            ui->umountButton->setEnabled(true);
            ui->copyToFlashButton->setEnabled(true);
            ui->moveToFlashButton->setEnabled(true);
            message = tr("Before removing USB drive, click \"Umount\"!");
            ui->attentionFlashLabel->setVisible(true);
            ui->attentionFlashLabel->setText(message);
        }
        else {
            ui->umountButton->setEnabled(false);
            ui->copyToFlashButton->setEnabled(false);
            ui->moveToFlashButton->setEnabled(false);
            message = tr("Insert USB drive formatted in FAT!");
            ui->attentionFlashLabel->setVisible(true);
            ui->attentionFlashLabel->setText(message);
        }
    }
}

void MemoryPage::on_ftpUploadButton_released()
{
    ui->ftpUploadButton->setEnabled(false);
    ui->attentionLabel->hide();
    _selectedFiles = convertToQStringList(ui->listWidget->selectedItems());
    _ftpUploader->setParameters(restoreFtpServer(), restoreFtpPath(), restoreFtpLogin(), restoreFtpPassword());
    for (const QString& item : _selectedFiles) {
        QFileInfo fileInfo(_currentDir, item);
        if (fileInfo.exists()) {
            const QString sourcePath = fileInfo.absoluteFilePath();
            const QString destinationPath = fileInfo.fileName();
            ui->progressBar->setVisible(true);
            ui->progressBarLabel->setVisible(true);
            ui->progressBarLabel->setText(tr("Uploading files..."));
            ui->progressBar->setValue(0);
            if (!_ftpUploader->uploadFile(sourcePath, destinationPath)) {
                ui->progressBarLabel->setText(tr("Error! cannot upload file ") + fileInfo.fileName());
                ui->progressBar->setValue(100);
            }
        }
        else {
            qDebug() << "File: " << fileInfo.absoluteFilePath() << " does not exist!";
        }
    }
    ui->cancelButton->setEnabled(true);
}

void MemoryPage::on_cancelButton_released()
{
    ui->attentionLabel->hide();
    _fileManager->cancel();
    _isFileManagerInProcess = false;
    _ftpUploader->abortCurrentReply();
    ui->progressBarLabel->setText(tr("Upload cancelled!"));
    operationImpossible(QString(tr("Canceling ...")));
    ui->cancelButton->setEnabled(false);
    qDebug() << "Cancel button released";
}

void MemoryPage::onUploadProgressChanged(qint64 done, qint64 total, qint64 speed)
{
    int percent = qRound(static_cast<float>(done * 100.0f) / static_cast<float>(total));
    QString text = QString("%1 KB / %2 KB (%3 KB/s)").arg(done / 1024).arg(total / 1024).arg(speed / 1024);
    ui->progressBar->setFormat(text);
    ui->progressBar->setValue(percent);
}

void MemoryPage::onUploadFinished()
{
    ui->progressBarLabel->setVisible(true);
    ui->progressBarLabel->setText(tr("Upload finished!"));
    QTimer::singleShot(2000, this, &MemoryPage::onUploadDone);
}

void MemoryPage::onUploadError()
{
    ui->progressBarLabel->setVisible(true);
    ui->progressBarLabel->setText(tr("Error uploading file!"));

    QTimer::singleShot(2000, this, &MemoryPage::onUploadDone);
}

void MemoryPage::on_listWidget_currentRowChanged(int currentRow)
{
    if (currentRow == -1) {
        ui->groupBox->setVisible(false);
        return;
    }
    const QString& fileName = ui->listWidget->currentItem()->text();
    getAviconFileInfo(fileName);
}

void MemoryPage::onFileProgress(int progress, QString message)
{
    ui->fileProgressBar->setValue(progress);
    ui->fileProgressBarLabel->setText(QString(tr("File: ")) + message);
}

void MemoryPage::onTotalProgress(int progress, QString message)
{
    ui->totalProgressBar->setValue(progress);
    ui->totalProgressBarLabel->setText(QString(tr("Total: ")) + message);
}

void MemoryPage::onFinishedOperation(QString message)
{
    fileManagerFinishedOperation(ExitCode::Success, message);
}

void MemoryPage::onAbortedOperation(QString message)
{
    fileManagerFinishedOperation(ExitCode::Abort, message);
}

void MemoryPage::onCancelOperation(QString message)
{
    fileManagerFinishedOperation(ExitCode::Cancel, message);
}
