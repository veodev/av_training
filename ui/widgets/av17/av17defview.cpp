#include "av17defview.h"
#include "ui_av17defview.h"

#include <qwt_plot_marker.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_text.h>
#include <qwt_text_label.h>
#include <qwt_plot_item.h>
#include <qwt_plot_renderer.h>

#include <QSizePolicy>
#include "DataFileUnit.h"
#include "debug.h"
#include "styles.h"
#include <QScrollBar>
#include <QBuffer>
#include <cmath>
#include <QFont>

#include "core.h"
#include "settings.h"
#include "roles.h"
#include "channelscontroller.h"
#include "colors.h"
#include "ChannelsIds.h"
#include "registration.h"

const float OffsetRuBehind = -42;
const float OffsetRuBetween = 472.5;

Av17DefView::Av17DefView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Av17DefView)
    , _firstControlsArea(true)
{
    ui->setupUi(this);
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    // -------------------------------------
    //   Projections
    // -------------------------------------

    _railLH = new QwtPlot(ui->scrollArea);
    _railLW = new QwtPlot(ui->scrollArea);
    _railWH = new QwtPlot(ui->scrollArea);

    _railLH->setAutoFillBackground(false);

    _railLH->titleLabel()->setFont(QFont("Helvetica", 10));
    _railLH->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 20);
    _railLH->setAxisScale(QwtPlot::xBottom, -av17config.ScanWidth, av17config.ScanWidth, 20);
    _railLH->setAxisScale(QwtPlot::yLeft, 45, 0);
    _railLH->enableAxis(QwtPlot::xBottom, false);
    _railLH->enableAxis(QwtPlot::xTop, true);
    _railLH->setAxisAutoScale(QwtPlot::xTop, true);
    _firstControlsArea = true;
    CalculateSizes();
    _railLH->setGeometry(0, 0, _l, _h);

    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->enableXMin(false);
    grid->enableYMin(false);
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
    grid->setMinorPen(Qt::gray, 0, Qt::DotLine);
    grid->attach(_railLH);

    _railLW->footerLabel()->setFont(QFont("Helvetica", 10));
    _railLW->titleLabel()->setFont(QFont("Helvetica", 10));
    _railLW->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 20);
    _railLW->setAxisScale(QwtPlot::xBottom, -av17config.ScanWidth, av17config.ScanWidth, 20);
    _railLW->setAxisScale(QwtPlot::yLeft, 38, -38);
    _railLW->enableAxis(QwtPlot::xBottom, false);
    _railLW->enableAxis(QwtPlot::xTop, true);
    _railLW->enableAxis(QwtPlot::yLeft, true);
    _railLW->setGeometry(0, _h, _l, _wl);

    QwtPlotGrid* gridLW = new QwtPlotGrid;
    gridLW->enableXMin(false);
    gridLW->enableYMin(false);
    gridLW->enableX(true);
    gridLW->enableY(true);
    gridLW->setMajorPen(Qt::gray, 0, Qt::DotLine);
    gridLW->setMinorPen(Qt::gray, 0, Qt::DotLine);
    gridLW->attach(_railLW);

    _railWH->titleLabel()->setFont(QFont("Helvetica", 10));
    _railWH->setAxisScale(QwtPlot::xTop, -38, 38);
    _railWH->setAxisScale(QwtPlot::xBottom, -38, 38);
    _railWH->setAxisScale(QwtPlot::yLeft, 45, 0);
    _railWH->enableAxis(QwtPlot::yLeft, true);
    _railWH->enableAxis(QwtPlot::xBottom, false);
    _railWH->enableAxis(QwtPlot::xTop, true);
    _railWH->setGeometry(_l, 0, _w, _h);

    QwtPlotGrid* gridWH = new QwtPlotGrid;
    gridWH->enableXMin(false);
    gridWH->enableYMin(false);
    gridWH->enableX(true);
    gridWH->enableY(true);
    gridWH->setMajorPen(Qt::gray, 0, Qt::DotLine);
    gridWH->setMinorPen(Qt::white, 0, Qt::DotLine);
    gridWH->attach(_railWH);

    // -------------------------------------
    //   BScan 0 channel
    // -------------------------------------
    _bscan = new DefItemBScan(ui->scrollArea);

    // -------------------------------------
    //   Controls Area
    // -------------------------------------
    QFont font;
    font.setPointSize(22);
    font.setBold(true);
    font.setWeight(75);

    _thresholdSpinBoxNumber = new SpinBoxNumber();
    Q_ASSERT(_thresholdSpinBoxNumber != NULL);
    ASSERT(connect(_thresholdSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &Av17DefView::ThresholdValueChanged));
    _thresholdSpinBoxNumber->enableCaption();
    _thresholdSpinBoxNumber->setStepBy(5);   // 2
    _thresholdSpinBoxNumber->setMinimum(0);  // 25
    _thresholdSpinBoxNumber->setMaximum(100);
    _thresholdSpinBoxNumber->setValue(50, false);

    _cutOffStartSpinBoxNumber = new SpinBoxNumber();
    Q_ASSERT(_cutOffStartSpinBoxNumber != NULL);
    ASSERT(connect(_cutOffStartSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &Av17DefView::CutOffStartValueChanged));
    _cutOffStartSpinBoxNumber->enableCaption();
    _cutOffStartSpinBoxNumber->setMinimum(-av17config.ScanWidth);
    _cutOffStartSpinBoxNumber->setMaximum(av17config.ScanWidth);
    int defaultWideCuttingOff = qRound(0.76 * av17config.ScanWidth);
    _cutOffStartSpinBoxNumber->setValue(-defaultWideCuttingOff, false);

    _cutOffEndSpinBoxNumber = new SpinBoxNumber();
    Q_ASSERT(_cutOffEndSpinBoxNumber != NULL);
    ASSERT(connect(_cutOffEndSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &Av17DefView::CutOffEndValueChanged));
    _cutOffEndSpinBoxNumber->enableCaption();
    _cutOffEndSpinBoxNumber->setMinimum(-av17config.ScanWidth);
    _cutOffEndSpinBoxNumber->setMaximum(av17config.ScanWidth);
    _cutOffEndSpinBoxNumber->setValue(defaultWideCuttingOff, false);

    _cutOff_ProjectionList = new SpinBoxList();
    Q_ASSERT(_cutOff_ProjectionList != NULL);
    ASSERT(connect(_cutOff_ProjectionList, &SpinBoxList::valueChanged, this, &Av17DefView::CutOff_ProjectionListValueChanged));
    _cutOff_ProjectionList->enableCaption();

    _cutOff_OnOffList = new SpinBoxList();
    Q_ASSERT(_cutOff_OnOffList != NULL);
    ASSERT(connect(_cutOff_OnOffList, &SpinBoxList::valueChanged, this, &Av17DefView::CutOff_OnOffListValueChanged));
    _cutOff_OnOffList->enableCaption();

    _thresholdBScanSpinBoxNumber = new SpinBoxNumber();
    Q_ASSERT(_thresholdBScanSpinBoxNumber != NULL);
    ASSERT(connect(_thresholdBScanSpinBoxNumber, &SpinBoxNumber::valueChanged, this, &Av17DefView::ThresholdBScanChanged));
    _thresholdBScanSpinBoxNumber->enableCaption();
    _thresholdBScanSpinBoxNumber->setStepBy(2);
    _thresholdBScanSpinBoxNumber->setMinimum(-12);  // 25
    _thresholdBScanSpinBoxNumber->setMaximum(18);
    _thresholdBScanSpinBoxNumber->setValue(-6);


    _nextButton = new QPushButton();
    _nextButton->setMinimumHeight(50);
    _nextButton->setFont(font);
    _nextButton->setStyleSheet(Styles::button());  // Styles::areaButton()
    _nextButton->setFocusPolicy(Qt::NoFocus);
    _nextButton->setMinimumWidth(200);
    ASSERT(connect(_nextButton, &QPushButton::released, this, &Av17DefView::onNextButtonReleased));

    setControlsAreaVisible(false);
    ui->controlsArea->reset();
    ui->controlsArea->addWidget(_thresholdSpinBoxNumber);
    ui->controlsArea->addWidget(_cutOff_ProjectionList);
    ui->controlsArea->addWidget(_cutOff_OnOffList);
    ui->controlsArea->addWidget(_cutOffStartSpinBoxNumber);
    ui->controlsArea->addWidget(_cutOffEndSpinBoxNumber);
    ui->controlsArea->addWidget(_thresholdBScanSpinBoxNumber);
    ui->controlsArea->addWidget(_nextButton);
    setControlsAreaVisible(true);

    retranslateUi();

    _startVerticalProjCut = -defaultWideCuttingOff;
    endVerticalProjCut = defaultWideCuttingOff;
    _startHorizontalProjCut = -38;
    endHorizontalProjCut = 38;
    _startCrossSectionProjCut = 0;
    endCrossSectionProjCut = 45;

    _markerStartVertical = new QwtPlotMarker();
    _markerStartVertical->setLabelOrientation(Qt::Vertical);
    _markerStartVertical->setLineStyle(QwtPlotMarker::VLine);
    _markerStartVertical->setLinePen(Qt::blue, 0, Qt::DashDotLine);  // black
    _markerStartVertical->setXValue(static_cast<float>(_startVerticalProjCut));
    _markerStartVertical->attach(_railLH);

    _markerEndVertical = new QwtPlotMarker();
    _markerEndVertical->setLabelOrientation(Qt::Vertical);
    _markerEndVertical->setLineStyle(QwtPlotMarker::VLine);
    _markerEndVertical->setLinePen(Qt::blue, 0, Qt::DashDotLine);
    _markerEndVertical->setXValue(static_cast<float>(endVerticalProjCut));
    _markerEndVertical->attach(_railLH);

    _markerStartHorizontal = new QwtPlotMarker();
    _markerStartHorizontal->setLabelOrientation(Qt::Horizontal);
    _markerStartHorizontal->setLineStyle(QwtPlotMarker::HLine);
    _markerStartHorizontal->setLinePen(Qt::blue, 0, Qt::DashDotLine);
    _markerStartHorizontal->setYValue(_startHorizontalProjCut);
    _markerStartHorizontal->attach(_railLW);

    _markerEndHorizontal = new QwtPlotMarker();
    _markerEndHorizontal->setLabelOrientation(Qt::Horizontal);
    _markerEndHorizontal->setLineStyle(QwtPlotMarker::HLine);
    _markerEndHorizontal->setLinePen(Qt::blue, 0, Qt::DashDotLine);
    _markerEndHorizontal->setYValue(endHorizontalProjCut);
    _markerEndHorizontal->attach(_railLW);

    _markerStartCrossSection = new QwtPlotMarker();
    _markerStartCrossSection->setLabelOrientation(Qt::Horizontal);
    _markerStartCrossSection->setLineStyle(QwtPlotMarker::HLine);
    _markerStartCrossSection->setLinePen(Qt::blue, 0, Qt::DashDotLine);
    _markerStartCrossSection->setYValue(_startCrossSectionProjCut);
    _markerStartCrossSection->attach(_railWH);

    _markerEndCrossSection = new QwtPlotMarker();
    _markerEndCrossSection->setLabelOrientation(Qt::Horizontal);
    _markerEndCrossSection->setLineStyle(QwtPlotMarker::HLine);
    _markerEndCrossSection->setLinePen(Qt::blue, 0, Qt::DashDotLine);
    _markerEndCrossSection->setYValue(endCrossSectionProjCut);
    _markerEndCrossSection->attach(_railWH);

    _R3D = new T3DRecons;
    _defitemLH = new DefItemLH();
    _defitemLH->attach(_railLH);
    _defitemLW = new DefItemLW();
    _defitemLW->attach(_railLW);
    _defitemWH = new DefItemWH();
    _defitemWH->attach(_railWH);

    ui->scrollArea->adjustSize();
    ui->scrollArea->updateGeometry();

    CalculateSizes();
    _railLH->setGeometry(0, 2, _l, _h);
    _railLW->setGeometry(0, _h + 4, _l, _wl);
    _railWH->setGeometry(_l, 2, _w, _h);

    QScrollBar* scrollBar = ui->scrollArea->horizontalScrollBar();
    ASSERT(connect(scrollBar, &QScrollBar::valueChanged, this, &Av17DefView::scrollBarValueChanged));
    scrollBar->setValue(0);
    scrollBar->setRange(0, 20);
    scrollBar->setSingleStep(1);
    scrollBar->setPageStep(12);
    ui->scrollArea->horizontalScrollBar()->setInvertedControls(true);

    _bscan->setGeometry(34, _h + _wl + 5, _l - 34, 60);  // 47 41

    _scanStep = restoreHorizontalScannerEncoderStep();
}

