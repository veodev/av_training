#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_picker.h>
#include <qwt_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_canvas.h>

#include "ascanplot.h"
#include "gateintervalsymbol.h"

AScanPlot::AScanPlot(QWidget* parent)
    : QwtPlot(parent)
    , _currentIndex(0)
    , _factor(1)
    , _minX(0)
    , _maxX(A_SCAN_DEFAULD_DATA_SIZE)
    , _minY(0)
    , _maxY(A_SCAN_AMPLITUDE_MAX)
    , _grid(0)
    , _zoomer(0)
    , _panner(0)
    , _leftMarker(0)
    , _rightMarker(0)
    , _maximumHMarker(0)
    , _maximumHMarkerLabel(0)
    , _maximumVMarker(0)
    , _maximumVMarkerLabel(0)
    , _maximumSymbolMarker(0)
    , _tvgCurveEnabled(false)
    , _gatesEnabled(false)
    , _isMaximumHMarkerLabelVisible(false)
    , _isMaximumVMarkerLabelVisible(false)
{
    setAutoReplot(false);

    _grid = new QwtPlotGrid;
    _grid->enableX(true);
    _grid->enableY(true);
    setGridMajorPen();
    setGridMinorPen();
    _grid->setVisible(false);
    _grid->attach(this);

    const int signalBushesCount = A_SCAN_MAX_SIGNALS;
    QBrush signalBrushes[A_SCAN_MAX_SIGNALS];
    signalBrushes[0].setStyle(Qt::SolidPattern);
    signalBrushes[0].setColor(Qt::blue);
    signalBrushes[1].setStyle(Qt::SolidPattern);
    signalBrushes[1].setColor(Qt::red);

    QPen tvgCurvePen(Qt::darkRed);
    tvgCurvePen.setWidth(3);

    for (int i = 0; i < A_SCAN_MAX_SIGNALS; ++i) {
        _signals[i].signalCurves = new QwtPlotCurve();
        setSignalCurveRenderAntialiased(true, i);
        setSignalCurvePen(QPen(Qt::black), i);
        setSignalCurveBrush(signalBrushes[i % signalBushesCount], i);
        _signals[i].isEnabled = false;
        AScanPlot::initSignalData(_signals[i]);
        _signals[i].signalCurves->setZ(20 + (A_SCAN_MAX_SIGNALS - i - 1));
        _signals[i].signalCurves->setRenderHint(QwtPlotItem::RenderAntialiased, true);

        _signals[i].tvgCurve = new QwtPlotCurve();
        setTvgCurvePen(tvgCurvePen, i);
        setTvgCurveRenderAntialiased(true, i);
        setTvgCurvePen(tvgCurvePen);
        _signals[i].tvgCurve->setVisible(false);
        _signals[i].tvgCurve->setZ(23);
        _signals[i].tvgCurve->attach(this);
        _signals[i].tvgCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

        _signals[i].gateCurve = new QwtPlotIntervalCurve();
        _signals[i].gateCurve->setStyle(QwtPlotIntervalCurve::NoCurve);
        QwtIntervalSymbol* gateSymbol = new GateIntervalSymbol();
        QPen gatePen = gateSymbol->pen();
        gatePen.setWidth(3);
        gatePen.setColor(Qt::darkGreen);
        gateSymbol->setPen(gatePen);
        gateSymbol->setWidth(18);
        _signals[i].gateCurve->setSymbol(gateSymbol);
        _signals[i].gateCurve->setOrientation(Qt::Horizontal);
        _signals[i].gateCurve->setRenderHint(QwtPlotItem::RenderAntialiased, false);
        _signals[i].gateCurve->setVisible(false);
        _signals[i].gateCurve->setZ(25);  // between signals and markers
        _signals[i].gateCurve->attach(this);
    }
    _signals[0].signalCurves->attach(this);
    _signals[0].isEnabled = true;

    setXRange(_minX, _maxX);
    setYRange(_minY, _maxY);
    enableAxis(QwtPlot::yLeft, false);

    QwtPlotCanvas* canvas = new QwtPlotCanvas();
    canvas->setFrameStyle(QFrame::NoFrame);
    canvas->setPaintAttribute(QwtPlotCanvas::BackingStore, false);
    canvas->setPalette(Qt::white);
    setCanvas(canvas);

#if 0
    _zoomer = new QwtPlotZoomer(canvas);
    _zoomer->setRubberBandPen(QPen(Qt::black));
    _zoomer->setTrackerPen(QColor(Qt::black));
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
#endif

    _panner = new QwtPlotPanner(canvas);
    _panner->setMouseButton(Qt::MidButton);

    _leftMarker = new QwtPlotMarker();
    _leftMarker->setLineStyle(QwtPlotMarker::VLine);
    _leftMarker->setLinePen(QPen(Qt::black, 1, Qt::DashLine));
    _leftMarker->setXValue(0.0);
    _leftMarker->attach(this);
    _leftMarker->setVisible(false);

    _rightMarker = new QwtPlotMarker();
    _rightMarker->setLineStyle(QwtPlotMarker::VLine);
    _rightMarker->setLinePen(QPen(Qt::black, 1, Qt::DashLine));
    _rightMarker->setXValue(0.0);
    _rightMarker->attach(this);
    _rightMarker->setVisible(false);

    _maximumHMarkerLabel = new QwtText();
    _maximumHMarkerLabel->setFont(QFont("Helvetica", 10, QFont::Bold));
    _maximumHMarkerLabel->setColor(Qt::red);
    _maximumHMarker = new QwtPlotMarker();
    _maximumHMarker->setLineStyle(QwtPlotMarker::HLine);
    _maximumHMarker->setLinePen(QPen(Qt::red, 1, Qt::DashLine));
    _maximumHMarker->setValue(0.0, 0.0);
    _maximumHMarker->setVisible(false);
    _maximumHMarker->setLabel(*_maximumHMarkerLabel);
    _maximumHMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    _maximumHMarker->attach(this);

    _maximumVMarkerLabel = new QwtText();
    _maximumVMarkerLabel->setFont(QFont("Helvetica", 10, QFont::Bold));
    _maximumVMarkerLabel->setColor(Qt::red);
    _maximumVMarker = new QwtPlotMarker();
    _maximumVMarker->setLineStyle(QwtPlotMarker::VLine);
    _maximumVMarker->setLinePen(QPen(Qt::red, 1, Qt::DashLine));
    _maximumVMarker->setValue(0.0, 0.0);
    _maximumVMarker->setVisible(false);
    _maximumVMarker->setLabel(*_maximumVMarkerLabel);
    _maximumVMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    _maximumVMarker->attach(this);

    _maximumSymbolMarker = new QwtPlotMarker();
    _maximumSymbolMarker->setLineStyle(QwtPlotMarker::Cross);
    _maximumSymbolMarker->setLinePen(QPen(Qt::red, 1, Qt::NoPen));
    _maximumSymbolMarker->setValue(0.0, 0.0);
    _maximumSymbolMarker->setVisible(false);
    _maximumSymbolMarker->setSymbol(new QwtSymbol(QwtSymbol::Cross, QColor(Qt::black), QColor(Qt::black), QSize(16, 16)));
    _maximumSymbolMarker->attach(this);
}

