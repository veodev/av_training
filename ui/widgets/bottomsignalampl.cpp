#include "bottomsignalampl.h"

#define MIN_SIGNAL_VALUE 5
#define MIN_HEGHT_COEFF 0.02F

BottomSignalAmplView::BottomSignalAmplView(QWidget* parent)
    : QWidget(parent)
    , _amplValue(0)
{
    resize(20, 100);
    QPalette pal;
    pal.setBrush(this->backgroundRole(), QBrush(Qt::white));
    this->setPalette(pal);
    _rect.setRect(0, 0, this->width(), this->height());
    setAutoFillBackground(true);
    this->setVisible(false);
}

BottomSignalAmplView::~BottomSignalAmplView()
{
}

void BottomSignalAmplView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPalette pal;
    QColor color;
    color.setHsv(_amplValue / 2, 255, 255);
    pal.setBrush(this->backgroundRole(), QBrush(color));
    this->setPalette(pal);
    QPainter painter(this);
    QBrush brush(Qt::white);

    int height = this->height();

    if (_amplValue < MIN_SIGNAL_VALUE)
        _rect.setHeight(height - static_cast<int>(height * MIN_HEGHT_COEFF));
    else
        _rect.setHeight(height - static_cast<int>(static_cast<float>(_amplValue) / 255.0f * height));

    painter.fillRect(_rect, brush);
}

void BottomSignalAmplView::setSignalAmplValue(int value)
{
    _amplValue = value;
}