Av17DefView::~Av17DefView()
{
    if (_R3D != NULL) {
        delete _R3D;
    }
    delete _railWH;
    delete _railLW;
    delete _railLH;
    delete ui;
}

void Av17DefView::setModel(QAbstractItemModel* channelsModel)
{
    Q_ASSERT(channelsModel);
    _channelsModel = channelsModel;
}

void Av17DefView::CalculateSizes()
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    if (ui->controlsArea->isVisible()) {
        _projWidth = width() - ui->controlsArea->width() - 20;
        if (_firstControlsArea) {
            _h = qRound(height() / 3.3f);  // 2.8f
            _w = (_projWidth - 1) / (av17config.ScanWidth * 2 / 75 + 1);
            _wl = _w;
            _l = _projWidth - 1 - _w;
        }
        else {
            _h = qRound(height() / 3.3f);  // 2.8f
            _w = (_projWidth - 1) / (av17config.ScanWidth * 2 / 75 + 1);
            _wl = _h * 65 / 45;
            _l = _projWidth - 1 - _w;
        }
    }
    else {
        _projWidth = width() - 20;
        _h = qRound(height() / 3.3f);  // 2.8f
        _w = (_projWidth - 1) / (av17config.ScanWidth * 2 / 75 + 1);
        _wl = _h * 65 / 45;  //_w;
        _l = _projWidth - 1 - _w;
    }
}

