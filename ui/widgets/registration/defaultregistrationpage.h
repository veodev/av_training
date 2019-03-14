#ifndef DEFAULTREGISTRATIONPAGE_H
#define DEFAULTREGISTRATIONPAGE_H

#include <QWidget>
#include "registrationpage.h"
#include "notificationselectionwidget.h"

namespace Ui
{
class DefaultRegistrationPage;
}

class DefaultRegistrationPage : public RegistrationPage
{
    Q_OBJECT

public:
    explicit DefaultRegistrationPage(QWidget* parent = 0);
    ~DefaultRegistrationPage();

    // RegistrationPage interface
    QString getOperator();
    QStringList getOperatorsList();
    QString getDirectionName();
    QString getTrackNumber();
    int getLeftSide();
    int getControlRail();
    int getDirection();
    QString getLine();
    void setRegistrationState(bool state);
    void setDeviceType(DeviceType type);
    int getControlObject();
    void updateTrackMarks();
    QString getStation();
    QString getFileName();

protected:
    bool event(QEvent* e);
    void setVisible(bool visible);

private slots:
    void on_regServiceOnOffButton_released();
    void on_regOnOffButton_released();
    void on_selectOperator1Button_released();
    void on_selectOperator2Button_released();
    void on_selectOperator3Button_released();
    void on_unselectOperator2PushButton_released();
    void on_unselectOperator3PushButton_released();
    void on_selectLineButton_released();
    void on_directionCodeButton_released();
    void onDirectionSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onControlRailSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onWorkSideSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void on_trackNumberButton_released();
    void onOperatorSelected(const QString& operatorName);
    void onOperatorDeleted(const QString& operatorName);
    void onLineSelected(const QString& line, bool isControlSection);
    void onLineDeleted(const QString& line);
    void onServiceDialogExited(bool status);

    void on_notificationsButton_released();

    void on_defaultFileNameButton_released();

    void on_fileNameButton_released();
    void onFileNameChanged(const QString& viewFilePath, const QString &fullFilePath);
    void onFileNameByDefault();

    void on_correctionButton_toggled(bool checked);

private:
    Ui::DefaultRegistrationPage* ui;
    TrackMarksSelector* _trackMarksSelector;
    NotificationSelectionWidget* _notificationSelectionWidget;
    void changeDirectionCode();
    void setTrackNumber();
    void retranslateRegistrationPage();
    void setupTrackMarks();
    void updateOperatorsList();
};

#endif  // DEFAULTREGISTRATIONPAGE_H
