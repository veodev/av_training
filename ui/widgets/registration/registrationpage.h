#ifndef REGISTRATIONPAGE_H
#define REGISTRATIONPAGE_H

#include <QWidget>

#include "fileplacementwidget.h"
#include "operatorselectionwidget.h"
#include "lineselectionwidget.h"
#include "coredefinitions.h"

#include "trackmarks/trackmarksselector.h"

class RegistrationPage : public QWidget
{
    Q_OBJECT
protected:
    OperatorSelectionWidget* _operatorSelectionWidget;
    LineSelectionWidget* _lineSelectionWidget;
    FilePlacementWidget* _filePlacementWidget;

    int _selectOperatorNumber;
    bool _isRegistrationStarted;
    QString _fileName;

public:
    explicit RegistrationPage(QWidget* parent = Q_NULLPTR);

    virtual int getControlRail() = 0;
    virtual int getDirection() = 0;
    virtual void setRegistrationState(bool state) = 0;
    virtual QString getLine() = 0;
    virtual void setDeviceType(DeviceType type) = 0;
    virtual QString getOperator() = 0;
    virtual QStringList getOperatorsList() = 0;
    virtual QString getDirectionName() = 0;
    virtual QString getTrackNumber() = 0;
    virtual int getLeftSide() = 0;
    virtual int getControlObject() = 0;
    virtual void updateTrackMarks() = 0;
    virtual QString getStation() = 0;
    virtual QString getFileName() = 0;

protected:
    virtual bool event(QEvent* e) = 0;

signals:
    void startStopRegistration();
    void correctRegistration();
    void startStopService();
    void controlRailChanged(int index, const QString& value, const QVariant& userData);
    void workSideChanged(int index, const QString& value, const QVariant& userData);
    void operatorsListChanged(QStringList);
    void activeOperatorChanged(QString);

public slots:
};

#endif  // REGISTRATIONPAGE_H