void Av17DefView::scrollBarValueChanged(int x)
{
    int d, m;
    m = ui->scrollArea->horizontalScrollBar()->maximum();
    if (m != 0) {
        d = (_projWidth - ui->scrollArea->width()) * x / m;
        if (d <= 0) {
            d = 0;
        }
    }
    else {
        d = 0;
    }
    _railLH->setGeometry(-d, 0 + 2, _l, _h);
    _railLW->setGeometry(-d, _h + 4, _l, _wl);
    _railWH->setGeometry(_l - d, 0 + 2, _w, _h);
    _bscan->setGeometry(-d + 32, _h + _wl + 5, _l - 32, 60);  // height()-2
}

bool Av17DefView::controlsAreaVisible()
{
    return ui->controlsArea->isVisible();
}

void Av17DefView::setControlsAreaVisible(bool isVisible)
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();
    if (isVisible) {
        _railLH->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 20);
        _railLW->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 20);
        _railWH->setAxisScale(QwtPlot::xTop, -38, 38, 20);
    }
    else {
        _railLH->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 10);
        _railLW->setAxisScale(QwtPlot::xTop, -av17config.ScanWidth, av17config.ScanWidth, 10);
        _railLW->setAxisScale(QwtPlot::yLeft, 38, -38);
        _railWH->setAxisScale(QwtPlot::xTop, -38, 38);
        _firstControlsArea = false;
    }

    ui->controlsArea->setVisible(isVisible);
    CalculateSizes();
    _railLH->setGeometry(0, 0 + 2, _l, _h);
    _railLW->setGeometry(0, _h + 4, _l, _wl);
    _railWH->setGeometry(_l, 0 + 2, _w, _h);
    _bscan->setGeometry(32, _h + _wl + 5, _l - 32, 60);  // height()-2

    replotProjections();
}

