#ifndef AV17DEFVIEW_H
#define AV17DEFVIEW_H

#include <QWidget>

#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_painter.h>
#include <QPainter>
#include <qwt_text.h>
#include <qwt_text_label.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_item.h>
#include <qwt_pixel_matrix.h>

#include "spinboxnumber.h"
#include "spinboxlist.h"
#include <QPushButton>
#include <QPixmap>
#include <QModelIndex>

#include "av17projections.h"
#include "dc_definitions.h"

class QScrollBar;

namespace Ui
{
class Av17DefView;
}

class DefItem : public QwtPlotItem
{
public:
    DefItem();
    ~DefItem();
    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }
    void Init();
    virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;

private:
    void DrawFlaw(QPainter* painter) const;

    TLevelRecons* FRecon;
    T3DRecons* R3D;

    int _max_h, _max_x, _max_l;
    int _min_h, _min_x, _min_l;
};

class DefItemLH : public QwtPlotItem
{
public:
    DefItemLH();
    ~DefItemLH() {}
    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }
    virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
    void Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl);

private:
    T3DRecons* _R3D;

    int _max_h, _max_x, _max_l;
    int _min_h, _min_x, _min_l;
};

class DefItemLW : public QwtPlotItem
{
public:
    DefItemLW();
    ~DefItemLW() {}
    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }
    virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
    void Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl);

private:
    T3DRecons* _R3D;

    int _max_h, _max_x, _max_l;
    int _min_h, _min_x, _min_l;
};

class DefItemWH : public QwtPlotItem
{
public:
    DefItemWH();
    ~DefItemWH() {}
    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }
    virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
    void Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl);

private:
    T3DRecons* _R3D;

    int _max_h, _max_x, _max_l;
    int _min_h, _min_x, _min_l;
};

typedef std::vector<tDaCo_BScanSignals> tSignalsArray;

class DefItemBScan : public QWidget
{
    Q_OBJECT
public:
    DefItemBScan(QWidget* parent = 0);
    ~DefItemBScan();

    void paintEvent(QPaintEvent* event);
    void setYRange(int startBscangate, int endBscangate)
    {
        _minY = startBscangate;
        _maxY = endBscangate;
    }
    void setMSMStrob(int startGate, int endGate)
    {
        _startGateMSM = startGate;
        _endGateMSM = endGate;
    }

    void clearSignals();
    void setStartB(int startb)
    {
        _startB = startb;
    }
    void setEndB(int endb)
    {
        _endB = endb;
    }
    void setBRange(int startb, int endb);
    void setWorkRail(eDeviceSide workrail)
    {
        _workrail = workrail;
    }
    void setThreshold(int value);
    void onSpan(const BScanDisplaySpan& span);
public slots:
    void onDataContainerData(pDaCo_DateFileItem);

private:
    int Border;
    QRect MMRect;
    QPoint Pt1, Pt2;
    int _maxY, _minY;
    int _startB, _endB;
    int _startGateMSM, _endGateMSM;
    unsigned char _dotSize;
    int _sigSize;
    double _scanStep;  // Horizontal scan step
    int _threshold;
    tSignalsArray _sigarr;
    eDeviceSide _workrail;
    bool _isProbePulseFromAbove;
};

class Av17DefView : public QWidget
{
    Q_OBJECT

public:
    explicit Av17DefView(QWidget* parent = 0);
    ~Av17DefView();

    bool controlsAreaVisible();
    void setControlsAreaVisible(bool isVisible);
    void setModel(QAbstractItemModel* channelsModel);

protected:
    bool event(QEvent* e);

private:
    void CalculateSizes();
    void retranslateUi();

    enum RailProjections
    {
        projectionVertical = 0,
        projectionHorizontal = 1,
        projectionCrossSection = 2
    };
    void setVisible(bool visible);
    bool InitDef(int threshold);
    void replotProjections();

signals:
    void doNextButtonPressed();

public slots:
    void loadBScan0Chan();

private slots:
    void CutOffStartValueChanged(qreal value);
    void CutOffEndValueChanged(qreal value);
    void ThresholdValueChanged(qreal value);
    void ThresholdBScanChanged(qreal value);
    void CutOff_OnOffListValueChanged(int index, const QString& value, const QVariant& userData);
    void CutOff_ProjectionListValueChanged(int index, const QString& value, const QVariant& userData);
    void scrollBarValueChanged(int);
    void onNextButtonReleased();

private:
    Ui::Av17DefView* ui;

    int _h;
    int _w;
    int _l;
    int _wl;  // Sizes of projections
    int _startVerticalProjCut, endVerticalProjCut;
    int _startHorizontalProjCut, endHorizontalProjCut;
    int _startCrossSectionProjCut, endCrossSectionProjCut;
    int _projWidth;

    QwtPlot* _railLH;
    QwtPlot* _railWH;
    QwtPlot* _railLW;
    QwtPlot* _bplot;

    QwtPlotMarker* _markerStartVertical;
    QwtPlotMarker* _markerEndVertical;
    QwtPlotMarker* _markerStartHorizontal;
    QwtPlotMarker* _markerEndHorizontal;
    QwtPlotMarker* _markerStartCrossSection;
    QwtPlotMarker* _markerEndCrossSection;

    SpinBoxNumber* _thresholdSpinBoxNumber;
    SpinBoxNumber* _cutOffStartSpinBoxNumber;
    SpinBoxNumber* _cutOffEndSpinBoxNumber;
    SpinBoxList* _cutOff_ProjectionList;
    SpinBoxList* _cutOff_OnOffList;
    SpinBoxNumber* _thresholdBScanSpinBoxNumber;
    QPushButton* _nextButton;

    DefItemLH* _defitemLH;
    DefItemLW* _defitemLW;
    DefItemWH* _defitemWH;
    TLevelRecons* _FRecon;
    T3DRecons* _R3D;

    int _max_h, _max_x, _max_l;
    int _min_h, _min_x, _min_l;

    DefItemBScan* _bscan;
    double _scanStep;  // Horizontal scan step

    QAbstractItemModel* _channelsModel;
    bool _firstControlsArea;
};

#endif  // AV17DEFVIEW_H
