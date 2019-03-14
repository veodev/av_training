#include <QAbstractItemModel>

#include "bscantape.h"
#include "ui_bscantape.h"
#include "bscanplot.h"

#include "bscangates.h"
#include "roles.h"
#include "debug.h"
#include "colors.h"
#include "ChannelsIds.h"

static const QVector<int> rolesStartEndColor(QVector<int>() << StartGateRole << EndGateRole << BScanNormalColorRole);

BScanTape::BScanTape(QWidget* parent)
    : QWidget(parent)
    , _isProbePulseFromAbove(true)
    , _maxGateSize(0)
    , ui(new Ui::BScanTape)
    , _model(nullptr)
{
    ui->setupUi(this);
    setMaximumWidth(B_SCAN_PLOT_WIDTH);
}

BScanTape::~BScanTape()
{
    delete ui;
}

void BScanTape::addSignalsN0EMS(const tDaCo_BScanSignalList& sig, unsigned short gatestart, unsigned short gateend, unsigned char count, unsigned int multiply, unsigned char color, bool invertYScale)
{
    ui->bScanPlot->addSignalsN0EMS(sig, gatestart, gateend, count, multiply, color, invertYScale);
}

void BScanTape::addSignalsNORMAL(const tDaCo_BScanSignalList& sig, unsigned char count, unsigned int multiply, unsigned char color, bool invertYScale)
{
    ui->bScanPlot->addSignalsNORMAL(sig, count, multiply, color, invertYScale);
}

void BScanTape::replotBScanPlot()
{
    ui->bScanPlot->update();
}

void BScanTape::nextStep(signed char direction)
{
    ui->bScanPlot->nextStep(direction);
}

void BScanTape::setCurrentColorScheme(const std::array<std::array<unsigned short, 4>, 3>& currentColorScheme)
{
    ui->bScanPlot->setCurrentColorScheme(currentColorScheme);
}

void BScanTape::setForwardColor(unsigned short forwardColor)
{
    ui->bScanPlot->setForwardColor(forwardColor);
    const QColor& col = getColorFromRGB565(forwardColor);
    ui->forwardGates->setBackgroundColor(col);
    ui->backwardGates->setBackgroundColor(col);
}

void BScanTape::setBackwardColor(unsigned short backwardColor)
{
    ui->bScanPlot->setBackwardColor(backwardColor);
}

unsigned short BScanTape::getForwardColor() const
{
    return ui->bScanPlot->getForwardColor();
}

unsigned short BScanTape::getBackwardColor() const
{
    return ui->bScanPlot->getBackwardColor();
}

void BScanTape::setDrawMode(int mode)
{
    ui->bScanPlot->setDrawMode(mode);
    if (mode == 3) {
        ui->forwardGates->setIsIndividualColors(true);
        ui->backwardGates->setIsIndividualColors(true);
    }
    else {
        ui->forwardGates->setIsIndividualColors(false);
        ui->backwardGates->setIsIndividualColors(false);
    }
}

void BScanTape::setPlotVisible(bool isVisible)
{
    ui->bScanPlot->setVisible(isVisible);
    ui->bScanPlot->update();
}


void BScanTape::cloneBScanPlot(const BScanPlot& obj, const QVector<int>& indexes)
{
    ui->bScanPlot->clone(obj, indexes);
}

void BScanTape::setThresholdDb(int thresholdDb)
{
    ui->bScanPlot->setThresholdDb(thresholdDb);
}

void BScanTape::setGatesScale(int maxEndGate)
{
    _maxGateSize = maxEndGate;
    ui->forwardGates->setMaxGateSize(maxEndGate);
    ui->backwardGates->setMaxGateSize(maxEndGate);
}

void BScanTape::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model != nullptr);
    _model = model;

    resetGates();

    int rowCount = _model->rowCount();
    const QModelIndex& index = _model->index(0, 0);
    if (rowCount == 2) {
        int gateCount = _model->data(index, GateCountRole).toInt();
        if (gateCount == 1) {
            ui->forwardGates->setGateLocation(BScanGates::Parallel);
            ui->backwardGates->setGateLocation(BScanGates::Parallel);
        }
        else if (gateCount == 2) {
            ui->forwardGates->setGateLocation(BScanGates::InLine);
            ui->backwardGates->setGateLocation(BScanGates::InLine);
        }
    }
    const QModelIndex& topLeft = index;
    const QModelIndex& bottomRight = _model->index(rowCount - 1, _model->columnCount() - 1);

    onDataChanged(topLeft, bottomRight, rolesStartEndColor);
}