bool Av17DefView::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void Av17DefView::CutOffStartValueChanged(qreal value)
{
    switch (_cutOff_ProjectionList->index()) {
    case projectionVertical:
        _startVerticalProjCut = qRound(value);
        _markerStartVertical->setXValue(value);
        break;
    case projectionHorizontal:
        _startHorizontalProjCut = qRound(value);
        _markerStartHorizontal->setYValue(value);
        break;
    case projectionCrossSection:
        _startCrossSectionProjCut = qRound(value);
        _markerStartCrossSection->setYValue(value);
        break;
    }

    if (_R3D != NULL && _cutOff_OnOffList->index() == 0) {
        A17Data& av17data = HeadScannerData::instance().getData();
        int maxh = std::min(_max_h, av17data.Y_mm_to_Idx(endCrossSectionProjCut));
        int minh = std::max(_min_h, av17data.Y_mm_to_Idx(_startCrossSectionProjCut));

        int maxx = std::min(_max_x, _R3D->OffsetPos_mm_to_Idx(endHorizontalProjCut));
        int minx = std::max(_min_x, _R3D->OffsetPos_mm_to_Idx(_startHorizontalProjCut));

        int maxl = std::min(_max_l, _R3D->AlongPos_mm_to_Idx(endVerticalProjCut));
        int minl = std::max(_min_l, _R3D->AlongPos_mm_to_Idx(_startVerticalProjCut));

        _defitemLH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        _defitemLW->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        _defitemWH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
    }
    replotProjections();
}

void Av17DefView::CutOffEndValueChanged(qreal value)
{
    switch (_cutOff_ProjectionList->index()) {
    case projectionVertical:
        endVerticalProjCut = qRound(value);
        _markerEndVertical->setXValue(value);
        break;
    case projectionHorizontal:
        endHorizontalProjCut = qRound(value);
        _markerEndHorizontal->setYValue(value);
        break;
    case projectionCrossSection:
        endCrossSectionProjCut = qRound(value);
        _markerEndCrossSection->setYValue(value);
        break;
    }
    if (_R3D != NULL && _cutOff_OnOffList->index() == 0) {
        A17Data& av17data = HeadScannerData::instance().getData();
        int maxh = std::min(_max_h, av17data.Y_mm_to_Idx(endCrossSectionProjCut));
        int minh = std::max(_min_h, av17data.Y_mm_to_Idx(_startCrossSectionProjCut));

        int maxx = std::min(_max_x, _R3D->OffsetPos_mm_to_Idx(endHorizontalProjCut));
        int minx = std::max(_min_x, _R3D->OffsetPos_mm_to_Idx(_startHorizontalProjCut));

        int maxl = std::min(_max_l, _R3D->AlongPos_mm_to_Idx(endVerticalProjCut));
        int minl = std::max(_min_l, _R3D->AlongPos_mm_to_Idx(_startVerticalProjCut));

        _defitemLH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        _defitemLW->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        _defitemWH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
    }
    replotProjections();
}

void Av17DefView::ThresholdValueChanged(qreal value)
{
    // replot defect with cut off and new threshold
    if (InitDef(qRound(value))) {
        if (_R3D != NULL && _cutOff_OnOffList->index() == 0) {
            A17Data& av17data = HeadScannerData::instance().getData();
            int maxh = std::min(_max_h, av17data.Y_mm_to_Idx(endCrossSectionProjCut));
            int minh = std::max(_min_h, av17data.Y_mm_to_Idx(_startCrossSectionProjCut));

            int maxx = std::min(_max_x, _R3D->OffsetPos_mm_to_Idx(endHorizontalProjCut));
            int minx = std::max(_min_x, _R3D->OffsetPos_mm_to_Idx(_startHorizontalProjCut));

            int maxl = std::min(_max_l, _R3D->AlongPos_mm_to_Idx(endVerticalProjCut));
            int minl = std::max(_min_l, _R3D->AlongPos_mm_to_Idx(_startVerticalProjCut));

            _defitemLH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
            _defitemLW->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
            _defitemWH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        }
    }
    // redraw projection plots
    replotProjections();
}

void Av17DefView::ThresholdBScanChanged(qreal value)
{
    Q_ASSERT(_bscan);
    if (_bscan) {
        _bscan->setThreshold(qRound(value));
        _bscan->repaint();
    }
}

