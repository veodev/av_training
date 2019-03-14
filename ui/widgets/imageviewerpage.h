#ifndef IMAGEVIEWERPAGE_H
#define IMAGEVIEWERPAGE_H

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

class QLabel;

namespace Ui
{
class ImageViewerPage;
}

class ImageViewerPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewerPage(QWidget* parent = 0);
    ~ImageViewerPage();

    void setCurrentDirAndFile(const QString& dir, const QString& file);
    void updateViewer();
    void removeCurrentFile();

signals:
    void doRemoveFiles(const QString&);
    void noFiles();

protected:
    bool event(QEvent* e);
    void mousePressEvent(QMouseEvent* e);

private slots:
    void hideFileName();
    void showFileName();
    void on_nextButton_released();
    void on_prevButton_released();
    void on_deleteButton_released();

private:
    Ui::ImageViewerPage* ui;
    QDir _currentDir;
    QFileInfoList _fileList;
    QString _currentFileName;
    QLabel* _fileName;
};

#endif  // IMAGEVIEWERPAGE_H
