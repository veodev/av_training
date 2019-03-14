#include "bscanplot.h"
#include "settings.h"
#include "debug.h"
#include "settings.h"
#include "colors.h"
#include <QElapsedTimer>
#include <QResizeEvent>
#include <algorithm>


#ifdef BSCAN_PROFILER
#include <chrono>
#endif


BScanPlot::BScanPlot(QWidget* parent)
    : QWidget(parent)
    , _isInvalid(true)
    , _isProbePulseFromAbove(restoreProbePulseLocation())
    , _threshold(0)
    , _dotSize(static_cast<unsigned char>(restoreBScanDotSize()))
    , _opacity(0)
    , _bscanScale(0)
    , _minY(0)
    , _maxY(B_SCAN_MAX_DURATION)
    , _forwardColor(B_SCAN_BACKGROUND_FORWARD)
    , _backwardColor(B_SCAN_BACKGROUND_BACKWARD)
    , _height(0)
    , _width(B_SCAN_PLOT_WIDTH)
    , _screenWidth(B_SCAN_OFFSET)
    , _invisibleAreaSize(0)
    , _currentShift(0)
    , _steps(0)
    , _encoderStepPerPixel(0)
    , _encoderCorrection(encoderCorrection())
    , _currentDrawMode(BScanNormalDrawMode)
    , _marksFontMetrics(_marksFont)
{
    _invisibleAreaSize = _width - _screenWidth;
    Q_ASSERT(_invisibleAreaSize > 0);

    _height = static_cast<unsigned short>(height());
    _heightScale = static_cast<float>(_height) / _maxY;
    _pixels.reserve(_height);
    _pixels.resize(_height);

    _image = QImage(reinterpret_cast<unsigned char*>(_pixels.data()), static_cast<int>(_width), static_cast<int>(_height), QImage::Format_RGB16);
    std::fill_n(reinterpret_cast<unsigned short*>(_pixels.data()), _width * _height, _forwardColor);
    _imageRect = QRect(0, 0, _screenWidth, _height);

    changeScale(static_cast<float>(restoreBscanScale()));

    setThresholdDb(0);
    _drawCurves[0] = true;
    _drawCurves[1] = true;
    _drawCurves[2] = true;
    _drawCurves[3] = true;

    _marksFont.setPointSize(12);
    _marksFontMetrics = QFontMetrics(_marksFont);

    _displayMarks.reserve(16);

    _currentShift = 0;

    _opacity = qRound(255.0f * (restoreOpacityImportantArea() / 100.0f));
    QColor autoColor(restoreColorImportantAreaColorR(), restoreColorImportantAreaColorG(), restoreColorImportantAreaColorB(), _opacity);
    _autoMarksColor = autoColor;

    for (auto& scheme : _currentColorScheme) {
        scheme[0] = B_SCAN_COLOR_1;
        scheme[1] = B_SCAN_COLOR_2;
        scheme[2] = B_SCAN_COLOR_3;
        scheme[3] = B_SCAN_COLOR_4;
    }
#ifdef BSCAN_PROFILER
    for (auto& value : _profilerFilter) {
        value = 0;
    }
    _filterCurPos = 0;

#endif
}

BScanPlot::~BScanPlot()
{
    _displayMarks.clear();
}

void BScanPlot::clone(const BScanPlot& obj, const QVector<int>& indexes)
{
    _isInvalid = true;
    _isProbePulseFromAbove = obj._isProbePulseFromAbove;
    _threshold = obj._threshold;
    _dotSize = obj._dotSize;
    _opacity = obj._opacity;
    _bscanScale = obj._bscanScale;
    _minY = obj._minY;
    _maxY = obj._maxY;
    _forwardColor = obj._forwardColor;
    _backwardColor = obj._backwardColor;
    _height = height();
    _width = B_SCAN_PLOT_WIDTH;
    _screenWidth = width();
    _imageRect = QRect(0, 0, _screenWidth, _height);
    _invisibleAreaSize = _width - _screenWidth;
    _currentShift = 0;
    _steps = 0;
    _encoderStepPerPixel = obj._encoderStepPerPixel;
    _encoderCorrection = obj._encoderCorrection;
    _heightScale = static_cast<float>(_height) / _maxY;
    _currentDrawMode = obj._currentDrawMode;
    for (auto& curve : _drawCurves) {
        curve = false;
    }
    for (auto index : indexes) {
        Q_ASSERT(index < 4);
        _drawCurves[index] = true;
    }
    _currentColorScheme = obj._currentColorScheme;
    _pixels.resize(_height);

    changeScale(static_cast<float>(restoreBscanScale()));

    _image = QImage(reinterpret_cast<unsigned char*>(_pixels.data()), static_cast<int>(_width), static_cast<int>(_height), QImage::Format_RGB16);
    replot();
}