void Av17DefView::CutOff_OnOffListValueChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);

    if (index == 0) {
        _markerStartVertical->show();
        _markerEndVertical->show();
        _markerStartHorizontal->show();
        _markerEndHorizontal->show();
        _markerStartCrossSection->show();
        _markerEndCrossSection->show();

        _cutOffStartSpinBoxNumber->setEnabled(true);
        _cutOffEndSpinBoxNumber->setEnabled(true);

        if (_R3D != NULL) {
            A17Data& av17data = HeadScannerData::instance().getData();
            int maxh = std::min(_max_h, av17data.Y_mm_to_Idx(endCrossSectionProjCut));
            int minh = std::max(_min_h, av17data.Y_mm_to_Idx(_startCrossSectionProjCut));

            int maxx = std::min(_max_x, _R3D->OffsetPos_mm_to_Idx(endHorizontalProjCut));
            int minx = std::max(_min_x, _R3D->OffsetPos_mm_to_Idx(_startHorizontalProjCut));

            int maxl = std::min(_max_l, _R3D->AlongPos_mm_to_Idx(endVerticalProjCut));
            int minl = std::max(_min_l, _R3D->AlongPos_mm_to_Idx(_startVerticalProjCut));

            _defitemLH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
            _defitemLW->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
            _defitemWH->Init(_R3D, maxh, maxx, maxl, minh, minx, minl);
        }
    }
    if (index == 1) {
        _markerStartVertical->hide();
        _markerEndVertical->hide();
        _markerStartHorizontal->hide();
        _markerEndHorizontal->hide();
        _markerStartCrossSection->hide();
        _markerEndCrossSection->hide();

        _defitemLH->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
        _defitemLW->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
        _defitemWH->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
    }
    // redraw projection plots
    replotProjections();
}
void Av17DefView::CutOff_ProjectionListValueChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(userData);

    TAv17Config& av17config = HeadScannerData::instance().getConfig();

    switch (_cutOff_ProjectionList->index()) {
    case projectionVertical:
        _cutOffStartSpinBoxNumber->setMinimum(-av17config.ScanWidth);
        _cutOffStartSpinBoxNumber->setMaximum(av17config.ScanWidth);
        _cutOffEndSpinBoxNumber->setMinimum(-av17config.ScanWidth);
        _cutOffEndSpinBoxNumber->setMaximum(av17config.ScanWidth);
        _cutOffStartSpinBoxNumber->setValue(_startVerticalProjCut, false);
        _cutOffEndSpinBoxNumber->setValue(endVerticalProjCut, false);
        break;
    case projectionHorizontal:
        _cutOffStartSpinBoxNumber->setMinimum(-38);
        _cutOffStartSpinBoxNumber->setMaximum(38);
        _cutOffEndSpinBoxNumber->setMinimum(-38);
        _cutOffEndSpinBoxNumber->setMaximum(38);
        _cutOffStartSpinBoxNumber->setValue(_startHorizontalProjCut, false);
        _cutOffEndSpinBoxNumber->setValue(endHorizontalProjCut, false);
        break;
    case projectionCrossSection:
        _cutOffStartSpinBoxNumber->setMinimum(0);
        _cutOffStartSpinBoxNumber->setMaximum(45);
        _cutOffEndSpinBoxNumber->setMinimum(0);
        _cutOffEndSpinBoxNumber->setMaximum(45);
        _cutOffStartSpinBoxNumber->setValue(_startCrossSectionProjCut, false);
        _cutOffEndSpinBoxNumber->setValue(endCrossSectionProjCut, false);
        break;
    }
    // redraw marker value control - start and end
    // replot defect with cut off
    // redraw projection plots
    replotProjections();
}

void Av17DefView::onNextButtonReleased()
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();
    av17data.setNeedInit(true);
    // save projectons plots
    setControlsAreaVisible(false);

    QwtPlotRenderer renderer;
    QImage* image = new QImage(_railLH->width(), _railLH->height(), QImage::Format_ARGB32);

    //    RailLH->replot();
    //    renderer.renderDocument(RailLH, "d://av31//test.png", QSizeF(150,80));
    //    renderer.renderDocument(RailLH, "d://av31//test.png", QSizeF(RailLH->width(), RailLH->height()),1);
    //    renderer.exportTo(RailLH, "testLH.png");
    //    renderer.exportTo(RailLW, "testLW.png");
    //    renderer.exportTo(RailWH, "testWH.png");
    //    renderer.exportTo(RailLH, "test.pdf");

    renderer.renderTo(_railLH, *image);
    QBuffer buffer(&av17data.getBaLH());
    buffer.open(QIODevice::WriteOnly);
    image->save(&buffer, "PNG");  // writes image into ba in PNG format
    delete image;

    image = new QImage(_railLW->width(), _railLW->height(), QImage::Format_ARGB32);
    renderer.renderTo(_railLW, *image);
    QBuffer buffer2(&av17data.getBaLW());
    buffer2.open(QIODevice::WriteOnly);
    image->save(&buffer2, "PNG");  // writes image into ba in PNG format
    delete image;

    image = new QImage(_railWH->width(), _railWH->height(), QImage::Format_ARGB32);
    renderer.renderTo(_railWH, *image);

    QBuffer buffer3(&av17data.getBaWH());
    buffer3.open(QIODevice::WriteOnly);
    image->save(&buffer3, "PNG");  // writes image into ba in PNG format
    delete image;


    image = new QImage(_bscan->width(), _bscan->height(), QImage::Format_ARGB32);
    _bscan->render(image, QPoint(), QRegion(0, 0, _bscan->width(), _bscan->height()));
    QBuffer bufferB(&av17data.getBaBScan());
    bufferB.open(QIODevice::WriteOnly);
    image->save(&bufferB, "PNG");  // writes image into ba in PNG format
                                   //    image->save("bscan0.png","PNG");
    delete image;

    // save to file
    av17config.threshold = qRound(_thresholdSpinBoxNumber->value());

    setControlsAreaVisible(true);

    emit doNextButtonPressed();
}

void Av17DefView::retranslateUi()
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    _thresholdSpinBoxNumber->setCaption(tr("Threshold"));
    _thresholdSpinBoxNumber->refresh();
    _cutOffStartSpinBoxNumber->setCaption(tr("Start"));
    _cutOffStartSpinBoxNumber->setSuffix(tr("mm"));
    _cutOffStartSpinBoxNumber->refresh();
    _cutOffEndSpinBoxNumber->setCaption(tr("End"));
    _cutOffEndSpinBoxNumber->setSuffix(tr("mm"));
    _cutOffEndSpinBoxNumber->refresh();
    _cutOff_ProjectionList->setCaption(tr("Projection"));
    _cutOff_ProjectionList->clear();
    _cutOff_ProjectionList->addItem(tr("Vertical"), projectionVertical);
    _cutOff_ProjectionList->addItem(tr("Horizontal"), projectionHorizontal);
    _cutOff_ProjectionList->addItem(tr("Cross-section"), projectionCrossSection);
    _cutOff_OnOffList->setCaption(tr("Cutting off"));  //"Clipping"
    _cutOff_OnOffList->clear();
    _cutOff_OnOffList->addItem(tr("On"), true);
    _cutOff_OnOffList->addItem(tr("Off"), false);

    _nextButton->setText(tr("Next"));
    if (av17config.WorkRail == 0) {
        _railLW->setFooter(QwtText(tr("0°, left")));
    }
    else {
        _railLW->setFooter(QwtText(tr("0°, right")));
    }

    _thresholdBScanSpinBoxNumber->setCaption(tr("Threshold B"));
    _thresholdBScanSpinBoxNumber->setSuffix(tr("dB"));
    _thresholdBScanSpinBoxNumber->refresh();
}

