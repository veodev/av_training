#ifndef PERMISSIONSPAGE_H
#define PERMISSIONSPAGE_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "popuppasswordform.h"


namespace Ui
{
class PermissionsPage;
}

class PermissionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PermissionsPage(QWidget* parent = 0);
    ~PermissionsPage();
    void setDeviceType(DeviceType type);

private:
    void updatePermissions();
    void configureLabel(bool active, QLabel* label, QPushButton* button);
    void spawnPasswordPopup(DevicePermissions permission);

private slots:
    void on_configurePasswordsButton_released();
    void on_factoryPermissionButton_released();
    void on_labPermissionButton_released();
    void on_managerPermissionButton_released();
    void on_ekasuiPermissionButton_released();
    void onPopupPasswordEntered();
signals:
    void configurePasswordsPressed();

private:
    Ui::PermissionsPage* ui;
    PopupPasswordForm* _popupPasswordForm;
};

#endif  // PERMISSIONSPAGE_H