void BScanTape::resetGates()
{
    ui->forwardGates->resetGates();
    ui->backwardGates->resetGates();
}

void BScanTape::setBScanVerticalScale(int minY, int maxY)
{
    ui->bScanPlot->setYRange(minY, maxY);
}

void BScanTape::setBScanHorizontalScale(float value)
{
    ui->bScanPlot->changeScale(value);
}

void BScanTape::setBscanCurveVisible(int index, bool isVisible)
{
    ui->bScanPlot->setVisibleDotCurve(index, isVisible);
}

void BScanTape::setAllBscanCurvesVisible(bool isVisible)
{
    ui->bScanPlot->setVisibleAllDotCurves(isVisible);
}

void BScanTape::setBScanDotSize(unsigned char size)
{
    ui->bScanPlot->setDotSize(size);
}

void BScanTape::setProbePulseLocation(bool isProbePulseFromAbove)
{
    _isProbePulseFromAbove = isProbePulseFromAbove;
    ui->bScanPlot->setProbePulseLocation(isProbePulseFromAbove);
    if (_model != nullptr) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, _model->columnCount() - 1);
        onDataChanged(topLeft, bottomRight, QVector<int>() << StartGateRole << EndGateRole);
    }
}

void BScanTape::clearTape()
{
    ui->bScanPlot->clearPlot();
}

bool BScanTape::getProbePulseLocation() const
{
    return ui->bScanPlot->getLocationProbePulse();
}

int BScanTape::getMaxGateSize() const
{
    return _maxGateSize;
}

void BScanTape::addMarker(const QColor& color, const QString& markerText, bool isLabelVisible, int vOffset)
{
    ui->bScanPlot->addMark(color, markerText, isLabelVisible, vOffset);
}

void BScanTape::addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, unsigned char multiply)
{
    ui->bScanPlot->addRectangle(offset, width, posy1, posy2, multiply);
}

void BScanTape::setThreshold(uchar value)
{
    ui->bScanPlot->setThresholdDb(value);
}

const BScanPlot& BScanTape::bScanPlot() const
{
    return *(ui->bScanPlot);
}

void BScanTape::setGateForDevice(const QModelIndex& index, int gateCount, unsigned short startGate, unsigned short endGate, int row, int column, Direction direction)
{
#ifdef TARGET_AVICONDB
    if (_isProbePulseFromAbove) {
        if (_model->data(index, ChannelIdRole).toInt() != N55MSLW)
            setGate(gateCount, _maxGateSize - startGate, _maxGateSize - endGate, row, column, direction);
        else
            setGate(gateCount, startGate, endGate, row, column, direction);
    }
    else {
        if (_model->data(index, ChannelIdRole).toInt() == N55MSLU)
            setGate(gateCount, _maxGateSize - startGate, _maxGateSize - endGate, row, column, direction);
        else
            setGate(gateCount, startGate, endGate, row, column, direction);
    }
#else
    if (_isProbePulseFromAbove) {
        setGate(gateCount, _maxGateSize - startGate, _maxGateSize - endGate, row, column, direction);
    }
    else {
        setGate(gateCount, startGate, endGate, row, column, direction);
    }
#endif
}

