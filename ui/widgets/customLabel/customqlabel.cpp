#include "customqlabel.h"
#include <QDebug>

CustomQLabel::CustomQLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent)
{
}

CustomQLabel::~CustomQLabel() {}

void CustomQLabel::mousePressEvent(QMouseEvent* me)
{
    Q_UNUSED(me);
    qDebug() << "PRESSED";
    emit clicked();
}
