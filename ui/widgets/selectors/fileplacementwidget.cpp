#include "fileplacementwidget.h"
#include "ui_fileplacementwidget.h"
#include "appdatapath.h"

#include <unistd.h>

FilePlacementWidget::FilePlacementWidget(QWidget *parent) : QWidget(parent)
  , ui(new Ui::FilePlacementWidget)
  , _parentWidget(parent)
  , _model(new QFileSystemModel(this))
{
    this->hide();
    ui->setupUi(this);

    QPalette bgPal = ui->bgWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->bgWidget->setPalette(bgPal);

    QPalette windowPal = ui->windowWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->windowWidget->setPalette(windowPal);
    ui->windowWidget->setAttribute(Qt::WA_NoSystemBackground, false);

    _model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    _model->setRootPath(registrationsPath());

    ui->directoriesListView->setModel(_model);
    ui->directoriesListView->setRootIndex(_model->index(registrationsPath()));

    connect(ui->newDirectoryLineEdit, &QLineEdit::textChanged, this, &FilePlacementWidget::onNewDirectory);
    connect(ui->directoriesListView, &QListView::clicked, this, &FilePlacementWidget::onDirectorySelected);
    connect(ui->fileNameLineEdit, &QLineEdit::textChanged, this, &FilePlacementWidget::onNewFileName);

    ui->fileNameLineEdit->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Zа-яА-я0-9]+"), ui->fileNameLineEdit));
}

FilePlacementWidget::~FilePlacementWidget()
{
    delete ui;
}

void FilePlacementWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    raise();
    show();
}

bool FilePlacementWidget::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

bool FilePlacementWidget::checkDirectoryName()
{
    QDir dir(registrationsPath());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        if (it.fileName() == ui->newDirectoryLineEdit->text()) {
            return false;
        }
    }
    return true;
}

bool FilePlacementWidget::checkFileName()
{
    QDir dir(registrationsPath() + ui->selectedDirectoryLineEdit->text());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    return (!dir.entryList().contains(ui->fileNameLineEdit->text() + QString(".a31")));
}

void FilePlacementWidget::on_closeButton_released()
{
    this->hide();
}

void FilePlacementWidget::on_resetSelectedDirectoryButton_released()
{
    ui->selectedDirectoryLineEdit->clear();
    ui->directoriesListView->clearSelection();
}

void FilePlacementWidget::on_resetFileNameButton_released()
{
    ui->fileNameLineEdit->clear();
}

void FilePlacementWidget::on_addDirectoryButton_released()
{
    if (!checkDirectoryName()) {
        ui->newDirectoryLineEdit->setStyleSheet("color: red");
        ui->infoLabel->setText(QString(tr("Directory already exists!")));
    }
    else {
        QDir(registrationsPath()).mkdir(ui->newDirectoryLineEdit->text().toUtf8());
#ifdef Q_OS_LINUX
        sync();
#endif
        ui->newDirectoryLineEdit->clear();
        ui->newDirectoryLineEdit->setStyleSheet("");
    }
}

void FilePlacementWidget::onNewDirectory(const QString &text)
{
    if (text.isEmpty()) {
        ui->newDirectoryLineEdit->setStyleSheet("");
    }
    ui->infoLabel->clear();
}

void FilePlacementWidget::onNewFileName(const QString &text)
{
    if (text.isEmpty()) {
        ui->fileNameLineEdit->setStyleSheet("");
    }
    ui->infoLabel->clear();
}

void FilePlacementWidget::on_resetNewDirectoryNameButton_released()
{
    ui->newDirectoryLineEdit->clear();
}

void FilePlacementWidget::onDirectorySelected(const QModelIndex &index)
{
    ui->selectedDirectoryLineEdit->setText(_model->data(index, QFileSystemModel::FileNameRole).toString());
}

void FilePlacementWidget::on_okButton_released()
{
    if (!checkFileName()) {
        ui->fileNameLineEdit->setStyleSheet("color: red");
        ui->infoLabel->setText(QString(tr("File already exists!")));
        return;
    }

    if (ui->selectedDirectoryLineEdit->text().isEmpty() || ui->fileNameLineEdit->text().isEmpty()) {
        emit fileNameAndPlacementByDefault();
    }
    else {
        QString viewFilePath = ui->selectedDirectoryLineEdit->text() + QString("/") + ui->fileNameLineEdit->text() + QString(".a31");
        QString fullFilePath = registrationsPath() + viewFilePath;
        emit fileNameAndPlacementChanged(viewFilePath, fullFilePath);
    }
    this->hide();
}
