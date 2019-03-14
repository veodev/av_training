#ifndef HANDLATERALBUTTONSVIEW_H
#define HANDLATERALBUTTONSVIEW_H

#include <QWidget>

#include "scanlateralpanel.h"

class QAbstractItemModel;
class LateralButtonItem;
class QVBoxLayout;

class HandLateralButtonsView : public QWidget
{
    Q_OBJECT

public:
    explicit HandLateralButtonsView(QWidget* parent = 0);
    ~HandLateralButtonsView();

    virtual void setModel(QAbstractItemModel* model);
    void blockButtons(bool isBlock);

public slots:
    void detachModel();

signals:
    void clicked(int index);

private:
    QList<LateralButtonItem*> _items;
    QAbstractItemModel* _model;
    QVBoxLayout* _layout;
#if defined TARGET_AVICON31  || defined TARGET_AVICON31E
    QList<QWidget*> _emptyWidgets;
#endif
};

#endif  // HANDLATERALBUTTONSVIEW_H