void BScanTape::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    const int gateCount = _model->data(topLeft, GateCountRole).toInt();
    const int bottomRightRowIndex = bottomRight.row();
    const int bottomRightColumnIndex = bottomRight.column();
    for (int row = topLeft.row(); row <= bottomRightRowIndex; ++row) {
        for (int column = topLeft.column(); column <= bottomRightColumnIndex; ++column) {
            const QModelIndex& index = _model->index(row, column);
            if (!index.isValid()) {
                continue;
            }
            const QVariant& value = _model->data(index, FakedChannelRole);
            Q_ASSERT(value.isValid() == true);
            if (value.toBool()) {
                continue;
            }
            unsigned short startGate = _model->data(index, StartGateRole).toInt();
            unsigned short endGate = _model->data(index, EndGateRole).toInt();
            Direction direction = static_cast<Direction>(_model->data(index, DirectionRole).toInt());
            for (int role : roles) {
                if (role == StartGateRole || role == EndGateRole) {
                    setGateForDevice(index, gateCount, startGate, endGate, row, column, direction);
                }
                if (role == BScanNormalColorRole || role == BScanSpecialColorRole || role == BScanIndividualColorRole) {
                    unsigned char colorIndex = row * 2 + column;
                    unsigned short normalColor = _model->data(index, BScanNormalColorRole).toUInt();
                    unsigned short specialColor = _model->data(index, BScanSpecialColorRole).toUInt();
                    unsigned short individualColor = _model->data(index, BScanIndividualColorRole).toUInt();
                    eBScanColorDescr channelColorIndex = static_cast<eBScanColorDescr>(_model->data(index, BScanColorIndexRole).toUInt());
                    ui->bScanPlot->setColorsForIndex(colorIndex, normalColor, specialColor, individualColor);

                    if (channelColorIndex == cdTMDirect1) {
                        gateColor[0][0] = getColorFromRGB565(specialColor);
                        gateColor[0][1] = getColorFromRGB565(normalColor);
                        gateColor[0][2] = getColorFromRGB565(specialColor);
                        gateColor[0][3] = getColorFromRGB565(normalColor);
                        gateColor[1][0] = getColorFromRGB565(individualColor);
                        gateColor[1][1] = getColorFromRGB565(individualColor);
                        gateColor[1][2] = getColorFromRGB565(individualColor);
                        gateColor[1][3] = getColorFromRGB565(individualColor);
                    }
                    else {
                        if (gateCount == 2) {
                            gateColor[0][column] = getColorFromRGB565(normalColor);
                            gateColor[1][column] = getColorFromRGB565(individualColor);
                        }
                        else {
                            gateColor[0][colorIndex] = getColorFromRGB565(normalColor);
                            gateColor[1][colorIndex] = getColorFromRGB565(individualColor);
                        }
                    }
                    setGateForDevice(index, gateCount, startGate, endGate, row, column, direction);

                    ui->forwardGates->update();
                    ui->backwardGates->update();
                }
            }
        }
    }
}


DeviceSide BScanTape::getSide() const
{
    return _side;
}

void BScanTape::setSide(const DeviceSide& side)
{
    _side = side;
}

bool BScanTape::getIsInvalid() const
{
    return ui->bScanPlot->getIsInvalid();
}

void BScanTape::setIsInvalid(bool isInvalid)
{
    return ui->bScanPlot->setIsInvalid(isInvalid);
}

void BScanTape::setAutoMarksColor(const QColor& color)
{
    ui->bScanPlot->setAutoMarksColor(color);
}

void BScanTape::setOpacity(int opacity)
{
    ui->bScanPlot->setOpacity(opacity);
}

void BScanTape::setEncoderCorrection(float encoderCorrection)
{
    ui->bScanPlot->setEncoderCorrection(encoderCorrection);
}

void BScanTape::setGate(int gateCount, unsigned short startGate, unsigned short endGate, int row, int column, Direction direction)
{
    if (gateCount == 2) {
        if (direction == ForwardDirection) {
            ui->forwardGates->setGate(startGate, endGate, row, gateColor[0][column], gateColor[1][column]);
        }
        else if (direction == BackwardDirection) {
            ui->backwardGates->setGate(startGate, endGate, row, gateColor[0][column], gateColor[1][column]);
        }
    }
    else {
        if (direction == ForwardDirection) {
            ui->forwardGates->setGate(startGate, endGate, row, gateColor[0][row * 2 + column], gateColor[1][row * 2 + column]);
        }
        else if (direction == BackwardDirection) {
            ui->backwardGates->setGate(startGate, endGate, row, gateColor[0][row * 2 + column], gateColor[1][row * 2 + column]);
        }
    }

    if (direction == UnknownDirection) {
        if (column == 0) {
            ui->forwardGates->setGate(startGate, endGate, row, gateColor[0][row * 2 + column], gateColor[1][row * 2 + column]);
        }
        else if (column == 1) {
            ui->backwardGates->setGate(startGate, endGate, row, gateColor[0][column], gateColor[1][column]);
        }
    }
}
