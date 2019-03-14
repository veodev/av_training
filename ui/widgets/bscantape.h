#ifndef BSCANTAPE_H
#define BSCANTAPE_H

#include <QWidget>
#include "modeltypes.h"
#include "coredefinitions.h"
#include "dc_definitions.h"

#include <array>

namespace Ui
{
class BScanTape;
}

class BScanPlot;
class BScanGates;
class QAbstractItemModel;

class BScanTape : public QWidget
{
    Q_OBJECT

public:
    explicit BScanTape(QWidget* parent = nullptr);
    ~BScanTape();
    void addSignalsN0EMS(const tDaCo_BScanSignalList& sig, unsigned short gatestart, unsigned short gateend, unsigned char count, unsigned int multiply, unsigned char color, bool invertYScale);
    void addSignalsNORMAL(const tDaCo_BScanSignalList& sig, unsigned char count, unsigned int multiply, unsigned char color, bool invertYScale);

    void replotBScanPlot();
    void nextStep(signed char direction);
    void cloneBScanPlot(const BScanPlot& obj, const QVector<int>& indexes);
    void setThresholdDb(int thresholdDb);
    void setGatesScale(int maxEndGate);
    void setGatesLocation();
    void setModel(QAbstractItemModel* model);
    void resetGates();
    void setBScanVerticalScale(int minY, int maxY);
    void setThreshold(uchar value);
    const BScanPlot& bScanPlot() const;
    void setGate(int gateCount, unsigned short startGate, unsigned short endGate, int row, int column, Direction direction = UnknownDirection);
    void setBScanHorizontalScale(float value);
    void setBscanCurveVisible(int index, bool isVisible);
    void setAllBscanCurvesVisible(bool isVisible);
    void setBScanDotSize(unsigned char size);
    void setProbePulseLocation(bool isProbePulseFromAbove);

    void addMarker(const QColor& color, const QString& markerText, bool isLabelVisible = false, int vOffset = 0);
    void addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, unsigned char multiply);

    void clearTape();

    bool getProbePulseLocation() const;
    int getMaxGateSize() const;

    DeviceSide getSide() const;
    void setSide(const DeviceSide& side);

    bool getIsInvalid() const;
    void setIsInvalid(bool isInvalid);

    void setAutoMarksColor(const QColor& color);
    void setOpacity(int opacity);
    void setEncoderCorrection(float encoderCorrection);
    void setCurrentColorScheme(const std::array<std::array<unsigned short, 4>, 3>& currentColorScheme);
    void setForwardColor(unsigned short forwardColor);
    void setBackwardColor(unsigned short backwardColor);
    unsigned short getForwardColor() const;
    unsigned short getBackwardColor() const;
    void setDrawMode(int mode);
    void setPlotVisible(bool isVisible);

public slots:
    virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
    void setGateForDevice(const QModelIndex& index, int gateCount, unsigned short startGate, unsigned short endGate, int row, int column, Direction direction);

private:
    bool _isProbePulseFromAbove;
    unsigned short _maxGateSize;
    std::array<std::array<QColor, 4>, 2> gateColor;
    DeviceSide _side;
    Ui::BScanTape* ui;
    QAbstractItemModel* _model;
};

#endif  // BSCANTAPE_H
