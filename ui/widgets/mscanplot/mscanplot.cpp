#include <QGraphicsSvgItem>
#include <QResizeEvent>
#include <QSvgRenderer>
#include <QTimer>

#include "mscanplot.h"
#include "debug.h"

static const int timerInterval = 150;
static const int intervalToShowPoint = 1500;

MScanPlot::MScanPlot(QWidget* parent)
    : QGraphicsView(parent)
    , _svgItem(nullptr)
    , _pen(QColor(Qt::red))
    , _crossPen(QColor(Qt::red))
    , _brush(QColor(Qt::red))
    , _radiusOfPoint(5)
    , _timer(this)
    , _isNeedToUpdate(false)
{
    _crossPen.setWidth(3);
    setScene(new QGraphicsScene(this));
    setViewportUpdateMode(FullViewportUpdate);

    _timer.setInterval(timerInterval);
    _timer.setSingleShot(false);
    ASSERT(connect(&_timer, &QTimer::timeout, this, &MScanPlot::timeout));

    setViewportUpdateMode(NoViewportUpdate);
    this->setFocusPolicy(Qt::NoFocus);
}

MScanPlot::~MScanPlot()
{
    disconnect(&_timer, &QTimer::timeout, this, &MScanPlot::timeout);
    _timer.stop();
    resetAllPoints();
    if (_svgItem != nullptr) {
        delete _svgItem;
    }
}

void MScanPlot::setScheme(const QString& fileName)
{
    QGraphicsScene* s = scene();

    if (_svgItem != nullptr) {
        s->removeItem(_svgItem);
        delete _svgItem;
    }

    _svgItem = new QGraphicsSvgItem(fileName);
    s->addItem(_svgItem);
    fitSvgInView();
}

bool MScanPlot::drawPoint(const QString& id, qreal position, bool isRise)
{
    ItemsIterator iterator = _items.find(id);
    ItemsIterator endIt = _items.end();
    while (iterator != endIt && iterator.key() == id) {
        Item& item = iterator.value();
        if (item.position == position) {
            item.expired = intervalToShowPoint;
            return true;
        }
        ++iterator;
    }

    QRectF boundsOnElement = getBoundsOnElementById(id);
    if (boundsOnElement.isEmpty()) {
        return false;
    }
    qreal x = boundsOnElement.x() + (boundsOnElement.width() * position);
    qreal y = boundsOnElement.y() + (boundsOnElement.height() * (isRise ? 1 - position : position));
    boundsOnElement.setCoords(x - _radiusOfPoint, y - _radiusOfPoint, x + _radiusOfPoint, y + _radiusOfPoint);
    QGraphicsEllipseItem* graphicsItem = new QGraphicsEllipseItem(boundsOnElement);
    graphicsItem->setBrush(_brush);
    graphicsItem->setPen(_pen);

    addItemToScene(id, graphicsItem, position);

    return true;
}

bool MScanPlot::drawCross(const QString& id)
{
    ItemsIterator iterator = _items.find(id);
    if (iterator == _items.end()) {
        const QRectF& boundsOnElement = getBoundsOnElementById(id);
        if (boundsOnElement.isEmpty()) {
            return false;
        }
        qreal h = boundsOnElement.height() / 2;
        qreal w = boundsOnElement.width() / 2;
        qreal x = boundsOnElement.x() + (w / 2);
        qreal y = boundsOnElement.y() + (h / 2);

        QGraphicsLineItem* graphicsItem = new QGraphicsLineItem(x, y, x + w, y + h);
        graphicsItem->setPen(_crossPen);
        addItemToScene(id, graphicsItem);
        graphicsItem = new QGraphicsLineItem(x, y + h, x + w, y);
        graphicsItem->setPen(_crossPen);
        addItemToScene(id, graphicsItem);
    }
    else {
        while (iterator != _items.end() && iterator.key() == id) {
            iterator.value().expired = intervalToShowPoint;
            ++iterator;
        }
    }

    return true;
}

