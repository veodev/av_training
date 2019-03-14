#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <vector>
#include <QString>
#include "coredefinitions.h"

class Notification
{
    int _id;
    int _path_id;
    int _rail;
    int _km;
    int _pk;
    int _metre;
    QString _def;
    QString _note;
    QString _author;
    QString _datetime;

public:
    Notification(int id, int path_id, int rail, int km, int pk, int metre, const QString& def, const QString& note, const QString& author, const QString& datetime)
        : _id(id)
        , _path_id(path_id)
        , _rail(rail)
        , _km(km)
        , _pk(pk)
        , _metre(metre)
        , _def(def)
        , _note(note)
        , _author(author)
        , _datetime(datetime)
    {
    }
    QString getName() const
    {
        return QString("[") + QString::number(_id) + QString("] (") + QString::number(_km) + "," + QString::number(_pk) + "," + QString::number(_metre) + ") {" + _def + "} <" + _note + ">";
    }
    int getId() const
    {
        return _id;
    }
    int getPath_id() const
    {
        return _path_id;
    }
    int getRail() const
    {
        return _rail;
    }
    int getKm() const
    {
        return _km;
    }
    int getPk() const
    {
        return _pk;
    }
    int getMetre() const
    {
        return _metre;
    }
    QString getDef() const
    {
        return _def;
    }
    QString getNote() const
    {
        return _note;
    }
    QString getAuthor() const
    {
        return _author;
    }
    QString getDatetime() const
    {
        return _datetime;
    }
};

class NotificationManager
{
public:
    NotificationManager(){};
    virtual ~NotificationManager(){};
    virtual NotificationStatus getStatus() = 0;
    virtual std::vector<std::pair<QString, int>> getRailroads() = 0;
    virtual std::vector<std::pair<QString, int>> getPreds(int railroad) = 0;
    virtual std::vector<std::pair<QString, int>> getTrackSections(int pred) = 0;
    virtual std::vector<std::pair<QString, int>> getPaths(int trackSection) = 0;
    virtual std::vector<Notification> getNotifications(int path) = 0;
    //    virtual QString getRailroadsByid();
    //    virtual QString getPredsByid();
    //    virtual QString getTrackSectionsByid();
    //    virtual QString getPathsByid();
};

#endif  // NOTIFICATIONMANAGER_H
