#ifndef SCANLATERALBUTTONITEM_H
#define SCANLATERALBUTTONITEM_H

#include "lateralbuttonitem.h"
#include "scanlateralpanel.h"

class ScanLateralButtonItem : public LateralButtonItem
{
    Q_OBJECT

public:
    ScanLateralButtonItem(int index = 0, LateralPanelType type = LeftPanel, QWidget* parent = 0);
    ~ScanLateralButtonItem();

    void setType(LateralPanelType type);

signals:
    void clicked(int index, LateralPanelType side);

protected slots:
    void onPressed();

protected:
    LateralPanelType _type;
};

#endif  // SCANLATERALBUTTONITEM_H
