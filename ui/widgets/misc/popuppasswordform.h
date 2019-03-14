#ifndef POPUPPASSWORDFORM_H
#define POPUPPASSWORDFORM_H

#include <QWidget>
#include "coredefinitions.h"

namespace Ui
{
class PopupPasswordForm;
}

class PopupPasswordForm : public QWidget
{
    Q_OBJECT

public:
    explicit PopupPasswordForm(QWidget* parent = 0);
    ~PopupPasswordForm();
    void popup();

    void setCurrentTruePassword(DevicePermissions permission, const QString& currentTruePassword);
    DevicePermissions currentPermission() const;

    void setPasswordMode(const QString& modeName, bool isNew);

    QString newPassword() const;

private slots:
    void on_oneButton_released();
    void on_twoButton_released();
    void on_threeButton_released();
    void on_fourButton_released();
    void on_fiveButton_released();
    void on_sixButton_released();
    void on_sevenButton_released();
    void on_eightButton_released();
    void on_nineButton_released();
    void on_clearButton_released();
    void on_zeroButton_released();
    void on_enterButton_released();
    void on_exitButton_released();

signals:
    void passwordEntered();
    void exitPressed();

private:
    Ui::PopupPasswordForm* ui;
    QWidget* parentWidget;
    bool _mode;
    bool _changePasswordMode;
    QString _currentTruePassword;
    QString _newPassword;
    DevicePermissions _currentPermission;
};

#endif  // POPUPPASSWORDFORM_H
