#include <QLabel>
#include <QTimer>

#include "imageviewerpage.h"
#include "ui_imageviewerpage.h"

ImageViewerPage::ImageViewerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageViewerPage)
{
    ui->setupUi(this);
    _fileName = new QLabel(ui->label);
    _fileName->setFixedWidth(this->width());
    _fileName->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _fileName->setStyleSheet("background-color: rgba(0, 0, 0, 180); color: rgba(255, 255, 255, 255); font: bold 20px");
    _fileName->setVisible(false);

    ui->prevButton->setIconSize(QSize(32, 32));
    ui->prevButton->setIcon(QPixmap(":/media_previous_black_19x32.png"));
    ui->prevButton->setText("");

    ui->nextButton->setIconSize(QSize(32, 32));
    ui->nextButton->setIcon(QPixmap(":/media_next_black_32_19.png"));
    ui->nextButton->setText("");

    ui->deleteButton->setIconSize(QSize(32, 32));
    ui->deleteButton->setIcon(QPixmap(":/bin_black_26x32.png"));
    ui->deleteButton->setText("");
}

ImageViewerPage::~ImageViewerPage()
{
    delete _fileName;
    delete ui;
}

void ImageViewerPage::setCurrentDirAndFile(const QString& dir, const QString& file)
{
    _currentDir.setPath(dir);
    _currentFileName = file;
    _fileList = _currentDir.entryInfoList();

    for (const QFileInfo& fileInfo : _fileList) {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
            _fileList.removeOne(fileInfo);
            continue;
        }
    }

    if (_currentFileName == "" && !_fileList.isEmpty()) {
        _currentFileName = _fileList[0].fileName();
    }

    updateViewer();
}

void ImageViewerPage::updateViewer()
{
    QPixmap image(_currentDir.absolutePath() + "/" + _currentFileName);
    ui->label->setPixmap(image.scaled(ui->label->width(), ui->label->height(), Qt::KeepAspectRatio));
    _fileName->setText(_currentFileName);
    showFileName();
}

void ImageViewerPage::removeCurrentFile()
{
    _currentDir.remove(_currentFileName);
    _fileList.removeOne(QFileInfo(_currentDir, _currentFileName));
    if (!_fileList.isEmpty()) {
        _currentFileName = _fileList[0].fileName();
    }
    else {
        ui->label->clear();
        emit noFiles();
    }
    updateViewer();
}

bool ImageViewerPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void ImageViewerPage::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    _fileName->setText(_currentFileName);
    _fileName->setVisible(true);
}

void ImageViewerPage::hideFileName()
{
    _fileName->setVisible(false);
}

void ImageViewerPage::showFileName()
{
    _fileName->setVisible(true);
    QTimer::singleShot(5000, this, &ImageViewerPage::hideFileName);
}

void ImageViewerPage::on_nextButton_released()
{
    if (_fileList.isEmpty()) {
        return;
    }

    int index = _fileList.indexOf(QFileInfo(_currentDir, _currentFileName));
    if ((index + 1) >= _fileList.count()) {
        _currentFileName = _fileList[0].fileName();
    }
    else {
        _currentFileName = _fileList[index + 1].fileName();
    }
    updateViewer();
}

void ImageViewerPage::on_prevButton_released()
{
    if (_fileList.isEmpty()) {
        return;
    }
    int index = _fileList.indexOf(QFileInfo(_currentDir, _currentFileName));
    if (index >= 1) {
        _currentFileName = _fileList[index - 1].fileName();
    }
    else {
        _currentFileName = _fileList[_fileList.count() - 1].fileName();
    }
    updateViewer();
}

void ImageViewerPage::on_deleteButton_released()
{
    emit doRemoveFiles(_currentFileName + tr(" will be deleted."));
    ui->label->update();
    updateViewer();
}
