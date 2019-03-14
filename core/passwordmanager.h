#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include "coredefinitions.h"
#include <QString>
#include <map>

class PasswordManager
{
    std::map<DevicePermissions, QString> _passwords;

public:
    PasswordManager();
    void resetAllPasswords();
    QString getPasswordForPermission(DevicePermissions permission);
    void setPasswordForPermission(DevicePermissions permission, const QString& password);
    QString passFromCDU();

private:
    void savePasswords();
    void loadPasswords();
};

#endif  // PASSWORDMANAGER_H
