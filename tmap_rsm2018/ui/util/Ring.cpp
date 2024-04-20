#include "Ring.h"
#include <QSGGeometry>
#include <QSGNode>
#include <qmath.h>
#include <QDebug>
#include <QSGMaterial>
#include <QSGFlatColorMaterial>

namespace SKT
{

Ring::Ring(QQuickItem *parent) : QQuickItem(parent)
{
    mSegment = 0;
    mInnerRadius = 0;
    mOuterRadius = 0;
    mBeginAngle = 0.0;
    mEndAngle = 360.0;
    setFlag(ItemHasContents, true);
    //qDebug() << "Ring created...";
}

qreal radian(quint32 i, quint32 segment)
{
    return qDegreesToRadians((i/qreal(segment)) * 360);
}

QSGNode* Ring::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGGeometryNode *node = 0;
    QSGGeometry *geometry = 0;

    if (NULL == oldNode) {
//        qDebug() << "#1";
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), mSegment*6);
        geometry->setLineWidth(mOuterRadius - mInnerRadius);
        geometry->setDrawingMode(GL_TRIANGLES);//QSGGeometry::DrawTriangles);
        node->setGeometry(geometry);
        node->setFlag(QSGGeometryNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(mColor);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
//        qDebug() << "#2";
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        geometry->allocate(mSegment*6);
    }
    QSGGeometry::Point2D *vert = geometry->vertexDataAsPoint2D();

    qreal rBegin = qDegreesToRadians(mBeginAngle);
    qreal scale = (mEndAngle - mBeginAngle) / 360.0f;
    qreal r0;
    qreal r1;
    QPointF p0;
    QPointF p1;
    QPointF p2;
    QPointF p3;
    qreal s;
    qreal c;
    qreal dx = mOuterRadius + x();
    qreal dy = mOuterRadius + y();

    for (quint32 i=0; i<mSegment; ++i) {
        r0 = radian(i, mSegment);
        r1 = radian(i+1, mSegment);
        r0 = rBegin + r0 * scale;
        r1 = rBegin + r1 * scale;
        c = -qCos(r0);
        s = qSin(r0);
        p0.setX(mOuterRadius * c + dx);
        p0.setY(mOuterRadius * s + dy);
        p1.setX(mInnerRadius * c + dx);
        p1.setY(mInnerRadius * s + dy);

        c = -qCos(r1);
        s = qSin(r1);
        p2.setX(mOuterRadius * c + dx);
        p2.setY(mOuterRadius * s + dy);
        p3.setX(mInnerRadius * c + dx);
        p3.setY(mInnerRadius * s + dy);

        vert[i*6+0].set(p0.x(), p0.y());
        vert[i*6+1].set(p1.x(), p1.y());
        vert[i*6+2].set(p2.x(), p2.y());

        vert[i*6+3].set(p1.x(), p1.y());
        vert[i*6+4].set(p3.x(), p3.y());
        vert[i*6+5].set(p2.x(), p2.y());
    }

    //qDebug() << "#3 - " << mSegment;
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

QRectF Ring::boundingRect() const
{
    return QRectF(0, 0, mOuterRadius*2, mOuterRadius*2);
}

}//SKT
