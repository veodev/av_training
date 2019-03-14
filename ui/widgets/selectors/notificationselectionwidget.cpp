#include "notificationselectionwidget.h"
#include "ui_notificationselectionwidget.h"

#include "debug.h"
#include "notifications/defaultnotificationmanager.h"
#include "core.h"

NotificationSelectionWidget::NotificationSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _parentWidget(parent)
    , ui(new Ui::NotificationSelectionWidget)
{
    this->hide();
    ui->setupUi(this);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    QPalette bgPal = ui->bgWidget->palette();
    bgPal.setColor(QPalette::Active, QPalette::Background, QColor(120, 120, 130, 120));
    bgPal.setColor(QPalette::Inactive, QPalette::Background, QColor(120, 120, 130, 120));
    ui->bgWidget->setPalette(bgPal);

    QPalette windowPal = ui->windowWidget->palette();
    windowPal.setColor(QPalette::Active, QPalette::Background, QColor(255, 255, 255));
    windowPal.setColor(QPalette::Inactive, QPalette::Background, QColor(255, 255, 255));
    ui->windowWidget->setPalette(windowPal);
    ui->windowWidget->setAttribute(Qt::WA_NoSystemBackground, false);

    _railroadSelector = new GenericSelectionWidget(this);
    ASSERT(connect(_railroadSelector, &GenericSelectionWidget::selected, this, &NotificationSelectionWidget::railroadSelected));
    _predSelector = new GenericSelectionWidget(this);
    ASSERT(connect(_predSelector, &GenericSelectionWidget::selected, this, &NotificationSelectionWidget::predSelected));
    _trackSectionSelector = new GenericSelectionWidget(this);
    ASSERT(connect(_trackSectionSelector, &GenericSelectionWidget::selected, this, &NotificationSelectionWidget::trackSectionSelected));
    _pathSelector = new GenericSelectionWidget(this);
    ASSERT(connect(_pathSelector, &GenericSelectionWidget::selected, this, &NotificationSelectionWidget::pathSelected));

    _notificationManager = new DefaultNotificationManager();

    updateLists();
}

NotificationSelectionWidget::~NotificationSelectionWidget()
{
    delete _notificationManager;
    _notificationManager = nullptr;
    delete ui;
}

void NotificationSelectionWidget::popup()
{
    QSize parentSize = _parentWidget->size();
    setMinimumSize(parentSize);
    setMaximumSize(parentSize);
    show();
}

void NotificationSelectionWidget::setValues(const QString& railroad, const QString& pred, const QString& trackSection, const QString& path)
{
    qDebug() << "Searching for notifications [" << railroad << "," << pred << "," << trackSection << "," << path << "]";
}

void NotificationSelectionWidget::on_readyButton_released()
{
    Core::instance().setNotifications(_notificationManager->getNotifications(_pathId));
    hide();
}

void NotificationSelectionWidget::on_railroadSelectionButton_released()
{
    updateLists();
    _railroadSelector->popup();
}

void NotificationSelectionWidget::on_predSelectionButton_released()
{
    updateLists();
    _predSelector->popup();
}

void NotificationSelectionWidget::on_trackSectionSelectionButton_released()
{
    updateLists();
    _trackSectionSelector->popup();
}

void NotificationSelectionWidget::on_pathSelectionButton_released()
{
    updateLists();
    _pathSelector->popup();
}

void NotificationSelectionWidget::on_exitButton_released()
{
    hide();
}

void NotificationSelectionWidget::railroadSelected(QString value, int rrId)
{
    ui->railroadSelectionButton->setText(value);
    _rrId = rrId;
    updateLists();
}

void NotificationSelectionWidget::predSelected(QString value, int predId)
{
    ui->predSelectionButton->setText(value);
    _predId = predId;
    updateLists();
}

void NotificationSelectionWidget::trackSectionSelected(QString value, int trackSecId)
{
    ui->trackSectionSelectionButton->setText(value);
    _trackSecId = trackSecId;
    updateLists();
}

void NotificationSelectionWidget::pathSelected(QString value, int pathId)
{
    ui->pathSelectionButton->setText(value);
    _pathId = pathId;
    updateLists();
}

