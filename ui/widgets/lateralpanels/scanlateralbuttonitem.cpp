#include "scanlateralbuttonitem.h"
#include "debug.h"

ScanLateralButtonItem::ScanLateralButtonItem(int index, LateralPanelType type, QWidget* parent)
    : LateralButtonItem(index, parent)
{
    setType(type);
    ASSERT(connect(this, &ScanLateralButtonItem::pressed, this, &ScanLateralButtonItem::onPressed));
}

ScanLateralButtonItem::~ScanLateralButtonItem()
{
}

void ScanLateralButtonItem::setType(LateralPanelType type)
{
    _type = type;

    if (_type == LeftPanel) {
        setIcon(QPixmap(":/arrow_right_black_22x33.png"));
    }
    else if (_type == RightPanel) {
        setIcon(QPixmap(":/arrow_left_black_22x33.png"));
    }
}

void ScanLateralButtonItem::onPressed()
{
    emit clicked(_index, _type);
}
