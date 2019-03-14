#include "filemanager.h"
#include "appdatapath.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#if defined(Q_OS_LINUX)
#include <unistd.h>
#endif

const qint64 FILE_BLOCK_SIZE_BYTES = 1024 * 1024;
const bool MOVE_FILES = true;

FileManager::FileManager(QObject* parent)
    : QObject(parent)
    , _isCancelOperation(false)
    , _copyState(false)
    , _prevPercent(0)
{
}

void FileManager::cancel()
{
    _isCancelOperation.store(true);
}

bool FileManager::isCopying()
{
    return _copyState.load();
}

bool FileManager::copyFile(QString& fileName, QString& destPath, QString& sourcePath)
{
    _copyState.store(true);
    _prevPercent = 0;
    QFile inFile(sourcePath + "/" + fileName);
    QFile outFile(destPath + "/" + fileName);
    if (!inFile.exists()) {
        _copyState.store(false);
        return false;
    }
    inFile.open(QFile::ReadOnly);
    outFile.open(QFile::WriteOnly);
    qint64 writtenBytes = 0;
    qint64 size = inFile.size();
    qint64 bytes;
    QByteArray tempArray;
    if (inFile.size() != 0) {
        while (true) {
            if (inFile.size() - inFile.pos() >= FILE_BLOCK_SIZE_BYTES) {
                tempArray = inFile.read(FILE_BLOCK_SIZE_BYTES);
                bytes = outFile.write(tempArray);
                tempArray.clear();
            }
            else {
                tempArray = inFile.read(inFile.size() - inFile.pos());
                bytes = outFile.write(tempArray);
                tempArray.clear();
            }

            if (!outFile.flush()) {
                inFile.close();
                outFile.close();
                _copyState.store(false);
                return false;
            }

            if (bytes >= 0) {
                writtenBytes += bytes;
            }
            else {
                inFile.close();
                outFile.close();
                _copyState.store(false);
                return false;
            }
            int value = qRound(((writtenBytes) / static_cast<float>(size)) * 100.0f);
            if (abs(value - _prevPercent) >= 1) {
                _prevPercent = value;
                emit fileProgress(value, fileName);
            }
            if (outFile.size() == inFile.size()) {
                break;
            }
        }
    }
    inFile.close();
    outFile.close();
    _copyState.store(false);
    return true;
}

ExitCode FileManager::copyFiles(QStringList& fileNames, QString& destPath, QString& sourcePath, bool isMove)
{
    for (int i = 0; i < fileNames.size(); ++i) {
        if (_isCancelOperation.load()) {
            _isCancelOperation.store(false);
            syncFileSystem();
            return ExitCode::Cancel;
        }
        if (!copyFile(fileNames[i], destPath, sourcePath)) {
            syncFileSystem();
            return ExitCode::Abort;
        }
        if (isMove) {
            if (!QFile(sourcePath + "/" + fileNames[i]).remove()) {
                syncFileSystem();
                return ExitCode::Abort;
            }
        }
        int value = qRound(((i + 1) / static_cast<float>(fileNames.size())) * 100.0f);
        emit totalProgress(value, QString(tr("%1 from %2")).arg(i + 1).arg(fileNames.size()));
    }
    _isCancelOperation.store(false);
    syncFileSystem();
    return ExitCode::Success;
}

ExitCode FileManager::deleteFiles(QStringList& fileNames, QString& sourcePath)
{
    for (int i = 0; i < fileNames.size(); ++i) {
        if (_isCancelOperation.load()) {
            _isCancelOperation.store(false);
            syncFileSystem();
            return ExitCode::Cancel;
        }
        if (!QFile(sourcePath + "/" + fileNames[i]).remove()) {
            syncFileSystem();
            return ExitCode::Abort;
        }
        int value = qRound(((i + 1) / static_cast<float>(fileNames.size())) * 100.0f);
        emit totalProgress(value, QString(tr("%1 from %2")).arg(i + 1).arg(fileNames.size()));
    }
    _isCancelOperation.store(false);
    syncFileSystem();
    return ExitCode::Success;
}

void FileManager::syncFileSystem()
{
#if defined(Q_OS_LINUX)
    sync();
#endif
}

void FileManager::onCopyFiles(QStringList fileNames, QString destPath, QString sourcePath)
{
    QString message;
    ExitCode exitCode = copyFiles(fileNames, destPath, sourcePath);

    switch (exitCode) {
    case ExitCode::Success:
        message = QString(tr("Was copied to the USB drive files: ")) + QString::number(fileNames.size());
        emit finishedOperation(message);
        break;
    case ExitCode::Cancel:
        message = QString(tr("Copying canceled!"));
        emit cancelOperation(message);
        break;
    case ExitCode::Abort:
        message = QString(tr("Copying aborted!"));
        emit abortOperation(message);
        break;
    }
}

void FileManager::onMoveFiles(QStringList fileNames, QString destPath, QString sourcePath)
{
    QString message;
    ExitCode exitCode;
    exitCode = copyFiles(fileNames, destPath, sourcePath, MOVE_FILES);
    switch (exitCode) {
    case ExitCode::Cancel:
        message = QString(tr("Moving canceled!"));
        syncFileSystem();
        emit cancelOperation(message);
        break;
    case ExitCode::Abort:
        message = QString(tr("Moving aborted!"));
        syncFileSystem();
        emit abortOperation(message);
        break;
    case ExitCode::Success:
        message = QString(tr("Was moved to the USB drive files: ")) + QString::number(fileNames.size());
        emit finishedOperation(message);
        break;
    }
}

void FileManager::onDeleteFiles(QStringList fileNames, QString sourcePath)
{
    QString message;
    ExitCode exitCode = deleteFiles(fileNames, sourcePath);
    switch (exitCode) {
    case ExitCode::Success:
        message = QString(tr("Was removed files: ")) + QString::number(fileNames.size());
        syncFileSystem();
        emit finishedOperation(message);
        break;
    case ExitCode::Cancel:
        message = QString(tr("Deleting canceled!"));
        syncFileSystem();
        emit cancelOperation(message);
        break;
    case ExitCode::Abort:
        message = QString(tr("Deleting aborted!"));
        syncFileSystem();
        emit abortOperation(message);
        break;
    }
}

void FileManager::started()
{
    qDebug() << "File manager started!";
}
