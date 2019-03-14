#include "passwordmanager.h"
#include "settings.h"
#include "core.h"

PasswordManager::PasswordManager()
{
    loadPasswords();
}

void PasswordManager::resetAllPasswords()
{
    _passwords[factoryPermissions] = "12345";
    _passwords[labPermissions] = "12345";
    _passwords[managerPermissions] = "12345";
    _passwords[ekasuiPermissions] = "12345";
}

QString PasswordManager::getPasswordForPermission(DevicePermissions permission)
{
    return _passwords[permission];
}

void PasswordManager::setPasswordForPermission(DevicePermissions permission, const QString& password)
{
    _passwords[permission] = password;
    savePasswords();
}

QString PasswordManager::passFromCDU()
{
    const QString& cduSN = cduSerialNumber();
    int snSize = cduSN.size() - 1;
    Q_ASSERT(snSize > 0);
    QString pass;
    for (int i = 0; i <= snSize; ++i) {
        pass[snSize - i] = cduSN[i];
    }
    return pass;
}

void PasswordManager::savePasswords()
{
    saveFactoryPassword(_passwords[factoryPermissions]);
    saveLabPassword(_passwords[labPermissions]);
    saveManagerPassword(_passwords[managerPermissions]);
    saveEKASUIPassword(_passwords[ekasuiPermissions]);
}

void PasswordManager::loadPasswords()
{
    _passwords[factoryPermissions] = passFromCDU();
    _passwords[labPermissions] = restoreLabPassword();
    _passwords[managerPermissions] = restoreManagerPassword();
    _passwords[ekasuiPermissions] = restoreEKASUIPassword();
}
