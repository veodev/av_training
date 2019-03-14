#ifndef SCANLATERALBUTTONSVIEW_H
#define SCANLATERALBUTTONSVIEW_H

#include <QWidget>

#include "scanlateralpanel.h"

class QAbstractItemModel;
class ScanLateralButtonItem;
class QVBoxLayout;

class ScanLateralButtonsView : public QWidget
{
    Q_OBJECT

public:
    explicit ScanLateralButtonsView(QWidget* parent = 0);
    ~ScanLateralButtonsView();

    void setType(LateralPanelType type);

    virtual void setModel(QAbstractItemModel* model);
    void blockButtons(bool isBlock);

public slots:
    void detachModel();

signals:
    void clicked(int index, LateralPanelType side);

protected slots:
    virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

protected:
    void setSeparatorVisible(bool value);

private:
    LateralPanelType _type;
    QList<ScanLateralButtonItem*> _items;
    QAbstractItemModel* _model;
    QVBoxLayout* _layout;
    QList<QWidget*> _separators;
};

#endif  // SCANLATERALBUTTONSVIEW_H