void Av17DefView::replotProjections()
{
    _railLH->replot();
    _railLW->replot();
    _railWH->replot();
}

void Av17DefView::loadBScan0Chan()
{
    Core& core = Core::instance();

    if (_bscan != NULL) {
        TAv17Config& av17config = HeadScannerData::instance().getConfig();
        A17Data& av17data = HeadScannerData::instance().getData();

        _bscan->clearSignals();

        eDeviceSide workrail;
        av17config.WorkRail = restoreHeadScannerWorkRail();
        if (av17config.WorkRail == 0) {
            workrail = dsLeft;
        }
        else if (av17config.WorkRail == 1) {
            workrail = dsRight;
        }
        else {
            workrail = dsNone;
        }
        _bscan->setWorkRail(workrail);
        if (av17config.WorkRail == 0) {
            _railLW->setFooter(QwtText(tr("Channel 0°, left")));
        }
        else {
            _railLW->setFooter(QwtText(tr("Channel 0°, right")));
        }

        bool isBehind = restoreRuLocation();
        float c;
        if (isBehind) {
            c = OffsetRuBehind;
        }
        else {
            c = OffsetRuBetween;
        }

        int zeroPoint = core.registration().GetCurSysCoord() - qRound((abs(av17data.XDefOffset) * _scanStep + c) / encoderCorrection());
        _bscan->setBRange(qRound(zeroPoint - 100 / encoderCorrection()), qRound(zeroPoint + 100 / encoderCorrection()));

        int coord = core.registration().GetMaxDisCoord();
        int from = coord - 8000;
        if (from < 0) {
            from = 0;
        }

        BScanDisplaySpan span;
        if (core.registration().getSpan(from, coord, span)) {
            _bscan->onSpan(span);
        }

        const QModelIndex& index = Core::instance().channelsController()->modelIndexByCidSideAndGateIndex(N0EMS, workrail, 0);  // side
        if (index.isValid()) {
            int StartBscangate = _channelsModel->data(index, StartBscangateRole).toInt();
            int EndBscangate = _channelsModel->data(index, EndBscangateRole).toInt();
            _bscan->setYRange(StartBscangate, EndBscangate);
        }
        int startGate = 0, endGate = 0, startGateR = 0, endGateR = 0;
        Core::instance().channelsController()->getGatesForMsmChannel(startGate, endGate, startGateR, endGateR);
        if (workrail == dsRight) {
            startGate = startGateR;
            endGate = endGateR;
        }
        _bscan->setMSMStrob(startGate, endGate);
    }
}

void Av17DefView::setVisible(bool visible)
{
    if (visible == true) {
        A17Data& av17data = HeadScannerData::instance().getData();
        if (InitDef(qRound(_thresholdSpinBoxNumber->value()))) {
            _defitemLH->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
            _defitemLW->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
            _defitemWH->Init(_R3D, _max_h, _max_x, _max_l, _min_h, _min_x, _min_l);
            _railLH->replot();
            _railLW->replot();
            _railWH->replot();
        }
        av17data.setNeedInit(false);
    }
    QWidget::setVisible(visible);
}

bool Av17DefView::InitDef(int threshold)
{
    A17Data& av17data = HeadScannerData::instance().getData();

    if (_R3D != NULL) {
        delete _R3D;
    }
    _R3D = new T3DRecons;

    if (_R3D == NULL) {
        return false;
    }

    _R3D->SetData(&av17data);

    _R3D->Make(threshold);

    _max_h = 0;
    _max_x = 0;
    _max_l = 0;

    _min_h = static_cast<int>(_R3D->FPixels3D.size());        // 0;
    _min_x = static_cast<int>(_R3D->FPixels3D[0].size());     // 0;
    _min_l = static_cast<int>(_R3D->FPixels3D[0][0].size());  // 0;

    int hsize = static_cast<int>(_R3D->FPixels3D.size());

    for (int h = 0; h < hsize; h++) {
        int xsize = static_cast<int>(_R3D->FPixels3D[h].size());
        for (int x = 0; x < xsize; x++) {
            int lsize = static_cast<int>(_R3D->FPixels3D[h][x].size());
            for (int l = 0; l < lsize; l++) {
                if (_R3D->FPixels3D[h][x][l]) {
                    _max_h = std::max(_max_h, h);
                    _min_h = std::min(_min_h, h);

                    _max_x = std::max(_max_x, x);
                    _min_x = std::min(_min_x, x);

                    _max_l = std::max(_max_l, l);
                    _min_l = std::min(_min_l, l);
                }
            }
        }
    }
    return true;
}

