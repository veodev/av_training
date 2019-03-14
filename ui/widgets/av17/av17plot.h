#ifndef AV17PLOT_H
#define AV17PLOT_H

#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>

class QwtPlotIntervalCurve;
class QwtIntervalSample;
class QwtPlotCurve;
class QwtPlotZoomer;
class QwtPlotZoneItem;

const int BASELINEMINUS45 = 256 * 2 + 2;
const int BASELINE0 = 256 + 1;
const int BASELINEPLUS45 = 0;

class Av17Plot : public QwtPlot
{
public:
    explicit Av17Plot(QWidget* parent = 0);
    ~Av17Plot();
    QwtPlotCurve *curveminus45, *curveplus45, *curve0;
    QwtPlotMarker* markerX;
};

#endif  // AV17PLOT_H