AScanPlot::~AScanPlot()
{
    for (unsigned char i = 0; i < A_SCAN_MAX_SIGNALS; ++i) {
        delete _signals[i].signalCurves;
        delete _signals[i].tvgCurve;
        delete _signals[i].gateCurve;
    }
    delete _maximumHMarkerLabel;
    delete _maximumHMarker;
    delete _maximumVMarkerLabel;
    delete _maximumVMarker;
    delete _maximumSymbolMarker;
    delete _leftMarker;
    delete _rightMarker;
    delete _panner;
}

void AScanPlot::reset()
{
    for (int signalIndex = 0; signalIndex < A_SCAN_MAX_SIGNALS; ++signalIndex) {
        Signal* signal = &_signals[signalIndex];
        for (int i = 0; i < signal->xData.size(); ++i) {
            signal->ySignalData[i] = 0;
            signal->yTvgData[i] = 0;
        }
        signal->signalCurves->setSamples(signal->xData, signal->ySignalData);
        signal->tvgCurve->setSamples(signal->xData, signal->yTvgData);

        for (int gateIndex = 0; gateIndex < _signals[signalIndex].gateSamples.size(); ++gateIndex) {
            setGate(-1, -1, -1, gateIndex, signalIndex);
            ;
        }
    }
    setLeftMarkerValue(-1);
    setRightMarkerValue(-1);
    replot();
}

