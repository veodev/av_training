#ifndef FILEPLACEMENTWIDGET_H
#define FILEPLACEMENTWIDGET_H

#include <QWidget>
#include <QFileSystemModel>

namespace Ui {
class FilePlacementWidget;
}

class QFileSystemModel;

class FilePlacementWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;

public:
    explicit FilePlacementWidget(QWidget *parent = 0);
    ~FilePlacementWidget();

    void popup();

protected:
    bool event(QEvent* e);

private:
    bool checkDirectoryName();
    bool checkFileName();

signals:
    void fileNameAndPlacementChanged(const QString& viewFilePath, const QString& fullFilePath);
    void fileNameAndPlacementByDefault();

private slots:
    void on_closeButton_released();
    void on_resetSelectedDirectoryButton_released();
    void on_resetFileNameButton_released();
    void on_addDirectoryButton_released();
    void on_resetNewDirectoryNameButton_released();

    void onNewDirectory(const QString& text);
    void onNewFileName(const QString& text);
    void onDirectorySelected(const QModelIndex &index);

    void on_okButton_released();

private:
    Ui::FilePlacementWidget *ui;
    QFileSystemModel* _model;
};

#endif // FILEPLACEMENTWIDGET_H