void BScanPlot::setYRange(unsigned char minY, unsigned char maxY)
{
    _minY = minY;
    _maxY = maxY;
    _heightScale = static_cast<float>(_height) / _maxY;
    replot();
}

bool BScanPlot::getLocationProbePulse() const
{
    return _isProbePulseFromAbove;
}

void BScanPlot::setThresholdDb(uchar thresholdDb)
{
    _threshold = thresholdDb;
}

void BScanPlot::setProbePulseLocation(bool isProbePulseFromAbove)
{
    _isProbePulseFromAbove = isProbePulseFromAbove;
    replot();
}

void BScanPlot::changeScale(float value)
{
    //_encoderCorrection = static_cast<float>(encoderCorrection());
    _bscanScale = static_cast<unsigned char>(qRound(value * 10.0f));
    Q_ASSERT(_screenWidth != 0);
    Q_ASSERT(_encoderCorrection != 0.0f);
    _encoderStepPerPixel = ((value * 1000.0f / _encoderCorrection) / static_cast<float>(_screenWidth));
    replot();
}

void BScanPlot::setDrawMode(int mode)
{
    _currentDrawMode = static_cast<BScanDrawMode>(mode);
    replot();
}


unsigned short BScanPlot::getColorN0EMS(unsigned short gatestart, unsigned short gateend, unsigned char depth, unsigned char amplitude, unsigned char index)
{
    switch (_currentDrawMode) {
    case BScanNormalDrawMode:
        return getColorForCurrentScheme((depth >= gatestart && depth <= gateend) ? 0 : 1, index);
    case BScanCScanDrawMode:
        return HLStoRGB16(165 - 165 * amplitude / 16);
    case BScanColorDrawMode:
        return getGradientColor(amplitude / 15.0f, 0xFFFF, getColorForCurrentScheme(0, index));
    case BScanIndividualDrawMode:
        return getColorForCurrentScheme(2, index);
    }
}

unsigned short BScanPlot::getColorNormal(unsigned char amplitude, unsigned char index)
{
    switch (_currentDrawMode) {
    case BScanNormalDrawMode:
        return getColorForCurrentScheme(0, index);
    case BScanCScanDrawMode:
        return HLStoRGB16(165 - 165 * amplitude / 16);
    case BScanColorDrawMode:
        return getGradientColor(amplitude / 15.0f, 0xFFFF, getColorForCurrentScheme(0, index));
    case BScanIndividualDrawMode:
        return getColorForCurrentScheme(2, index);
    }
}

void BScanPlot::resizeHeight(int newHeight)
{
    _height = static_cast<unsigned short>(newHeight);
    _heightScale = static_cast<float>(_height) / _maxY;
    _pixels.resize(_height);
    _image = QImage(reinterpret_cast<unsigned char*>(_pixels.data()), static_cast<int>(_width), static_cast<int>(_height), QImage::Format_RGB16);
    std::fill_n(reinterpret_cast<unsigned short*>(_pixels.data()), _width * _height, _forwardColor);
    _imageRect = QRect(0, 0, _screenWidth, _height);
    _isInvalid = true;
}

void BScanPlot::resizeWidth(int newWidth)
{
    _screenWidth = static_cast<unsigned short>(newWidth);
    _imageRect = QRect(0, 0, _screenWidth, _height);
    _invisibleAreaSize = _width - _screenWidth;
    Q_ASSERT(_screenWidth != 0);
    Q_ASSERT(_encoderCorrection != 0.0f);
    _encoderStepPerPixel = ((_bscanScale * 100.0f / _encoderCorrection) / static_cast<float>(_screenWidth));
    _isInvalid = true;
}

