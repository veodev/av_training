#ifndef MSCANPAGE_H
#define MSCANPAGE_H

#include <QWidget>

#include "Definitions.h"
#include "Device_definitions.h"

namespace Ui
{
class MScanPage;
}

class QAbstractItemModel;

class MScanPage : public QWidget
{
    Q_OBJECT

public:
    explicit MScanPage(QWidget* parent = 0);
    ~MScanPage();

    void startDrawing();
    void stopDrawing();

    void setDotSize(int value);

public slots:
    void setVisible(bool visible);

private slots:
    void onSchemeChanged(int index);
    void onBScan2Data(QSharedPointer<tDEV_BScan2Head> head);
    void onAlarmData(QSharedPointer<tDEV_AlarmHead> head);

private:
    void setScheme(int index);
    QString createId(QAbstractItemModel* model, const QModelIndex& index, eDeviceSide side);

private:
    Ui::MScanPage* ui;
    int _scheme;
    bool _isEntered;
};

#endif  // MSCANPAGE_H
