#ifndef MSCANPLOT_H
#define MSCANPLOT_H

#include <QGraphicsView>
#include <QPen>
#include <QBrush>
#include <QMultiMap>
#include <QTimer>

class QGraphicsSvgItem;
class QAbstractGraphicsShapeItem;

class MScanPlot : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MScanPlot(QWidget* parent = 0);
    ~MScanPlot();

    void setScheme(const QString& fileName);

    bool drawPoint(const QString& id, qreal position, bool isRise = false);
    bool drawCross(const QString& id);
    void resetPoints(const QString& id);
    void resetAllPoints();

    QPen pen() const;
    void setPen(const QPen& pen);

    QBrush brush() const;
    void setBrush(const QBrush& brush);

    qreal radiusOfPoint() const;
    void setRadiusOfPoint(qreal radiusOfPoint);

    void showJustElement(const QString& id);

    void updateViewport();

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void fitSvgInView();
    QRectF getBoundsOnElementById(const QString& id);
    void addItemToScene(const QString& id, QGraphicsItem* graphicsItem, qreal position = 0);

private slots:
    void timeout();

private:
    QGraphicsSvgItem* _svgItem;
    QPen _pen;
    QPen _crossPen;
    QBrush _brush;
    qreal _radiusOfPoint;
    struct Item
    {
        QGraphicsItem* graphicsItem;
        int expired;
        qreal position;
    };
    typedef QMultiMap<QString, Item>::iterator ItemsIterator;
    typedef QMultiMap<QString, Item> Items;
    Items _items;
    QTimer _timer;
    bool _isNeedToUpdate;
};

#endif  // MSCANPLOT_H