void BScanPlot::addSignalsN0EMS(const tDaCo_BScanSignalList& sig, unsigned short gatestart, unsigned short gateend, unsigned char count, unsigned int multiply, unsigned char index, bool invertYScale)
{
    if (_drawCurves[index]) {
        const bool retranslateDepth = _isProbePulseFromAbove == invertYScale;
        const unsigned short pixelPos = _screenWidth + _currentShift;
        Q_ASSERT(pixelPos < _width);
        Q_ASSERT(multiply != 0);

        for (unsigned char i = 0; i < count; ++i) {
            const unsigned char amplitude = sig[i].Ampl;
            if (amplitude >= _threshold) {
                unsigned char depth = sig[i].Delay / multiply;
                if (depth > _maxY) {
                    depth = _maxY;
                }
                unsigned short color = getColorN0EMS(gatestart, gateend, depth, amplitude, index);

                if (retranslateDepth) {
                    depth = _maxY - depth;
                }
                drawSignal(depth, color, pixelPos);
            }
        }
    }
}

void BScanPlot::addSignalsNORMAL(const tDaCo_BScanSignalList& sig, unsigned char count, unsigned int multiply, unsigned char index, bool invertYScale)
{
    if (_drawCurves[index]) {
        const bool retranslateDepth = _isProbePulseFromAbove == invertYScale;
        const unsigned short pixelPos = _screenWidth + _currentShift;
        Q_ASSERT(pixelPos < _width);
        Q_ASSERT(multiply != 0);

        for (unsigned char i = 0; i < count; ++i) {
            const unsigned char amplitude = sig[i].Ampl;
            if (amplitude >= _threshold) {
                unsigned char depth = sig[i].Delay / multiply;
                Q_ASSERT(depth <= _maxY);
                unsigned short color = getColorNormal(amplitude, index);
                if (retranslateDepth) {
                    depth = _maxY - depth;
                }
                drawSignal(depth, color, pixelPos);
            }
        }
    }
}


void BScanPlot::moveEverything()
{
    if (_currentShift > 0) {
        // Shifting each row of the image to the left

        if (_currentShift > _invisibleAreaSize) {
            _currentShift = _invisibleAreaSize - 1;
        }

        for (BScanPixelLine& row : _pixels) {
            std::move(&row[_currentShift], &row[_currentShift + _screenWidth], row.begin());
        }

        // Shifting marks to the left and removing old ones
        if (!_displayMarks.empty()) {
            shiftMarksAndRemove();
        }
    }
}

void BScanPlot::resetSteps()
{
    // Do not reset steps if there were not enough steps for pixel offset to happen
    if (!((_steps > 0) && (_currentShift == 0))) {
        _steps = 0;
        _currentShift = 0;
    }
}

void BScanPlot::nextStep(signed char direction)
{
    const unsigned char absStep = static_cast<unsigned char>(std::abs(direction));
    const unsigned short color = (direction >= 1) ? _forwardColor : _backwardColor;
    unsigned short prevShift;
    if (_steps == 0) {
        prevShift = 0;
    }
    else {
        prevShift = _currentShift + 1;
    }
    _steps += absStep;
    Q_ASSERT(_encoderStepPerPixel != 0.0f);
    _currentShift = static_cast<unsigned short>((static_cast<float>(_steps) / _encoderStepPerPixel));
    if ((_currentShift + 1) < _invisibleAreaSize) {
        Q_ASSERT(prevShift <= (_currentShift + 1));
        if (prevShift != (_currentShift + 1)) {
            for (auto& row : _pixels) {
                std::fill(&row[_screenWidth + prevShift], &row[_screenWidth + _currentShift + 1], color);
            }
        }
        else {
            for (auto& row : _pixels) {
                row[_screenWidth + prevShift] = color;
            }
        }
    }
    else {
        moveEverything();
        resetSteps();
    }
}

