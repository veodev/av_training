#ifndef DEFAULTNOTIFICATIONMANAGER_H
#define DEFAULTNOTIFICATIONMANAGER_H

#include "notificationmanager.h"
#include <QtSql>

class DefaultNotificationManager : public NotificationManager
{
    NotificationStatus _status;

public:
    DefaultNotificationManager();
    ~DefaultNotificationManager();
    QSqlDatabase sdb;
    std::vector<std::pair<QString, int>> getRailroads();
    std::vector<std::pair<QString, int>> getPreds(int railroad);
    std::vector<std::pair<QString, int>> getTrackSections(int pred);
    std::vector<std::pair<QString, int>> getPaths(int trackSection);
    std::vector<Notification> getNotifications(int path);
    NotificationStatus getStatus();
};

#endif  // DEFAULTNOTIFICATIONMANAGER_H
