#ifndef HANDLATERALPANELITEM_H
#define HANDLATERALPANELITEM_H

#include <QWidget>

#include "lateralpanelfsm.h"

namespace Ui
{
class HandLateralPanelItem;
}

class HandLateralPanelItem : public QWidget
{
    Q_OBJECT

public:
    explicit HandLateralPanelItem(QWidget* parent = 0, int index = 0);
    ~HandLateralPanelItem();

    void setAngle(int value);

    void reset();

    void select();
    void unselect();
    void alarm();
    void unalarm();

signals:
    void clicked(int index);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

protected:
    LateralPanelFsm _states;

protected slots:
    void setNormalState();
    void setAlarmState();
    void setSelectedState();
    void setAlarmAndSelectedState();

private:
    Ui::HandLateralPanelItem* ui;

    int _index;
};

#endif  // HANDLATERALPANELITEM_H
