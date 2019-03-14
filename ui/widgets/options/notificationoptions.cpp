#include "notificationoptions.h"
#include "ui_notificationoptions.h"
#include "settings.h"
#include "debug.h"

NotificationOptions::NotificationOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NotificationOptions)
{
    ui->setupUi(this);
    ui->notificationDistanceSpinBox->setValue(restoreNotificationDistance());
    ASSERT(connect(ui->notificationDistanceSpinBox, &SpinBoxNumber::valueChanged, this, &NotificationOptions::onDistanceChanged));
}

NotificationOptions::~NotificationOptions()
{
    delete ui;
}

void NotificationOptions::onDistanceChanged(qreal value)
{
    qDebug() << "Changed notification distance to" << value;
    saveNotificationDistance(value);
}