void DefItemLW::Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl)
{
    _R3D = Recons3D;
    _min_h = minh;
    _min_l = minl;
    _min_x = minx;
    _max_h = maxh;
    _max_l = maxl;
    _max_x = maxx;
}

// void DefItemLW::DrawFlaw(QPainter *painter) const
DefItemLW::DefItemLW()
    : _R3D(0)
    , _max_h(0)
    , _max_x(0)
    , _max_l(0)
    , _min_h(0)
    , _min_x(0)
    , _min_l(0)
{
}

void DefItemLW::draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const
{
    Q_UNUSED(canvasRect);
    painter->setBrush(Qt::SolidPattern);

    for (int h = _max_h; h >= _min_h; h--) {
        QColor cl;
        if (_max_h == _min_h) {
            cl = HLStoRGB(165, 116, 218);
        }
        else {
            cl = HLStoRGB(165 - (165 * (_max_h - h) / (_max_h - _min_h)), 116, 218);
        }
        for (int x = _min_x; x <= _max_x; x++) {
            for (int l = _min_l; l <= _max_l; l++) {
                Q_ASSERT(_R3D);
                if (_R3D->FPixels3D.at(h).at(x).at(l)) {
                    QRectF Rt(_R3D->AlongPos_Idx_to_mm(l), _R3D->OffsetPos_Idx_to_mm(x), 1, 1);
                    painter->fillRect(xMap.transform(xMap, yMap, Rt), cl);
                }
            }
        }
    }
}

void DefItemWH::Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl)
{
    _R3D = Recons3D;
    _min_h = minh;
    _min_l = minl;
    _min_x = minx;
    _max_h = maxh;
    _max_l = maxl;
    _max_x = maxx;
}

DefItemWH::DefItemWH()
    : _R3D(0)
    , _max_h(0)
    , _max_x(0)
    , _max_l(0)
    , _min_h(0)
    , _min_x(0)
    , _min_l(0)
{
}

void DefItemWH::draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const
{
    Q_UNUSED(canvasRect);
    A17Data& av17data = HeadScannerData::instance().getData();

    painter->setPen(QPen(Qt::SolidPattern, 1));
    painter->drawLine(qRound(xMap.transform(-20.0f)), qRound(yMap.transform(0.0f)), qRound(xMap.transform(20.0f)), qRound(yMap.transform(0.0f)));
    painter->drawLine(qRound(xMap.transform(36.0f)),  //прав.бок
                      qRound(yMap.transform(9.5f)),
                      qRound(xMap.transform(37.3f)),
                      qRound(yMap.transform(35.6f)));

    QRectF Rt(3.0f, 0.0f, 32.7f, 19.0f);
    painter->drawArc(xMap.transform(xMap, yMap, Rt), 0, qRound(90.0f * 16.0f));

    painter->drawLine(qRound(xMap.transform(37.15f)),  //прав подголов.
                      qRound(yMap.transform(35.6f)),
                      qRound(xMap.transform(12.5f)),
                      qRound(yMap.transform(42.0f)));

    painter->drawLine(qRound(xMap.transform(-36.0f)),  //лев. бок
                      qRound(yMap.transform(9.5f)),
                      qRound(xMap.transform(-37.3f)),
                      qRound(yMap.transform(35.6f)));

    Rt = QRectF(-3.0f, 0.0f, -33.0f, 19.0f);
    painter->drawArc(xMap.transform(xMap, yMap, Rt), qRound(90.0f * 16.0f), qRound(90.0f * 16.0f));

    painter->drawLine(qRound(xMap.transform(-37.0f)),  //лев.подголов.
                      qRound(yMap.transform(35.6f)),
                      qRound(xMap.transform(-12.5f)),
                      qRound(yMap.transform(42.0f)));


    for (int h = _min_h; h <= _max_h; h++) {
        float yMM = av17data.Y_Idx_to_mm(h);
        for (int x = _min_x; x <= _max_x; x++) {
            int offsetMM = _R3D->OffsetPos_Idx_to_mm(x);
            for (int l = _max_l; l >= _min_l; l--) {
                Q_ASSERT(_R3D);
                if (_R3D->FPixels3D.at(h).at(x).at(l)) {
                    QColor cl;
                    if (_max_l == _min_l) {
                        cl = HLStoRGB(165, 116, 218);
                    }
                    else {
                        cl = HLStoRGB(165 - (165 * (_max_l - l) / (_max_l - _min_l)), 116, 218);
                    }

                    QRectF newRect(offsetMM, yMM, 1, 1);
                    painter->fillRect(xMap.transform(xMap, yMap, newRect), cl);
                }
            }
        }
    }
}

void DefItemLH::Init(T3DRecons* Recons3D, int maxh, int maxx, int maxl, int minh, int minx, int minl)
{
    _R3D = Recons3D;
    _min_h = minh;
    _min_l = minl;
    _min_x = minx;
    _max_h = maxh;
    _max_l = maxl;
    _max_x = maxx;
}

DefItemLH::DefItemLH()
    : _R3D(0)
    , _max_h(0)
    , _max_x(0)
    , _max_l(0)
    , _min_h(0)
    , _min_x(0)
    , _min_l(0)
{
}

