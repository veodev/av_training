#ifndef EKASUIREGISTRATIONPAGE_H
#define EKASUIREGISTRATIONPAGE_H

#include <QWidget>
#include "registrationpage.h"

class EKASUIDirectionSelectionWidget;
class EKASUITrackNumberSelectionWidget;
class EKASUIStationSelectionWidget;

namespace Ui
{
class EKASUIRegistrationPage;
}

class EKASUIRegistrationPage : public RegistrationPage
{
    Q_OBJECT

    TrackMarksSelector* _trackMarksSelector;

public:
    explicit EKASUIRegistrationPage(QWidget* parent = 0);
    ~EKASUIRegistrationPage();

protected:
    bool event(QEvent* e);

private slots:
    void onControlObjectSpinBoxChanged(int index, const QString& value, const QVariant& userData);
    void onLineDeleted(const QString& line);
    void onLineSelected(const QString& line, bool isControlSection);
    void onOperatorDeleted(const QString& operatorName);
    void onOperatorSelected(const QString& operatorName);
    void on_regServiceOnOffButton_released();
    void on_regOnOffButton_released();
    void on_selectOperator1Button_released();
    void on_selectOperator2Button_released();
    void on_selectOperator3Button_released();
    void on_unselectOperator2PushButton_released();
    void on_unselectOperator3PushButton_released();
    void on_selectLineButton_released();
    void onServiceDialogExited(bool status);
    void on_ekasuiDirectionPushButton_released();
    void on_trackNumberSelectionPushButton_released();
    void on_ekasuiStationPushButton_released();

    void onDirectionSelected(QString direction);
    void onDirectionCancel();

    void onTrackNumberSelected(QString tracknumber);
    void onTrackNumberCancel();

    void onStationSelected(QString station);
    void onStationCancel();

private:
    Ui::EKASUIRegistrationPage* ui;
    EKASUIDirectionSelectionWidget* _directionSelectionWidget;
    EKASUITrackNumberSelectionWidget* _trackNumberSelectionWidget;
    EKASUIStationSelectionWidget* _stationSelectionWidget;

    void switchRegistrationControlObject(int index);

    // RegistrationPage interface
    void setupTrackMarks();
    void retranslateRegistrationPage();
    void updateOperatorsList();

public:
    int getControlRail();
    int getDirection();
    void setRegistrationState(bool state);
    QString getLine();
    void setDeviceType(DeviceType type);
    QString getOperator();
    QStringList getOperatorsList();
    QString getDirectionName();
    QString getTrackNumber();
    int getLeftSide();
    int getControlObject();
    void updateTrackMarks();
    QString getStation();
    QString getFileName();
};

#endif  // EKASUIREGISTRATIONPAGE_H