void AScanPlot::replot()
{
    QwtPlot::replot();
}

void AScanPlot::setSignalData(unsigned char* data, int index, int dataSize)
{
    Q_ASSERT(data);
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    Q_ASSERT(dataSize > 0);

    Signal* signal = &_signals[index];

    if (signal->xData.size() != dataSize) {
        initSignalData(*signal, dataSize, _factor);
    }
    for (int i = 0; i < dataSize; ++i) {
        signal->ySignalData[i] = data[i];
    }
    signal->signalCurves->setSamples(signal->xData, signal->ySignalData);
}

void AScanPlot::setFactor(float value)
{
    _factor = value;
    for (int signalIndex = 0; signalIndex < A_SCAN_MAX_SIGNALS; ++signalIndex) {
        Signal* signal = &_signals[signalIndex];
        int dataSize = signal->xData.size();
        for (int i = 0; i < dataSize; ++i) {
            signal->xData[i] = _factor * i;
            signal->tvgCurve->setSamples(signal->xData, signal->yTvgData);
        }
        for (int i = 0; i < signal->gateSamples.size(); ++i) {
            signal->gateSamples[i].interval.setInterval(signal->gateMins[i] * _factor, signal->gateMaxes[i] * _factor);
            signal->gateCurve->setSamples(_signals[signalIndex].gateSamples);
        }
    }
}

float AScanPlot::factor()
{
    return _factor;
}

void AScanPlot::setXRange(qreal minX, qreal maxX)
{
    _minX = minX;
    _maxX = maxX;
    setAxisScale(QwtPlot::xBottom, _minX, _maxX);
}

void AScanPlot::setYRange(qreal minY, qreal maxY)
{
    _minY = minY;
    _maxY = maxY;
    setAxisScale(QwtPlot::yLeft, _minY, _maxY);
}

void AScanPlot::setGridVisible(bool visible)
{
    _grid->setVisible(visible);
}

void AScanPlot::enableSignal(bool enable, int index)
{
    Q_ASSERT(index >= 0 && index < A_SCAN_MAX_SIGNALS);

    if (enable == true) {
        if (_signals[index].isEnabled == false) {
            _signals[index].isEnabled = true;
            _signals[index].signalCurves->attach(this);
        }
    }
    else {
        if (_signals[index].isEnabled == true) {
            if (index && _currentIndex == index) {
                setActiveSignal(0);
            }
            _signals[index].isEnabled = false;
            _signals[index].signalCurves->detach();
        }
    }
}

bool AScanPlot::isSignalEnabled(int index)
{
    Q_ASSERT(index >= 0 && index < A_SCAN_MAX_SIGNALS);

    return _signals[index].isEnabled;
}

void AScanPlot::setActiveSignal(int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);

    if (_currentIndex == index || _signals[index].isEnabled == false) {
        return;
    }

    int z = _signals[_currentIndex].signalCurves->z();
    _signals[_currentIndex].signalCurves->setZ(_signals[index].signalCurves->z());
    _signals[index].signalCurves->setZ(z);

    _currentIndex = index;
}

void AScanPlot::setActiveTvg(int index)
{
    if (_tvgCurveEnabled) {
        _signals[_currentIndex].tvgCurve->setVisible(false);
        _signals[index].tvgCurve->setVisible(true);
    }
}