void DefItemLH::draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const
{
    Q_UNUSED(canvasRect);

    A17Data& av17data = HeadScannerData::instance().getData();

    painter->setBrush(Qt::SolidPattern);

    const QColor& defColor = HLStoRGB(165, 116, 218);

    for (int h = _min_h; h <= _max_h; ++h) {
        const std::vector<std::vector<bool>>& hTemp = _R3D->FPixels3D.at(h);
        float hMM = av17data.Y_Idx_to_mm(h);
        for (int x = _min_x; x <= _max_x; ++x) {
            const std::vector<bool>& xTemp = hTemp.at(x);
            const QColor& setColor = _max_x == _min_x ? defColor : HLStoRGB(165 - (165 * (x - _min_x) / (_max_x - _min_x)), 116, 218);
            for (int l = _min_l; l <= _max_l; ++l) {
                if (xTemp.at(l)) {
                    painter->fillRect(xMap.transform(xMap, yMap, QRectF(_R3D->AlongPos_Idx_to_mm(l), hMM, 1, 1)), setColor);
                }
            }
        }
    }
}

DefItemBScan::DefItemBScan(QWidget* parent)
    : QWidget(parent)
    , Border(0)
    , _maxY(75)  // B_SCAN_MAX_DURATION
    , _minY(0)
    , _startB(0)
    , _endB(0)
    , _startGateMSM(0)
    , _endGateMSM(0)
    , _scanStep(restoreHorizontalScannerEncoderStep())
    , _threshold(3)
    , _isProbePulseFromAbove(restoreProbePulseLocation())
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    if (av17config.WorkRail == 0) {
        _workrail = dsLeft;
    }
    else if (av17config.WorkRail == 1) {
        _workrail = dsRight;
    }
    else {
        _workrail = dsNone;
    }

    _dotSize = static_cast<unsigned char>(restoreBScanDotSize());

    _sigSize = 200 / encoderCorrection() + 1;
    _sigarr.reserve(_sigSize);
    tDaCo_BScanSignals item;
    item.Count = 0;
    item.Channel = N0EMS;
    item.Side = _workrail;
    for (int i = 0; i < _sigSize; ++i) {
        _sigarr.push_back(item);
    }
}

DefItemBScan::~DefItemBScan()
{
    _sigarr.clear();
}

void DefItemBScan::clearSignals()
{
    size_t sigsize = _sigarr.size();
    for (size_t i = 0; i < sigsize; ++i) {
        _sigarr[i].Count = 0;
    }
}

void DefItemBScan::setThreshold(int thresholdDb)
{
    static int amplToDb[16] = {-12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
    if (thresholdDb > 15) {
        return;
    }
    unsigned char i = 0;
    if (thresholdDb > amplToDb[0]) {
        for (i = 1; i < 15; ++i) {
            if ((thresholdDb > amplToDb[i - 1]) && (thresholdDb <= amplToDb[i])) {
                break;
            }
        }
    }
    _threshold = i;
}

void DefItemBScan::setBRange(int startb, int endb)
{
    _startB = startb;
    _endB = endb;
    _sigarr.resize(endb - startb + 1);
}

void DefItemBScan::onDataContainerData(pDaCo_DateFileItem pDaCoItem) {}

void DefItemBScan::onSpan(const BScanDisplaySpan& span)
{
    for (const BScanDisplayData& currentItem : span) {
        int curcoord = currentItem.SysCrd;
        if (curcoord >= _startB && curcoord <= _endB) {
            if (currentItem.Dir > 0) {
                for (int coord = curcoord, i = currentItem.Dir; i > 0 && coord >= _startB; i--, coord--) {
                    _sigarr.at(coord - _startB).Count = 0;
                }
            }
            for (std::vector<tDaCo_BScanSignals>::const_iterator it = currentItem.Signals.begin(), total = currentItem.Signals.end(); it != total; ++it) {
                CID chan = it.operator*().Channel;
                eDeviceSide side = it.operator*().Side;
                if (chan == 1 && side == _workrail) {
                    _sigarr.at(curcoord - _startB) = it.operator*();
                }
            }
        }
    }
}

void DefItemBScan::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    Border = 0;
    QPainter painter(this);
    QRect r = QRect(Border, Border, width() - 1 - Border, height() - 1 - Border);

    painter.eraseRect(r);
    painter.fillRect(r, Qt::white);

    size_t sigsize = _sigarr.size();
    int drawHeight = height();
    int drawWidth = width();
    for (size_t i = 0; i < sigsize; ++i) {
        for (int j = 0; j < _sigarr[i].Count; ++j) {
            //
            unsigned char amplitude = _sigarr[i].Signals[j].Ampl & 0x0f;
            if (amplitude >= _threshold) {
                int depthF = _sigarr[i].Signals[j].Delay / 3.f;
                QColor col;
                if (depthF >= _startGateMSM && depthF < _endGateMSM)
                    col = Qt::red;  // Red
                else
                    col = Qt::blue;             // Blue
                if (!_isProbePulseFromAbove) {  // QRgb
                    if (depthF <= _maxY) {
                        depthF = _maxY - depthF;
                    }
                    else {
                        qDebug() << "addSignals Y Overflow, chan=" << 1 << " depth=" << depthF << ",_maxY=" << _maxY;
                        depthF = 0;
                    }
                }
                int pixY = drawHeight * depthF / _maxY;
                int pixX = i * drawWidth / sigsize;
                if (pixY < drawHeight) {
                    painter.drawPoint(pixX, pixY);
                    int sizeX = 1, sizeY = 1;
                    if (pixY + 1 < drawHeight) {
                        sizeY = 2;
                    }
                    if (pixX + 1 < drawWidth) {
                        sizeX = 2;
                    }
                    painter.fillRect(pixX, pixY, sizeX, sizeY, col);
                }
            }
        }
    }
}
