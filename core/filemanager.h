#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <coredefinitions.h>
#include <atomic>

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    void cancel();
    bool isCopying();

private:
    bool copyFile(QString& fileName, QString& destPath, QString& sourcePath);

    ExitCode copyFiles(QStringList& fileNames, QString& destPath, QString& sourcePath, bool _isMove = false);
    ExitCode deleteFiles(QStringList& fileNames, QString& sourcePath);
    void syncFileSystem();

signals:
    void totalProgress(int percents, QString message);
    void fileProgress(int percents, QString message);
    void resetFileProgress();
    void resetTotalProgress();
    void finishedOperation(QString message);
    void abortOperation(QString message);
    void cancelOperation(QString message);

public slots:
    void onCopyFiles(QStringList fileNames, QString destPath, QString sourcePath);
    void onMoveFiles(QStringList fileNames, QString destPath, QString sourcePath);
    void onDeleteFiles(QStringList fileNames, QString sourcePath);

    void started();

private:
    std::atomic_bool _isCancelOperation;
    std::atomic_bool _copyState;
    int _prevPercent;
};

#endif  // FILEMANAGER_H
