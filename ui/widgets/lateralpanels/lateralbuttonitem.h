#ifndef LATERALBUTTONITEM_H
#define LATERALBUTTONITEM_H

#include <QPushButton>

class LateralButtonItem : public QPushButton
{
    Q_OBJECT

public:
    LateralButtonItem(int index = 0, QWidget* parent = nullptr);
    ~LateralButtonItem();

signals:
    void clicked(int index);

protected slots:
    void onPressed();

protected:
    int _index;
};

#endif  // LATERALBUTTONITEM_H