void AScanPlot::setActiveGates(int index)
{
    if (_gatesEnabled) {
        _signals[_currentIndex].gateCurve->setVisible(false);
        _signals[index].gateCurve->setVisible(true);
    }
}

int AScanPlot::activeSignal()
{
    return _currentIndex;
}

void AScanPlot::setLeftMarkerVisible(bool visible)
{
    _leftMarker->setVisible(visible);
}

bool AScanPlot::isLeftMarkerVisible()
{
    return _leftMarker->isVisible();
}

void AScanPlot::setRightMarkerVisible(bool visible)
{
    _rightMarker->setVisible(visible);
}

bool AScanPlot::isRightMarkerVisible()
{
    return _rightMarker->isVisible();
}

void AScanPlot::setLeftMarkerValue(qreal value)
{
    if (value > _maxX) {
        value = _maxX;
    }
    else if (value < _minX) {
        value = _minX;
    }
    _leftMarker->setXValue(value);
}

qreal AScanPlot::leftMarkerValue()
{
    return _leftMarker->xValue();
}

void AScanPlot::setRightMarkerValue(qreal value)
{
    if (value > _maxX) {
        value = _maxX;
    }
    else if (value < _minX) {
        value = _minX;
    }
    _rightMarker->setXValue(value);
}

qreal AScanPlot::rightMarkerValue()
{
    return _rightMarker->xValue();
}

void AScanPlot::setMaximumMarkerVisible(bool horizontal, bool vertical)
{
    _maximumSymbolMarker->setVisible(horizontal || vertical);
    _maximumHMarker->setVisible(horizontal);
    _maximumVMarker->setVisible(vertical);
}

void AScanPlot::setMaximumMarkerLabelVisible(bool horizontal, bool vertical)
{
    _isMaximumHMarkerLabelVisible = horizontal;
    _isMaximumVMarkerLabelVisible = vertical;
}

bool AScanPlot::isMaximumMarkerVisible()
{
    return _maximumSymbolMarker->isVisible();
}

void AScanPlot::setMaximumMarkerValue(qreal hValue, qreal vValue)
{
    vValue *= _factor;
    _maximumSymbolMarker->setValue(vValue, hValue);
    _maximumHMarker->setValue(0, hValue);
    if (_isMaximumHMarkerLabelVisible == true) {
        _maximumHMarkerLabel->setText(QString::number(hValue));
        _maximumHMarker->setLabel(*_maximumHMarkerLabel);
    }
    _maximumVMarker->setValue(vValue, 0);
    if (_isMaximumVMarkerLabelVisible == true) {
        _maximumVMarkerLabel->setText(QString::number(vValue, 'f', 1));
        _maximumVMarker->setLabel(*_maximumVMarkerLabel);
    }
}

void AScanPlot::setGate(int level, int min, int max, int gateIndex, int signalIndex)
{
    Q_ASSERT(signalIndex < A_SCAN_MAX_SIGNALS);

    if (gateIndex >= _signals[signalIndex].gateSamples.size()) {
        _signals[signalIndex].gateSamples.resize(gateIndex + 1);
        _signals[signalIndex].gateMins.resize(gateIndex + 1);
        _signals[signalIndex].gateMaxes.resize(gateIndex + 1);
    }

    _signals[signalIndex].gateSamples[gateIndex].value = level;
    _signals[signalIndex].gateSamples[gateIndex].interval.setInterval(min * _factor, max * _factor);
    _signals[signalIndex].gateMins[gateIndex] = min;
    _signals[signalIndex].gateMaxes[gateIndex] = max;
    _signals[signalIndex].gateCurve->setSamples(_signals[signalIndex].gateSamples);
}

void AScanPlot::enableGates(bool enable)
{
    _gatesEnabled = enable;
    if (enable == true) {
        _signals[_currentIndex].gateCurve->setVisible(true);
    }
    else {
        _signals[_currentIndex].gateCurve->setVisible(false);
    }
}

