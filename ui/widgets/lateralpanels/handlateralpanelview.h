#ifndef HANDLATERALPANELVIEW_H
#define HANDLATERALPANELVIEW_H

#include <QWidget>

#include "handlateralpanelitem.h"

namespace Ui
{
class HandLateralPanelView;
}

class QAbstractItemModel;

class HandLateralButtonsView;

class HandLateralPanelView : public QWidget
{
    Q_OBJECT

public:
    explicit HandLateralPanelView(QWidget* parent = 0);
    ~HandLateralPanelView();

    bool select(int index);
    bool selectHeadScan(int index);
    void unselect(int index);
    void clearSelection();

    virtual void setModel(QAbstractItemModel* model);

    void setDependedButtonsView(HandLateralButtonsView* dependedButtonsView);
    void blockPanel(bool isBlock);

public slots:
    void setVisible(bool visible);

signals:
    void clicked(int index);

protected slots:
    virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void modelAboutToBeReset();

private:
    void reset();

private:
    Ui::HandLateralPanelView* ui;
    QList<HandLateralPanelItem*> _items;
    QAbstractItemModel* _model;
    HandLateralButtonsView* _dependedButtonsView;
    int _curentSelectionIndex;
};

#endif  // HANDLATERALPANELVIEW_H
