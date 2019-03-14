#include "memoryimportpage.h"
#include "ui_memoryimportpage.h"
#include "appdatapath.h"
#include "debug.h"
#include <QDateTime>
#include <QFileInfo>
#include <QProcess>

#include "sys/mount.h"
#include <unistd.h>

MemoryImportPage::MemoryImportPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MemoryImportPage)
{
    ui->setupUi(this);
    _usbStatus = false;
    ui->updateFilesButton->setEnabled(false);
    ui->checkUSBButton->setEnabled(false);
    ui->filesListWidget->addItem("marks.sqlite");
    setCurrentFile(None);
    checkUSB();
}

MemoryImportPage::~MemoryImportPage()
{
    delete ui;
}

void MemoryImportPage::on_checkUSBButton_released()
{
    checkUSB();
    if (_usbStatus) {
        ui->updateFilesButton->setEnabled(true);
    }
    else {
        ui->updateFilesButton->setEnabled(false);
    }
}

void MemoryImportPage::on_unmountUSBButton_released()
{
    if (_usbStatus) {
        int isUmount = -1;
#if defined(Q_OS_LINUX)
        sync();
        isUmount = umount(_usbRootPath.toStdString().c_str());
#endif
        if (isUmount == 0) {
            ui->usbStatusLabel->setText(tr("USB unmounted!"));
        }
    }
}

void MemoryImportPage::on_updateFilesButton_released()
{
    if (_usbStatus) {
        if (_currentFileType != None) {
            sync();
            QFile currentFile(_currentFilePath);
            currentFile.remove();
            sync();
            QFile usbFile(_usbRootPath + _usbSearchPath);
            usbFile.copy(_currentFilePath);
            sync();
        }
    }
}

void MemoryImportPage::setCurrentFile(MemoryImportPage::SystemFiles file)
{
    _currentFileType = file;
    switch (file) {
    case None:
        ui->fileNameLabel->setText(tr("None"));
        _currentFilePath = "";
        ui->descriptionLabel->setText("");
        ui->versionLabel->setText("");
        _usbSearchPath = "";
        break;
    case NotificationsDB:
        ui->fileNameLabel->setText("marks.sqlite");
        _currentFilePath = marksPath() + "marks.sqlite";
        ui->descriptionLabel->setText(tr("Operator's notifications database"));
        QString version;
        QFileInfo currentFile(_currentFilePath);
        if (currentFile.exists()) {
            const QDateTime& date = currentFile.lastModified();
            version = date.toString("dd.MM.yyyy");
        }
        else {
            version = tr("File not found!");
        }
        ui->versionLabel->setText(version);
        _usbSearchPath = "/marks.sqlite";

        break;
    }
}

void MemoryImportPage::on_filesListWidget_itemSelectionChanged()
{
    const auto& selected = ui->filesListWidget->selectedItems();
    if (selected.isEmpty()) {
        ui->fileNameLabel->setText("");
        ui->versionLabel->setText("");
        ui->descriptionLabel->setText("");
        ui->newVersionLabel->setText("");
    }
    else {
        const QString& tmp = selected.at(0)->data(0).toString();
        if (tmp == "marks.sqlite") {
            setCurrentFile(NotificationsDB);
        }
        ui->checkUSBButton->setEnabled(true);
    }
}

void MemoryImportPage::checkUSB()
{
    sync();
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
    ui->newVersionLabel->setText("");
    if (flashPath.isEmpty()) {
        ui->usbStatusLabel->setText(tr("Cannot mount USB!"));
        _usbStatus = false;
    }
    else {
        if (flashFileSystem == QString("vfat")) {
            ui->usbStatusLabel->setText(tr("USB Mounted!"));
            QFileInfo newFile(_usbRootPath + _usbSearchPath);
            QString version;
            if (newFile.exists()) {
                const QDateTime& date = newFile.lastModified();
                version = date.toString("dd.MM.yyyy");
                _usbStatus = true;
            }
            else {
                version = tr("File not found!");
                _usbStatus = false;
            }
            ui->newVersionLabel->setText(version);
            _usbRootPath = flashPath;
        }
        else {
            ui->usbStatusLabel->setText(tr("Unsupported filesystem!"));
            _usbStatus = false;
        }
    }
}
