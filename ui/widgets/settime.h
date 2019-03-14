#ifndef SETTIME_H
#define SETTIME_H

#include <QWidget>
#include <QTime>

#include <QGeoPositionInfo>
#include <QGeoSatelliteInfo>

#if defined TARGET_AVICON31
#include "geoposition.h"
#endif

namespace Ui
{
class SetTime;
}

class SetTime : public QWidget
{
    Q_OBJECT

public:
    explicit SetTime(QWidget* parent = 0);
    ~SetTime();

    void configureUiForAviconDbAviconDbHs();
    void configureUiForAvicon15();

signals:
    void leaveTheWidget();

public slots:
    void setVisible(bool visible);
    void onUpdateTimeAndCoordinate(const QGeoPositionInfo& info);

protected:
    bool event(QEvent* e);

private:
    bool runProcess(const QString& program);
    void setHours(int value);
    void setMinutes(int value);
    void setSeconds(int value);
    void setClock();
    int convertToCurrentHours(const int& timeOffset, const int& hours);

private slots:
    void on_okButton_released();
    void on_setHoursDownPushButton_released();
    void on_setHoursUpPushButton_released();
    void on_setMinutesDownPushButton_released();
    void on_setMinutesUpPushButton_released();
    void on_setSecondsDownPushButton_released();
    void on_setSecondsUpPushButton_released();
    void on_updateWithAutoTimeZoneButton_released();
    void on_updateWithManualTimeZoneButton_released();
    void onOffsetSpinboxValueChanged(qreal value);

private:
    Ui::SetTime* ui;
#if defined TARGET_AVICON31
    GeoPosition* _geoPosition;
#endif
    QTime _time;
    QTime _timeFromSatellite;
    int _timeOffsetManual;
    int _timeOffsetAuto;
};

#endif  // SETTIME_H