void NotificationSelectionWidget::updateLists()
{
    Q_ASSERT(_railroadSelector);
    Q_ASSERT(_predSelector);
    Q_ASSERT(_trackSectionSelector);
    Q_ASSERT(_pathSelector);
    Q_ASSERT(_notificationManager);
    ui->readyButton->setEnabled(false);
    ui->notificationsList->clear();
    ui->notificationsCount->setText(QString::number(0));
    const auto status = _notificationManager->getStatus();
    if (status == NotificationStatus_OK) {
        ui->dbStatusLabel->setText(tr("DB Status: Loaded"));
        const auto& rr = _notificationManager->getRailroads();
        _railroadSelector->setValues(tr("Railroads"), rr);
        ui->railroadSelectionButton->setEnabled(true);
        QString currentRailRoadTextButton = ui->railroadSelectionButton->text();
        if (!ui->railroadSelectionButton->text().isEmpty() && std::find_if(rr.begin(), rr.end(), [currentRailRoadTextButton](const std::pair<QString, int>& rrlam) { return rrlam.first == currentRailRoadTextButton; }) != rr.end()) {
            const auto& preds = _notificationManager->getPreds(_rrId);
            _predSelector->setValues(tr("Pred"), preds);
            ui->predSelectionButton->setEnabled(true);
            QString currentPredSelectionTextButton = ui->predSelectionButton->text();
            if (!ui->predSelectionButton->text().isEmpty() && std::find_if(preds.begin(), preds.end(), [currentPredSelectionTextButton](const std::pair<QString, int>& pred) { return pred.first == currentPredSelectionTextButton; }) != preds.end()) {
                const auto& trackSections = _notificationManager->getTrackSections(_predId);
                _trackSectionSelector->setValues(tr("Track sections"), trackSections);
                ui->trackSectionSelectionButton->setEnabled(true);
                QString currentTrackSelectionTextButton = ui->trackSectionSelectionButton->text();
                if (!ui->trackSectionSelectionButton->text().isEmpty()
                    && std::find_if(trackSections.begin(), trackSections.end(), [currentTrackSelectionTextButton](const std::pair<QString, int>& trackSectionlam) { return trackSectionlam.first == currentTrackSelectionTextButton; }) != trackSections.end()) {
                    const auto& paths = _notificationManager->getPaths(_trackSecId);
                    _pathSelector->setValues(tr("Paths"), paths);
                    ui->pathSelectionButton->setEnabled(true);
                    QString currentPathSelectionTextButton = ui->pathSelectionButton->text();
                    if (!ui->pathSelectionButton->text().isEmpty() && std::find_if(paths.begin(), paths.end(), [currentPathSelectionTextButton](const std::pair<QString, int>& pathlam) { return pathlam.first == currentPathSelectionTextButton; }) != paths.end()) {
                        ui->notificationsList->clear();
                        const auto& values = _notificationManager->getNotifications(_pathId);
                        for (const auto& item : values) {
                            ui->notificationsList->addItem(item.getName());
                        }
                        ui->readyButton->setEnabled(true);
                        ui->notificationsCount->setText(QString::number(values.size()));
                    }
                    else {
                        ui->pathSelectionButton->setText("");
                        ui->notificationsList->clear();
                    }
                }
                else {
                    ui->trackSectionSelectionButton->setText("");
                    ui->pathSelectionButton->setText("");
                    ui->pathSelectionButton->setEnabled(false);
                    ui->notificationsList->clear();
                }
            }
            else {
                ui->predSelectionButton->setText("");
                ui->trackSectionSelectionButton->setText("");
                ui->trackSectionSelectionButton->setEnabled(false);
                ui->pathSelectionButton->setText("");
                ui->pathSelectionButton->setEnabled(false);
                ui->notificationsList->clear();
            }
        }
        else {
            ui->railroadSelectionButton->setText("");
            ui->predSelectionButton->setText("");
            ui->predSelectionButton->setEnabled(false);
            ui->trackSectionSelectionButton->setText("");
            ui->trackSectionSelectionButton->setEnabled(false);
            ui->pathSelectionButton->setText("");
            ui->pathSelectionButton->setEnabled(false);
            ui->notificationsList->clear();
        }
    }
    else {
        ui->railroadSelectionButton->setText("");
        ui->railroadSelectionButton->setEnabled(false);
        ui->predSelectionButton->setText("");
        ui->predSelectionButton->setEnabled(false);
        ui->trackSectionSelectionButton->setText("");
        ui->trackSectionSelectionButton->setEnabled(false);
        ui->pathSelectionButton->setText("");
        ui->pathSelectionButton->setEnabled(false);
        ui->notificationsList->clear();
        if (status == NotificationStatus_NO_DB) {
            ui->dbStatusLabel->setText(tr("DB Status: Not found"));
        }
        else if (status == NotificationStatus_ERROR) {
            ui->dbStatusLabel->setText(tr("DB Status: Error"));
        }
    }
}
