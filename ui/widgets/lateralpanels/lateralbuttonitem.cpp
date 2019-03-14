#include "lateralbuttonitem.h"
#include "debug.h"

LateralButtonItem::LateralButtonItem(int index, QWidget* parent)
    : QPushButton(parent)
    , _index(index)
{
    ASSERT(connect(this, &QPushButton::pressed, this, &LateralButtonItem::onPressed));
    setIcon(QPixmap(":/arrow_right_black_22x33.png"));
}

LateralButtonItem::~LateralButtonItem() {}

void LateralButtonItem::onPressed()
{
    emit clicked(_index);
}
