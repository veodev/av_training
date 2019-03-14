#ifndef ASCAN_H
#define ASCAN_H

#include <qglobal.h>
#include <QWidget>
#include <QPen>
#include <QBrush>

#include <qwt_plot.h>

class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotMarker;
class QwtText;
class QwtPlotIntervalCurve;
class QwtIntervalSample;

#define A_SCAN_DEFAULD_DATA_SIZE 232
#define A_SCAN_AMPLITUDE_MAX 255
#define A_SCAN_MAX_SIGNALS 2

class AScanPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit AScanPlot(QWidget* parent = 0);
    ~AScanPlot();

    void reset();

    void replot();

    void setSignalData(unsigned char* data, int index = 0, int dataSize = A_SCAN_DEFAULD_DATA_SIZE);

    void setFactor(float value = 1);
    float factor();

    void setXRange(qreal minX, qreal maxX);
    void setYRange(qreal minY, qreal maxY);

    void setGridVisible(bool visible = true);

    void enableSignal(bool enable = true, int index = 0);
    bool isSignalEnabled(int index = 0);

    void setActiveSignal(int index);
    void setActiveTvg(int index);
    void setActiveGates(int index);

    int activeSignal();

    void setLeftMarkerVisible(bool visible = true);
    bool isLeftMarkerVisible();

    void setRightMarkerVisible(bool visible = true);
    bool isRightMarkerVisible();

    void setLeftMarkerValue(qreal value);
    qreal leftMarkerValue();

    void setRightMarkerValue(qreal value);
    qreal rightMarkerValue();

    void setMaximumMarkerVisible(bool horizontal = true, bool vertical = true);
    void setMaximumMarkerLabelVisible(bool horizontal = true, bool vertical = true);
    bool isMaximumMarkerVisible();
    void setMaximumMarkerValue(qreal hValue, qreal vValue);

    void setGate(int level, int min, int max, int gateIndex = 0, int signalIndex = 0);
    void enableGates(bool enable = true);

    void setTvg(unsigned char* data, int index = 0, int dataSize = A_SCAN_DEFAULD_DATA_SIZE);
    void enableTvg(bool enable = true);

    void setGridMajorPen(const QPen& pen = QPen(Qt::black, 0, Qt::DotLine));
    void setGridMinorPen(const QPen& pen = QPen(Qt::gray, 0, Qt::DotLine));

    void setSignalCurvePen(const QPen& pen = QPen(Qt::black), int index = 0);
    void setSignalCurveBrush(const QBrush& brush = QBrush(Qt::blue, Qt::SolidPattern), int index = 0);
    void setSignalCurveRenderAntialiased(bool on = true, int index = 0);

    void setTvgCurvePen(const QPen& pen = QPen(Qt::darkRed), int index = 0);
    void setTvgCurveRenderAntialiased(bool on = true, int index = 0);

    void setGatesSymbolPen(const QPen& pen = QPen(Qt::darkGreen), int index = 0);

    void setFillingAScanState(bool isFillingAScan);

private:
    struct Signal
    {
        QwtPlotCurve* signalCurves;
        QVector<qreal> ySignalData;
        bool isEnabled;

        QVector<qreal> yTvgData;
        QwtPlotCurve* tvgCurve;

        QVector<qreal> xData;

        QwtPlotIntervalCurve* gateCurve;
        QVector<QwtIntervalSample> gateSamples;
        QVector<int> gateMins;
        QVector<int> gateMaxes;
    };

    static void initSignalData(AScanPlot::Signal& signal, int dataSize = A_SCAN_DEFAULD_DATA_SIZE, float factor = 1);

private:
    int _currentIndex;

    Signal _signals[A_SCAN_MAX_SIGNALS];
    float _factor;

    qreal _minX;
    qreal _maxX;
    qreal _minY;
    qreal _maxY;

    QwtPlotGrid* _grid;
    QwtPlotZoomer* _zoomer;
    QwtPlotPanner* _panner;
    QwtPlotMarker* _leftMarker;
    QwtPlotMarker* _rightMarker;

    QwtPlotMarker* _maximumHMarker;
    QwtText* _maximumHMarkerLabel;
    QwtPlotMarker* _maximumVMarker;
    QwtText* _maximumVMarkerLabel;
    QwtPlotMarker* _maximumSymbolMarker;

    bool _tvgCurveEnabled;
    bool _gatesEnabled;

    bool _isMaximumHMarkerLabelVisible;
    bool _isMaximumVMarkerLabelVisible;
};

#endif  // ASCAN_H