void AScanPlot::setTvg(unsigned char* data, int index, int size)
{
    Q_ASSERT(data);
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    Q_ASSERT(size >= 0);

    Signal* signal = &_signals[index];

    if (signal->xData.size() != size) {
        initSignalData(*signal, size, _factor);
    }
    for (int i = 0; i < size; ++i) {
        signal->yTvgData[i] = data[i];
    }
    signal->tvgCurve->setSamples(signal->xData, signal->yTvgData);
}

void AScanPlot::enableTvg(bool enable)
{
    _tvgCurveEnabled = enable;
    if (enable == true) {
        _signals[_currentIndex].tvgCurve->setVisible(true);
    }
    else {
        _signals[_currentIndex].tvgCurve->setVisible(false);
    }
}

void AScanPlot::setGridMajorPen(const QPen& pen)
{
    _grid->setMajorPen(pen);
}

void AScanPlot::setGridMinorPen(const QPen& pen)
{
    _grid->setMajorPen(pen);
}

void AScanPlot::setSignalCurvePen(const QPen& pen, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    _signals[index].signalCurves->setPen(pen);
}

void AScanPlot::setSignalCurveBrush(const QBrush& brush, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    _signals[index].signalCurves->setBrush(brush);
}

void AScanPlot::setSignalCurveRenderAntialiased(bool on, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    _signals[index].signalCurves->setRenderHint(QwtPlotItem::RenderAntialiased, on);
}

void AScanPlot::setTvgCurvePen(const QPen& pen, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    _signals[index].tvgCurve->setPen(pen);
}

void AScanPlot::setTvgCurveRenderAntialiased(bool on, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);
    _signals[index].tvgCurve->setRenderHint(QwtPlotItem::RenderAntialiased, on);
}

void AScanPlot::setGatesSymbolPen(const QPen& pen, int index)
{
    Q_ASSERT(index < A_SCAN_MAX_SIGNALS);

    QwtIntervalSymbol* symbol = const_cast<QwtIntervalSymbol*>(_signals[index].gateCurve->symbol());
    symbol->setPen(pen);
}

void AScanPlot::setFillingAScanState(bool isFillingAScan)
{
    if (isFillingAScan == true) {
        setSignalCurvePen(QPen(Qt::black), 0);
        setSignalCurvePen(QPen(Qt::black), 1);
        QBrush signalBrushes[A_SCAN_MAX_SIGNALS];
        signalBrushes[0].setStyle(Qt::SolidPattern);
        signalBrushes[0].setColor(Qt::blue);
        signalBrushes[1].setStyle(Qt::SolidPattern);
        signalBrushes[1].setColor(Qt::red);
        setSignalCurveBrush(signalBrushes[0 % A_SCAN_MAX_SIGNALS], 0);
        setSignalCurveBrush(signalBrushes[1 % A_SCAN_MAX_SIGNALS], 1);
    }
    else {
        setSignalCurvePen(QPen(Qt::blue), 0);
        setSignalCurvePen(QPen(Qt::red), 1);
        setSignalCurveBrush(QBrush(Qt::NoBrush), 0);
        setSignalCurveBrush(QBrush(Qt::NoBrush), 1);
        setSignalCurveRenderAntialiased(false, 0);
        setSignalCurveRenderAntialiased(false, 1);
        setTvgCurvePen(QPen(Qt::darkRed, 3), 0);
        setTvgCurvePen(QPen(Qt::darkRed, 3), 1);
        setTvgCurveRenderAntialiased(false, 0);
        setTvgCurveRenderAntialiased(false, 1);
        setGatesSymbolPen(QPen(Qt::darkGreen, 3));
    }

    replot();
}

void AScanPlot::initSignalData(Signal& signal, int dataSize, float factor)
{
    signal.xData.resize(dataSize);
    signal.ySignalData.resize(dataSize);
    signal.yTvgData.resize(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        signal.xData[i] = factor * i;
        signal.ySignalData[i] = 0;
    }
}
