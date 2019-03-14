#ifndef TABLESETTINGSVIEW_H
#define TABLESETTINGSVIEW_H

#include <QWidget>
#include <QMap>

#include "tablesettingsitem.h"

namespace Ui
{
class TableSettingsView;
}

enum ControledRail
{
    None = -1,
    Single,
    Both
};

class TableSettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit TableSettingsView(QWidget* parent = 0);
    ~TableSettingsView();

    void setModel(QAbstractItemModel* model);
    void deleteItems();
    void setItemLeftRailParams(TableSettingsItem* item, QModelIndex& index);
    void setItemRightRailParams(TableSettingsItem* item, QModelIndex& index);
    void setItemRailParams(TableSettingsItem* item, QModelIndex& index);
    void correctChannelName(TableSettingsItem* item, QModelIndex& index, bool isBoth = true);
    void setContoledRail(ControledRail value);

protected:
    bool event(QEvent* e);

protected slots:
    virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
    Ui::TableSettingsView* ui;

    QAbstractItemModel* _model;
    QMap<int, int> _channelsMap;
    ControledRail _controledRail;
};

#endif  // TABLESETTINGSVIEW_H
