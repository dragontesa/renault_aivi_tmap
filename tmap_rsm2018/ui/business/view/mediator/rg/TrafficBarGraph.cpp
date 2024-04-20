#include "TrafficBarGraph.h"
#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGFlatColorMaterial>
#include <QList>
#include <QtMath>

namespace SKT {


class BarNode: public QSGGeometryNode
{
    QSGGeometry mGeometry;
    QSGFlatColorMaterial mMaterial;
    QColor mColor;
public:

    BarNode() :
    mGeometry(QSGGeometry::defaultAttributes_Point2D(),4,6) {
        setGeometry(&mGeometry);
        setMaterial(&mMaterial);
        mGeometry.setDrawingMode(GL_TRIANGLE_STRIP);
        // index data
        quint16* indics =  mGeometry.indexDataAsUShort();
        indics[0] = 0;
        indics[1] = 1;
        indics[2] = 2;
        indics[3] = 0;
        indics[4] = 3;
        indics[5] = 2;
        mGeometry.markIndexDataDirty();
    }

    void updateBar(const QRectF& bounds, const QColor& color)
    {
        qreal x = bounds.x();
        qreal y = bounds.y();
        qreal w = bounds.width();
        qreal h = bounds.height();
        QSGGeometry* g = geometry();
        // QSGGeometry::updateRectGeometry(g,bounds);

        // vertex data
        QSGGeometry::Point2D *v = g->vertexDataAsPoint2D();
        v[0].set(x,y);
        v[1].set(x,y+h);
        v[2].set(x+w,y+h);
        v[3].set(x+w,y);

        g->markVertexDataDirty();

        QSGFlatColorMaterial* m = static_cast<QSGFlatColorMaterial*>(material());
        m->setColor(color);

        markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    }
};


class BarGraph: public QSGNode
{
public:
    BarGraph() {
    }

    void updateBoundRect(const QRectF& bounds) {
        boundRect = bounds;
        qreal y = bounds.y();
        qreal h = bounds.height();

        if (childCount() == 0) return;
        BarNode* n    = static_cast<BarNode*>(firstChild());
        qreal    high = 0.f;
        int      cnt  = samples.size();
        int      i    = 0;
        while (n) {
            QSGGeometry* g = n->geometry();
            QSGGeometry::Point2D *v = g->vertexDataAsPoint2D();
            if (i == cnt)
                break;
            qreal s = samples.at(i++).barSize;
            high  += s;
            QRectF r(v[0].x,y+h-high, v[2].x-v[1].x, s);
            QColor c = static_cast<QSGFlatColorMaterial*>(n->material())->color();
            n->updateBar(r,c);
            n = static_cast<BarNode*>(n->nextSibling());
        }

        markDirty(QSGNode::DirtyGeometry);
    }

    void updateBarSamples(QList<BarContext> newSamples)
    {
        removeAllChildNodes();
        samples = newSamples;
        if (newSamples.size() == 0) {
            markDirty(QSGNode::DirtyNodeRemoved);
            return;
        }
        qreal  x = boundRect.x();
        qreal  y = boundRect.y();
        qreal  w = boundRect.width();
        qreal  h = boundRect.height();

        qreal high = 0.f;
        for (auto b : newSamples) {
            high += b.barSize;
            QRectF r(x,y+h-high,w,b.barSize);
            BarNode* n = new BarNode();
            n->updateBar(r,b.color);
            appendChildNode(n);
        }
    }

    void resizeBar(const QList<BarContext>& contexts)
    {
        samples = contexts;
        updateBoundRect(boundRect);
    }

    QList<BarContext> samples;
    QRectF boundRect;
};



TrafficBarGraph::TrafficBarGraph():
    mGeometryChanged(false),
    mTrafficChanged(false),
    mTrafficBarResized(false)
{
    setFlag(ItemHasContents, true);
}

void TrafficBarGraph::setTrafficBarSamples(const QVariant& traffics)
{
    QVariantList vars = qvariant_cast<QVariantList>(traffics);
    mTrafficBarSamples.clear();

    if (vars.size() == 0) {
        mTrafficChanged = true;
        update();
        return;
    }

    for (auto v : vars) {
        QVariantList list = qvariant_cast<QVariantList>(v);
        qreal  a = list.at(0).toReal(); // distance
        qreal  b = list.at(1).toReal(); // bar size
        QColor c = list.at(2).value<QColor>(); // color
        mTrafficBarSamples.append(BarContext(a,b,c));
    }

    mTrafficChanged = true;
    update();
}

void TrafficBarGraph::resizeTrafficBar(const QVariant& traffics)
{
    mTrafficBarSamples.clear();
    QVariantList vars = qvariant_cast<QVariantList>(traffics);
    for (auto v: vars) {
        QVariantList list = qvariant_cast<QVariantList>(v);
        qreal  a = list.at(0).toReal(); // distance
        qreal  b = list.at(1).toReal(); // bar size
        QColor c = list.at(2).value<QColor>(); // color
        mTrafficBarSamples.append(BarContext(a,b,c));
    }

   mTrafficBarResized = true;
   update();
}

void TrafficBarGraph::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    mGeometryChanged = true;
    update();
    QQuickItem::geometryChanged(newGeometry,oldGeometry);
}

QSGNode *TrafficBarGraph::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QRectF rect = boundingRect();

    BarGraph *n = static_cast<BarGraph*>(oldNode);
    if (!n) {
        n = new BarGraph();
    }

    if (mTrafficBarResized) {
        n->resizeBar(mTrafficBarSamples);
    }

    if (mTrafficChanged) {
        n->updateBarSamples(mTrafficBarSamples);
    }

    if (mGeometryChanged) {
        n->updateBoundRect(rect);
    }

    mGeometryChanged = false;
    mTrafficChanged = false;
    mTrafficBarResized = false;
    return n;
}

}
