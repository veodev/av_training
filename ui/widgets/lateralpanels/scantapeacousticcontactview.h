#ifndef SCANTAPEACOUSTICCONTACTVIEW_H
#define SCANTAPEACOUSTICCONTACTVIEW_H

#include <QWidget>
#include <QMap>
#include <QSharedPointer>
#include <QElapsedTimer>

#include "scanlateralpanel.h"
#include "Device_definitions.h"

class QVBoxLayout;
class QAbstractItemModel;
class ScanTapeAcousticContactItem;

enum AcousticContactSegment
{
    TopSegment,
    BottomSegment
};

struct Item
{
    int tapeNumber;
    AcousticContactSegment segment;
};

class ScanTapeAcousticContactView : public QWidget
{
    Q_OBJECT

public:
    explicit ScanTapeAcousticContactView(QWidget* parent = 0);
    ~ScanTapeAcousticContactView();

    void setModel(QAbstractItemModel* model);
    void setType(LateralPanelType type);
    void clearView();

public slots:
    void onBScanData(QSharedPointer<tDEV_BScan2Head> head);

private slots:
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void deleteItems();

private:
    void setSeparatorVisible(bool value);
    bool checkNotification();

private:
    QAbstractItemModel* _model;
    QVBoxLayout* _layout;
    LateralPanelType _type;
    QMap<int, Item> _channelsMap;
    std::vector<ScanTapeAcousticContactItem*> _items;
    std::vector<QWidget*> _separators;
};

#endif  // SCANTAPEACOUSTICCONTACTVIEW_H
