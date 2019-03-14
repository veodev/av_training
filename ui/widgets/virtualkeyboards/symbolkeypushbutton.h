#ifndef SYMBOLKEYPUSHBUTTON_H
#define SYMBOLKEYPUSHBUTTON_H

#include "keypushbutton.h"
#include "virtualkeyboardbase.h"
class QWidget;

class SymbolKeyPushButton : public KeyPushButton
{
    Q_OBJECT

public:
    explicit SymbolKeyPushButton(QWidget* parent = 0);

private slots:
    void getKeyPress();
};

#endif  // SYMBOLKEYPUSHBUTTON_H
