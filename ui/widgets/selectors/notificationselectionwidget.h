#ifndef NOTIFICATIONSELECTIONWIDGET_H
#define NOTIFICATIONSELECTIONWIDGET_H

#include <QWidget>
#include "genericselectionwidget.h"
#include "notifications/notificationmanager.h"

namespace Ui
{
class NotificationSelectionWidget;
}

class NotificationSelectionWidget : public QWidget
{
    Q_OBJECT

    QWidget* _parentWidget;
    GenericSelectionWidget* _railroadSelector;
    GenericSelectionWidget* _predSelector;
    GenericSelectionWidget* _trackSectionSelector;
    GenericSelectionWidget* _pathSelector;
    NotificationManager* _notificationManager;

public:
    explicit NotificationSelectionWidget(QWidget* parent = 0);
    ~NotificationSelectionWidget();
    void popup();
    void setValues(const QString& railroad, const QString& pred, const QString& trackSection, const QString& path);

private slots:
    void on_readyButton_released();
    void on_railroadSelectionButton_released();
    void on_predSelectionButton_released();
    void on_trackSectionSelectionButton_released();
    void on_pathSelectionButton_released();
    void on_exitButton_released();
    void railroadSelected(QString value, int rrId);
    void predSelected(QString value, int predId);
    void trackSectionSelected(QString value, int trackSecId);
    void pathSelected(QString value, int pathId);

private:
    void updateLists();
    Ui::NotificationSelectionWidget* ui;
    int _rrId;
    int _predId;
    int _trackSecId;
    int _pathId;
};

#endif  // NOTIFICATIONSELECTIONWIDGET_H
