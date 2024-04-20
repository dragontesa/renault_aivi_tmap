#ifndef TRAFFICBARGRAPH_H
#define TRAFFICBARGRAPH_H

#include <QQuickItem>
#include <QList>

namespace SKT {

struct BarContext
{
    qreal distance;
    qreal barSize;
    QColor color;
    BarContext(qreal d, qreal b, QColor c) :
        distance(d),
        barSize(b),
        color(c)
    {
    }
};

class TrafficBarGraph : public QQuickItem
{
    Q_OBJECT
public:
    TrafficBarGraph();
    Q_PROPERTY(QVariant trafficBarSamples WRITE setTrafficBarSamples)
    void setTrafficBarSamples(const QVariant& trafficBarSamples);
    Q_INVOKABLE void resizeTrafficBar(const QVariant& traffics);

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

signals:

private:
    bool  mGeometryChanged;
    bool  mTrafficChanged;
    bool  mTrafficBarResized;
    QList<BarContext> mTrafficBarSamples;
};
}

#endif // TRAFFICBARGRAPH_H
