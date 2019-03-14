#ifndef SCANLATERALPANELTAPEITEM_H
#define SCANLATERALPANELTAPEITEM_H

#include <QWidget>
#include <QModelIndexList>
#include <QTimer>
#include <QResizeEvent>

#include "lateralpanelfsm.h"
#include "scanlateralpanel.h"
#include "scanlateralpanelview.h"

class QAbstractItemModel;

namespace Ui
{
class ScanLateralPanelTapeItem;
}

class ScanLateralPanelTapeItem : public QWidget
{
    Q_OBJECT

public:
    enum ItemPosition
    {
        topItem = 0,
        bottomItem = 1
    };

    explicit ScanLateralPanelTapeItem(QWidget* parent = 0);
    ~ScanLateralPanelTapeItem();

    void reset();

    void setTopNote(const QString& value);
    void setBottomNote(const QString& value);

    void setSens(int value);
    void setTopSens(int value);
    void setTopSensVisible(bool visible = true);
    bool isTopSensSetted() const;
    void setBottomSens(int value);
    void setBottomSensVisible(bool visible = true);
    bool isBottomSensSetted() const;

    void clearSelection();
    QModelIndexList selectedRows() const;

    void select(int tapeIndex, ItemPosition index = topItem);
    void unselect(ItemPosition index = topItem);

    virtual void setModel(QAbstractItemModel* model);
    void setSide(LateralPanelType side);
    void setScanLateralViewPointer(QObject* obj);

protected:
    void alarm(ItemPosition index = topItem);
    void unalarm(ItemPosition index = topItem);

    void sensInRange(ItemPosition index = topItem);
    void sensOutOfRange(ItemPosition index = topItem);
    void resizeEvent(QResizeEvent* event);

protected:
    LateralPanelFsm _states[2];

protected slots:
    virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

    void setTopNormalState();
    void setBottomNormalState();
    void setTopAlarmState();
    void setBottomAlarmState();
    void setTopSelectedState();
    void setBottomSelectedState();
    void setTopAlarmAndSelectedState();
    void setBottomAlarmAndSelectedState();

    void setTopNormalHideBlinkState();
    void setBottomNormalHideBlinkState();
    void setTopAlarmHideBlinkState();
    void setBottomAlarmHideBlinkState();
    void setTopSelectedHideBlinkState();
    void setBottomSelectedHideBlinkState();
    void setTopAlarmAndSelectedHideBlinkState();
    void setBottomAlarmAndSelectedHideBlinkState();

    void modelDestroyed();
    void stopBlinkTopSens();
    void stopBlinkBottomSens();
    void onBlinkingTopSens(bool isGlow);
    void onBlinkingBottomSens(bool isGlow);
    void onBlink(bool isGlow);

private:
    void updateCell(const QModelIndex& index, int role);
    void generateTopPixmaps(int width, int height);
    void generateBottomPixmaps(int width, int height);
    void updateViewItem();
    void drawPixmap(QPixmap* pix, const QColor& penColor, const QColor& brushColor, const QString& text);

private:
    Ui::ScanLateralPanelTapeItem* ui;

    LateralPanelType _side;
    bool _isTopSensSeted;
    bool _isBottomSensSeted;
    QString _prevTopSens;
    QString _prevBottomSens;
    QAbstractItemModel* _model;
    bool _isTopSensBlinking;
    bool _isBottomSensBlinking;

    QPixmap* _topNormalPix;
    QPixmap* _topSelectedPix;
    QPixmap* _topAlarmedPix;
    QPixmap* _topSelectedAlarmedPix;

    QPixmap* _topNormalHideBlinkPix;
    QPixmap* _topSelectedHideBlinkPix;
    QPixmap* _topAlarmedHideBlinkPix;
    QPixmap* _topSelectedAlarmedHideBlinkPix;

    QPixmap* _bottomNormalPix;
    QPixmap* _bottomSelectedPix;
    QPixmap* _bottomAlarmedPix;
    QPixmap* _bottomSelectedAlarmedPix;

    QPixmap* _bottomNormalHideBlinkPix;
    QPixmap* _bottomSelectedHideBlinkPix;
    QPixmap* _bottomAlarmedHideBlinkPix;
    QPixmap* _bottomSelectedAlarmedHideBlinkPix;
};

#endif  // SCANLATERALPANELTAPEITEM_H