void BScanPlot::setDotSize(unsigned char size)
{
    _dotSize = size;
    replot();
}

void BScanPlot::setVisibleDotCurve(int index, bool isVisible)
{
    Q_ASSERT(index < 4);
    _drawCurves[static_cast<unsigned int>(index)] = isVisible;
    replot();
}

void BScanPlot::setVisibleAllDotCurves(bool isVisible)
{
    for (int i = 0; i < 4; ++i) {
        _drawCurves[i] = isVisible;
    }
    replot();
}

void BScanPlot::addMark(const QColor& color, const QString& text, bool visible, int vOffset)
{
    DisplayMark mark;
    mark._posx = _screenWidth + _currentShift;
    mark._offset = vOffset;
    mark._text = text;
    mark._textWidth = _marksFontMetrics.width(text);
    mark._color = color;
    mark._visible = visible;
    _displayMarks.push_back(mark);
}


void BScanPlot::addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, unsigned char multiply)
{
    QRect rect;
    Q_ASSERT(_encoderStepPerPixel != 0.0f);
    Q_ASSERT(_height != 0);
    Q_ASSERT(_maxY != 0);
    rect.setX(_screenWidth + _currentShift - qMin(static_cast<unsigned int>(qFloor(static_cast<float>(offset) / _encoderStepPerPixel)), static_cast<unsigned int>(_screenWidth + _currentShift)));
    rect.setWidth(static_cast<float>(width) / _encoderStepPerPixel);
    if (rect.right() <= 0) {
        return;
    }
    Q_ASSERT(multiply != 0);
    unsigned char depth = posy1 / multiply;

    if (!_isProbePulseFromAbove) {
        depth = std::max(0, _maxY - depth);
    }

    unsigned short posY1 = static_cast<unsigned short>((depth / static_cast<float>(_maxY)) * static_cast<float>(_height));

    depth = posy2 / multiply;

    if (!_isProbePulseFromAbove) {
        depth = std::max(0, _maxY - depth);
    }

    unsigned short posY2 = static_cast<unsigned short>((depth / static_cast<float>(_maxY)) * static_cast<float>(_height));

    rect.setY(posY1 > posY2 ? posY2 : posY1);
    rect.setHeight(std::abs(posY1 - posY2));

    QPainter imagePainter(&_image);
    imagePainter.fillRect(rect, _autoMarksColor);
    imagePainter.end();
}

void BScanPlot::clearPlot()
{
    std::fill_n(reinterpret_cast<unsigned short*>(_pixels.data()), _width * _height, _forwardColor);
    _displayMarks.clear();
    _steps = 0;
    _currentShift = 0;
    replot();
}

void BScanPlot::drawMarks(QPainter& painter)
{
    painter.setFont(_marksFont);
    for (DisplayMark& mark : _displayMarks) {
        if (mark._posx > 0) {
            painter.setPen(QPen(mark._color));
            painter.drawLine(static_cast<int>(mark._posx), 0, static_cast<int>(mark._posx), static_cast<int>(_height));
            if (mark._visible) {
                int textPosX = static_cast<int>(mark._posx) - mark._textWidth - 5;
                int textPosY = (_height / 3) + mark._offset;
                QBrush brush(QColor(mark._color));
                painter.fillRect(QRect(textPosX, textPosY - 15, mark._textWidth, 18), brush);
                painter.setPen(QPen(QColor(255, 255, 255)));
                painter.drawText(textPosX, textPosY, mark._text);
            }
        }
    }
}

