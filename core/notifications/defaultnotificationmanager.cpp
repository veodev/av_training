#include "defaultnotificationmanager.h"

#include "appdatapath.h"

DefaultNotificationManager::DefaultNotificationManager()
    : NotificationManager()
{
    sdb = QSqlDatabase::addDatabase("QSQLITE");
    QFileInfo dbInfo(marksPath() + "marks.sqlite");
    if (dbInfo.exists()) {
        sdb.setDatabaseName(marksPath() + "marks.sqlite");
        _status = NotificationStatus_OK;
    }
    else {
        _status = NotificationStatus_NO_DB;
    }
}

DefaultNotificationManager::~DefaultNotificationManager() {}

std::vector<std::pair<QString, int>> DefaultNotificationManager::getRailroads()
{
    std::vector<std::pair<QString, int>> temp;
    if (!sdb.open()) {
        qDebug() << sdb.lastError().text();
        _status = NotificationStatus_ERROR;
        return temp;
    }

    QSqlQuery query("SELECT id, Name FROM RailRoads");

    while (query.next()) {
        std::pair<QString, int> railR;
        railR.first = query.value(1).toString();
        railR.second = query.value(0).toInt();
        temp.push_back(railR);
    }

    sdb.close();

    return temp;
}

std::vector<std::pair<QString, int>> DefaultNotificationManager::getPreds(int railroad)
{
    std::vector<std::pair<QString, int>> temp;
    if (!sdb.open()) {
        qDebug() << sdb.lastError().text();
        _status = NotificationStatus_ERROR;
        return temp;
    }

    QSqlQuery query("SELECT id, Name FROM Pred WHERE RailRoad_id = " + QString::number(railroad));

    while (query.next()) {
        std::pair<QString, int> preds;
        preds.first = query.value(1).toString();
        preds.second = query.value(0).toInt();
        temp.push_back(preds);
    }

    sdb.close();

    return temp;
}

std::vector<std::pair<QString, int>> DefaultNotificationManager::getTrackSections(int pred)
{
    std::vector<std::pair<QString, int>> temp;
    if (!sdb.open()) {
        qDebug() << sdb.lastError().text();
        _status = NotificationStatus_ERROR;
        return temp;
    }

    QSqlQuery query("SELECT id, Name FROM TrackSections WHERE Pred_id = " + QString::number(pred));

    while (query.next()) {
        std::pair<QString, int> trackS;
        trackS.first = query.value(1).toString();
        trackS.second = query.value(0).toInt();
        temp.push_back(trackS);
    }

    sdb.close();
    return temp;
}

std::vector<std::pair<QString, int>> DefaultNotificationManager::getPaths(int trackSection)
{
    std::vector<std::pair<QString, int>> temp;
    if (!sdb.open()) {
        qDebug() << sdb.lastError().text();
        _status = NotificationStatus_ERROR;
        return temp;
    }

    QSqlQuery query("SELECT id, Name FROM Paths WHERE TrackSection_id = " + QString::number(trackSection));

    while (query.next()) {
        std::pair<QString, int> pairPath;
        pairPath.first = query.value(1).toString();
        pairPath.second = query.value(0).toInt();
        temp.push_back(pairPath);
    }

    sdb.close();
    return temp;
}

std::vector<Notification> DefaultNotificationManager::getNotifications(int path)
{
    std::vector<Notification> temp;
    if (!sdb.open()) {
        qDebug() << sdb.lastError().text();
        _status = NotificationStatus_ERROR;
        return temp;
    }

    QSqlQuery query("SELECT * FROM Marks WHERE Path_id = " + QString::number(path));

    while (query.next()) {
        temp.emplace_back(query.value(0).toInt(), query.value(1).toInt(), query.value(2).toInt(), query.value(3).toInt(), query.value(4).toInt(), query.value(5).toInt(), query.value(6).toString(), query.value(7).toString(), query.value(8).toString(), query.value(9).toString());
    }

    sdb.close();
    return temp;
}

NotificationStatus DefaultNotificationManager::getStatus()
{
    return _status;
}
