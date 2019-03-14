#ifndef PASSWORDMANAGERPAGE_H
#define PASSWORDMANAGERPAGE_H

#include <QLabel>
#include <QWidget>
#include "coredefinitions.h"

class PopupPasswordForm;
class PasswordManager;

namespace Ui
{
class PasswordManagerPage;
}

class PasswordManagerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordManagerPage(QWidget* parent = 0);
    ~PasswordManagerPage();

private:
    void updatePasswords();
    void fillStars(PasswordManager* manager, DevicePermissions permission, QLabel* label);
    void makePopup(PasswordManager* manager, DevicePermissions permission, const QString& label);

private slots:
    void on_changeFactoryPassword_released();
    void on_resetFactoryPassword_released();
    void on_changeLabPassword_released();
    void on_resetLabPassword_released();
    void on_changeManagerPassword_released();
    void on_resetManagerPassword_released();
    void on_changeEKASUIPassword_released();
    void on_resetEKASUIPassword_released();

    void onNewPassword();

private:
    Ui::PasswordManagerPage* ui;
    PopupPasswordForm* _popupPasswordForm;
};

#endif  // PASSWORDMANAGERPAGE_H
