#ifndef SCANLATERALPANELITEM_H
#define SCANLATERALPANELITEM_H

#include <QWidget>

#include "scanlateralpaneltapeitem.h"
#include "lateralpanelfsm.h"
#include "scanlateralpanel.h"

class QAbstractItemModel;

namespace Ui
{
class ScanLateralPanelItem;
}

class ScanLateralPanelItem : public QWidget
{
    Q_OBJECT

public:
    explicit ScanLateralPanelItem(int index = 0, QWidget* parent = 0);
    ~ScanLateralPanelItem();

    void reset();

    void hideAngle();
    void hideAngleNote();
    void setAngle(unsigned int value);
    void setAngleNote(const QString& value);

    void setModel(QAbstractItemModel* model);
    void setSide(LateralPanelType side);

    bool select(int tapeIndex);
    void unselect();
    void alarm();
    void unalarm();

signals:
    void clicked(int index);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

protected slots:
    void setNormalState();
    void setAlarmState();
    void setSelectedState();
    void setAlarmAndSelectedState();

private:
    bool _firstSelectedTop;
    int _index;
    LateralPanelFsm _state;
    Ui::ScanLateralPanelItem* ui;
};

#endif  // SCANLATERALPANELITEM_H
