#ifndef NOTIFICATIONOPTIONS_H
#define NOTIFICATIONOPTIONS_H

#include <QWidget>

namespace Ui
{
class NotificationOptions;
}

class NotificationOptions : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationOptions(QWidget* parent = 0);
    ~NotificationOptions();
private slots:
    void onDistanceChanged(qreal value);

private:
    Ui::NotificationOptions* ui;
};

#endif  // NOTIFICATIONOPTIONS_H
