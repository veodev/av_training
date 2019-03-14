#ifndef BSCANPAGE_H
#define BSCANPAGE_H

#include <QWidget>
#include <QSharedPointer>
#include <QMap>
#include <QHBoxLayout>
#include <QLabel>

#include "coredefinitions.h"
#include "modeltypes.h"
#include "styles.h"
#include "bscanmovementcontrols.h"
#include "spinboxcolor.h"
#include "dc_definitions.h"
#include "ChannelsIds.h"

namespace Ui
{
class BScanPage;
}

class SpinBoxNumber;
class SpinBoxList;
class QPushButton;
class QAbstractItemModel;
class BScanTape;
class BScanPlot;

struct ItemOfMap
{
    unsigned char edge;
    unsigned char color;
    unsigned char tapeNumber;
    unsigned char delayMultiply;
};

class BScanPage : public QWidget
{
    Q_OBJECT

    struct CurveByDirectionAndEdge
    {
        int tapeIndex;
        int curveNumber;
        Direction direction;
        enum EdgesType
        {
            NoEdgeType = 0,
            WorkEdgeType = 1,
            UnworkEdgeType = 2,
        };
        EdgesType edge;
    };

public:
    explicit BScanPage(QWidget* parent = 0);
    ~BScanPage();

    void setDotSize(int value);

    bool controlsAreaVisible();
    void setControlsAreaVisible(bool isVisible = false);
    void setModel(QAbstractItemModel* model);
    const BScanPlot& bScanPlotByTapeIndex(unsigned int tapeIndex) const;
    void resetTapeStates();
    void addMarker(const QColor& color, const QString& markerText, DeviceSide side = NoneDeviceSide, int vOffset = 0);
    void addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, DeviceSide side, CID channel);
    void resetBscans();
    void setProbePulseLocation(bool isProbePulseFromAbove);
    void plugTheBscan(bool isPlug);
    void setBottomSignalWigetVisible(bool state);
    void setMovementsMode(bool mode);

    bool getImportaintAreaIndication() const;
    void setImportaintAreaIndication(bool importaintAreaIndication);
    void setImportaintAreaOpacity(float value);
    void setImportaintAreaColor(const QColor& color);
    void enableParallelVideoBrowsing(bool isEnabled);
    void setMode(bool mode);

public slots:
    void detachModel();
    void setIncreaseTracks(bool isIncreaseTracks);
    void setAdditiveForSens(int additiveForSens);
    void onHotKeyReleased(Actions action);

signals:
    void doBScanPlotSelected(int tapeIndex, int index);
    void fpsChanged(double value);
    void doChangeDrawMode(int mode);

protected:
    void setupUi();
    void retranslateUi();
    void retranslateRailBscan();
    void retranslateEdgeBscan();
    void retranslateDirectionBscan();
    void retranslateDrawModes();
    void retranslateColorSchemes();
    bool retranslateNavigation();
    bool event(QEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void setSeparatorVisible(bool value);
    void applyTapesVisibleFilters();
    void applyDirectionAndEdgeFilter();

protected slots:
    virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void setVisible(bool visible);

private slots:
    void onChangeRailsBscan(int index, const QString& value, const QVariant& userData);
    void onThresholdBscanChanged(qreal value);
    void on_changeBscanScale(const qreal& value);
    void onChannelsByDirectionChanged(int index, const QString& value, const QVariant& userData);
    void onMagnifierChanged(int index, const QString& value, const QVariant& userData);
    void onEdgeRailBscan(int index, const QString& value, const QVariant& userData);
    void onDrawModeChanged(int index, const QString& value, const QVariant& userData);
    void onColorSchemeChanged(int index, const QString& value, const QVariant& userData);
    void OnNavigationChanged(int index, const QString& value, const QVariant& userData);

    void onTapeSelected(int tapeIndex, int position, int side);
    void replotTimeout();
    void onStartBoltJoint();
    void onStopBoltJoint();
    void onSetMinRegistrationThreshold(int value);
    void onDataContainerData(pDaCo_DateFileItem pDaCoItem);
    void fillTapes();
    void onMovementModeChanged(bool mode);
    void onMovementLeft();
    void onMovementRight();

    void onBottomSignalAmpl(QSharedPointer<int>);

signals:
    void bscanScaleChanged(double value);

private:
    void drawSpan(BScanDisplaySpan& span);
    void drawLabel(const tDaCo_Label& label, DeviceSide side);
    void drawEvent(unsigned char event);
    void drawData(const tDaCo_Data& data, int disCoord);
    QString getMediaName(tMediaType type);
    QString getCoordMetric(int km, int m);
    QString getCalibrationQualityMsg(tQualityCalibrationRecType type);
    inline bool getInversion(CID chan)
    {
#if defined TARGET_AVICONDBs
        bool invert = false;
        // Ось сигнала В-развертки для канала N55MSLU направлена сверху вниз
        // а для канала N55MSLW снизу  независимо от режима В - развертки "зондирующий импульс сверху/снизу.
        if (ui->bScanTape->getProbePulseLocation()) {
            if (chan == N55MSLW) {  // Преворот оси Y В-развертки для канала N55MSLW
                invert = true;
            }
            else {
                invert = false;
            }
        }
        else {
            if (chan == N55MSLU) {  // // Преворот оси Y В-развертки для канала N55MSLU
                invert = true;
            }
            else {
                invert = false;
            }
        }
        return invert;
#else
        Q_UNUSED(chan);
        return false;
#endif
    }
    inline bool isChannelZero(CID chan)
    {
#if defined TARGET_AVICONDB
        return chan == 61;
#else
        return chan == N0EMS;
#endif
    }

private:
    bool _isMarkersTextVisible;
    bool _isIncreaseTracks;
    bool _viewMode;
    bool _isProbePulseFromAbove;
    bool _importaintAreaIndication;
    bool _isParallelVideoBrowsing;

    int _oldDisCoord;
    int _lastDisCoord;
    int _screensMovementOffset;

    int _rectID;
    int _additiveForSens;
    int _currentMagnifierFilter;
    float _bscanScale;

    float _encoderCorrection;
    CID _cid;
    DeviceSide _currentRailFilter;
    DeviceSide _deviceSide;

    QVariant _currentDirection;
    QVariant _currentEdge;

    Ui::BScanPage* ui;
    SpinBoxNumber* _displayThresholdBscan;
    SpinBoxNumber* _scaleBscan;
    SpinBoxList* _railBscan;

    SpinBoxList* _channelsByDirectionSpinBox;
    SpinBoxList* _edgeRailBscan;
    SpinBoxList* _magnifierBscan;
    SpinBoxList* _navigationBscan;

    BScanMovementControls* _movementControls;
    SpinBoxList* _drawModeSpinBox;
    SpinBoxList* _colorSchemeSpinBox;

    QAbstractItemModel* _model;
    QTimer* _replotTimer;

    QColor _labelColor;
    QColor _autoMarkColor;

    std::vector<BScanTape*> _bScanTapes;
    QList<CurveByDirectionAndEdge> _curvesByDirectionAndEdge;
    QList<int> _tapesForBoltJoinMap;
    QList<QWidget*> _separators;
    QMap<DeviceSide, QList<bool>> _railFilters;
    QMap<int, ItemOfMap> _channelsMap;
    QList<QList<bool>> _magnifierFilters;

    bool _fileOpened;

    void checkInvalidTapes();
    void plotPart(int center);
    void replotViewMode();
    void replotRealtimeMode();
};

#endif  // BSCANPAGE_H
