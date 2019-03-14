#ifndef SCANLATERALPANELVIEW_H
#define SCANLATERALPANELVIEW_H

#include <QWidget>

#include "scanlateralpaneltapeitem.h"
#include "scanlateralpanel.h"

class QAbstractItemModel;
class QVBoxLayout;

class ScanLateralPanelItem;
class ScanLateralButtonsView;

class ScanLateralPanelView : public QWidget
{
    Q_OBJECT

public:
    explicit ScanLateralPanelView(QWidget* parent = nullptr);
    ~ScanLateralPanelView();

    void setType(LateralPanelType type);

    bool select(int index);
    void unselect(int index);
    void clearSelection();
    void alarm(int index);
    void unalarm(int index);

    virtual void setModel(QAbstractItemModel* model);

    void setDependedPanel(ScanLateralPanelView* panel);
    void setDependedButtonsView(ScanLateralButtonsView* dependedButtonsView);
    void blockButtons(bool isBlock);

public slots:
    void setVisible(bool visible);
    void detachModel();
    void onBlinkingTimerTimeout();

signals:
    void clicked(int index, LateralPanelType side);
    void blinkingTimerTimeOut(bool isGlow);

protected:
    void setSeparatorVisible(bool value);

protected slots:
    virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void modelAboutToBeReset();

private slots:
    void itemClicked(int index);

private:
    QVBoxLayout* _layout;
    LateralPanelType _type;
    QList<ScanLateralPanelItem*> _items;
    QList<QWidget*> _separators;
    QAbstractItemModel* _model;
    ScanLateralPanelView* _dependedPanel;
    ScanLateralButtonsView* _dependedButtonsView;
    int _curentSelectionIndex;
    QTimer _blinkingTimer;
    bool _isGlow;
};

#endif  // SCANLATERALPANELVIEW_H