void BScanPlot::paintEvent(QPaintEvent* event)
{
    event->accept();
    // _fpsTimer.start();
    // Current shift in pixels
    moveEverything();
#ifdef BSCAN_PROFILER
    auto start = std::chrono::steady_clock::now();
#endif

    if (_painter.begin(this)) {
        // Qt::ColorOnly | Qt::ThresholdDither | Qt::AvoidDither | Qt::NoFormatConversion | Qt::NoOpaqueDetection
        _painter.drawImage(_imageRect, _image, _imageRect);

        // Drawing marks with text
        if (!_displayMarks.empty()) {
            drawMarks(_painter);
        }

        // Benchmark
        // qint64 time = _fpsTimer.nsecsElapsed();
        // painter.setPen(Qt::black);
        // painter.drawText(QPoint(5, 15), QString::number(time / 1000));

        _painter.end();
    }


#ifdef BSCAN_PROFILER
    auto end = std::chrono::steady_clock::now();
    auto elapsed = end - start;
    _profilerFilter[_filterCurPos] = elapsed.count();
    if (_filterCurPos != (_profilerFilter.size() - 1)) {
        ++_filterCurPos;
    }
    else {
        _filterCurPos = 0;

        std::sort(_profilerFilter.begin(), _profilerFilter.end());

        unsigned long long totalTime = std::accumulate(_profilerFilter.begin() + 4, _profilerFilter.end() - 4, 0ull);
        int min = *std::min_element(_profilerFilter.begin(), _profilerFilter.end());
        int max = *std::max_element(_profilerFilter.begin(), _profilerFilter.end());
        qDebug() << reinterpret_cast<int>(this) << "AVG:" << totalTime / (_profilerFilter.size() - 8) << "MIN:" << min << "MAX:" << max << "MID:" << _profilerFilter.at(_profilerFilter.size() / 2);
    }
#endif
    resetSteps();
}

void BScanPlot::resizeEvent(QResizeEvent* event)
{
    // qDebug() << "RESIZE:" << event->size();
    if (event->size().width() == -1 && event->size().height() == -1) {
        return;
    }

    if (event->oldSize().height() != event->size().height()) {
        resizeHeight(event->size().height());
    }
    if (event->oldSize().width() != event->size().width()) {
        resizeWidth(event->size().width());
    }
}

void BScanPlot::replot()
{
    update();
}

unsigned short BScanPlot::getBackwardColor() const
{
    return _backwardColor;
}

unsigned short BScanPlot::getForwardColor() const
{
    return _forwardColor;
}

void BScanPlot::setBackwardColor(unsigned short backwardColor)
{
    _backwardColor = backwardColor;
    _isInvalid = true;
    repaint();
}

void BScanPlot::setColorsForIndex(unsigned char index, unsigned short normalColor, unsigned short specialColor, unsigned short individualColor)
{
    _currentColorScheme.at(0).at(index) = normalColor;
    _currentColorScheme.at(1).at(index) = specialColor;
    _currentColorScheme.at(2).at(index) = individualColor;
    _isInvalid = true;
    repaint();
}

void BScanPlot::setForwardColor(unsigned short forwardColor)
{
    _forwardColor = forwardColor;
    _isInvalid = true;
    repaint();
}

void BScanPlot::setCurrentColorScheme(const BScanColorScheme& currentColorScheme)
{
    _currentColorScheme = currentColorScheme;
    replot();
}

void BScanPlot::shiftMarksAndRemove()
{
    std::for_each(_displayMarks.begin(), _displayMarks.end(), [this](DisplayMark& mark) { mark._posx -= _currentShift; });
    _displayMarks.erase(std::remove_if(_displayMarks.begin(), _displayMarks.end(), [](const DisplayMark& mark) { return (mark._posx < 0); }), _displayMarks.end());
}

void BScanPlot::setEncoderCorrection(float encoderCorrection)
{
    Q_ASSERT(encoderCorrection != 0.0f);
    _encoderCorrection = encoderCorrection;
    _encoderStepPerPixel = ((_bscanScale * 100.0f / _encoderCorrection) / static_cast<float>(_screenWidth));
    replot();
}

void BScanPlot::setOpacity(int opacity)
{
    Q_ASSERT(opacity >= 0 && opacity <= 255);
    _opacity = static_cast<unsigned char>(opacity);
    _autoMarksColor.setAlpha(_opacity);
    replot();
}

bool BScanPlot::getIsInvalid() const
{
    return _isInvalid;
}

void BScanPlot::setIsInvalid(bool isInvalid)
{
    _isInvalid = isInvalid;
}

void BScanPlot::setAutoMarksColor(const QColor& color)
{
    QColor newColor = color;
    newColor.setAlpha(_opacity);
    _autoMarksColor = newColor;
    replot();
}
