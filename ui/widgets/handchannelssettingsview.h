#ifndef HANDCHANNELSSETTINGSVIEW_H
#define HANDCHANNELSSETTINGSVIEW_H

#include <QWidget>
#include "modeltypes.h"

class QAbstractItemModel;
class HandChannelsSettingsItem;

namespace Ui
{
class HandChannelsSettingsView;
}

class HandChannelsSettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit HandChannelsSettingsView(QWidget* parent = nullptr);
    ~HandChannelsSettingsView();

    void setModel(QAbstractItemModel* model);
    void deleteItems();
    void setChannelsParams(HandChannelsSettingsItem* item, const QModelIndex& index);

protected:
    bool event(QEvent* e);

protected slots:
    virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
    Ui::HandChannelsSettingsView* ui;
    QAbstractItemModel* _model;
};

#endif  // HANDCHANNELSSETTINGSVIEW_H
