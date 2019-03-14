#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#include "DataFileUnit.h"
#include "av17plot.h"
#include "settings.h"

Av17Plot::Av17Plot(QWidget* parent)
    : QwtPlot(parent)
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    restoreHeadScannerParams(av17config.XPathStep, av17config.YPathStep, av17config.StartY, av17config.ScanWidth, av17config.ScanHeight, av17config.CurrentY, av17config.ScannerOrientation, av17config.WorkRail, av17config.threshold);

    setAxisScale(QwtPlot::yLeft, 0, 255 * 3 + 3);
    enableAxis(yLeft, false);
    setAxisScale(QwtPlot::xBottom, -av17config.ScanWidth, av17config.ScanWidth, 10);  //-100,100,10

    curveminus45 = new QwtPlotCurve();
    curveplus45 = new QwtPlotCurve();
    curve0 = new QwtPlotCurve();

    curveminus45->setPen(Qt::black, 1);  // цвет и толщина кривой
    curveplus45->setPen(Qt::black, 1);   // цвет и толщина кривой
    curve0->setPen(Qt::black, 1);        // цвет и толщина кривой  darkGreen

    curveminus45->setBaseline(BASELINEMINUS45);
    curve0->setBaseline(BASELINE0);
    curveplus45->setBaseline(BASELINEPLUS45);

    QBrush brush = QBrush(Qt::cyan);
    curveminus45->setBrush(brush);
    brush.setColor(Qt::red);
    curveplus45->setBrush(brush);
    brush.setColor(Qt::green);
    curve0->setBrush(brush);

    curveminus45->attach(this);
    curveplus45->attach(this);
    curve0->attach(this);

    markerX = new QwtPlotMarker();
    markerX->setLabelOrientation(Qt::Vertical);
    markerX->setLineStyle(QwtPlotMarker::VLine);
    markerX->setLinePen(Qt::black, 0, Qt::DashDotLine);
    markerX->setXValue(0);
    markerX->attach(this);
}

Av17Plot::~Av17Plot()
{
    delete markerX;
    delete curveminus45;
    delete curveplus45;
    delete curve0;
}