void MScanPlot::resetPoints(const QString& id)
{
    QGraphicsScene* s = scene();
    QList<MScanPlot::Item> values = _items.values(id);
    for (auto value : values) {
        QGraphicsItem* item = value.graphicsItem;
        s->removeItem(item);
        delete item;
    }
    _items.remove(id);
}

void MScanPlot::resetAllPoints()
{
    QGraphicsScene* s = scene();
    QMultiMap<QString, MScanPlot::Item>::iterator iter = _items.begin();
    while (iter != _items.end()) {
        QGraphicsItem* item = (*iter).graphicsItem;
        s->removeItem(item);
        delete item;
        ++iter;
    }
    _items.clear();
}

QPen MScanPlot::pen() const
{
    return _pen;
}

void MScanPlot::setPen(const QPen& pen)
{
    _pen = pen;
}

QBrush MScanPlot::brush() const
{
    return _brush;
}

void MScanPlot::setBrush(const QBrush& brush)
{
    _brush = brush;
}

qreal MScanPlot::radiusOfPoint() const
{
    return _radiusOfPoint * 2;
}

void MScanPlot::setRadiusOfPoint(qreal radiusOfPoint)
{
    _radiusOfPoint = radiusOfPoint / 2;
}

void MScanPlot::showJustElement(const QString& id)
{
    QSvgRenderer* renderer = _svgItem->renderer();

    QGraphicsSvgItem* item = new QGraphicsSvgItem();
    item->setSharedRenderer(renderer);
    item->setElementId(id);
    const QRectF& boundsOnElement = renderer->boundsOnElement(id);
    QPointF pos(boundsOnElement.x(), boundsOnElement.y());
    item->setPos(pos);

    scene()->removeItem(_svgItem);
    scene()->addItem(item);
}

void MScanPlot::updateViewport()
{
    if (_isNeedToUpdate) {
        viewport()->update();
        _isNeedToUpdate = false;
    }
}

void MScanPlot::paintEvent(QPaintEvent* event)
{
    QGraphicsView::paintEvent(event);
}

void MScanPlot::resizeEvent(QResizeEvent* event)
{
    fitSvgInView();
    QGraphicsView::resizeEvent(event);
}

void MScanPlot::fitSvgInView()
{
    if (_svgItem != nullptr) {
        setSceneRect(_svgItem->boundingRect());
        fitInView(_svgItem, Qt::IgnoreAspectRatio);
    }
}

QRectF MScanPlot::getBoundsOnElementById(const QString& id)
{
    if (_svgItem == 0) {
        return QRectF();
    }

    QSvgRenderer* svgRenderer = _svgItem->renderer();

    if (svgRenderer->elementExists(id) == false) {
        return QRectF();
    }

    return svgRenderer->boundsOnElement(id);
}

void MScanPlot::addItemToScene(const QString& id, QGraphicsItem* graphicsItem, qreal position)
{
    if (graphicsItem == 0) {
        QWARNING << "Graphics item is NULL";
    }

    MScanPlot::Item point;
    point.expired = intervalToShowPoint;
    point.graphicsItem = graphicsItem;
    point.position = position;
    scene()->addItem(graphicsItem);
    _items.insert(id, point);
    _isNeedToUpdate = true;

    if (_timer.isActive() == false) {
        _timer.start();
    }
}

void MScanPlot::timeout()
{
    QGraphicsScene* s = scene();
    QMultiMap<QString, MScanPlot::Item>::iterator iter = _items.begin();
    while (iter != _items.end()) {
        (*iter).expired -= timerInterval;
        if ((*iter).expired <= 0) {
            QGraphicsItem* item = (*iter).graphicsItem;
            s->removeItem(item);
            delete item;
            iter = _items.erase(iter);
            _isNeedToUpdate = true;
        }
        else {
            ++iter;
        }
    }
    if (_items.count() == 0) {
        _timer.stop();
    }
    updateViewport();
}
