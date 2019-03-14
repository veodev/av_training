#ifndef MEMORYIMPORTPAGE_H
#define MEMORYIMPORTPAGE_H

#include <QWidget>

namespace Ui
{
class MemoryImportPage;
}

class MemoryImportPage : public QWidget
{
    Q_OBJECT

public:
    enum SystemFiles
    {
        None = 0,
        NotificationsDB = 1
    };
    explicit MemoryImportPage(QWidget* parent = 0);
    ~MemoryImportPage();

private slots:
    void on_checkUSBButton_released();

    void on_unmountUSBButton_released();

    void on_updateFilesButton_released();

    void on_filesListWidget_itemSelectionChanged();

private:
    void checkUSB();
    void setCurrentFile(SystemFiles file);
    QString _usbSearchPath;
    QString _usbRootPath;
    QString _currentFilePath;
    bool _usbStatus;
    MemoryImportPage::SystemFiles _currentFileType;
    Ui::MemoryImportPage* ui;
};

#endif  // MEMORYIMPORTPAGE_H
