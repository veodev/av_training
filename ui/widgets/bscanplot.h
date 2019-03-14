#ifndef BSCANPLOT_H
#define BSCANPLOT_H

#include <QVector>
#include <QWidget>
#include <QtMath>
#include "dc_definitions.h"
//#include <QElapsedTimer>
#include <deque>
#include <array>
#include <QPainter>

//#define BSCAN_PROFILER

const static unsigned char B_SCAN_MAX_DURATION = 255;
const static unsigned char B_SCAN_REPAINT_COUNTER = 60;
const static unsigned int B_SCAN_BYTES_PER_PIXEL = 2;
const static unsigned int B_SCAN_OFFSET = 520;
#if defined(IMX_DEVICE)
const static unsigned int B_SCAN_PLOT_WIDTH = 640;
#else
const static unsigned int B_SCAN_PLOT_WIDTH = 4040;
#endif

const static unsigned short B_SCAN_COLOR_1 = 0xF800;  // RED
const static unsigned short B_SCAN_COLOR_2 = 0x001F;  // BLUE
const static unsigned short B_SCAN_COLOR_3 = 0x4EFB;  // CYAN
const static unsigned short B_SCAN_COLOR_4 = 0x3E49;  // MAGENTA

const static unsigned short B_SCAN_COLOR_R = 0x9800;  // RED
const static unsigned short B_SCAN_COLOR_G = 0x07E0;  // GREEN
const static unsigned short B_SCAN_COLOR_Y = 0xFFE0;  // YELLOW
const static unsigned short B_SCAN_COLOR_B = 0x001F;  // BLUE

const static unsigned short B_SCAN_BACKGROUND_FORWARD = 0xFFFF;
const static unsigned short B_SCAN_BACKGROUND_BACKWARD = 0xFFE0;

class BScanPlot : public QWidget
{
    Q_OBJECT

    struct DisplayMark
    {
        int _posx;
        int _offset;
        int _textWidth;
        QColor _color;
        QString _text;
        bool _visible;
    };

    struct DisplayRect
    {
        int _posx;
        int _width;
        unsigned short _posy;
        unsigned short _heght;
    };

    struct Zone
    {
        bool _direction;
        unsigned char _size;
        Zone(bool direction, unsigned short size)
            : _direction(direction)
            , _size(size)
        {
        }
    };

    enum BScanDrawMode
    {
        BScanNormalDrawMode = 0,
        BScanCScanDrawMode = 1,
        BScanColorDrawMode = 2,
        BScanIndividualDrawMode = 3
    };

    using BScanColorScheme = std::array<std::array<unsigned short, 4>, 3>;
    using BScanPixelLine = std::array<unsigned short, B_SCAN_PLOT_WIDTH>;

private:
    inline void drawSignal(unsigned char depth, unsigned short color, unsigned int pixelLeft)
    {
        const unsigned int pixY = static_cast<unsigned int>(depth * _heightScale);

        if ((pixY + _dotSize) < _height) {
            switch (_dotSize) {
            case 3:
                _pixels[pixY + 2][pixelLeft] = color;
                //[[fallthrough]];
            case 2:
                _pixels[pixY + 1][pixelLeft] = color;
                //[[fallthrough]];
            case 1:
                _pixels[pixY][pixelLeft] = color;
                break;
            }
        }
    }

public:
    explicit BScanPlot(QWidget* parent = 0);
    virtual ~BScanPlot();

    void clone(const BScanPlot& obj, const QVector<int>& indexes);

    void setYRange(unsigned char minY, unsigned char maxY);

    bool getLocationProbePulse() const;
    void setThresholdDb(uchar thresholdDb);
    void setProbePulseLocation(bool isProbePulseFromAbove);
    void changeScale(float value);
    void addSignalsN0EMS(const tDaCo_BScanSignalList& sig, unsigned short gatestart, unsigned short gateend, unsigned char count, unsigned int multiply, unsigned char index, bool invertYScale = false);
    void addSignalsNORMAL(const tDaCo_BScanSignalList& sig, unsigned char count, unsigned int multiply, unsigned char index, bool invertYScale = false);
    void nextStep(signed char direction);
    void setDotSize(unsigned char size);
    void setVisibleDotCurve(int index, bool isVisible = true);
    void setVisibleAllDotCurves(bool isVisible);
    void addMark(const QColor& color, const QString& text, bool visible, int vOffset);
    void clearPlot();

    bool getIsInvalid() const;
    void setIsInvalid(bool isInvalid);

    void setAutoMarksColor(const QColor& color);

    void addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, unsigned char multiply);

    void setOpacity(int opacity);
    void setEncoderCorrection(float encoderCorrection);

    void setCurrentColorScheme(const BScanColorScheme& currentColorScheme);

    void setForwardColor(unsigned short forwardColor);
    void setBackwardColor(unsigned short backwardColor);
    void setColorsForIndex(unsigned char index, unsigned short normalColor, unsigned short specialColor, unsigned short individualColor);

    void setDrawMode(int mode);

    unsigned short getForwardColor() const;
    unsigned short getBackwardColor() const;

private:
    void drawMarks(QPainter& painter);
    void shiftMarksAndRemove();
    inline unsigned short getColorForCurrentScheme(unsigned char special, unsigned char color)
    {
        return _currentColorScheme[special][color];
    }
    unsigned short getColorN0EMS(unsigned short gatestart, unsigned short gateend, unsigned char depth, unsigned char amplitude, unsigned char index);
    unsigned short getColorNormal(unsigned char amplitude, unsigned char index);
    void resizeHeight(int newHeight);
    void resizeWidth(int newWidth);

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

public slots:
    void replot();

private:
    bool _isInvalid;
    bool _isProbePulseFromAbove;
    unsigned char _threshold;
    unsigned char _dotSize;
    unsigned char _opacity;
    unsigned char _bscanScale;
    unsigned char _minY;
    unsigned char _maxY;

    unsigned short _forwardColor;
    unsigned short _backwardColor;

    unsigned short _height;
    unsigned short _width;
    unsigned short _screenWidth;
    unsigned short _invisibleAreaSize;

    unsigned short _currentShift;
    unsigned short _steps;

    float _encoderStepPerPixel;
    float _encoderCorrection;
    float _heightScale;

    BScanDrawMode _currentDrawMode;

    std::array<bool, 4> _drawCurves;
    BScanColorScheme _currentColorScheme;

    std::vector<BScanPixelLine> _pixels;
    std::vector<DisplayMark> _displayMarks;

    QFont _marksFont;
    QFontMetrics _marksFontMetrics;
    QColor _autoMarksColor;
    QImage _image;
    QRect _imageRect;
    QPainter _painter;

    // QElapsedTimer _fpsTimer;
#ifdef BSCAN_PROFILER
    std::array<int, 32> _profilerFilter;
    unsigned int _filterCurPos;
#endif
    void moveEverything();
    void resetSteps();
};

#endif  // BSCANPLOT_H
