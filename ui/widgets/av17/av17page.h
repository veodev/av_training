#ifndef AV17PAGE_H
#define AV17PAGE_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>
#include <qwt_painter.h>
#include <QModelIndex>

#include "Device_definitions.h"

#include "av17plot.h"

namespace Ui
{
class Av17Page;
}

class Av17Page : public QWidget
{
    Q_OBJECT

public:
    explicit Av17Page(QWidget* parent = 0);
    ~Av17Page();

protected:
    bool event(QEvent* e);
    void retranslateUi();

signals:
    void doStartAv17DefView();
    void doXDefOffset(int);
    void doLoadBScan0Chan();

public slots:
    void setVisible(bool visible);
    void CountCenterOfDefect(QSharedPointer<tDEV_BScan2Head> head);

private slots:
    void on_ResetButton_released();
    void on_MinButton_released();
    void on_MaxButton_released();
    void on_FinishButton_released();
    void onBScanData(QSharedPointer<tDEV_BScan2Head>);
    void onPathStepData(QSharedPointer<tDEV_PathStepData>);
    void RedrawOnTimer();

private:
    Ui::Av17Page* ui;
    int XSys;
    int YSys;
    unsigned int XIndex;
    unsigned int YIndex;
    int _xDefOffset;
    QTimer _timer;
    unsigned long _startTime;
    QVector<QPointF> points1;
    QVector<QPointF> points2;
    QVector<QPointF> points3;
    QAbstractItemModel* _headScanChannelsModel;
    void StartScan();
    void AutoTuning(bool isEnable);


public:
    int yindex();
    int ysys();
    void setModel(QAbstractItemModel* headScanChannelsModel);
    void setXDefOffset(int x)
    {
        _xDefOffset = x;
    }
};

class Av17ScanProgress : public QWidget
{
    //    Q_OBJECT

public:
    explicit Av17ScanProgress(QWidget* parent = 0);
    ~Av17ScanProgress();
    void paintEvent(QPaintEvent* event);
    void setYsysParentPtr(int* value);
    void setYindexParentPtr(unsigned int* value);

private:
    int Border;
    QRect MMRect;
    QPoint Pt1, Pt2;
    int* ysysParentPtr;
    unsigned int* yindexParentPtr;

    QPoint MMtoPoint(float mmX, float mmY);
    int yindex();
    int ysys();
};

#endif  // AV17PAGE_H
